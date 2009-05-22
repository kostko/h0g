/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/lightmanager.h"
#include "scene/light.h"
#include "scene/camera.h"

#include <boost/foreach.hpp>

namespace IID {

LightManager::LightManager()
  : m_lightVersionCounter(0)
{
}
    
void LightManager::addLight(Light *light)
{
  if (m_lights.find(light->getName()) != m_lights.end())
    return;
  
  m_lights[light->getName()] = light;
}

void LightManager::removeLight(Light *light)
{
  m_lights.erase(light->getName());
}

void LightManager::updateLight(Light *light)
{
}

inline bool compareLightsLessThan(const Light *l1, const Light *l2)
{
  // FIXME this should compare brightness
  return l1->getLastSquaredDistance() < l2->getLastSquaredDistance();
}

void LightManager::computeAffectingLights(LightList &lights, const Vector3f &position, float radius) const
{
  const LightList &candidateLights = m_lightsInFrustum;
  
  lights.clear();
  lights.reserve(candidateLights.size());
  
  BOOST_FOREACH(Light *l, candidateLights) {
    // Calculate distance to light source
    float distance = l->getSquaredDistanceTo(position);
    
    if (l->getType() == Light::DirectionalLight) {
      // Directional lights are always included
      //l->computeBrightnessTo(position);
      lights.push_back(l);
    } else {
      // Check if the light is in range
      float range = l->getAttenuationRange() + radius;
      if (distance <= range*range) {
        //l->computeBrightnessTo(position);
        lights.push_back(l);
      }
    }
  }
  
  // Sort lights
  std::stable_sort(lights.begin(), lights.end(), compareLightsLessThan);
}

void LightManager::findLightsInFrustum(Camera *camera)
{
  // First iterate through the lights and find if they are within the frustum;
  // currently this is just a simple walk through all the registred lights,
  // we should implement this using an octree later
  m_testCache.clear();
  m_testCache.reserve(m_lights.size());
  
  typedef std::pair<std::string, Light*> Item;
  BOOST_FOREACH(Item i, m_lights) {
    Light *l = i.second;
    
    if (l->isVisible()) {
      LightCacheItem cache;
      cache.light = l;
      cache.type = l->getType();
      
      if (l->getType() == Light::DirectionalLight) {
        // Directional lights are always visible since they are "infinite"
        cache.position = Vector3f::Zero();
        cache.range = 0;
        m_testCache.push_back(cache);
      } else {
        // Check if the light's attenuation range is within the frustum
        cache.range = l->getAttenuationRange();
        cache.position = l->getWorldPosition();
        
        if (camera->containsSphere(cache.position, cache.range) != Camera::Outside) {
          m_testCache.push_back(cache);
        }
      }
    }
  }
  
  // Now check if lights have changed (this is separated, because sorting
  // may be expensive)
  if (m_lightCache != m_testCache) {
    m_lightsInFrustum.resize(m_testCache.size());
    LightList::iterator i = m_lightsInFrustum.begin();
    
    BOOST_FOREACH(LightCacheItem item, m_testCache) {
      *i = item.light;
    }
    
    // Update cache
    m_lightCache.swap(m_testCache);
    
    // Increment version number
    m_lightVersionCounter++;
  }
}

}
