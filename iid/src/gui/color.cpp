/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "gui/color.h"

namespace IID {

namespace GUI {

Color::Color()
  : m_red(0),
    m_green(0),
    m_blue(0),
    m_alpha(0)
{
}

Color::Color(int r, int g, int b, int a)
  : m_red((float) r / 255.0),
    m_green((float) g / 255.0),
    m_blue((float) b / 255.0),
    m_alpha((float) a / 255.0)
{
}

Vector4f Color::getVector(Point point) const
{
  return Vector4f(getRed(point), getGreen(point), getBlue(point), getAlpha(point));
}

float Color::getRed(Point point) const
{
  return m_red;
}

float Color::getGreen(Point point) const
{
  return m_green;
}

float Color::getBlue(Point point) const
{
  return m_blue;
}

float Color::getAlpha(Point point) const
{
  return m_alpha;
}

}

}
