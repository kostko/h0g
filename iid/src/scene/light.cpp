/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/light.h"
#include "scene/lightmanager.h"
#include "scene/scene.h"
#include "renderer/statebatcher.h"

namespace IID {

Light::Light(const std::string &name, SceneNode *parent)
  : SceneNode(name, parent),
    m_type(PointLight)
{
}

Light::~Light()
{
}

void Light::setType(Type type)
{
  m_type = type;
}

void Light::setProperties(const Vector4f &ambient, const Vector4f &diffuse, const Vector4f &specular,
                          float attConst, float attLin, float attQuad)
{
  m_ambient = ambient;
  m_diffuse = diffuse;
  m_specular = specular;
  m_attConst = attConst;
  m_attLin = attLin;
  m_attQuad = attQuad;
}

void Light::updateNodeSpecific()
{
  SceneNode::updateNodeSpecific();
  
  // Ensure that the light is up to date
  if (m_lightManager)
    m_lightManager->updateLight(this);
}

void Light::updateSceneFromParent()
{
  SceneNode::updateSceneFromParent();
  
  // Ensure that the light gets registred in the light manager
  m_lightManager->addLight(this);
}

void Light::clearConnectionToScene()
{
  // Ensure that the light gets unregistred from the light manager
  if (m_lightManager)
    m_lightManager->removeLight(this);
  
  SceneNode::clearConnectionToScene();
}

}
