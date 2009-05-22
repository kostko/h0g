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

class Entity;

/**
 * A trigger manager is responsible for dispatching collision
 * events to subscribed entities.
 */
class TriggerManager {
public:
    /**
     * Class constructor.
     */
    TriggerManager();
    
    /**
     * Dispatches collision events to entities.
     *
     * @param objectA First collision object
     * @param objectB Second collision object
     */
    void dispatchCollisionEvent(btCollisionObject *objectA, btCollisionObject *objectB);
    
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
};

}

#endif
