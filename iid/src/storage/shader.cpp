/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/shader.h"
#include "drivers/base.h"
#include "context.h"

namespace IID {

Shader::Shader(Storage *storage, const std::string &itemId, Item *parent)
  : Item(storage, "Shader", itemId, "", parent),
    m_shader(0)
{
}

Shader::~Shader()
{
  delete m_shader;
}

void Shader::setSource(const std::string &vertex, const std::string &fragment)
{
  // Just create a shader and we are done
  m_shader = m_storage->context()->driver()->createShader(vertex.c_str(), fragment.c_str());
}

void Shader::activate() const
{
  if (m_shader)
    m_shader->activate();
}

void Shader::deactivate() const
{
  if (m_shader)
    m_shader->deactivate();
}

Item *ShaderFactory::create(Storage *storage, const std::string &itemId, Item *parent)
{
  return new Shader(storage, itemId, parent);
}

}
