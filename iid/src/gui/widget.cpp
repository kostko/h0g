/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "gui/widget.h"
#include "gui/painter.h"
#include "gui/manager.h"
#include "events/mouse.h"
#include "events/keyboard.h"
#include "events/dispatcher.h"

#include <boost/foreach.hpp>

namespace IID {

namespace GUI {

Widget::Widget(Widget *parent)
  : m_parent(0),
    m_position(0, 0),
    m_size(0, 0),
    m_lastMouseIn(0)
{
  reparent(parent);
}

Widget::~Widget()
{
  BOOST_FOREACH(Widget *w, m_children) {
    delete w;
  }
}

Widget *Widget::getParent() const
{
  return m_parent;
}

void Widget::reparent(Widget *parent)
{
  // Clear the old parent first
  if (m_parent) {
    m_parent->m_children.erase(std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this));
    
    if (m_parent->m_lastMouseIn == this)
      m_parent->m_lastMouseIn = 0;
  }
  
  // Insert into the new parent
  if (parent) {
    parent->m_children.push_back(this);
  }
  
  m_parent = parent;
}

Vector2i Widget::mapToGlobal(int x, int y) const
{
  return mapToGlobal(Vector2i(x, y));
}

Vector2i Widget::mapToGlobal(const Vector2i &p) const
{
  if (!m_parent)
    return p;
  
  return m_parent->mapToGlobal(m_position + p);
}

void Widget::setPosition(int x, int y)
{
  m_position = Vector2i(x, y);
  m_globalPosition = mapToGlobal(0, 0);
}

Vector2i Widget::getPosition() const
{
  return m_position;
}

Vector2i Widget::getGlobalPosition() const
{
  return m_globalPosition;
}

void Widget::setSize(int width, int height)
{
  m_size = Vector2i(width, height);
  // TODO resize event
}

Vector2i Widget::getSize() const
{
  return m_size;
}

int Widget::getWidth() const
{
  return m_size[0];
}

int Widget::getHeight() const
{
  return m_size[1];
}


void Widget::setEnabled(bool enabled)
{
  m_enabled = enabled;
}

bool Widget::isEnabled() const
{
  return m_enabled;
}

Widget *Widget::childAt(int x, int y) const
{
  Vector2i p(x, y);
  
  // Search through all the children to find the right one
  BOOST_FOREACH(Widget *w, m_children) {
    if ((p.cwise() >= w->getGlobalPosition()).all() && (p.cwise() <= w->getGlobalPosition() + w->getSize()).all())
      return w;
  }
  
  return 0;
}

void Widget::paintEvent(Painter *painter)
{
  // Perform local painting first
  paint(painter);
  
  // Now propagate to children
  BOOST_FOREACH(Widget *w, m_children) {
    painter->save();
    painter->translate(w->getPosition());
    painter->setClipRegion(Vector2i(0, -1), w->getSize() + Vector2i(1, 1));
    w->paintEvent(painter);
    painter->restore();
  }
}

void Widget::paint(Painter *painter)
{
  // By default we just paint a rectangle
  painter->drawRect(0, 0, m_size[0], m_size[1]);
}

bool Widget::event(Event *event)
{
  // Dispatch event to some subwidget or take it ourselves depending on the event
  switch (event->getType()) {
    case Event::MouseEvent: {
      // Find out the widget under the mouse
      MouseEvent *ev = static_cast<MouseEvent*>(event);
      Widget *w = childAt(ev->x(), ev->y());
      if (m_lastMouseIn != w) {
        // Handle mouse enter/leave events
        if (m_lastMouseIn)
          m_lastMouseIn->leaveEvent(ev);
        
        if (w)
          w->enterEvent(ev);
        
        m_lastMouseIn = w;
      }
      
      // Check event type and propagate it accordingly
      if (w && w->event(ev))
        return true;
      
      // If not handled, handle it locally
      switch (ev->getMouseEventType()) {
        case MouseEvent::MouseMove: return mouseMoveEvent(ev);
        case MouseEvent::MouseButtonPress: return mousePressEvent(ev);
        case MouseEvent::MouseButtonRelease: return mouseReleaseEvent(ev);
      }
      break;
    }
    case Event::KeyboardEvent: {
      // TODO
      break;
    }
  }
  
  return false;
}

bool Widget::mouseMoveEvent(MouseEvent *event)
{
  return true;
}

bool Widget::mousePressEvent(MouseEvent *event)
{
  return true;
}

bool Widget::mouseReleaseEvent(MouseEvent *event)
{
  return true;
}

void Widget::enterEvent(Event *event)
{
}

void Widget::leaveEvent(Event *event)
{
}

void Widget::keyPressEvent(KeyboardEvent *event)
{
}

void Widget::keyReleaseEvent(KeyboardEvent *event)
{
}

void Widget::grabMouse()
{
  Manager::getInstance()->grabMouse(this);
}
    
void Widget::ungrabMouse()
{
  Manager::getInstance()->ungrabMouse(this);
}

}

}
