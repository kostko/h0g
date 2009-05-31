/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "gui/image.h"
#include "gui/painter.h"
#include "storage/texture.h"

namespace IID {

namespace GUI {

Image::Image(Widget *parent)
  : Widget(parent),
    m_texture(0)
{
}

void Image::setTexture(IID::Texture *texture)
{
  m_texture = texture;
}

IID::Texture *Image::texture() const
{
  return m_texture;
}

void Image::paint(Painter *painter)
{
  if (!m_texture)
    return;
  
  painter->drawImage(0, 0, getWidth(), getHeight(), m_texture);
}

}

}
