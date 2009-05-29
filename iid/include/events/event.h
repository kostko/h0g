/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_EVENTS_EVENT_H
#define IID_EVENTS_EVENT_H

#include "globals.h"

#include <string>
#include <list>

namespace IID {

class Event {
public:
    /**
     * Valid event types.
     */
    enum Type {
      KeyboardEvent,
      MouseEvent
    };
    
    /**
     * Class constructor.
     *
     * @param type Event type
     */
    Event(Type type);
    
    /**
     * Class destructor.
     */
    virtual ~Event();
    
    /**
     * Returns this event's type.
     */
    Type getType() const { return m_type; }
private:
    Type m_type;
};

}

#endif
