/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_TIMING_H
#define IID_TIMING_H

#include <time.h>

namespace IID {

/**
 * This class is used to perform timing operations.
 */
class Clock {
public:
    /**
     * Class constructor.
     */
    Clock();
    
    /**
     * Resets the clock to current time.
     */
    void reset();
    
    /**
     * Returns the number of microseconds from last clock
     * restart.
     */
    float getTimeMicroseconds() const;
    
    /**
     * Returns the number of milliseconds from last clock
     * restart.
     */
    float getTimeMilliseconds() const;
private:
    // The actual time keeping variable
    timespec m_lastReset;
};

}

#endif

