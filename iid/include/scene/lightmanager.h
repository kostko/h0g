/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_LIGHTMANAGER_H
#define IID_SCENE_LIGHTMANAGER_H

#include "globals.h"

#include <list>

namespace IID {

class SceneNode;
class Light;

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
     * Computes lights that are affecting the object ordered by their
     * brightness.
     *
     * @param position Object position
     * @param count Number of lights to compute
     * @return A list of lights
     */
    std::list<Light*> computeAffectingLights(const Vector3f &position, int count = 3);
private:
    // A list of all lights
    boost::unordered_map<std::string, Light*> m_lights;
};

}

#endif
