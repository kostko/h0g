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
    m_type(PointLight),
    m_visible(true)
{
}

Light::~Light()
{
}

float Light::getSquaredDistanceTo(const Vector3f &point) const
{
  if (m_type == DirectionalLight) {
    m_lastDistance = 0;
  } else {
    m_lastDistance = (point - m_worldPosition).squaredNorm();
  }
  
  return m_lastDistance;
}

void Light::setType(Type type)
{
  m_type = type;
}

void Light::setVisible(bool value)
{
  m_visible = value;
}

void Light::setAttenuation(float range, float constant, float linear, float quadratic)
{
  m_range = range;
  m_attConst = constant;
  m_attLin = linear;
  m_attQuad = quadratic;
}

void Light::setDiffuseColor(float r, float g, float b)
{
  m_diffuse[0] = r;
  m_diffuse[1] = g;
  m_diffuse[2] = b;
}

void Light::setSpecularColor(float r, float g, float b)
{
  m_specular[0] = r;
  m_specular[1] = g;
  m_specular[2] = b;
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
