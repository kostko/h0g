/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_RENDERER_RENDRABLE_H
#define IID_RENDERER_RENDRABLE_H

#include "globals.h"
#include "scene/light.h"

namespace IID {

class Mesh;
class Texture;
class Shader;
class Material;

/**
 * An abstract rendrable class. This represents anything that can be
 * rendered by the renderer.
 */
class Rendrable {
friend class StateBatcher;
friend class RenderQueueCompare;
public:
    /**
     * Class destructor.
     */
    virtual ~Rendrable() {};
    
    /**
     * Returns this rendrable's mesh.
     */
    virtual Mesh *getMesh() const = 0;
    
    /**
     * Returns this rendrable's texture.
     */
    virtual Texture *getTexture() const = 0;
    
    /**
     * Returns this rendrable's shader.
     */
    virtual Shader *getShader() const = 0;
    
    /**
     * Returns this rendrable's material.
     */
    virtual Material *getMaterial() const = 0;
    
    /**
     * Returns the rendrable's world transformation.
     */
    virtual const Transform3f &worldTransform() const = 0;
    
    /**
     * Returns the light list of affecting lights.
     */
    virtual const LightList &getLights() const = 0;
};

}

#endif
