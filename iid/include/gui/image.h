/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_GUI_IMAGE_H
#define IID_GUI_IMAGE_H

#include "globals.h"
#include "gui/widget.h"

namespace IID {

class Texture;

namespace GUI {

/**
 * A simple image widget.
 */
class Image : public Widget {
public:
    /**
     * Class constructor.
     *
     * @param parent Parent widget
     */
    Image(Widget *parent);
    
    /**
     * Sets the texture that will be used for painting.
     *
     * @param texture Texture instance
     */
    void setTexture(IID::Texture *texture);
    
    /**
     * Returns the set texture.
     */
    IID::Texture *texture() const;
protected:
    /**
     * Paints this widget.
     *
     * @param painter Painter to be used for painting
     */
    void paint(Painter *painter);
private:
    IID::Texture *m_texture;
};

}

}

#endif
