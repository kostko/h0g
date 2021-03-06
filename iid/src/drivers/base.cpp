/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "drivers/base.h"

namespace IID {

DTexture::DTexture(Format format)
  : m_format(format)
{
}

Driver::Driver(const std::string &name)
  : m_name(name),
    m_dispatcher(0)
{
}

void Driver::setEventDispatcher(AbstractEventDispatcher *dispatcher)
{
  m_dispatcher = dispatcher;
}

SoundBuffer::SoundBuffer(const std::string &fileName) 
  : m_fileName(fileName)
{
}

}
