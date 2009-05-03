/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "events/dispatcher.h"
#include "events/keyboard.h"

namespace IID {

EventDispatcher::EventDispatcher(Context *context)
  : m_context(context)
{
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::keyboardEvent(bool special, int key)
{
  KeyboardEvent event(special, key);
  signalKeyboard(&event);
}

}

