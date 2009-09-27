/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "timing.h"

namespace IID {

Clock::Clock()
{
  reset();
}

void Clock::reset()
{
  clock_gettime(CLOCK_REALTIME, &m_lastReset);
}

float Clock::getTimeMicroseconds() const
{
  timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return (now.tv_sec - m_lastReset.tv_sec) * 1000000.0 + (now.tv_nsec - m_lastReset.tv_nsec) / 1000.0;
}

float Clock::getTimeMilliseconds() const
{
  return getTimeMicroseconds() / 1000.0;
}

}

