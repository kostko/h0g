/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/rendrable.h"
#include "scene/scene.h"
#include "renderer/statebatcher.h"
#include "storage/mesh.h"
#include "storage/texture.h"
#include "storage/shader.h"

namespace IID {

RendrableNode::RendrableNode(const std::string &name, SceneNode *parent)
  : SceneNode(name, parent),
    m_mesh(0),
    m_texture(0),
    m_shader(0),
    m_material(0),
    m_stateBatcher(0)
{
}

RendrableNode::~RendrableNode()
{
}

void RendrableNode::setMesh(Mesh *mesh)
{
  m_mesh = mesh;
  m_localBounds = mesh->getAABB();
}

void RendrableNode::setTexture(Texture *texture)
{
  m_texture = texture;
}

void RendrableNode::setShader(Shader *shader)
{
  m_shader = shader;
}

void RendrableNode::setMaterial(Material *material)
{
  m_material = material;
}

void RendrableNode::render()
{
  if (!m_stateBatcher)
    m_stateBatcher = m_scene->stateBatcher();
  
  m_stateBatcher->add(
    m_shader,
    m_mesh,
    m_texture,
    m_material,
    m_worldTransform
  );
}

}
