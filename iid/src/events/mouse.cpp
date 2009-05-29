/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "events/mouse.h"
#include "events/dispatcher.h"

namespace IID {

MouseEvent::MouseEvent(int x, int y, int button, int state)
  : Event(Event::MouseEvent),
    m_x(x),
    m_y(y),
    m_button(button),
    m_state(state)
{
  m_mouseType = MouseMove;
  
  if (button > -1) {
    m_mouseType = (state == EventDispatcher::ButtonPressed) ? MouseButtonPress : MouseButtonRelease;
  }
}

}

