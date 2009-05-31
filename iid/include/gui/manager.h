/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_GUI_MANAGER_H
#define IID_GUI_MANAGER_H

#include "globals.h"

namespace IID {

class Context;
class MouseEvent;
class KeyboardEvent;

namespace GUI {

class Painter;
class Widget;
class WindowManager;

/**
 * The IID GUI manager is responsible for top-level widget management
 * and is the entry point into the GUI subsystem.
 */
class Manager {
public:
    /**
     * Class constructor.
     *
     * @param context Engine context
     */
    Manager(Context *context);
    
    /**
     * Class destructor.
     */
    ~Manager();
    
    /**
     * Returns the current manager instance.
     */
    static Manager *getInstance() { return m_instance; }
    
    /**
     * Updates the GUI. This method should be called every frame to
     * process any animations within the interface.
     *
     * @param dt Time difference to last call
     */
    void update(float dt);
    
    /**
     * Renders the GUI.
     */
    void render();
    
    /**
     * Handles the mouse move event.
     *
     * @param event Event to handle
     */
    void handleMouseMove(MouseEvent *event);
    
    /**
     * Handles the mouse button press event.
     *
     * @param event Event to handle
     * @return True if the event has been handled, false otherwise
     */
    bool handleMousePress(MouseEvent *event);
    
    /**
     * Handles the mouse button release event.
     *
     * @param event Event to handle
     * @return True if the event has been handled, false otherwise
     */
    bool handleMouseRelease(MouseEvent *event);
    
    /**
     * Handles a keyboard event.
     *
     * @param event Event to handle
     * @return True if the event has been handled, false otherwise
     */
    bool handleKeyboard(KeyboardEvent *event);
    
    /**
     * Redirects all mouse events directly to the specified widget.
     *
     * @param widget Widget that wants to grab the mouse
     */
    void grabMouse(Widget *widget);
    
    /**
     * Releases the held mouse grab.
     *
     * @param widget Widget that wants to release the mouse
     */
    void ungrabMouse(Widget *widget);
    
    /**
     * Redirects all keyboard events directly to the specified widget.
     *
     * @param widget Widget that wants to grab the keyboard
     */
    void grabKeyboard(Widget *widget);
    
    /**
     * Releases the held mouse grab.
     *
     * @param widget Widget that wants to release the keyboard
     */
    void ungrabKeyboard(Widget *widget);
    
    /**
     * Returns the window manager widget.
     */
    WindowManager *getWindowManager() const;
private:
    // Engine context instance
    Context *m_context;
    
    // Last manager instance
    static Manager *m_instance;
    
    // Painter for drawing 2D stuff
    Painter *m_painter;
    
    // Root widget
    WindowManager *m_root;
    Widget *m_mouseGrabWidget;
    Widget *m_keyboardGrabWidget;
};

}

}

#endif
