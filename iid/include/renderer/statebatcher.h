/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_RENDERER_STATEBATCHER_H
#define IID_RENDERER_STATEBATCHER_H

#include "globals.h"
#include "scene/light.h"
#include "renderer/rendrable.h"

#include <set>
#include <list>

namespace IID {

class Scene;
class Context;
class Shader;
class Mesh;
class Texture;
class Material;
class Driver;
class ParticleEmitter;
class Light;

struct RenderQueueParticles {
    Shader *shader;
    Texture *texture;
    int size;
    float *vertices;
    float *colors;
    Transform3f transform;
  
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct RenderQueueCompare {
  bool operator()(Rendrable *n1, Rendrable *n2)
  {
    // Compare shaders first
    Shader *shader1 = n1->getShader();
    Shader *shader2 = n2->getShader();
    if (shader1 != shader2)
      return shader1 < shader2;
    
    // Compare textures second
    Texture *texture1 = n1->getTexture();
    Texture *texture2 = n2->getTexture();
    if (texture1 != texture2)
      return texture1 < texture2;
    
    // Compare materials third
    Material *mat1 = n1->getMaterial();
    Material *mat2 = n2->getMaterial();
    if (mat1 != mat2)
      return mat1 < mat2;
    
    // Compare meshes last
    Mesh *mesh1 = n1->getMesh();
    Mesh *mesh2 = n2->getMesh();
    if (mesh1 != mesh2)
      return mesh1 < mesh2;
    
    return true;
  }
};

/**
 * State batcher is used to batch render requests in such a way so
 * render state changes are minimized.
 */
class StateBatcher {
public:
    /**
     * Class constructor.
     *
     * @param scene Scene instance
     */
    StateBatcher(Scene *scene);
    
    /**
     * Adds a rendrable object to the render queue.
     *
     * @param rendrable A valid rendrable object
     */
    void addToQueue(Rendrable *rendrable);
    
    /**
     * Adds a particle emitter to the render queue.
     *
     * @param shader Shader instance
     * @param texture Texture instance
     * @param size Size of the vertex and color arrays
     * @param vertices Array of vertices
     * @param colors Array of colors
     * @param transform Model transformation
     */
    void addParticleEmitter(Shader *shader, Texture *texture, int size, float *vertices, float *colors, 
                            const Transform3f &transform);
    
    /**
     * Renders all nodes in the render queue. After this method is called, the
     * render queue is cleared.
     */
    void render();
private:
    Scene *m_scene;
    Context *m_context;
    
    // Render queue
    std::set<Rendrable*, RenderQueueCompare> m_renderQueue;
    std::list<RenderQueueParticles*> m_emitters;
    
    // Pointer to the driver to avoid lookups
    Driver *m_driver;
};

}

#endif
