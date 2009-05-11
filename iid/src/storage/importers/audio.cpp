/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/importers/audio.h"
#include "storage/storage.h"
#include "storage/sound.h"
#include "drivers/openal.h"
#include "logger.h"

#include <string>
#include <stdio.h>

namespace IID {

SoundImporter::SoundImporter(Context *context)
  : Importer(context)
{
}

void SoundImporter::load(Storage *storage, Item *item, const std::string &filename)
{
    // Default type
    std::string type = "Effect";
    OpenALBuffer *buffer;
    
    if (item->getType() != "Sound") {
        m_logger->error("Sounds can only be imported into Sound items!");
        return;
    }
    
    Sound *sound = static_cast<Sound*>(item);
    
    // Try to get the sound type
    if (item->hasAttribute("Sound.Type")) 
    {
        type = item->getAttribute("Sound.Type") ["value"];
    }
    
    // Load file into a buffer
    buffer = new OpenALBuffer(filename.c_str());
    buffer->init();
    sound->setSoundBuffer(buffer);
    sound->setType(type);
    
    m_logger->info("Loaded sound: " + filename);
}

}
