/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/lightmanager.h"
#include "scene/light.h"

namespace IID {

LightManager::LightManager()
{
}
    
void LightManager::addLight(Light *light)
{
}

void LightManager::removeLight(Light *light)
{
}

void LightManager::updateLight(Light *light)
{
}

std::list<Light*> LightManager::computeAffectingLights(const Vector3f &position, int count)
{
}

}
