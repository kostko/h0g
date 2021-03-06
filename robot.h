/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef HOG_ROBOT_H
#define HOG_ROBOT_H

// IID includes
#include "scene/scene.h"
#include "scene/node.h"
#include "storage/storage.h"
#include "storage/sound.h"
#include "entities/entity.h"

// Particle system
#include "scene/particles.h"

// Bullet dynamics
#include <btBulletDynamicsCommon.h>

#include <vector>

class EntityMotionState;
class Weapon;
class AIController;
class MapBody;

/**
 * This class describes active character state.
 */
class CharacterState {
public:
    enum Movement {
      FORWARD = 1,
      BACKWARD = 2,
      LEFT = 4,
      RIGHT = 8
    };
    
    // Movement bitfield
    int movement;
    
    // Jump
    bool jump;
    
    // Hover
    bool hover;
    
    /**
     * Class constructor.
     */
    CharacterState()
      : movement(0),
        jump(false),
        hover(false)
    {}
};

/**
 * The main character controller.
 */
class Robot : public IID::Entity, public btActionInterface {
public:
    /**
     * Class constructor.
     */
    Robot(IID::Context *context, IID::Camera *camera, AIController *ai);
    
    /**
     * This method gets called on every step of the simulation and is responsible for
     * maintaining control over the robot.
     *
     * @param world World instance
     * @param dt Delta time
     */
    void updateAction(btCollisionWorld *world, btScalar dt);
    
    /**
     * Returns the distance between the robot and the floor. This is done
     * via ray tests.
     */
    float getFloorDistance();
    
    /**
     * Debug drawer (required by btActionInterface).
     */
    void debugDraw(btIDebugDraw *drawer);
    
    /**
     * Enables or disables hover mode.
     */
    void hover();
    
    /**
     * Enables or disables forward-vector movement.
     */
    void forward(bool value);
    
    /**
     * Enables or disables backward-vector movement.
     */
    void backward(bool value);
    
    /**
     * Enables or disables left-vector movement.
     */
    void left(bool value);
    
    /**
     * Enables or disables right-vector movement.
     */
    void right(bool value);
    
    /**
     * Raise weapon target angle.
     */
    void weaponUp();
    
    /**
     * Lower weapon target angle.
     */
    void weaponDown();
    
    /**
     * Fires the weapon.
     */
    void weaponFire();
    
    /**
     * Returns the robot's body.
     */
    btRigidBody *getBody() const;
    
    /**
     * Returns the robot's scene node.
     */
    IID::SceneNode *getSceneNode() const;
    
    /**
     * Returns the robot's map body object.
     */
    MapBody *getMapBody() const;
    
    /**
     * Produce taunting sound.
     */
    void taunt();
    
    /**
     * Switches currently selected weapon.
     */
    void switchWeapon();
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
    
    // Player view
    IID::Camera *m_camera;
    
    // Hover stuff
    float m_hoverDeltaTime;
    
    // Controller stuff
    float m_turnAngle;
    float m_maxLinearVelocity;
    float m_walkVelocity;
    float m_turnVelocity;
    
    // Sounds
    boost::unordered_map<std::string, IID::Sound*> m_sounds;
    
    // Character state
    CharacterState m_state;
    
    // Weapon system
    std::vector<Weapon*> m_weaponInventory;
    int m_weaponIdx;
    Weapon *m_weapon;
    
    // Exhaust
    IID::ParticleEmitter *m_exhaust;
    
    // AI stuff
    AIController *m_ai;
    MapBody *m_mapBody;
};

#endif
