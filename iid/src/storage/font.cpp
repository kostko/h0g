/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/font.h"
#include "drivers/base.h"
#include "context.h"

#include <boost/foreach.hpp>

namespace IID {

Font::Font(Storage *storage, const std::string &itemId, Item *parent)
  : Item(storage, "Font", itemId, "", parent),
    m_driver(storage->context()->driver())
{
}

Font::~Font()
{
  typedef std::pair<unsigned short, DFont*> Child;
  BOOST_FOREACH(Child c, m_sizes) {
    delete c.second;
  }
}

void Font::setFontFilename(const std::string &filename)
{
  m_filename = filename;
}

DFont *Font::getFont(unsigned short size)
{
  if (m_sizes.find(size) == m_sizes.end()) {
    // When the size is not available yet, create one
    m_sizes[size] = m_driver->createFont(m_filename, size);
  }
  
  return m_sizes[size];
}

Item *FontFactory::create(Storage *storage, const std::string &itemId, Item *parent)
{
  return new Font(storage, itemId, parent);
}

}
