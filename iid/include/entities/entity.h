/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_ENTITIES_ENTITY_H
#define IID_ENTITIES_ENTITY_H

#include "globals.h"
#include <btBulletDynamicsCommon.h>

namespace IID {

class Context;

/**
 * Game entity that has a collision shape in the world and can respond
 * to triggers.
 */
class Entity {
friend class TriggerManager;
public:
    /**
     * Class constructor.
     *
     * @param context Engine context
     * @param type Entity type
     */
    Entity(Context *context, const std::string &type);
    
    /**
     * Returns the entity's identifier.
     */
    std::string getType() const { return m_type; }
    
    /**
     * Returns true if this entity is enabled. Disabled entites will not
     * be notified of collisions.
     */
    bool isEnabled() const { return m_enabled; }
    
    /**
     * Returns true if this entity wants triggers when it collides with
     * the environment.
     */
    bool wantsEnvironmentCollisions() const { return m_environmentCollisions; }
protected:
    /**
     * Sets up the collision object for this entity. This method must be
     * called if you want to subscribe to triggers.
     */
    void setCollisionObject(btCollisionObject *object);
    
    /**
     * Sets enabled state for this entity.
     *
     * @param value True for enabled, false for disabled
     */
    void setEnabled(bool value);
    
    /**
     * Sets trigger behaviour when entity collides with the environment
     * and not another entity. Default is to ignore environmental
     * collisions.
     *
     * @param value True to enable environmental collisions, false otherwise
     */
    void setWantEnvironmentCollisions(bool value);
    
    /**
     * This method gets called for collision triggers.
     *
     * @param entity Entity that has collided with this one (NULL when
     *               collision is with the environment)
     */
    virtual void trigger(Entity *entity) {};
private:
    Context *m_context;
    std::string m_type;
    bool m_environmentCollisions;
    bool m_enabled;
    btCollisionObject *m_collisionObject;
};

}

#endif
