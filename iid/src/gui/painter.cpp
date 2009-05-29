/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "gui/painter.h"
#include "drivers/base.h"

namespace IID {

namespace GUI {

Painter::Painter(Driver *driver, int width, int height)
  : m_driver(driver),
    m_width(width),
    m_height(height)
{
  // Initialize state
  m_state.penColor = Color(255, 255, 255, 255);
  m_state.origin = Vector2i(0, 0);
  m_state.clipping = false;
}

void Painter::setPenColor(const Color &color)
{
  m_state.penColor = color;
}

void Painter::drawLine(int x1, int y1, int x2, int y2)
{
  // Adjust for current origin
  x1 += m_state.origin[0];
  y1 += m_state.origin[1];
  x2 += m_state.origin[0];
  y2 += m_state.origin[1];
  y1 = m_height - y1;
  y2 = m_height - y2;
  
  // Obtain colors
  Vector4f c1 = m_state.penColor.getVector(Color::UpperLeft);
  Vector4f c2 = m_state.penColor.getVector(Color::LowerRight);
  
  // Draw the line
  m_driver->drawLine(c1, Vector3f(x1, y1, 0.0), c2, Vector3f(x2, y2, 0.0));
}

void Painter::drawRect(int x, int y, int width, int height)
{
  // Adjust for current origin
  x += m_state.origin[0];
  y += m_state.origin[1];
  y = m_height - y;
  
  // Obtain colors
  Vector4f c1 = m_state.penColor.getVector(Color::UpperLeft);
  Vector4f c2 = m_state.penColor.getVector(Color::UpperRight);
  Vector4f c3 = m_state.penColor.getVector(Color::LowerRight);
  Vector4f c4 = m_state.penColor.getVector(Color::LowerLeft);
  
  // Draw the rectangle
  m_driver->drawRect(Vector3f(x, y, 0.0), Vector3f(width, height, 0.0), c1, c2, c3, c4);
}

void Painter::fillRect(int x, int y, int width, int height)
{
  // Adjust for current origin
  x += m_state.origin[0];
  y += m_state.origin[1];
  y = m_height - y;
  
  // Obtain colors
  Vector4f c1 = m_state.penColor.getVector(Color::UpperLeft);
  Vector4f c2 = m_state.penColor.getVector(Color::UpperRight);
  Vector4f c3 = m_state.penColor.getVector(Color::LowerRight);
  Vector4f c4 = m_state.penColor.getVector(Color::LowerLeft);
  
  // Draw the rectangle
  m_driver->fillRect(Vector3f(x, y, 0.0), Vector3f(width, height, 0.0), c1, c2, c3, c4);
}

void Painter::drawString(int x, int y, const std::string &str)
{
}

void Painter::translate(const Vector2i &dv)
{
  m_state.origin += dv;
}

void Painter::translate(int dx, int dy)
{
  translate(Vector2i(dx, dy));
}

Vector2i Painter::getOrigin() const
{
  return m_state.origin;
}

void Painter::save()
{
  m_stateStack.push_back(m_state);
}

void Painter::restore()
{
  m_state = m_stateStack.back();
  m_stateStack.pop_back();
  
  // Apply new state
  applyState();
}

void Painter::applyState()
{
  setClipping(m_state.clipping);
}

void Painter::setClipRegion(int x, int y, int width, int height)
{
  m_state.clipRegion = Vector4i(m_state.origin[0] + x, m_state.origin[1] + y, width, height);
  
  // Check if clipping is enabled and if so change the region
  if (m_state.clipping)
    m_driver->setScissorRegion(m_state.clipRegion);
}

void Painter::setClipRegion(const Vector2i &pos, const Vector2i &dim)
{
  setClipRegion(pos[0], pos[1], dim[0], dim[1]);
}

void Painter::setClipping(bool enable)
{
  m_state.clipping = enable;
  m_driver->setScissorTest(enable);
  
  // Setup scissor region when enabled
  if (enable)
    m_driver->setScissorRegion(m_state.clipRegion);
}

}

}
