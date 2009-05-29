/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_GUI_COLOR_H
#define IID_GUI_COLOR_H

#include "globals.h"

namespace IID {

namespace GUI {

/**
 * This class represents different color configurations.
 */
class Color {
public:
    /**
     * Possible points at which color values can be obtained.
     */
    enum Point {
      UpperLeft = 1,
      UpperRight,
      LowerRight,
      LowerLeft
    };
    
    /**
     * Class constructor.
     */
    Color();
    
    /**
     * Class constructor.
     *
     * @param r Red component
     * @param g Green component
     * @param b Blue component
     * @param a Alpha component
     */
    Color(int r, int g, int b, int a);
    
    /**
     * Returns all color components.
     *
     * @param point Color point
     */
    Vector4f getVector(Point point) const;
    
    /**
     * Returns the red component of this color at a specified point.
     *
     * @param point Color point
     */
    float getRed(Point point) const;
    
    /**
     * Returns the green component of this color at a specified point.
     *
     * @param point Color point
     */
    float getGreen(Point point) const;
    
    /**
     * Returns the blue component of this color at a specified point.
     *
     * @param point Color point
     */
    float getBlue(Point point) const;
    
    /**
     * Returns the alpha component of this color at a specified point.
     *
     * @param point Color point
     */
    float getAlpha(Point point) const;
private:
    float m_red;
    float m_green;
    float m_blue;
    float m_alpha;
};

}

}

#endif
