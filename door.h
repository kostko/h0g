/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef HOG_DOOR_H
#define HOG_DOOR_H

// IID includes
#include "scene/scene.h"
#include "scene/node.h"
#include "entities/entity.h"

// Bullet dynamics
#include <btBulletDynamicsCommon.h>

class EntityMotionState;

/**
 * A sliding door.
 */
class SlidingDoor : public IID::Entity, public btActionInterface {
public:
    /**
     * Door state.
     */
    enum State {
      Closed,
      Opening,
      Open,
      Closing
    };
    
    /**
     * Class constructor.
     *
     * @param context Engine context
     */
    SlidingDoor(IID::Context *context);
    
    /**
     * This method gets called on every step of the simulation.
     *
     * @param world World instance
     * @param dt Delta time
     */
    void updateAction(btCollisionWorld *world, btScalar dt);

    /**
     * Debug drawer (required by btActionInterface).
     */
    void debugDraw(btIDebugDraw *drawer) {}
    
    /**
     * Starts the opening sequence.
     */
    void open();
    
    /**
     * Starts the closing sequence.
     */
    void close();
protected:
    /**
     * This method gets called for collision triggers.
     *
     * @param entity Entity that has collided with this one (NULL when
     *               collision is with the environment)
     * @param type Trigger type
     */
    void trigger(Entity *entity, TriggerType type);
private:
    IID::SceneNode *m_sceneNode;
    btCollisionShape *m_shape;
    btRigidBody *m_body;
    EntityMotionState *m_motionState;
    btDynamicsWorld *m_world;
    
    // Door state
    State m_state;
    float m_slideStart;
    float m_slideLimit;
};

#endif
