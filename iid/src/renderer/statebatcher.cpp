/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "renderer/statebatcher.h"
#include "scene/scene.h"
#include "scene/viewtransform.h"
#include "scene/particles.h"
#include "storage/mesh.h"
#include "storage/texture.h"
#include "storage/material.h"
#include "storage/shader.h"
#include "drivers/base.h"
#include "context.h"

#include <boost/foreach.hpp>

namespace IID {

StateBatcher::StateBatcher(Scene *scene)
  : m_scene(scene),
    m_context(scene->context()),
    m_driver(m_context->driver())
{
}

void StateBatcher::add(Shader *shader, Mesh *mesh, Texture *texture, Material *material, const Transform3f &transform)
{
  // TODO to avoid constant allocations and frees, we could use a list of preallocated
  //      render queue nodes and just use those
  RenderQueueNode *n = new RenderQueueNode();
  n->shader = shader;
  n->mesh = mesh;
  n->texture = texture;
  n->material = material;
  n->transform = transform;
  
  // Insert the node into the render queue
  m_renderQueue.insert(n);
}

void StateBatcher::addLight(const Vector3f &position, const Vector4f &ambient, const Vector4f &diffuse,
                            const Vector4f &specular, float attConst, float attLin, float attQuad)
{
  // TODO to avoid constant allocations and frees, we could use a list of preallocated
  //      render queue nodes and just use those
  RenderQueueLight *l = new RenderQueueLight();
  l->position = position;
  l->ambient = ambient;
  l->diffuse = diffuse;
  l->specular = specular;
  l->attConst = attConst;
  l->attLin = attLin;
  l->attQuad = attQuad;
  
  // Insert the light into lights list
  m_lights.push_back(l);
}

void StateBatcher::addParticleEmitter(Shader *shader, Texture *texture, int size, float *vertices, float *colors,
                                      const Transform3f &transform)
{
  RenderQueueParticles *p = new RenderQueueParticles();
  p->shader = shader;
  p->texture = texture;
  p->size = size;
  p->vertices = vertices;
  p->colors = colors;
  p->transform = transform;

  // Insert into the particle emitter list
  m_emitters.push_back(p);
}

void StateBatcher::render()
{
  Shader *currentShader = 0;
  Texture *currentTexture = 0;
  Material *currentMaterial = 0;
  Mesh *currentMesh = 0;
  Transform3f viewTransform = m_scene->viewTransform()->transform();

  // First render all the lights
  int index = 0;
  BOOST_FOREACH(RenderQueueLight *l, m_lights) {
    Vector3f pos = viewTransform * l->position;
    
    m_driver->createLight(
      index++,
      pos.data(),
      l->ambient.data(),
      l->diffuse.data(),
      l->specular.data(),
      l->attConst,
      l->attLin,
      l->attQuad
    );
                            
    delete l;  
  }
  
  // Then render everything else
  BOOST_FOREACH(RenderQueueNode *n, m_renderQueue) {
    // Check if shader has changed
    if (currentShader != n->shader) {
      if (currentShader)
        currentShader->deactivate();
      
      n->shader->activate();
      currentShader = n->shader;
    }
    
    // Check if texture has changed
    if (currentTexture != n->texture) {
      if (currentTexture)
        currentTexture->unbind();
      
      if (n->texture)
        n->texture->bind();
      currentTexture = n->texture;
    }
    
    // Check if material has changed
    if (currentMaterial != n->material) {
      if (currentMaterial)
        currentMaterial->unbind();
      
      if (n->material)
        n->material->bind();
      currentMaterial = n->material;
    }
    
    // Check if mesh has changed
    if (currentMesh != n->mesh) {
      if (currentMesh)
        currentMesh->unbind();
      
      n->mesh->bind();
      currentMesh = n->mesh;
    }
    
    // Apply the transformation and draw the thingie
    m_driver->applyModelViewTransform((viewTransform * n->transform).data());
    currentMesh->draw();
    
    delete n;
  }
  
  // Draw particle emitters
  BOOST_FOREACH(RenderQueueParticles *p, m_emitters) {
    // Check if shader has changed
    if (currentShader != p->shader) {
      if (currentShader)
        currentShader->deactivate();
    
      if(p->shader)
        p->shader->activate();
      currentShader = p->shader;
    }

    // Check if texture has changed
    if (currentTexture != p->texture) {
      if (currentTexture)
        currentTexture->unbind();
    
      if (p->texture)
        p->texture->bind();
      currentTexture = p->texture;
    }

    // Draw the particle emitter
    m_driver->applyModelViewTransform((viewTransform * p->transform).data());
    m_driver->drawParticles(p->size, p->vertices, p->colors);
  }

  if (currentShader)
    currentShader->deactivate();

  m_lights.clear();
  m_renderQueue.clear();
  m_emitters.clear();
}

}
