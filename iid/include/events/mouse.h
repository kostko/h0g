/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_EVENTS_MOUSE_H
#define IID_EVENTS_MOUSE_H

#include "globals.h"
#include "events/event.h"

namespace IID {

class MouseEvent : public Event {
public:
    /**
     * Mouse event types.
     */
    enum Type {
      MouseMove,
      MouseButtonPress,
      MouseButtonRelease
    };
    
    /**
     * Class constructor.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param button Button pressed or released
     * @param state Button state
     */
    MouseEvent(int x, int y, int button = -1, int state = -1);
    
    /**
     * Returns X coordinate.
     */
    int x() const { return m_x; }
    
    /**
     * Returns Y coordinate.
     */
    int y() const { return m_y; }
    
    /**
     * Returns the pressed/released mouse button.
     */
    int getButton() const { return m_button; }
    
    /**
     * Returns the button state.
     */
    int getState() const { return m_state; }
    
    /**
     * Returns the mouse event type.
     */
    Type getMouseEventType() const { return m_mouseType; }
private:
    Type m_mouseType;
    int m_x;
    int m_y;
    int m_button;
    int m_state;
};

}

#endif
