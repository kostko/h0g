/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_EVENTS_DISPATCHER_H
#define IID_EVENTS_DISPATCHER_H

#include "events/base.h"

#include <boost/signal.hpp>

namespace IID {

class Context;
class KeyboardEvent;

class EventDispatcher : public AbstractEventDispatcher {
public:
    /**
     * Class constructor.
     *
     * @param context Engine context
     */
    EventDispatcher(Context *context);
    
    /**
     * Class destructor.
     */
    ~EventDispatcher();
    
    /**
     * Keyboard event handler.
     *
     * @param special Set to true if a special key has been pressed
     * @param key Key identifier
     */
    void keyboardEvent(bool special, int key);
public:
    // Signals
    boost::signal<void (KeyboardEvent*)> signalKeyboard;
private:
    Context *m_context;
};

}

#endif
