/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_STORAGE_SHADER_H
#define IID_STORAGE_SHADER_H

#include "storage.h"

namespace IID {

class DShader;

/**
 * This class represents a shader.
 */
class Shader : public Item {
public:
    /**
     * Class constructor.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param path Item path
     * @param parent Optional parent instance
     */
    Shader(Storage *storage, const std::string &itemId = "", Item *parent = 0);
    
    /**
     * Class destructor.
     */
    ~Shader();
    
    /**
     * Setup shader source, compile and link shader program.
     *
     * @param vertex Vertex shader source
     * @param fragment Fragment shader source
     */
    void setSource(const std::string &vertex, const std::string &fragment);
    
    /**
     * Activate this shader program.
     */
    void activate() const;
    
    /**
     * Deactivate this shader program.
     */
    void deactivate() const;
private:
    DShader *m_shader;
};

/**
 * A factory of Shader items.
 */
class ShaderFactory : public ItemFactory {
public:
    /**
     * Create a new Shader storage item.
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
