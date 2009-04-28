/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_RENDERER_STATEBATCHER_H
#define IID_RENDERER_STATEBATCHER_H

#include "globals.h"

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

struct RenderQueueNode {
  Shader *shader;
  Mesh *mesh;
  Texture *texture;
  Material *material;
  Transform3f transform;
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct RenderQueueLight {
  Vector3f position;
  Vector4f ambient;
  Vector4f diffuse;
  Vector4f specular;
  float attConst;
  float attLin;
  float attQuad;
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

struct RenderQueueCompare {
  bool operator()(RenderQueueNode *n1, RenderQueueNode *n2)
  {
    if (n1->shader != n2->shader)
      return n1->shader < n2->shader;
    
    if (n1->texture != n2->texture)
      return n1->texture < n2->texture;
    
    if (n1->material != n2->material)
      return n1->material < n2->material;
    
    if (n1->mesh != n2->mesh)
      return n1->mesh < n2->mesh;
    
    return &n1->transform < &n2->transform;
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
     * Adds a render request.
     *
     * @param shader Shader instance
     * @param mesh Mesh instance
     * @param texture Texture instance
     * @param material Material instance
     * @param transform Model transformation
     */
    void add(Shader *shader, Mesh *mesh, Texture *texture, Material *material, const Transform3f &transform);
    
    /**
     * Adds a light to the render queue.
     *
     * @param position Light position
     * @param ambient Ambient component
     * @param diffuse Diffuse component
     * @param specular Specular component
     * @param attConst Constant attenuation
     * @param attLin Linear attenuation
     * @param attQuad Quadratic attenuation
     */
    void addLight(const Vector3f &position, const Vector4f &ambient, const Vector4f &diffuse,
                  const Vector4f &specular, float attConst, float attLin, float attQuad);
    
    /**
     * Renders all nodes in the render queue. After this method is called, the
     * render queue is cleared.
     */
    void render();
private:
    Scene *m_scene;
    Context *m_context;
    
    // Render queue
    std::list<RenderQueueLight*> m_lights;
    std::set<RenderQueueNode*, RenderQueueCompare> m_renderQueue;
    
    // Pointer to the driver to avoid lookups
    Driver *m_driver;
};

}

#endif
