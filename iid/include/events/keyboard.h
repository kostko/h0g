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
     * @param special True if a special key has been pressed/released
     * @param key The key that was pressed/released
     * @param up Has the key been released
     */
    KeyboardEvent(bool special, int key, bool up);
    
    /**
     * Class destructor.
     */
    ~KeyboardEvent();
    
    /**
     * Returns true if a special key was pressed/released.
     */
    bool isSpecial() const { return m_special; }
    
    /**
     * Returns the key that was pressed/released.
     */
    int key() const { return m_key; }
    
    /**
     * Returns true if a key has been released.
     */
    bool isReleased() const { return m_up; }
private:
    bool m_special;
    int m_key;
    bool m_up;
};

}

#endif
