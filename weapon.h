/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef HOG_WEAPONS_H
#define HOG_WEAPONS_H

// IID includes
#include "scene/node.h"
#include "storage/storage.h"

// Bullet dynamics
#include <btBulletDynamicsCommon.h>

class EntityMotionState;
class Robot;

/**
 * An abstract weapon class.
 */
class Weapon {
public:
    /**
     * Class constructor.
     */
    Weapon();
    
    /**
     * Class destructor.
     */
    virtual ~Weapon();
    
    /**
     * Move target vector in upwards direction.
     */
    virtual void up();
    
    /**
     * Move target vector in downwards direction.
     */
    virtual void down();
    
    /**
     * Fires the weapon.
     */
    virtual void fire() = 0;
protected:
    IID::SceneNode *m_sceneNode;
    btRigidBody *m_body;
    EntityMotionState *m_motionState;
    btVector3 m_targetVector;
};

/**
 * A rocketlauncher weapon.
 */
class RocketLauncher : public Weapon {
public:
    /**
     * Class constructor.
     *
     * @param robot Robot that carries the weapon
     * @param world Dynamics world
     * @param scene Scene instance
     * @param storage Storage
     */
    RocketLauncher(Robot *robot, btDynamicsWorld *world, IID::Scene *scene, IID::Storage *storage);
    
    /**
     * Fires the weapon.
     */
    void fire();
};

#endif
