/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_EVENTS_BASE_H
#define IID_EVENTS_BASE_H

#include "globals.h"

namespace IID {

/**
 * An abstract event dispatcher interface. All event dispatchers must
 * subclass this and implement the specified methods to receive events
 * from drivers.
 */
class AbstractEventDispatcher {
public:
    /**
     * Valid key identifier for special keys.
     */
    enum Key {
      Up = 0,
      Down,
      Left,
      Right
    };
    
    /**
     * Keyboard event handler.
     *
     * @param special Set to true if a special key has been pressed
     * @param key Key identifier
     */
    virtual void keyboardEvent(bool special, int key) = 0;
};

}

#endif
