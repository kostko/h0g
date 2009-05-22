/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_LIGHTMANAGER_H
#define IID_SCENE_LIGHTMANAGER_H

#include "globals.h"
#include "scene/light.h"

#include <list>

namespace IID {

class SceneNode;
class Light;
class Camera;

/**
 * The light manager is responsible for computing light contributions for
 * a specific object. It is used by the scene graph to get a list of lights
 * affecting the current scene node.
 */
class LightManager {
public:
    /**
     * Class constructor.
     */
    LightManager();
    
    /**
     * Registers a light with the light manager. Only registred lights
     * will be considered when computing affectors.
     *
     * @param light Light instance
     */
    void addLight(Light *light);
    
    /**
     * Unregisters a light.
     *
     * @param light Light instance
     */
    void removeLight(Light *light);
    
    /**
     * Updates a light. This method should be called when a light world
     * transformation changes.
     *
     * @param light Light instance
     */
    void updateLight(Light *light);
    
    /**
     * Populates a list of lights that are affecting the camera's frustum.
     *
     * @param camera A valid camera instance
     */
    void findLightsInFrustum(Camera *camera);
    
    /**
     * Computes lights that are affecting the object ordered by their
     * brightness.
     *
     * @param lights Destination list of affecting lights
     * @param position Object position
     * @param radius Object radius
     */
    void computeAffectingLights(LightList &lights, const Vector3f &position, float radius) const;
    
    /**
     * Returns the current value of the light version counter. This is
     * used for detecting whether a node's light cache is up to date.
     */
    unsigned long getLightVersionCounter() const { return m_lightVersionCounter; }
private:
    // A list of all lights
    boost::unordered_map<std::string, Light*> m_lights;
    
    // Light version counter used to know when nodes have out-of-date lighting
    unsigned long m_lightVersionCounter;
    
    // Light cache
    struct LightCacheItem {
      Light *light;
      int type;
      float range;
      Vector3f position;
      
      /**
       * Comparison operator for cached items. Note that checking the light
       * pointer is not enough since light properties might have changed
       * between frames.
       */
      bool operator==(const LightCacheItem &other) const
      {
        return light == other.light && type == other.type && range == other.range &&
               position == other.position;
      }
      
      /**
       * Comparison operator.
       */
      bool operator!=(const LightCacheItem &other) const
      {
        return !(*this == other);
      }
    };
    
    typedef std::vector<LightCacheItem> LightCache;
    LightList m_lightsInFrustum;
    LightCache m_lightCache;
    LightCache m_testCache;
};

}

#endif
