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
     *
     * @param robot Robot that is assigned this weapon
     * @param context Engine context
     */
    Weapon(Robot *robot, IID::Context *context);
    
    /**
     * Class destructor.
     */
    virtual ~Weapon();
    
    /**
     * Performs basic weapon setup. This should work for standard weapons, if
     * you need to customize this, just copy the code and use it in your
     * weapon's constructor.
     *
     * @param modelPath Weapon mesh storage path
     * @param texturePath Weapon texture storage path
     */
    void setupWeapon(const std::string &modelPath, const std::string &texturePath);
    
    /**
     * Equips this weapon.
     */
    virtual void equip();
    
    /**
     * Unequips this weapon.
     */
    virtual void unequip();
    
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
    btCollisionShape *m_shape;
    btHingeConstraint *m_constraint;
    EntityMotionState *m_motionState;
    float m_targetAngle;
    Robot *m_robot;
    IID::Scene *m_scene;
    IID::Storage *m_storage;
    IID::Context *m_context;
    btDynamicsWorld *m_world;
};

/**
 * A rocketlauncher weapon.
 */
class RocketLauncher : public Weapon {
friend class Rocket;
public:
    /**
     * Class constructor.
     *
     * @param robot Robot that carries the weapon
     * @param context Engine context
     */
    RocketLauncher(Robot *robot, IID::Context *context);
    
    /**
     * Fires the weapon.
     */
    void fire();
};

/**
 * A gravity gun weapon.
 */
class GravityGun : public Weapon, public btActionInterface {
public:
    /**
     * Class constructor.
     *
     * @param robot Robot that carries the weapon
     * @param context Engine context
     */
    GravityGun(Robot *robot, IID::Context *context);
    
    /**
     * Equips this weapon.
     */
    void equip();
    
    /**
     * Unequips this weapon.
     */
    void unequip();
    
    /**
     * Fires the weapon.
     */
    void fire();
    
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
private:
    btRigidBody *m_pickedBody;
    btPoint2PointConstraint *m_pickConstraint;
    btVector3 m_lastPickPos;
    float m_lastPickDist;
};

#endif
