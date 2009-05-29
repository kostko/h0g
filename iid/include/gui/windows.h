/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_GUI_WINDOWS_H
#define IID_GUI_WINDOWS_H

#include "globals.h"
#include "gui/widget.h"
#include "gui/color.h"

namespace IID {

class Event;
class MouseEvent;

namespace GUI {

class Painter;

/**
 * A top-level widget for managing windows.
 */
class WindowManager : public Widget {
public:
    /**
     * Class constructor.
     */
    WindowManager();
    
    /**
     * Sets the background color.
     */
    void setBackgroundColor(const Color &color);
protected:
    /**
     * Process local mouse move event.
     *
     * @param event Event instance
     */
    bool mouseMoveEvent(MouseEvent *event);
    
    /**
     * Process local mouse button press event.
     *
     * @param event Event instance
     */
    bool mousePressEvent(MouseEvent *event);
    
    /**
     * Process local mouse button release event.
     *
     * @param event Event instance
     */
    bool mouseReleaseEvent(MouseEvent *event);
    
    /**
     * Paints this widget.
     *
     * @param painter Painter to be used for painting
     */
    void paint(Painter *painter);
private:
    Color m_backgroundColor;
};

}

}

#endif
