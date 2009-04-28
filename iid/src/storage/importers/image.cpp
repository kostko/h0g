/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/importers/image.h"
#include "storage/storage.h"
#include "storage/texture.h"
#include "logger.h"

// SDL image library
#include "SDL_image.h"

namespace IID {

ImageImporter::ImageImporter(Context *context)
  : Importer(context)
{
}

void ImageImporter::load(Storage *storage, Item *item, const std::string &filename)
{
  if (item->getType() != "Texture") {
    m_logger->error("Images can only be imported into Texture items!");
    return;
  }
  
  // We have a texture
  Texture *texture = static_cast<Texture*>(item);
  
  // Use SDL_image to import the image
  SDL_Surface *image = IMG_Load(filename.c_str());
  if (!image) {
    m_logger->error("Unable to load image file from '" + filename + "'!");
    return;
  }
  
  // Flip the image since SDL convention is broken
  unsigned char line[image->pitch];
  unsigned char *pixels = (unsigned char*) image->pixels;
  unsigned int pitch = image->pitch;
  int ybegin = 0;
  int yend = image->h - 1;
  
  while (ybegin < yend) {
    memcpy(line, pixels + pitch*ybegin, pitch);
    memcpy(pixels + pitch*ybegin, pixels + pitch*yend, pitch);
    memcpy(pixels + pitch*yend, line, pitch);
    ybegin++;
    yend--;
  }
  
  // Copy image into the texture item
  if (image->format->BytesPerPixel == 3)
    texture->setImage(Texture::RGB, image->w, image->h, (unsigned char*) image->pixels);
  else
    texture->setImage(Texture::RGBA, image->w, image->h, (unsigned char*) image->pixels);
  
  SDL_FreeSurface(image);
}

}

