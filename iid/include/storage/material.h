/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_STORAGE_MATERIAL_H
#define IID_STORAGE_MATERIAL_H

#include "storage.h"

namespace IID {

class Driver;

/**
 * This class represents a material.
 */
class Material : public Item {
public:
    /**
     * Class constructor.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param path Item path
     * @param parent Optional parent instance
     */
    Material(Storage *storage, const std::string &itemId = "", Item *parent = 0);
    
    /**
     * Class destructor.
     */
    ~Material();
    
    /**
     * Sets material properties.
     *
     * @param ambient Ambient component
     * @param diffuse Diffuse component
     * @param specular Specular component
     * @param emission Emission
     */
    void setProperties(const Vector3f &ambient, const Vector3f &diffuse,
                       const Vector3f &specular, float emission);
    
    /**
     * Binds this material.
     */
    void bind();
    
    /**
     * Unbinds this material.
     */
    void unbind() const;
private:
    // Material data
    Vector3f m_ambient;
    Vector3f m_diffuse;
    Vector3f m_specular;
    float m_emission;
    
    // Driver instance to avoid lookups
    Driver *m_driver;
};

/**
 * A factory of Material items.
 */
class MaterialFactory : public ItemFactory {
public:
    /**
     * Create a new Material storage item.
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
