/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/importers/3ds.h"

#include <iostream>

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
  return mind + ((maxd - mind) / 2.);
}

}
