/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/importers/collada.h"
#include "storage/storage.h"
#include "logger.h"

// TinyXML parser
#include "tinyxml/ticpp.h"

#include <boost/lexical_cast.hpp>
#include <list>
#include <stdio.h>

namespace IID {

ColladaMeshImporter::ColladaMeshImporter(Context *context)
  : MeshImporter(context)
{
}

void ColladaMeshImporter::load(Storage *storage, Item *item, const std::string &filename)
{
  if (item->getType() != "CompositeMesh") {
    m_logger->error("COLLADA files can only be imported into CompositeMesh items!");
    return;
  }
  
  // List of final parsed meshes
  std::list<SubmeshObject*> objects;
  int objectIdx = 0;
  
  try {
    ticpp::Document collada(filename.c_str());
    collada.LoadFile();
    std::string version;
    
    if (collada.FirstChildElement()->Value() != "COLLADA") {
      m_logger->error("Not a valid COLLADA file format!");
      return;
    }
    
    collada.FirstChildElement()->GetAttribute("version", &version);
    if (version != "1.4.1") {
      m_logger->error("Unsupported COLLADA format version!");
      return;
    }
    
    // Parse out geometries
    ticpp::Iterator<ticpp::Element> child;
    for (child = child.begin(collada.FirstChildElement()); child != child.end(); child++) {
      if ((*child).Value() == "library_geometries") {
        // Handle each geometry
        ticpp::Iterator<ticpp::Element> geom;
        for (geom = geom.begin(&*child); geom != geom.end(); geom++) {
          if ((*geom).Value() == "geometry") {
            float *vertices = 0;
            int vertexCount;
            float *normals = 0;
            int normalCount;
            float *tex = 0;
            int texCount;
            
            // Handle subcomponents (sources and faces)
            ticpp::Iterator<ticpp::Element> subcomp;
            for (subcomp = subcomp.begin((*geom).FirstChild()); subcomp != subcomp.end(); subcomp++) {
              if ((*subcomp).Value() == "source") {
                // It is a source, determine what kind of source (this is actually hack since mappings
                // are actually specified lower in the COLLADA file, but it will do for SketchUp exported files)
                std::string id = (*subcomp).GetAttributeOrDefault("id", "");
                int count = boost::lexical_cast<int>((*subcomp).FirstChild()->ToElement()->GetAttributeOrDefault("count", "0"));
                
                if (id.find("-position") != std::string::npos) {
                  // Vertex definition
                  vertices = new float[count];
                  std::string data = (*subcomp).FirstChild()->ToElement()->GetText();
                  size_t pos = -1;
                  
                  for (int i = 0; i < count; i++) {
                    size_t oldp = pos + 1;
                    pos = data.find(' ', pos + 1);
                    vertices[i] = boost::lexical_cast<float>(data.substr(oldp, pos - oldp));
                  }
                  
                  vertexCount = count / 3;
                } else if (id.find("-normal") != std::string::npos) {
                  // Normal definition
                  normals = new float[count];
                  std::string data = (*subcomp).FirstChild()->ToElement()->GetText();
                  size_t pos = -1;
                  
                  for (int i = 0; i < count; i++) {
                    size_t oldp = pos + 1;
                    pos = data.find(' ', pos + 1);
                    normals[i] = boost::lexical_cast<float>(data.substr(oldp, pos - oldp));
                  }
                  
                  normalCount = count / 3;
                } else if (id.find("-uv") != std::string::npos) {
                  // Texture coordinate mappings
                  tex = new float[count];
                  std::string data = (*subcomp).FirstChild()->ToElement()->GetText();
                  size_t pos = -1;
                  
                  for (int i = 0; i < count; i++) {
                    size_t oldp = pos + 1;
                    pos = data.find(' ', pos + 1);
                    tex[i] = boost::lexical_cast<float>(data.substr(oldp, pos - oldp));
                  }
                  
                  texCount = count / 2;
                }
              } else if ((*subcomp).Value() == "triangles") {
                // Face definitions per-material
                int faceCount = boost::lexical_cast<int>((*subcomp).GetAttributeOrDefault("count", "0"));
                bool hasTextures = false;
                
                // Create an object
                SubmeshObject *obj = new SubmeshObject();
                obj->name = "object" + boost::lexical_cast<std::string>(objectIdx++);
                obj->faceCount = faceCount;
                obj->indices = new unsigned int[faceCount * 3];
                obj->vertexCount = faceCount * 3;
                obj->vertices = new float[obj->vertexCount * 3];
                obj->normals = new float[obj->vertexCount * 3];
                obj->tex = new float[obj->vertexCount * 3];
                objects.push_back(obj);
                
                ticpp::Iterator<ticpp::Element> tri;
                for (tri = tri.begin(&*subcomp); tri != tri.end(); tri++) {
                  if ((*tri).Value() == "input") {
                    if ((*tri).GetAttributeOrDefault("semantic", "") == "TEXCOORD")
                      hasTextures = true;
                  } else if ((*tri).Value() == "p") {
                    std::stringstream ss((*tri).GetText());
                    int idx;
                    
                    // An array of stuff
                    for (int vidx = 0; vidx < faceCount * 3; vidx++) {
                      // Vertex
                      ss >> idx;
                      obj->vertices[3*vidx] = vertices[3*idx];
                      obj->vertices[3*vidx + 1] = vertices[3*idx + 1];
                      obj->vertices[3*vidx + 2] = vertices[3*idx + 2];
                      
                      // Normal
                      ss >> idx;
                      obj->normals[3*vidx] = normals[3*idx];
                      obj->normals[3*vidx + 1] = normals[3*idx + 1];
                      obj->normals[3*vidx + 2] = normals[3*idx + 2];
                      
                      // Texture coordinates
                      if (hasTextures) {
                        ss >> idx;
                        obj->tex[2*vidx] = tex[2*idx];
                        obj->tex[2*vidx + 1] = tex[2*idx + 1];
                      }
                      
                      obj->indices[vidx] = vidx;
                    }
                  }
                }
              }
            }
            
            // Free stuff
            delete vertices;
            delete normals;
            delete tex;
          }
        }
      }
    }
  } catch (ticpp::Exception &e) {
    // Failed to open collada file or parse error
    m_logger->error("Unable to open specified COLLADA model!");
  }
  
  // Perform submesh post-processing and generate storage items
  postProcessSubmeshObjects(item, objects);
}

}

