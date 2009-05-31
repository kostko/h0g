/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_STORAGE_FONT_H
#define IID_STORAGE_FONT_H

#include "storage.h"

namespace IID {

class DFont;
class Driver;

/**
 * This class represents a font.
 */
class Font : public Item {
public:
    /**
     * Class constructor.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param path Item path
     * @param parent Optional parent instance
     */
    Font(Storage *storage, const std::string &itemId = "", Item *parent = 0);
    
    /**
     * Class destructor.
     */
    ~Font();
    
    /**
     * Sets the font filename for this storage item.
     *
     * @param filename Font filename
     */
    void setFontFilename(const std::string &filename);

    /**
     * Returns the font instance.
     *
     * @param size Wanted font size
     */
    DFont *getFont(unsigned short size);
private:
    // Driver instance to avoid lookups
    Driver *m_driver;
    
    // Font source and mappings
    std::string m_filename;
    boost::unordered_map<unsigned short, DFont*> m_sizes;
};

/**
 * A factory of Font items.
 */
class FontFactory : public ItemFactory {
public:
    /**
     * Create a new Font storage item.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param parent Parent item
     * @return A valid Font item
     */
    Item *create(Storage *storage, const std::string &itemId, Item *parent);
};

}

#endif
