/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_GUI_BUTTON_H
#define IID_GUI_BUTTON_H

#include "globals.h"
#include "gui/widget.h"

#include <boost/signal.hpp>

namespace IID {

namespace GUI {

/**
 * A simple button widget.
 */
class Button : public Widget {
public:
    /**
     * Valid button states.
     */
    enum State {
      Raised,
      Sunken
    };
    
    /**
     * Class constructor.
     *
     * @param parent Parent widget
     */
    Button(Widget *parent);
    
    /**
     * Sets button text.
     *
     * @param text Text to display
     */
    void setText(const std::string &text);
    
    /**
     * Returns the button text.
     */
    std::string text() const;
    
    /**
     * Returns the current button state.
     */
    State getButtonState() const;
public:
    // Signals
    boost::signal<void ()> signalClicked;
protected:
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
    State m_state;
    std::string m_text;
};

}

}

#endif
