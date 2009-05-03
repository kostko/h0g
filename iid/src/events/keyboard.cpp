/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "events/keyboard.h"

namespace IID {

KeyboardEvent::KeyboardEvent(bool special, int key)
  : Event(Event::KeyboardEvent),
    m_special(special),
    m_key(key)
{
}

KeyboardEvent::~KeyboardEvent()
{
}

}

