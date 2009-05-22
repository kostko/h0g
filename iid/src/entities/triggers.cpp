/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "entities/triggers.h"
#include "entities/entity.h"

namespace IID {

TriggerManager::TriggerManager()
{
}
    
void TriggerManager::dispatchCollisionEvent(btCollisionObject *objectA, btCollisionObject *objectB)
{
  // Get user pointers to determine entities
  Entity *entityA = static_cast<Entity*>(objectA->getUserPointer());
  Entity *entityB = static_cast<Entity*>(objectB->getUserPointer());
  
  if (!entityA && !entityB)
    return;
  
  if (entityA && entityA->isEnabled() && (entityB || entityA->wantsEnvironmentCollisions())) {
    entityA->trigger(entityB);
  }
  
  if (entityB && entityB->isEnabled() && (entityA || entityB->wantsEnvironmentCollisions())) {
    entityB->trigger(entityA);
  }
}

void TriggerManager::registerEntity(Entity *entity)
{
}

void TriggerManager::unregisterEntity(Entity *entity)
{
}

}
