/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_GUI_WIDGET_H
#define IID_GUI_WIDGET_H

#include "globals.h"

#include <list>

namespace IID {

class Event;
class MouseEvent;
class KeyboardEvent;

namespace GUI {

class Painter;

/**
 * Base class for all widgets.
 */
class Widget {
friend class Manager;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    /**
     * Class constructor.
     *
     * @param parent Parent widget
     */
    Widget(Widget *parent = 0);
    
    /**
     * Class destructor.
     */
    virtual ~Widget();
    
    /**
     * Returns the parent widget.
     */
    Widget *getParent() const;
    
    /**
     * Reparents this widget.
     *
     * @param parent New parent widget
     */
    void reparent(Widget *parent);
    
    /**
     * Sets this widget's parent-relative position.
     *
     * @param x X coordinate
     * @param y Y coordinate
     */
    void setPosition(int x, int y);
    
    /**
     * Returns this widget's parent-relative position.
     */
    Vector2i getPosition() const;
    
    /**
     * Returns this widget's global position.
     */
    Vector2i getGlobalPosition() const;
    
    /**
     * Sets this widget's size.
     *
     * @param width Width
     * @param height Height
     */
    void setSize(int width, int height);
    
    /**
     * Returns this widget's size.
     */
    Vector2i getSize() const;
    
    /**
     * Returns this widget's width.
     */
    int getWidth() const;
    
    /**
     * Returns this widget's height.
     */
    int getHeight() const;
    
    /**
     * Sets the enabled state of this widget.
     *
     * @param enabled True for enabled, false for disabled
     */
    void setEnabled(bool enabled);
    
    /**
     * Returns true if this widget is currently enabled.
     */
    bool isEnabled() const;
    
    /**
     * Sets widget visibility.
     *
     * @param visible True for visible, false for invisible
     */
    void setVisible(bool visible);
    
    /**
     * Returns true if the widget is visible.
     */
    bool isVisible() const;
    
    /**
     * Returns the child widget at specified coordinates.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @return Widget at specified coordinates or NULL if there is none
     */
    Widget *childAt(int x, int y) const;
    
    /**
     * Maps widget-relative coordinates to global coordinates.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @return Global coordinates
     */
    Vector2i mapToGlobal(int x, int y) const;
    
    /**
     * Maps widget-relative coordinates to global coordinates.
     *
     * @param p Coordinate vector
     * @return Global coordinates
     */
    Vector2i mapToGlobal(const Vector2i &p) const;
protected:
    /**
     * Dispatches paint events.
     *
     * @param painter Painter to be used for painting
     */
    virtual void paintEvent(Painter *painter);
    
    /**
     * Paints this widget.
     *
     * @param painter Painter to be used for painting
     */
    virtual void paint(Painter *painter);
    
    /**
     * Processes a widget event. This will also properly propagate the
     * event to child widgets when necessary.
     *
     * @param event Event instance
     */
    virtual bool event(Event *event);
    
    /**
     * Process local mouse move event.
     *
     * @param event Event instance
     */
    virtual bool mouseMoveEvent(MouseEvent *event);
    
    /**
     * Process local mouse button press event.
     *
     * @param event Event instance
     */
    virtual bool mousePressEvent(MouseEvent *event);
    
    /**
     * Process local mouse button release event.
     *
     * @param event Event instance
     */
    virtual bool mouseReleaseEvent(MouseEvent *event);
    
    /**
     * Process local mouse enter event.
     *
     * @param event Event instance
     */
    virtual void enterEvent(Event *event);
    
    /**
     * Process local mouse leave event.
     *
     * @param event Event instance
     */
    virtual void leaveEvent(Event *event);
    
    /**
     * Process local key press event.
     *
     * @param event Event instance
     */
    virtual void keyPressEvent(KeyboardEvent *event);
    
    /**
     * Process local key release event.
     *
     * @param event Event instance
     */
    virtual void keyReleaseEvent(KeyboardEvent *event);
    
    /**
     * Redirects all mouse events to this widget directly.
     */
    void grabMouse();
    
    /**
     * Releases the mouse grab.
     */
    void ungrabMouse();
private:
    // Hierarchical structure information
    Widget *m_parent;
    std::list<Widget*> m_children;
    
    // Representation information
    Vector2i m_position;
    Vector2i m_globalPosition;
    Vector2i m_size;
    bool m_enabled;
    bool m_visible;
    
    // Mouse movement
    Widget *m_lastMouseIn;
};

}

}

#endif
