/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "events/event.h"

namespace IID {

Event::Event(Type type)
  : m_type(type)
{
}

Event::~Event()
{
}

}

