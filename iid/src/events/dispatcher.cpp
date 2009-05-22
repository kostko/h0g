/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "events/dispatcher.h"
#include "events/keyboard.h"
#include "events/mouse.h"

namespace IID {

EventDispatcher::EventDispatcher(Context *context)
  : m_context(context)
{
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::keyboardEvent(bool special, int key, bool up)
{
  KeyboardEvent event(special, key, up);
  signalKeyboard(&event);
}

void EventDispatcher::mousePressEvent(int x, int y, int button)
{
  MousePressEvent event(x, y, button);
  signalMousePress(&event);
}

}

