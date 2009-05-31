/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_STORAGE_TEXTURE_H
#define IID_STORAGE_TEXTURE_H

#include "storage.h"

namespace IID {

class DTexture;

/**
 * This class represents a texture.
 */
class Texture : public Item {
public:
    /**
     * Possible image formats.
     */
    enum Format {
      RGB,
      RGBA
    };
    
    /**
     * Class constructor.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param path Item path
     * @param parent Optional parent instance
     */
    Texture(Storage *storage, const std::string &itemId = "", Item *parent = 0);
    
    /**
     * Class destructor.
     */
    ~Texture();
    
    /**
     * Sets an image for this item.
     *
     * @param fmt Image format
     * @param width Width in pixels
     * @param height Height in pixels
     * @param image Image data
     */
    void setImage(Format fmt, int width, int height, unsigned char *image);
    
    /**
     * Binds this texture to the specified texture unit.
     *
     * @param textureUnit A valid texture unit identifier
     */
    void bind(int textureUnit = 0);
    
    /**
     * Unbinds this texture from the bound texture unit.
     */
    void unbind() const;
    
    /**
     * Returns the underlying texture instance.
     */
    DTexture *getTexture() const;
private:
    // Image data
    Format m_format;
    int m_width;
    int m_height;
    unsigned char *m_image;
    
    // Texture data
    int m_lastTextureUnit;
    DTexture *m_texture;
};

/**
 * A factory of Texture items.
 */
class TextureFactory : public ItemFactory {
public:
    /**
     * Create a new Texture storage item.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param parent Parent item
     * @return A valid Texture item
     */
    Item *create(Storage *storage, const std::string &itemId, Item *parent);
};

}

#endif
