/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef HOG_TOAD_H
#define HOG_TOAD_H

#include "enemy.h"
#include "storage/storage.h"
#include "storage/sound.h"

// Bullet dynamics
#include <btBulletDynamicsCommon.h>

/**
 * Represents a toad. R2's most dreaded enemy.
 */
class Toad : public Enemy {
public:
    /**
    * Class constructor.
    */
    Toad(const Vector3f &pos, btDynamicsWorld *world, IID::Scene *scene, IID::Storage *storage, 
         Robot *target, AIController *ai);
    
    /**
    * Class destructor.
    */
    ~Toad();
    
    /**
     * This method gets called on every step of the simulation and is responsible for
     * maintaining control over the robot.
     *
     * @param world World instance
     * @param dt Delta time
     */
    void updateAction(btCollisionWorld *world, btScalar dt);
    
    /**
     * Debug drawer (required by btActionInterface).
     */
    void debugDraw(btIDebugDraw *drawer);
    
    /**
     * Set the toad's position.
     *
     * @param position Position vector
     */
    void setPosition(Vector3f position);
    
    /**
    * The toad takes 'dmg' of damage.
    */
    void takeDamage(double dmg);
private:
    // How much damage the toad deals
    double m_damage;
    
    // Sounds
    boost::unordered_map<std::string, IID::Sound*> m_sounds;
    
    // Number of updates between hops
    double m_hopInterval;
    double m_hopDeltaTime;
};

#endif
