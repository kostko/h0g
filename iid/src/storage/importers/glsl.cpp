/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/importers/glsl.h"
#include "storage/storage.h"
#include "storage/shader.h"
#include "logger.h"

#include <iostream>
#include <fstream>
#include <boost/algorithm/string/trim.hpp>

namespace IID {

GLSLImporter::GLSLImporter(Context *context)
  : Importer(context)
{
}

void GLSLImporter::load(Storage *storage, Item *item, const std::string &filename)
{
  if (item->getType() != "Shader") {
    m_logger->error("GLSL source can only be imported into Shader items!");
    return;
  }
  
  // We have a shader
  Shader *shader = static_cast<Shader*>(item);
  
  // Parse the GLSL source file and extract vertex/fragment shaders
  std::ifstream file;
  file.open(filename.c_str());
  if (!file.is_open()) {
    m_logger->error("Unable to open GLSL file '" + filename + "'!");
    return;
  }
  
  std::string line;
  std::string vertexShader;
  std::string fragmentShader;
  int shaderType = 0;
  
  while (!file.eof()) {
    std::getline(file, line);
    boost::trim(line);
    
    if (line == "[Vertex_Shader]") {
      shaderType = 1;
      vertexShader.clear();
      continue;
    } else if (line == "[Fragment_Shader]") {
      shaderType = 2;
      fragmentShader.clear();
      continue;
    }
    
    switch (shaderType) {
      case 1: vertexShader.append(line + "\n"); break;
      case 2: fragmentShader.append(line + "\n"); break;
    }
  }
  
  file.close();
  
  // Configure shaders
  if (!shaderType) {
    m_logger->error("Missing shader declarations in GLSL source file!");
    return;
  }
  
  shader->setSource(vertexShader, fragmentShader);
}

}

