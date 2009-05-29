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
     * Valid mouse button identifiers.
     */
    enum MouseButton {
      MouseLeft = 0,
      MouseMiddle,
      MouseRight
    };
    
    /**
     * Valid mouse button states.
     */
    enum MouseButtonState {
      ButtonPressed = 0,
      ButtonReleased
    };
    
    /**
     * Keyboard event handler.
     *
     * @param special Set to true if a special key has been pressed/released
     * @param key Key identifier
     * @param up Has the key been released
     */
    virtual void keyboardEvent(bool special, int key, bool up) = 0;
    
    /**
     * Mouse press event handler.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param button Pressed button
     */
    virtual void mousePressEvent(int x, int y, int button) = 0;
    
    /**
     * Mouse button release event handler.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param button Released button
     */
    virtual void mouseReleaseEvent(int x, int y, int button) = 0;
    
    /**
     * Mouse move event handler.
     *
     * @param x X coordinate
     * @param y Y coordinate
     */
    virtual void mouseMoveEvent(int x, int y) = 0;
};

}

#endif
