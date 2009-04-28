/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/material.h"
#include "drivers/base.h"
#include "context.h"

#include <string.h>

namespace IID {

Material::Material(Storage *storage, const std::string &itemId, Item *parent)
  : Item(storage, "Material", itemId, "", parent),
    m_driver(storage->context()->driver())
{
}

Material::~Material()
{
}

void Material::setProperties(const Vector3f &ambient, const Vector3f &diffuse,
                             const Vector3f &specular, float emission)
{
  m_ambient = ambient;
  m_diffuse = diffuse;
  m_specular = specular;
  m_emission = emission;
}

void Material::bind()
{
  m_driver->applyMaterial(m_ambient.data(), m_diffuse.data(), m_specular.data(), m_emission);
}

void Material::unbind() const
{
}

Item *MaterialFactory::create(Storage *storage, const std::string &itemId, Item *parent)
{
  return new Material(storage, itemId, parent);
}

}
