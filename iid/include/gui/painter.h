/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_GUI_PAINTER_H
#define IID_GUI_PAINTER_H

#include "globals.h"
#include "gui/color.h"

#include <list>

namespace IID {

class Driver;

namespace GUI {

/**
 * The painter is responsible for drawing primitives via the graphics
 * driver.
 */
class Painter {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    /**
     * Class constructor.
     *
     * @param driver Graphics driver instance
     * @param width Viewport width
     * @param height Viewport height
     */
    Painter(Driver *driver, int width, int height);
    
    /**
     * Sets the pen color.
     *
     * @param color Pen color
     */
    void setPenColor(const Color &color);
    
    /**
     * Draws a line between two points.
     *
     * @param x1 First point X coordinate
     * @param y1 First point Y coordinate
     * @param x2 Second point X coordinate
     * @param y2 Second point Y coordinate
     */
    void drawLine(int x1, int y1, int x2, int y2);
    
    /**
     * Draws a rectangle.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Width
     * @param height Height
     */
    void drawRect(int x, int y, int width, int height);
    
    /**
     * Fills a rectangle.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Width
     * @param height Height
     */
    void fillRect(int x, int y, int width, int height);
    
    /**
     * Draws a string.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param str String to draw
     */
    void drawString(int x, int y, const std::string &str);
    
    /**
     * Translates current origin by specified amount.
     *
     * @param dx Delta in X direction
     * @param dy Delta in Y direction
     */
    void translate(int dx, int dy);
    
    /**
     * Translates current origin by specified vector.
     *
     * @param dv Delta vector
     */
    void translate(const Vector2i &dv);
    
    /**
     * Returns the current origin vector.
     */
    Vector2i getOrigin() const;
    
    /**
     * Saves current painter state.
     */
    void save();
    
    /**
     * Restores painter state by poping it from the state stack.
     */
    void restore();
    
    /**
     * Sets the current clipping region.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Width
     * @param height Height
     */
    void setClipRegion(int x, int y, int width, int height);
    
    /**
     * Sets the current clipping region.
     *
     * @param pos Coordinates
     * @param dim Dimensions
     */
    void setClipRegion(const Vector2i &pos, const Vector2i &dim);
    
    /**
     * Enables or disables clipping.
     *
     * @param enable True to enable clipping
     */
    void setClipping(bool enable);
protected:
    /**
     * Applies the current state.
     */
    void applyState();
private:
    // Graphics driver instance
    Driver *m_driver;
    
    // Viewport size
    int m_width;
    int m_height;
    
    // Current painter state and the state stack
    struct State {
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        
        // Current origin
        Vector2i origin;
        
        // Pen color
        Color penColor;
        
        // Clipping enabled flag and clip region
        bool clipping;
        Vector4i clipRegion;
    };
    
    State m_state;
    std::list<State, Eigen::aligned_allocator<State> > m_stateStack;
};

}

}

#endif
