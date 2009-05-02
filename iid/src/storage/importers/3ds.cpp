/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/importers/3ds.h"
#include "storage/storage.h"
#include "logger.h"

#include <list>
#include <stdio.h>

namespace IID {

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
  float *vertices = 0;
  float *tex = 0;
  unsigned int *indices = 0;
  std::list<SubmeshObject*> objects;
  SubmeshObject *obj;
  
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
        
        obj = new SubmeshObject();
        obj->name = objectId;
        objects.push_back(obj);
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
        obj->vertexCount = vertexCount;
        obj->vertices = vertices;
        
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
        obj->faceCount = faceCount;
        obj->indices = indices;
        
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
        obj->tex = tex;
        
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
  }
  
  fclose(f);
  
  // Perform submesh post-processing and generate storage items
  postProcessSubmeshObjects(item, objects);
}

}

