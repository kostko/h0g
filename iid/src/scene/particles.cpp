/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/particles.h"
#include "scene/scene.h"
#include "renderer/statebatcher.h"
#include "storage/mesh.h"
#include "storage/texture.h"
#include "storage/shader.h"
#include "drivers/base.h"
#include "context.h"

#include <stdlib.h>
#include <math.h>

namespace IID {

ParticleEmitter::ParticleEmitter(const std::string &name, int maxParticles, SceneNode *parent)
  : SceneNode(name, parent),
    m_animate(false),
    m_render(false),
    m_maxParticles(maxParticles),
    m_speedFactor(1.0),
    m_slowdown(2.0f),
    m_zoom(-40.0f),
    m_gravity(Vector3f(0.0,-0.8,0.0)),
    m_particles(0),
    m_vertices(0),
    m_colors(0),
    m_texture(0),
    m_shader(0),
    m_colorList(0),
    m_showBoundingBox(false)
{
  m_particles  = (Particle*) malloc(m_maxParticles * sizeof(Particle));
  m_vertices   = (float*) malloc(m_maxParticles * 3 * sizeof(float));
  m_colors   = (float*) malloc(m_maxParticles * 4 * sizeof(float));
  m_localBounds = AxisAlignedBox();
}

ParticleEmitter::~ParticleEmitter()
{
  free(m_particles);
  free(m_vertices);
  free(m_colors);
}

void ParticleEmitter::init()
{
  // Some temporary pointers for easier setup
  float *vertices = m_vertices;
  float *colors = m_colors;
  
  for (int i = 0; i < m_maxParticles; i++) 
  {
    Particle *p = &m_particles[i];
    
    // Point to the data array
    p->m_position = vertices;
    p->m_rgba = colors;
    p->m_gravity = m_gravity;
    
    // Set initial parameters
    reset(p);
    
    // Increment the pointers
    vertices += 3;
    colors += 4;
  }
  
  if (m_shader) {
    Driver *driver = m_scene->context()->driver();
    
    // Setup shader stuff
    m_shader->activate();
    DShader *program = driver->currentShader();
    // Set initial uniform values

    m_shader->deactivate();
  }
}

void ParticleEmitter::show(bool value)
{
  m_render = value;
}

void ParticleEmitter::start()
{
  m_animate = true;
}

void ParticleEmitter::stop()
{
  m_animate = false;
}

void ParticleEmitter::animate() 
{
    if (!m_animate)
      return;
    
    // Update the parameters
  for (int i = 0; i < m_maxParticles; i++) 
  {
    Particle *p = &m_particles[i];

    // Skip dead particles
    if (!p->m_active)
      continue;

    p->m_position[0] += p->m_velocity[0] / (m_slowdown * 1000);
    p->m_position[1] += p->m_velocity[1] / (m_slowdown * 1000);
    p->m_position[2] += p->m_velocity[2] / (m_slowdown * 1000);
    
    Vector3f point = Vector3f(p->m_position[0], p->m_position[1], p->m_position[2]);
    
    // Check for bounds
    if(!m_localBounds.contains(point))
    {
      // Kill the particle
      p->m_life = 0;
    }
    
    p->m_velocity += p->m_gravity;
    
    // Reduce life by fade (alpha component of the color)
    p->m_life -= p->m_fade;
    p->m_rgba[3] = p->m_life;

    if (p->m_life < 0.0f) {
      // Reset the particle's parameters
      reset(p);
    }
  }
}

void ParticleEmitter::setTexture(Texture *texture)
{
  m_texture = texture;
}

void ParticleEmitter::setShader(Shader *shader)
{
  m_shader = shader;
}

void ParticleEmitter::setGravity(float gx, float gy, float gz)
{
  m_gravity = Vector3f(gx, gy, gz);
}

void ParticleEmitter::setBounds(float width, float breadth, float height)
{
  m_localBounds = AxisAlignedBox(Vector3f(-width/2, -height/2, -breadth/2),
                                 Vector3f(width/2, height/2, breadth/2)  );
  needUpdate();
}

void ParticleEmitter::setColors(std::vector<Vector3f> colors)
{
  m_colorList = colors;
}

void ParticleEmitter::boostSpeed(float sfactor)
{
  m_speedFactor = sfactor;
}

void ParticleEmitter::render(StateBatcher *batcher)
{
  // Step in the simulation
  animate();
  
  if (m_render) {
    batcher->addParticleEmitter(
      m_shader,
      m_texture,
      m_maxParticles,
      m_vertices,
      m_colors,
      m_worldTransform
    );
  }
}

void ParticleEmitter::reset(Particle *p)
{
  p->m_active = true;
  p->m_life = 1.0f;
  p->m_fade = RANDOM_FADE();
  
  p->m_position[0] = 0.0;
  p->m_position[1] = 0.0;
  p->m_position[2] = 0.0;
  
  Vector3f color = m_colorList[(int)(m_colorList.size()*(float)rand()/(float)RAND_MAX)];
  p->m_rgba[0] = color[0];
  p->m_rgba[1] = color[1];
  p->m_rgba[2] = color[2];
  
  p->m_velocity[0] = RANDOM_SPEED() * m_speedFactor;
  p->m_velocity[1] = RANDOM_SPEED() * m_speedFactor;
  p->m_velocity[2] = RANDOM_SPEED() * m_speedFactor;
  
  p->m_rgba[3] = p->m_life;
}

Explosion::Explosion(const std::string &name, int maxParticles, SceneNode *parent)
  : ParticleEmitter(name, maxParticles, parent)
{
}

void Explosion::animate() 
{
  if (!m_animate)
    return;
  
  bool allDead = true;
  
    // Update the parameters
  for (int i = 0; i < m_maxParticles; i++) 
  {
    Particle *p = &m_particles[i];

    // Skip dead particles
    if (!p->m_active)
      continue;

    allDead = false;
    
    p->m_position[0] += p->m_velocity[0] / (m_slowdown * 1000);
    p->m_position[1] += p->m_velocity[1] / (m_slowdown * 1000);
    p->m_position[2] += p->m_velocity[2] / (m_slowdown * 1000);
    
    Vector3f point = Vector3f(p->m_position[0], p->m_position[1], p->m_position[2]);
    
    // Check for bounds
    if(!m_localBounds.contains(point))
    {
      // Kill the particle
      p->m_life = 0;
    }
    
    p->m_velocity += p->m_gravity;
    
    // Reduce life by fade (alpha component of the color)
    p->m_life -= p->m_fade;
    p->m_rgba[3] = p->m_life;
  }
  
  if (allDead) {
    m_render = false;
    m_animate = false;
  }
}

void Explosion::render(StateBatcher *batcher)
{
  // Step in the simulation
  animate();
  
  if (m_render) {
    batcher->addParticleEmitter(
      m_shader,
      m_texture,
      m_maxParticles,
      m_vertices,
      m_colors,
      m_worldTransform
    );
  }
}

}
