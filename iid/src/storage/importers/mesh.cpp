/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/importers/3ds.h"
#include "storage/storage.h"
#include "storage/mesh.h"
#include "storage/compositemesh.h"
#include "logger.h"

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using boost::format;

namespace IID {

MeshImporter::MeshImporter(Context *context)
  : Importer(context)
{
}

float *MeshImporter::computeNormals(int vertexCount, int faceCount, float *vertices, unsigned int *faces) const
{
  float *normals = new float[vertexCount * 3];
  unsigned int *tmp = new unsigned int[vertexCount];
  
  // Initialize normals
  for (int i = 0; i < vertexCount * 3; i++) {
    normals[i] = 0;
    
    if (i % 3 == 0)
      tmp[i / 3] = 0;
  }
  
  for (int i = 0; i < faceCount; i++) {
    unsigned int a = faces[i*3];
    unsigned int b = faces[i*3 + 1];
    unsigned int c = faces[i*3 + 2];
    Vector3f side0;
    Vector3f side1;
    Vector3f normal;
    
    side0 << vertices[3*a] - vertices[3*b], vertices[3*a + 1] - vertices[3*b + 1], vertices[3*a + 2] - vertices[3*b + 2];
    side1 << vertices[3*c] - vertices[3*b], vertices[3*c + 1] - vertices[3*b + 1], vertices[3*c + 2] - vertices[3*b + 2];
    normal = side1.cross(side0).normalized();
    
    normals[3*a] += normal[0];
    normals[3*a + 1] += normal[1];
    normals[3*a + 2] += normal[2];
    tmp[a]++;
    normals[3*b] += normal[0];
    normals[3*b + 1] += normal[1];
    normals[3*b + 2] += normal[2];
    tmp[b]++;
    normals[3*c] += normal[0];
    normals[3*c + 1] += normal[1];
    normals[3*c + 2] += normal[2];
    tmp[c]++;
  }
  
  // Compute average
  for (int i = 0; i < vertexCount; i++) {
    if (tmp[i] == 0)
      continue;
    
    Vector3f n(normals[3*i], normals[3*i + 1], normals[3*i + 2]);
    n = n / (float) tmp[i];
    n.normalize();
    
    normals[3*i] = n[0];
    normals[3*i + 1] = n[1];
    normals[3*i + 2] = n[2];
  }
  
  // Free temporary memory
  delete tmp;
  
  return normals;
}

void MeshImporter::scaleMesh(float factorX, float factorY, float factorZ, int vertexCount, float *vertices) const
{
  for (int i = 0; i < vertexCount; i++) {
    vertices[3*i] *= factorX;
    vertices[3*i + 1] *= factorY;
    vertices[3*i + 2] *= factorZ;
  }
}

void MeshImporter::translateMesh(const Vector3f &vector, int vertexCount, float *vertices) const
{
  for (int i = 0; i < vertexCount; i++) {
    vertices[3*i] += vector[0];
    vertices[3*i + 1] += vector[1];
    vertices[3*i + 2] += vector[2];
  }
}

Vector3f MeshImporter::geometricCenter(const Vector3f &mind, const Vector3f &maxd) const
{
  return (mind + maxd) * 0.5;
}

void MeshImporter::postProcessSubmeshObjects(Item *item, std::list<SubmeshObject*> objects) const
{
  int totalVertexCount = 0;
  int totalFaceCount = 0;
  int totalObjectCount = 0;
  
  // Should only the first object be imported or should all be imported
  bool composite = item->getType() == "CompositeMesh";
  
  // Compute normals and apply scaling
  BOOST_FOREACH(SubmeshObject *obj, objects) {
    // Compute mesh normals if not set
    if (!obj->normals)
     obj->normals = computeNormals(obj->vertexCount, obj->faceCount, obj->vertices, obj->indices);
    
    // Scale mesh if needed
    if (item->hasAttribute("Mesh.ScaleFactor")) {
      StringMap factors = item->getAttribute("Mesh.ScaleFactor");
      
      scaleMesh(
        boost::lexical_cast<float>(factors["x"]),
        boost::lexical_cast<float>(factors["y"]),
        boost::lexical_cast<float>(factors["z"]),
        obj->vertexCount,
        obj->vertices
      );
    }
    
    totalVertexCount += obj->vertexCount;
    totalFaceCount += obj->faceCount;
    totalObjectCount++;
  }
  
  // Determine global and local geometric centers
  Vector3f center;
  Vector3f dimensions;
  Vector3f globalMind(
    std::numeric_limits<float>::infinity(),
    std::numeric_limits<float>::infinity(),
    std::numeric_limits<float>::infinity()
  );
  Vector3f globalMaxd(
    -std::numeric_limits<float>::infinity(),
    -std::numeric_limits<float>::infinity(),
    -std::numeric_limits<float>::infinity()
  );
  
  BOOST_FOREACH(SubmeshObject *obj, objects) {
    Vector3f localMind(
      std::numeric_limits<float>::infinity(),
      std::numeric_limits<float>::infinity(),
      std::numeric_limits<float>::infinity()
    );
    Vector3f localMaxd(
      -std::numeric_limits<float>::infinity(),
      -std::numeric_limits<float>::infinity(),
      -std::numeric_limits<float>::infinity()
    );
    
    for (int i = 0; i < obj->vertexCount; i++) {
      for (int j = 0; j < 3; j++) {
        if (obj->vertices[3*i + j] < localMind[j])
          localMind[j] = obj->vertices[3*i + j];
        
        if (obj->vertices[3*i + j] < globalMind[j])
          globalMind[j] = obj->vertices[3*i + j];
        
        if (obj->vertices[3*i + j] > localMaxd[j])
          localMaxd[j] = obj->vertices[3*i + j];
        
        if (obj->vertices[3*i + j] > globalMaxd[j])
          globalMaxd[j] = obj->vertices[3*i + j];
      }
    }
    
    obj->center = geometricCenter(localMind, localMaxd);
    obj->dimensions = localMaxd - localMind;
    obj->mind = localMind;
    obj->maxd = localMaxd;
  }
  
  center = geometricCenter(globalMind, globalMaxd);
  dimensions = globalMaxd - globalMind;
  
  // Set global mesh bounding box (needed for composites)
  if (composite)
    static_cast<CompositeMesh*>(item)->setBounds(globalMind, globalMaxd);
  
  // Move all objects to (0, 0, 0) and update relative hints
  BOOST_FOREACH(SubmeshObject *obj, objects) {
    translateMesh(-obj->center, obj->vertexCount, obj->vertices);
    obj->mind -= obj->center;
    obj->maxd -= obj->center;
    obj->relative = obj->center - center;
  }
  
  // Create all objects
  BOOST_FOREACH(SubmeshObject *obj, objects) {
    Mesh *mesh;
    if (composite) {
      // We are loading into a composite mesh, so we create new subitems
      mesh = new Mesh(item->storage(), obj->name, item);
    } else {
      // We are only interested in the last object
      mesh = static_cast<Mesh*>(item);
    }
    
    // Setup our mesh
    mesh->setMesh(
      obj->vertexCount,
      obj->faceCount * 3,
      (unsigned char*) obj->vertices,
      (unsigned char*) obj->normals,
      (unsigned char*) obj->tex,
      (unsigned char*) obj->indices
    );
    
    // Setup mesh bounds
    mesh->setBounds(obj->mind, obj->maxd);
    
    // Configure parent-relative position hint
    StringMap relative;
    relative["x"] = boost::lexical_cast<std::string>(obj->relative[0]);
    relative["y"] = boost::lexical_cast<std::string>(obj->relative[1]);
    relative["z"] = boost::lexical_cast<std::string>(obj->relative[2]);
    mesh->setAttribute("Mesh.RelativePosition", relative);
    
    // Free object resources
    delete obj->vertices;
    delete obj->normals;
    delete obj->tex;
    delete obj->indices;
    delete obj;
  }
  
  // Log some statistics
  m_logger->info(str(format("Loaded %d objects containing %d vertices and %d faces.") % totalObjectCount % totalVertexCount % totalFaceCount));
}

}
