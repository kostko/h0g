/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "gui/windows.h"
#include "gui/painter.h"

namespace IID {

namespace GUI {

WindowManager::WindowManager()
  : Widget(),
    m_backgroundColor(Color(0, 0, 0, 0))
{
}

void WindowManager::setBackgroundColor(const Color &color)
{
  m_backgroundColor = color;
}

bool WindowManager::mouseMoveEvent(MouseEvent *event)
{
  // Return false so event gets propagated back to the application
  return false;
}

bool WindowManager::mousePressEvent(MouseEvent *event)
{
  // Return false so event gets propagated back to the application
  return false;
}

bool WindowManager::mouseReleaseEvent(MouseEvent *event)
{
  // Return false so event gets propagated back to the application
  return false;
}

void WindowManager::paint(Painter *painter)
{
  painter->save();
  painter->setPenColor(m_backgroundColor);
  painter->fillRect(0, 0, getWidth(), getHeight());
  painter->restore();
}

}

}

