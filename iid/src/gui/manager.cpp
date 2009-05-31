/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "gui/manager.h"
#include "gui/painter.h"
#include "gui/widget.h"
#include "gui/windows.h"
#include "gui/window.h"
#include "gui/button.h"
#include "events/mouse.h"
#include "events/keyboard.h"
#include "drivers/base.h"
#include "context.h"

namespace IID {

namespace GUI {

Manager *Manager::m_instance = 0;

Manager::Manager(Context *context)
  : m_context(context),
    m_root(new WindowManager()),
    m_mouseGrabWidget(m_root),
    m_keyboardGrabWidget(m_root)
{
  Vector2i dim = context->getViewportDimensions();
  m_instance = this;
  m_painter = new Painter(context->driver(), dim[0], dim[1]);
  m_root->setSize(dim[0] - 1, dim[1] - 1);
}

Manager::~Manager()
{
  delete m_painter;
  delete m_root;
}

void Manager::update(float dt)
{
}

void Manager::render()
{
  // Enter 2D mode
  m_context->driver()->enter2DMode();
  
  // Paint top-level widgets, events will propagate down the line
  m_painter->setClipping(true);
  m_painter->setClipRegion(0, 0, m_root->getWidth(), m_root->getHeight());
  m_root->paintEvent(m_painter);
  m_painter->setClipping(false);
  
  // Leave 2D mode
  m_context->driver()->leave2DMode();
}

void Manager::handleMouseMove(MouseEvent *event)
{
  m_mouseGrabWidget->event(event);
}

bool Manager::handleMousePress(MouseEvent *event)
{
  return m_mouseGrabWidget->event(event);
}

bool Manager::handleMouseRelease(MouseEvent *event)
{
  return m_mouseGrabWidget->event(event);
}

bool Manager::handleKeyboard(KeyboardEvent *event)
{
  return m_keyboardGrabWidget->event(event);
}

void Manager::grabMouse(Widget *widget)
{
  m_mouseGrabWidget = widget;
}

void Manager::ungrabMouse(Widget *widget)
{
  m_mouseGrabWidget = m_root;
}

void Manager::grabKeyboard(Widget *widget)
{
  m_keyboardGrabWidget = widget;
}

void Manager::ungrabKeyboard(Widget *widget)
{
  m_keyboardGrabWidget = m_root;
}

WindowManager *Manager::getWindowManager() const
{
  return m_root;
}

}

}
