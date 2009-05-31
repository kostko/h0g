/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "gui/font.h"
#include "storage/font.h"
#include "drivers/base.h"

#include <math.h>

namespace IID {

namespace GUI {

FontMetrics::FontMetrics(int width, int height)
  : m_width(width),
    m_height(height)
{
}

int FontMetrics::getWidth() const
{
  return m_width;
}

int FontMetrics::getHeight() const
{
  return m_height;
}

Font::Font()
  : m_font(0),
    m_size(12)
{
}

Font::Font(IID::Font *font, unsigned short size)
  : m_font(font),
    m_size(size)
{
}

void Font::setSize(unsigned short size)
{
  m_size = size;
}

unsigned short Font::getSize() const
{
  return m_size;
}

void Font::render(int x, int y, int z, const std::string &text)
{
  if (m_font)
    m_font->getFont(m_size)->render(x, y, z, text);
}

FontMetrics Font::getFontMetrics(const std::string &text) const
{
  if (m_font) {
    IID::FontMetrics m = m_font->getFont(m_size)->getBoundingBox(text);
    return FontMetrics((int) round(m.first[0] - m.second[0]), (int) round(m.first[1] - m.second[1]));
  }
  
  return FontMetrics(0, 0);
}

}

}
