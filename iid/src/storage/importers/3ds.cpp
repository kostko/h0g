/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/importers/3ds.h"
#include "storage/storage.h"
#include "storage/mesh.h"
#include "logger.h"

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <list>
#include <stdio.h>

using boost::format;

namespace IID {

/**
 * A class for holding 3DS objects so some post-processing can be
 * performed after they are loaded.
 */
struct Object3DS {
  std::string name;
  int vertexCount;
  int faceCount;
  float *vertices;
  float *tex;
  float *normals;
  unsigned int *indices;
  Vector3f center;
  Vector3f dimensions;
  Vector3f relative;
  Vector3f mind;
  Vector3f maxd;
};

ThreeDSMeshImporter::ThreeDSMeshImporter(Context *context)
  : MeshImporter(context)
{
}

void ThreeDSMeshImporter::load(Storage *storage, Item *item, const std::string &filename)
{
  if (item->getType() != "Mesh" && item->getType() != "CompositeMesh") {
    m_logger->error("3DS files can only be imported into Mesh/CompositeMesh items!");
    return;
  }
  
  FILE *f = fopen(filename.c_str(), "rb");
  if (!f) {
    m_logger->error("Unable to open 3DS file '" + filename + "'!");
    return;
  }
  
  // Should only the first object be imported or should all be imported
  bool composite = item->getType() == "CompositeMesh";
  
  // Determine file size
  int fileSize;
  fseek(f, 0, SEEK_END);
  fileSize = ftell(f);
  rewind(f);
  
  // Traverse the 3DS hierarchy and parse out stuff
  unsigned short chunkId;
  unsigned int chunkLength;
  unsigned short vertexCount;
  unsigned short faceCount;
  int totalVertexCount = 0;
  int totalFaceCount = 0;
  int totalObjectCount = 0;
  float *vertices = 0;
  float *tex = 0;
  unsigned int *indices = 0;
  int endOfObject = 0;
  std::list<Object3DS*> objects;
  Object3DS *obj;
  
  while (ftell(f) < fileSize) {
    fread(&chunkId, 2, 1, f);
    fread(&chunkLength, 4, 1, f);
    
    // Check chunk type
    switch (chunkId) {
      case 0x4d4d: {
        // Skip main chunk header
        break;
      }
      case 0x3d3d: {
        // Skip 3D editor chunk header
        break;
      }
      case 0x4000: {
        // Object chunk - parse object name and create a new mesh object if needed
        std::string objectId;
        char ch;
        do {
          fread(&ch, 1, 1, f);
          if (ch)
            objectId.push_back(ch);
        } while (ch);
        
        m_logger->info("Found object named '" + objectId + "' in 3DS file!");
        
        obj = new Object3DS();
        obj->name = objectId;
        
        endOfObject++;
        break;
      }
      case 0x4100: {
        // Skip triangular mesh chunk header
        break;
      }
      case 0x4110: {
        // Process vertex list
        fread(&vertexCount, sizeof(unsigned short), 1, f);
        vertices = new float[vertexCount * 3];
        endOfObject++;
        
        for (int i = 0; i < vertexCount; i++) {
          fread(&vertices[i*3], sizeof(float), 1, f);
          fread(&vertices[i*3 + 1], sizeof(float), 1, f);
          fread(&vertices[i*3 + 2], sizeof(float), 1, f);
        }
        break;
      }
      case 0x4120: {
        // Process face list
        unsigned short flags;
        fread(&faceCount, sizeof(unsigned short), 1, f);
        indices = new unsigned int[faceCount * 3];
        endOfObject++;
        
        for (int i = 0; i < faceCount; i++) {
          unsigned short a;
          unsigned short b;
          unsigned short c;
          
          fread(&a, sizeof(unsigned short), 1, f);
          fread(&b, sizeof(unsigned short), 1, f);
          fread(&c, sizeof(unsigned short), 1, f);
          fread(&flags, sizeof(unsigned short), 1, f);
          
          indices[i*3] = a;
          indices[i*3 + 1] = b;
          indices[i*3 + 2] = c;
        }
        break;
      }
      case 0x4140: {
        // Process texture mapping data
        fread(&vertexCount, sizeof(unsigned short), 1, f);
        tex = new float[vertexCount * 2];
        endOfObject++;
        
        for (int i = 0; i < vertexCount; i++) {
          fread(&tex[i*2], sizeof(float), 1, f);
          fread(&tex[i*2 + 1], sizeof(float), 1, f);
        }
        break;
      }
      default: {
        // Unknown chunk, skip its header and data block
        fseek(f, chunkLength - 6, SEEK_CUR);
        break;
      }
    }
    
    // Check if the current object has been read
    if (endOfObject == 4) {
      endOfObject = 0;
      
      // Compute mesh normals
      float *normals = computeNormals(vertexCount, faceCount, vertices, indices);
      
      // Scale mesh if needed
      if (item->hasAttribute("Mesh.ScaleFactor")) {
        StringMap factors = item->getAttribute("Mesh.ScaleFactor");
        
        scaleMesh(
          boost::lexical_cast<float>(factors["x"]),
          boost::lexical_cast<float>(factors["y"]),
          boost::lexical_cast<float>(factors["z"]),
          vertexCount,
          vertices
        );
      }
      
      obj->vertexCount = vertexCount;
      obj->faceCount = faceCount;
      obj->vertices = vertices;
      obj->tex = tex;
      obj->normals = normals;
      obj->indices = indices;
      objects.push_back(obj);
      
      totalVertexCount += vertexCount;
      totalFaceCount += faceCount;
      totalObjectCount++;
    }
  }
  
  // Determine global and local geometric centers
  Vector3f center;
  Vector3f dimensions;
  Vector3f globalMind(
    std::numeric_limits<float>::infinity(),
    std::numeric_limits<float>::infinity(),
    std::numeric_limits<float>::infinity()
  );
  Vector3f globalMaxd(0, 0, 0);
  
  BOOST_FOREACH(Object3DS *obj, objects) {
    Vector3f localMind(
      std::numeric_limits<float>::infinity(),
      std::numeric_limits<float>::infinity(),
      std::numeric_limits<float>::infinity()
    );
    Vector3f localMaxd(0, 0, 0);
    
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
  
  // Move all objects to (0, 0, 0) and update relative hints
  BOOST_FOREACH(Object3DS *obj, objects) {
    translateMesh(-obj->center, obj->vertexCount, obj->vertices);
    obj->relative = obj->center - center;
  }
  
  // Create all objects
  BOOST_FOREACH(Object3DS *obj, objects) {
    Mesh *mesh;
    if (composite) {
      // We are loading into a composite mesh, so we create new subitems
      mesh = new Mesh(storage, obj->name, item);
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
    mesh->setBounds(obj->mind, obj->maxd, (obj->dimensions / 2.).norm());
    
    // Configure parent-relative position hint
    StringMap relative;
    relative["x"] = boost::lexical_cast<std::string>(obj->relative[0]);
    relative["y"] = boost::lexical_cast<std::string>(obj->relative[1]);
    relative["z"] = boost::lexical_cast<std::string>(obj->relative[2]);
    mesh->setAttribute("Mesh.RelativePosition", relative);
    
    // Free object resources - note that vertices and indices are not freed
    // as they are used internally by Mesh objects
    delete obj->normals;
    delete obj->tex;
    delete obj;
  }
  
  // Log some statistics
  m_logger->info(str(format("Loaded %d objects containing %d vertices and %d faces.") % totalObjectCount % totalVertexCount % totalFaceCount));
  
  fclose(f);
}

}

