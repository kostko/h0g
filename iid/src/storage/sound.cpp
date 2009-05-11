/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */

#include "drivers/base.h"
#include "storage/sound.h"
#include "exceptions.h"

#include <string>

namespace IID {

Sound::Sound(Storage *storage, const std::string &itemId, Item *parent)
  : Item(storage, "Sound", itemId, "", parent),
    m_soundBuffer(0),
    m_type(Effect)
{
}

Sound::~Sound()
{
    delete m_soundBuffer;
}

void Sound::setType(const std::string &type) 
{
    // Check type
    if (type == "Effect") {
        m_type = Effect;
    }
    else if (type == "Ambiental") {
        m_type = Ambiental;
    }
    else {
        throw Exception("Unknown sound type: " + type);
    }
}

void Sound::setSoundBuffer(SoundBuffer *sound) 
{
    m_soundBuffer = sound;
}

SoundBuffer *Sound::getSoundBuffer()
{
    return m_soundBuffer;
}

Sound::SoundType Sound::getType()
{
    return m_type;
}

Item *SoundFactory::create(Storage *storage, const std::string &itemId, Item *parent)
{
    return new Sound(storage, itemId, parent);
}

}
