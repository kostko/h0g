/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/light.h"
#include "scene/scene.h"
#include "renderer/statebatcher.h"

namespace IID {

LightNode::LightNode(const std::string &name, SceneNode *parent)
  : SceneNode(name, parent),
    m_stateBatcher(0)
{
}

LightNode::~LightNode()
{
}

void LightNode::setProperties(const Vector4f &ambient, const Vector4f &diffuse, const Vector4f &specular,
                              float attConst, float attLin, float attQuad)
{
  m_ambient = ambient;
  m_diffuse = diffuse;
  m_specular = specular;
  m_attConst = attConst;
  m_attLin = attLin;
  m_attQuad = attQuad;
}

void LightNode::render()
{
  if (!m_stateBatcher)
    m_stateBatcher = m_scene->stateBatcher();
  
  m_stateBatcher->addLight(
    m_worldPosition,
    m_ambient,
    m_diffuse,
    m_specular,
    m_attConst,
    m_attLin,
    m_attQuad
  );
}

}
