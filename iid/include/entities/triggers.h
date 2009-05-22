/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_ENTITIES_TRIGGERS_H
#define IID_ENTITIES_TRIGGERS_H

#include <boost/signal.hpp>
#include <btBulletDynamicsCommon.h>

namespace IID {

class Context;
class Entity;

/**
 * A trigger manager is responsible for dispatching collision and
 * pick events to subscribed entities.
 */
class TriggerManager {
public:
    /**
     * Class constructor.
     *
     * @param context Context instance
     */
    TriggerManager(Context *context);
    
    /**
     * Dispatches collision events to entities.
     *
     * @param objectA First collision object
     * @param objectB Second collision object
     */
    void dispatchCollisionEvent(btCollisionObject *objectA, btCollisionObject *objectB);
    
    /**
     * Dispatches pick events to entities.
     *
     * @param x X coordinate
     * @param y Y coordinate
     */
    void dispatchPickEvent(int x, int y);
    
    /**
     * Registers an entity.
     *
     * @param entity Entity instance
     */
    void registerEntity(Entity *entity);
    
    /**
     * Unregisters an entity.
     *
     * @param entity Entity instance
     */
    void unregisterEntity(Entity *entity);
    
    /**
     * Sets the entity that "owns" picking events.
     */
    void setPickOwner(Entity *entity);
    
    /**
     * Returns the pick owning entity.
     */
    Entity *getPickOwner() const { return m_pickOwner; }
private:
    Context *m_context;
    Entity *m_pickOwner;
};

}

#endif
