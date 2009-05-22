/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "entities/triggers.h"
#include "entities/entity.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "context.h"

#include <boost/foreach.hpp>

namespace IID {

TriggerManager::TriggerManager(Context *context)
  : m_context(context),
    m_pickOwner(0)
{
}
    
void TriggerManager::dispatchCollisionEvent(btCollisionObject *objectA, btCollisionObject *objectB)
{
  // Get user pointers to determine entities
  Entity *entityA = static_cast<Entity*>(objectA->getUserPointer());
  Entity *entityB = static_cast<Entity*>(objectB->getUserPointer());
  
  if (!entityA && !entityB)
    return;
  
  if (entityA && entityA->isEnabled() && entityA->acceptsTrigger(Entity::CollisionTrigger) &&
      (entityB || entityA->wantsEnvironmentCollisions())) {
    entityA->trigger(entityB, Entity::CollisionTrigger);
  }
  
  if (entityB && entityB->isEnabled() && entityB->acceptsTrigger(Entity::CollisionTrigger) &&
      (entityA || entityB->wantsEnvironmentCollisions())) {
    entityB->trigger(entityA, Entity::CollisionTrigger);
  }
}

void TriggerManager::dispatchPickEvent(int x, int y)
{
  if (!m_pickOwner)
    return;
  
  Camera *camera = m_context->scene()->getCamera();
  
  if (camera) {
    Vector3f ray = camera->rayTo(x, y);
    Vector3f eye = camera->getEyePosition();
    btCollisionWorld *world = m_context->getDynamicsWorld();
    btVector3 rayTo(ray.x(), ray.y(), ray.z());
    btVector3 rayFrom(eye.x(), eye.y(), eye.z());
    
    btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
    world->rayTest(rayFrom, rayTo, rayCallback);
    
    if (rayCallback.hasHit()) {
      Entity *entity = static_cast<Entity*>(rayCallback.m_collisionObject->getUserPointer());
      
      if (entity && entity->isEnabled() && entity->acceptsTrigger(Entity::PickTrigger)) {
        entity->trigger(m_pickOwner, Entity::PickTrigger);
      }
    }
  }
}

void TriggerManager::update()
{
  // Delete any entities pending deletion
  BOOST_FOREACH(Entity *entity, m_deferredDeleteQueue) {
    delete entity;
  }
  
  m_deferredDeleteQueue.clear();
}

void TriggerManager::registerEntity(Entity *entity)
{
}

void TriggerManager::unregisterEntity(Entity *entity)
{
}

void TriggerManager::deferredDeleteEntity(Entity *entity)
{
  m_deferredDeleteQueue.push_back(entity);
}

void TriggerManager::setPickOwner(Entity *entity)
{
  m_pickOwner = entity;
}

}
