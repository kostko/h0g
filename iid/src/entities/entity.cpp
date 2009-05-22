/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "entities/entity.h"
#include "entities/triggers.h"
#include "context.h"

namespace IID {

Entity::Entity(Context *context, const std::string &type)
  : m_context(context),
    m_type(type),
    m_enabled(true),
    m_environmentCollisions(false),
    m_collisionObject(0)
{
}

void Entity::setCollisionObject(btCollisionObject *object)
{
  if (object) {
    m_collisionObject = object;
    m_collisionObject->setUserPointer(this);
    m_context->getTriggerManager()->registerEntity(this);
  } else {
    m_collisionObject->setUserPointer(0);
    m_collisionObject = 0;
    m_context->getTriggerManager()->unregisterEntity(this);
  }
}

void Entity::setEnabled(bool value)
{
  m_enabled = value;
}

void Entity::setWantEnvironmentCollisions(bool value)
{
  m_environmentCollisions = value;
}

}
