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

void StateBatcher::addToQueue(Rendrable *rendrable)
{
  m_renderQueue.insert(rendrable);
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
  
  // Render objects in the render queue
  BOOST_FOREACH(Rendrable *n, m_renderQueue) {
    // Check if shader has changed
    Shader *shader = n->getShader();
    if (currentShader != shader) {
      if (currentShader)
        currentShader->deactivate();
      
      shader->activate();
      currentShader = shader;
    }
    
    // Check if texture has changed
    Texture *texture = n->getTexture();
    if (currentTexture != texture) {
      if (currentTexture)
        currentTexture->unbind();
      
      if (texture)
        texture->bind();
      currentTexture = texture;
    }
    
    // Check if material has changed
    Material *material = n->getMaterial();
    if (currentMaterial != material) {
      if (currentMaterial)
        currentMaterial->unbind();
      
      if (material)
        material->bind();
      currentMaterial = material;
    }
    
    // Check if mesh has changed
    Mesh *mesh = n->getMesh();
    if (currentMesh != mesh) {
      if (currentMesh)
        currentMesh->unbind();
      
      mesh->bind();
      currentMesh = mesh;
    }
    
    // Apply lighting
    m_driver->applyModelViewTransform(viewTransform.data());
    m_driver->setupLights(n->getLights(), 3);
    
    // Apply the transformation and draw the thingie
    m_driver->applyModelViewTransform((viewTransform * n->worldTransform()).data());
    currentMesh->draw();
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

  m_renderQueue.clear();
  m_emitters.clear();
}

}
