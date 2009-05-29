/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "gui/window.h"
#include "gui/windows.h"
#include "gui/painter.h"

namespace IID {

namespace GUI {

Window::Window(WindowManager *parent)
  : Widget(parent)
{
}

void Window::paint(Painter *painter)
{
  painter->save();
  painter->setPenColor(Color(21, 142, 255, 180));
  painter->fillRect(0, 0, getWidth(), getHeight());
  painter->setPenColor(Color(255, 255, 255, 180));
  painter->drawRect(0, 0, getWidth(), getHeight());
  painter->restore();
}

}

}

