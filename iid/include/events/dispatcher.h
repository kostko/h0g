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
class MouseEvent;

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
     * @param up Has the key been released
     */
    void keyboardEvent(bool special, int key, bool up);
    
    /**
     * Mouse button press event handler.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param button Pressed button
     */
    void mousePressEvent(int x, int y, int button);
    
    /**
     * Mouse button release event handler.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param button Released button
     */
    void mouseReleaseEvent(int x, int y, int button);
    
    /**
     * Mouse move event handler.
     *
     * @param x X coordinate
     * @param y Y coordinate
     */
    void mouseMoveEvent(int x, int y);
public:
    // Signals
    boost::signal<void (KeyboardEvent*)> signalKeyboard;
    boost::signal<void (MouseEvent*)> signalMouseMove;
    boost::signal<void (MouseEvent*)> signalMousePress;
    boost::signal<void (MouseEvent*)> signalMouseRelease;
private:
    Context *m_context;
};

}

#endif
