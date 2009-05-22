/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "events/mouse.h"

namespace IID {

MouseEvent::MouseEvent(int x, int y)
  : Event(Event::MouseMoveEvent),
    m_x(x),
    m_y(y)
{
}

MousePressEvent::MousePressEvent(int x, int y, int button)
  : MouseEvent(x, y),
    m_button(button)
{
}

}

