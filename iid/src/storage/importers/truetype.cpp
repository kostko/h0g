/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/importers/truetype.h"
#include "storage/storage.h"
#include "storage/font.h"
#include "drivers/base.h"
#include "logger.h"
#include "context.h"

namespace IID {

TrueTypeImporter::TrueTypeImporter(Context *context)
  : Importer(context)
{
}

void TrueTypeImporter::load(Storage *storage, Item *item, const std::string &filename)
{
  if (item->getType() != "Font") {
    m_logger->error("TrueType fonts can only be imported into Font items!");
    return;
  }
  
  // We have a font
  Font *font = static_cast<Font*>(item);
  
  // Use the driver to create a test font to see if the file is valid
  DFont *rfont = m_context->driver()->createFont(filename, 12);
  if (!rfont) {
    m_logger->error("Unable to load TrueType font from '" + filename + "'!");
    return;
  }
  
  font->setFontFilename(filename);
  delete rfont;
}

}

