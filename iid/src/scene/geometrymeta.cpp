/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/geometrymeta.h"

#include <boost/foreach.hpp>

namespace IID {

GeometryMetadata::GeometryMetadata(btTriangleIndexVertexMaterialArray *geometry)
  : m_geometry(geometry)
{
  // Process connectivity
  for (int i = 0; i < m_geometry->getNumSubParts(); i++) {
    btIndexedMesh *mesh = &m_geometry->getIndexedMeshArray()[i];
    processConnectivity(mesh);
  }
  
  // Process edge angles
  for (int i = 0; i < m_geometry->getNumSubParts(); i++) {
    btIndexedMesh *mesh = &m_geometry->getIndexedMeshArray()[i];
    processEdgeAngles(mesh);
  }
}

GeometryMetadata::~GeometryMetadata()
{
  BOOST_FOREACH(btMaterialProperties *info, m_faceInfos) {
    delete info->m_materialBase;
    delete info->m_triangleMaterialsBase;
    delete info;
  }
}

void GeometryMetadata::processConnectivity(btIndexedMesh *mesh)
{
  unsigned int *triangles = (unsigned int*) mesh->m_triangleIndexBase;
  float *vertices = (float*) mesh->m_vertexBase;
  
  for (int i = 0; i < mesh->m_numTriangles; i++) {
    Triangle t(mesh, i);
    Vertex tri[3];
    
    for (int j = 0; j < 3; j++) {
      int k = 3*triangles[3*i + j];
      tri[j].push_back(vertices[k]);
      tri[j].push_back(vertices[k + 1]);
      tri[j].push_back(vertices[k + 2]);
    }
    
    // Compute triangle normal
    Vector3f side0, side1;
    side0 << tri[0][0] - tri[1][0], tri[0][1] - tri[1][1], tri[0][2] - tri[1][2];
    side1 << tri[0][2] - tri[1][0], tri[2][1] - tri[1][1], tri[2][2] - tri[1][2];
    m_normals[t] = side1.cross(side0).normalized();
    
    for (int j = 0; j < 3; j++) {
      Vertex a = tri[j];
      Vertex b = tri[(j + 1) % 3];
      Edge e = a < b ? Edge(a, b) : Edge(b, a);
      
      m_connectivity[e].push_back(t);
    }
  }
}

void GeometryMetadata::processEdgeAngles(btIndexedMesh *mesh)
{
  unsigned int *triangles = (unsigned int*) mesh->m_triangleIndexBase;
  float *vertices = (float*) mesh->m_vertexBase;
  
  btMaterialProperties *faceInfo = new btMaterialProperties();
  faceInfo->m_numMaterials = mesh->m_numTriangles;
  faceInfo->m_numTriangles = mesh->m_numTriangles;
  faceInfo->m_materialStride = sizeof(FaceInfo);
  faceInfo->m_triangleMaterialStride = sizeof(unsigned int);
  
  FaceInfo *infos = new FaceInfo[faceInfo->m_numMaterials];
  int *indices = new int[faceInfo->m_numMaterials];
  faceInfo->m_materialBase = (unsigned char*) infos;
  faceInfo->m_triangleMaterialsBase = (unsigned char*) indices;
  
  for (int i = 0; i < mesh->m_numTriangles; i++) {
    Triangle t(mesh, i);
    Vertex tri[3];
    
    for (int j = 0; j < 3; j++) {
      int k = 3*triangles[3*i + j];
      tri[j].push_back(vertices[k]);
      tri[j].push_back(vertices[k + 1]);
      tri[j].push_back(vertices[k + 2]);
    }
    
    // Save normal and map triangle to info descriptor
    Vector3f myNormal = m_normals[t];
    indices[i] = i;
    
    // Initialize all angles to 360°
    infos[i].edgeAngles[0] = 2.0 * M_PI;
    infos[i].edgeAngles[1] = 2.0 * M_PI;
    infos[i].edgeAngles[2] = 2.0 * M_PI;
    
    for (int j = 0; j < 3; j++) {
      Vertex a = tri[j];
      Vertex b = tri[(j + 1) % 3];
      Edge e = a < b ? Edge(a, b) : Edge(b, a);
      
      BOOST_FOREACH(Triangle n, m_connectivity[e]) {
        if (n != t) {
          // Compute angle between triangle normals bordering this edge
          infos[i].edgeAngles[j] = std::acos(myNormal.dot(m_normals[n]) / (myNormal.norm() * m_normals[n].norm()));
        }
      }
    }
  }
  
  // Register descriptors for later collection and also with the geometry
  m_faceInfos.push_back(faceInfo);
  m_geometry->addMaterialProperties(*faceInfo);
}

}
