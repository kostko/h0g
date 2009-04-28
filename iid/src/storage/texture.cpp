/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/texture.h"
#include "drivers/base.h"
#include "context.h"

#include <string.h>

namespace IID {

Texture::Texture(Storage *storage, const std::string &itemId, Item *parent)
  : Item(storage, "Texture", itemId, "", parent),
    m_image(0),
    m_texture(0)
{
}

Texture::~Texture()
{
  delete m_texture;
  delete m_image;
}

void Texture::setImage(Format fmt, int width, int height, unsigned char *image)
{
  // Free previously allocated image
  delete m_image;
  
  // Copy image data
  int components;
  DTexture::PixelFormat pformat;
  switch (fmt) {
    case RGBA: {
      components = 4;
      pformat = DTexture::RGBA;
      break;
    }
    default: {
      components = 3;
      pformat = DTexture::RGB;
      break;
    }
  }
  
  m_image = new unsigned char[width * height * components];
  memcpy(m_image, image, width * height * sizeof(unsigned char) * components);
  m_format = fmt;
  m_width = width;
  m_height = height;
  
  // Load texture
  m_texture = m_storage->context()->driver()->createTexture(DTexture::Texture2D);
  m_texture->bind(0);
  m_texture->setParameter(DTexture::WrapS, DTexture::Repeat);
  m_texture->setParameter(DTexture::WrapT, DTexture::Repeat);
  m_texture->setParameter(DTexture::MagFilter, DTexture::Linear);
  m_texture->setParameter(DTexture::MinFilter, DTexture::LinearMipmapNearest);
  m_texture->buildMipMaps2D(components, width, height, pformat, image);
  m_texture->unbind(0);
}

void Texture::bind(int textureUnit)
{
  m_texture->bind(textureUnit);
  m_lastTextureUnit = textureUnit;
}

void Texture::unbind() const
{
  m_texture->unbind(m_lastTextureUnit);
}

Item *TextureFactory::create(Storage *storage, const std::string &itemId, Item *parent)
{
  return new Texture(storage, itemId, parent);
}

}
