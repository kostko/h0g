/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_GUI_WINDOW_H
#define IID_GUI_WINDOW_H

#include "globals.h"
#include "gui/widget.h"

namespace IID {

namespace GUI {

class WindowManager;
class Painter;

/**
 * A simple window.
 */
class Window : public Widget {
public:
    /**
     * Class constructor.
     *
     * @param parent Window manager parent
     */
    Window(WindowManager *parent);
protected:
    /**
     * Paints this widget.
     *
     * @param painter Painter to be used for painting
     */
    void paint(Painter *painter);
};

}

}

#endif
