/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_GUI_FONT_H
#define IID_GUI_FONT_H

#include "globals.h"

namespace IID {

class Font;

namespace GUI {

/**
 * Class used for returning font metric information.
 */
class FontMetrics {
public:
    /**
     * Class constructor.
     *
     * @param width String width
     * @param height String height
     */
    FontMetrics(int width, int height);
    
    /**
     * Returns string's width.
     */
    int getWidth() const;
    
    /**
     * Returns string's height.
     */
    int getHeight() const;
private:
    int m_width;
    int m_height;
};

/**
 * This class represents a font.
 */
class Font {
public:
    /**
     * Class constructor.
     */
    Font();
    
    /**
     * Class constructor.
     *
     * @param font Font storage item
     * @param size Optional font size
     */
    Font(IID::Font *font, unsigned short size = 12);
    
    /**
     * Sets the size of this font.
     *
     * @param size Font size
     */
    void setSize(unsigned short size);

    /**
     * Returns this font's size.
     */
    unsigned short getSize() const;
    
    /**
     * Renders some text with this font.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     * @param text Text to render
     */
    void render(int x, int y, int z, const std::string &text);
    
    /**
     * Returns the font metrics for a specified string.
     *
     * @param text String
     * @return Font metrics for this string
     */
    FontMetrics getFontMetrics(const std::string &text) const;
private:
    IID::Font *m_font;
    unsigned short m_size;
};

}

}

#endif
