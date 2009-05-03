/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_EVENTS_KEYBOARD_H
#define IID_EVENTS_KEYBOARD_H

#include "globals.h"
#include "events/event.h"

namespace IID {

class KeyboardEvent : public Event {
public:
    /**
     * Class constructor.
     *
     * @param special True if a special key has been pressed
     * @param key The key that was pressed
     */
    KeyboardEvent(bool special, int key);
    
    /**
     * Class destructor.
     */
    ~KeyboardEvent();
    
    /**
     * Returns true if a special key was pressed.
     */
    bool isSpecial() const { return m_special; }
    
    /**
     * Returns the key that was pressed.
     */
    int key() const { return m_key; }
private:
    bool m_special;
    int m_key;
};

}

#endif
