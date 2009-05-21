/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef HOG_ENEMY_H
#define HOG_ENEMY_H

// IID includes
#include "scene/scene.h"
#include "scene/node.h"
#include "storage/storage.h"
#include "storage/sound.h"

// Bullet dynamics
#include <btBulletDynamicsCommon.h>

class Robot;
class EntityMotionState;
class AIController;
class MapBody;

/**
 * Represents an abstract enemy.
 */
class Enemy : public btActionInterface {
public:
    /**
     * Class constructor.
     */
    Enemy(btDynamicsWorld *world, IID::Scene *scene, Robot *target, AIController *ai);
    
    /**
     * Class constructor.
     */
    virtual ~Enemy() {};
    
    /**
     * This method gets called on every step of the simulation and is responsible for
     * maintaining control over the robot.
     *
     * @param world World instance
     * @param dt Delta time
     */
    virtual void updateAction(btCollisionWorld *world, btScalar dt) = 0;
    
    /**
     * Debug drawer (required by btActionInterface).
     */
    virtual void debugDraw(btIDebugDraw *drawer) = 0;
    
    /**
     * Set's the enemy's position in space.
     */
    virtual void setPosition(Vector3f position) = 0;
  
    /**
     * The enemy takes 'dmg' of damage.
     */
    virtual void takeDamage(double dmg) = 0;
  
    /**
     * Returns the enemy's map body object.
     */
    MapBody *getMapBody() const;
    
    /**
     * Is the enemy alive?
     */
    bool isAlive() const { return m_life > 0; }
protected:
    // Unique id
    static int m_enemyId;
    
    IID::SceneNode *m_sceneNode;
    IID::Scene *m_scene;
    btCollisionShape *m_shape;
    btRigidBody *m_body;
    EntityMotionState *m_motionState;
    btDynamicsWorld *m_world;
  
    // Enemy's life
    double m_life;
    
    // Geometrical properties
    Vector3f m_position;
    
    // Target instance
    Robot *m_target;
    
    // AI stuff
    AIController *m_ai;
    MapBody *m_mapBody;
    
    // Sounds
    boost::unordered_map<std::string, IID::Sound*> m_sounds;
};

#endif
