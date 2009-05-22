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
     * Class constructor.
     *
     * @param x X coordinate
     * @param y Y coordinate
     */
    MouseEvent(int x, int y);
    
    /**
     * Returns X coordinate.
     */
    int x() const { return m_x; }
    
    /**
     * Returns Y coordinate.
     */
    int y() const { return m_y; }
private:
    int m_x;
    int m_y;
};

class MousePressEvent : public MouseEvent {
public:
    /**
     * Class constructor.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param button Button that was pressed
     */
    MousePressEvent(int x, int y, int button);
    
    /**
     * Returns the pressed mouse button.
     */
    int getButton() const { return m_button; }
private:
    int m_button;
};

}

#endif
