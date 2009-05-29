/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "gui/button.h"
#include "gui/painter.h"
#include "events/mouse.h"

namespace IID {

namespace GUI {

Button::Button(Widget *parent)
  : Widget(parent),
    m_state(Raised)
{
}

bool Button::mousePressEvent(MouseEvent *event)
{
  // Take over the mouse so noone else can have it
  grabMouse();
  
  // Button is now sunken
  m_state = Sunken;
  
  return true;
}

bool Button::mouseReleaseEvent(MouseEvent *event)
{
  if (m_state != Sunken)
    return true;
  
  // Release the mouse and emit a click event if it's on us
  ungrabMouse();
  
  // Button is now raised again
  m_state = Raised;
  
  if (getParent()->childAt(event->x(), event->y()) == this)
    signalClicked();
  
  return true;
}

void Button::paint(Painter *painter)
{
  painter->save();
  
  switch (m_state) {
    case Sunken: painter->setPenColor(Color(217, 0, 0, 200)); break;
    case Raised: painter->setPenColor(Color(0, 0, 0, 255)); break;
  }
  
  painter->fillRect(0, 0, getWidth(), getHeight());
  painter->setPenColor(Color(255, 255, 255, 255));
  painter->drawRect(0, 0, getWidth(), getHeight());
  
  painter->restore();
}

Button::State Button::getButtonState() const
{
  return m_state;
}

}

}
