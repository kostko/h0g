/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "weapon.h"
#include "robot.h"
#include "motionstate.h"
#include "collisiongroups.h"

// Storage
#include "storage/mesh.h"
#include "storage/compositemesh.h"
#include "storage/texture.h"
#include "storage/shader.h"
#include "storage/sound.h"
#include "entities/entity.h"
#include "context.h"

// IID includes
#include "scene/particles.h"

#include <boost/lexical_cast.hpp>

using namespace IID;

Weapon::Weapon(Robot *robot, Context *context)
  : m_sceneNode(0),
    m_body(0),
    m_motionState(0),
    m_robot(robot),
    m_scene(context->scene()),
    m_storage(context->storage()),
    m_context(context),
    m_world(context->getDynamicsWorld()),
    m_targetAngle(0.0)
{
}

Weapon::~Weapon()
{
  delete m_body;
  delete m_sceneNode;
  delete m_motionState;
}

void Weapon::setupWeapon(const std::string &modelPath, const std::string &texturePath)
{
  CompositeMesh *weaponMesh = m_storage->get<CompositeMesh>(modelPath);
  Texture *texture = m_storage->get<Texture>(texturePath);
  Shader *shader = m_storage->get<Shader>("/Shaders/general");
  
  // Get robot's dimensions to properly attach the weapon
  Vector3f robotHalfSize = m_robot->getSceneNode()->getLocalBoundingBox().getHalfSize();
  Vector3f hs = weaponMesh->getAABB().getHalfSize();
  
  // Create the weapon's scene node
  m_sceneNode = m_scene->createNodeFromStorage(weaponMesh);
  m_sceneNode->setShader(shader);
  m_sceneNode->setTexture(texture);
  m_sceneNode->setPosition(-robotHalfSize[0] -hs[0] - 0.010, 0.0, 0.);
  m_sceneNode->setOrientation(
    AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
    AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
  
  // Create the weapon's physical shape and body
  m_shape = new btBoxShape(btVector3(hs[0], hs[1], hs[2]));
  float mass = 30.0;
  btVector3 localInertia(0, 0, 0);
  m_shape->calculateLocalInertia(mass, localInertia);
  
  // Create motion state
  m_motionState = new EntityMotionState(m_sceneNode);
  
  // Construct the rigid body that holds the weapon
  btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_motionState, m_shape, localInertia);
  m_body = new btRigidBody(cInfo);
}

void Weapon::equip()
{
  btTransform transform = m_robot->getBody()->getCenterOfMassTransform();
  m_body->setCenterOfMassTransform(transform);
  m_motionState->setWorldTransform(transform);
  
  // Create a hinge constraint for weapon rotation
  Vector3f robotHalfSize = m_robot->getSceneNode()->getLocalBoundingBox().getHalfSize();
  btTransform localA, localB;
  localA.setIdentity();
  localB.setIdentity();
  localA.getBasis().setEulerZYX(0.0, M_PI_2, 0.0);
  localA.setOrigin(btVector3(-robotHalfSize[0], 0.0, 0.2));
  localB.getBasis().setEulerZYX(0.0, M_PI_2, 0.0);
  localB.setOrigin(btVector3(0.015, 0.2, 0.0));
  
  m_constraint = new btHingeConstraint(*m_robot->getBody(), *m_body, localA, localB);
  m_constraint->setLimit(m_targetAngle, m_targetAngle + 0.1);
  
  m_scene->attachNode(m_sceneNode);
  m_scene->update();
  m_world->addRigidBody(m_body);
  m_world->addConstraint(m_constraint, true);
}

void Weapon::unequip()
{
  m_scene->detachNode(m_sceneNode);
  m_world->removeConstraint(m_constraint);
  m_world->removeRigidBody(m_body);
  delete m_constraint;
}

void Weapon::up()
{
  if (m_targetAngle >= M_PI*0.25)
    return;
  
  m_targetAngle += 0.1;
  m_constraint->setLimit(m_targetAngle, m_targetAngle + 0.1);
}

void Weapon::down()
{
  if (m_targetAngle - 0.1 < -0.3)
    return;
  
  m_targetAngle -= 0.1;
  m_constraint->setLimit(m_targetAngle, m_targetAngle + 0.1);
}

/**
 * This class represents a rocket missile fired by the rocket launcher.
 */
class Rocket : public Entity {
public:
    /**
     * Class constructor.
     */
    Rocket(RocketLauncher *launcher, Context *context)
      : Entity(context, "rocket")
    {
      m_world = context->getDynamicsWorld();
      m_scene = context->scene();
      Storage *storage = context->storage();
      
      // Create a new missile node and body
      CompositeMesh *missileMesh = storage->get<CompositeMesh>("/Models/rocket");
      Shader *shader = storage->get<Shader>("/Shaders/material");
      
      Vector3f whs = launcher->m_sceneNode->getLocalBoundingBox().getHalfSize();
      Vector3f hs = missileMesh->getAABB().getHalfSize();
      
      // Create the missile's scene node
      m_sceneNode = m_scene->createNodeFromStorage(missileMesh, "rocket" + boost::lexical_cast<std::string>(m_rocketId++));
      m_sceneNode->setShader(shader);
      m_sceneNode->setPosition(0.0, -whs[1] - 0.010, 0.0);
      m_sceneNode->setOrientation(
        AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
        AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
      );
      launcher->m_sceneNode->attachChild(m_sceneNode);
      m_sceneNode->separateNodeFromParent();
      
      // Create the missile's physical shape and body
      m_shape = new btBoxShape(btVector3(hs[0], hs[1], hs[2]));
      float mass = 1.0;
      btVector3 localInertia(0, 0, 0);
      m_shape->calculateLocalInertia(mass, localInertia);
      
      // Create motion state
      m_motionState = new EntityMotionState(m_sceneNode);
      
      // Construct the rigid body that holds the weapon
      btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_motionState, m_shape, localInertia);
      m_body = new btRigidBody(cInfo);
      m_world->addRigidBody(m_body);
      
      // Entity setup
      setCollisionObject(m_body);
      setWantEnvironmentCollisions(true);
      setTriggerFilter(Entity::CollisionTrigger);
      
      // Some eye-candy for when a rocket launches
      m_exhaust = new Explosion("Exhaust " + boost::lexical_cast<std::string>(m_rocketId), 20, m_sceneNode);
      m_exhaust->setTexture(storage->get<Texture>("/Textures/particle"));
      m_exhaust->setPosition(0, 0, 0);
      m_exhaust->setOrientation(
        AngleAxisf(0.0*M_PI, Vector3f::UnitX()) *
        AngleAxisf(0.5*M_PI, Vector3f::UnitY()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
      );
      m_exhaust->setGravity(0.0, 0.0, 0.0);
      m_exhaust->setBounds(0.3, 0.3, 0.3);
      
      std::vector<Vector3f> colors;
      colors.push_back(Vector3f(1, 0.9, 0));
      colors.push_back(Vector3f(0.9, 0.7, 0));
      colors.push_back(Vector3f(1, 0.9, 0));
      m_exhaust->setColors(colors);
      m_exhaust->boostSpeed(5.0);
      
      m_exhaust->init();
      m_sceneNode->attachChild(m_exhaust);
      
      // Prepare the explosion instance
      m_boom = new Explosion("Explosion "+ boost::lexical_cast<std::string>(m_rocketId), 200, m_sceneNode);
      m_boom->setTexture(storage->get<Texture>("/Textures/particle"));
      m_boom->setPosition(0, 0, 0);
      m_boom->setOrientation(
        AngleAxisf(0.0*M_PI, Vector3f::UnitX()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitY()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
      );
      m_boom->setGravity(0.0, -0.3, 0.0);
      m_boom->setBounds(3.0, 3.0, 3.0);
      
      colors.clear();
      colors.push_back(Vector3f(0.9, 0.1, 0));
      colors.push_back(Vector3f(0.9, 0.15, 0));
      colors.push_back(Vector3f(1, 0.2, 0));
      m_boom->setColors(colors);
      m_exhaust->boostSpeed(1000.0);
      
      m_boom->init();
      m_scene->attachNode(m_boom);
    }
    
    /**
     * Class destructor.
     */
    ~Rocket()
    {
      delete m_motionState;
      delete m_sceneNode;
      delete m_exhaust;
      delete m_boom;
      delete m_body;
      delete m_shape;
    }
    
    /**
     * Launches this rocket.
     */
    void launch(const btVector3 &vector)
    {
      m_body->applyCentralImpulse(vector * 10.0);
      m_armTimer.reset();
      m_exhaust->start();
      m_exhaust->show(true);
    }
    
    void trigger(Entity *entity, TriggerType type)
    {
      // Check if the weapon is already armed (it arms after 1 second); if it
      // impacts a toad it arms immediately
      if (m_armTimer.getTimeMilliseconds() < 1000 && (!entity || entity->getType() != "toad"))
        return;
      
      // If we collide with another rocket even after arming, ignore this
      if (entity && entity->getType() == "rocket")
        return;
      
      // Render explosion
      m_boom->setPosition(m_sceneNode->getWorldPosition());
      m_boom->start();
      m_boom->show(true);
      
      // Remove rocket from the scene
      setCollisionObject(0);
      m_world->removeRigidBody(m_body);
      m_scene->detachNode(m_sceneNode);
      
      // We must not delete this object here as it could cause crashes in further
      // trigger processing
      Entity::deferredDelete();
    }
private:
    static int m_rocketId;
    Scene *m_scene;
    SceneNode *m_sceneNode;
    Explosion *m_exhaust;
    Explosion *m_boom;
    btCollisionShape *m_shape;
    EntityMotionState *m_motionState;
    btRigidBody *m_body;
    btDynamicsWorld *m_world;
    btClock m_armTimer;
};

int Rocket::m_rocketId = 0;

RocketLauncher::RocketLauncher(Robot *robot, Context *context)
  : Weapon(robot, context)
{
  // Just use the default setup option
  setupWeapon("/Models/rocketlauncher", "/Textures/rocketlauncher");
}

void RocketLauncher::fire()
{
  // TODO handle reload time
  
  btTransform transform = m_body->getCenterOfMassTransform();
  btVector3 targetVec = transform.getBasis() * btVector3(0.0, -1.0, 0.0);
  targetVec.normalize();
  
  Rocket *rocket = new Rocket(this, m_context);
  rocket->launch(targetVec);
}

GravityGun::GravityGun(Robot *robot, Context *context)
  : Weapon(robot, context),
    m_pickedBody(0),
    m_pickConstraint(0)
{
  // Just use the default setup option
  setupWeapon("/Models/gravitygun", "/Textures/gravitygun");
}

void GravityGun::equip()
{
  Weapon::equip();
  m_world->addAction(this);
}

void GravityGun::unequip()
{
  if (m_pickedBody) {
    // Just remove the constraint without firing the body
    m_world->removeConstraint(m_pickConstraint);
    delete m_pickConstraint;
    
    m_pickConstraint = 0;
    m_pickedBody->forceActivationState(ACTIVE_TAG);
    m_pickedBody->setDeactivationTime(0.0);
    m_pickedBody = 0;
  }
  
  Weapon::unequip();
}

void GravityGun::fire()
{
  if (m_pickedBody) {
    m_world->removeConstraint(m_pickConstraint);
    delete m_pickConstraint;
    
    // Shoot the object in proper direction
    btTransform transform = m_body->getCenterOfMassTransform();
    btVector3 targetVec = transform.getBasis() * btVector3(0.0, -1.0, 0.0);
    targetVec.normalize();
    m_pickedBody->applyCentralImpulse(targetVec * 50.0);
    
    m_pickConstraint = 0;
    m_pickedBody->forceActivationState(ACTIVE_TAG);
    m_pickedBody->setDeactivationTime(0.0);
    m_pickedBody = 0;
  } else {
    btTransform transform = m_body->getCenterOfMassTransform();
    btVector3 targetVec = transform.getBasis() * btVector3(0.0, -1.0, 0.0);
    targetVec.normalize();
    
    // This depends on weapon's range
    btVector3 rayTo = transform.getOrigin() + targetVec * 2.0;
    targetVec += transform.getOrigin();
    
    // Perform a ray test (only movable objects are affected)
    btCollisionWorld::ClosestRayResultCallback rayCallback(targetVec, rayTo);
    rayCallback.m_collisionFilterMask = HogCollisionGroup::MovableObject;
    
    m_world->rayTest(targetVec, rayTo, rayCallback);
    if (rayCallback.hasHit()) {
      // Got an object to grab
      m_pickedBody = btRigidBody::upcast(rayCallback.m_collisionObject);
      if (m_pickedBody) {
        m_pickedBody->setActivationState(DISABLE_DEACTIVATION);
        
        m_pickConstraint = new btPoint2PointConstraint(*m_pickedBody, btVector3(0, 0, 0));
        m_pickConstraint->m_setting.m_impulseClamp = 30.0;
        m_pickConstraint->m_setting.m_tau = 0.1;
        m_world->addConstraint(m_pickConstraint);
        
        m_lastPickDist = (rayCallback.m_hitPointWorld - targetVec).length();
      }
    }
  }
}

void GravityGun::updateAction(btCollisionWorld *world, btScalar dt)
{
  if (m_pickedBody) {
    // Move pick constraint to move the body
    btTransform transform = m_body->getCenterOfMassTransform();
    btVector3 targetVec = transform.getBasis() * btVector3(0.0, -1.0, 0.0);
    targetVec.normalize();
    
    // Ensure that we don't embed this thing into a wall
    btVector3 rayTo = transform.getOrigin() + targetVec * m_lastPickDist;
    targetVec += transform.getOrigin();
    
    btCollisionWorld::ClosestRayResultCallback rayCallback(targetVec, rayTo);
    rayCallback.m_collisionFilterMask = btBroadphaseProxy::StaticFilter;
    
    m_world->rayTest(targetVec, rayTo, rayCallback);
    if (rayCallback.hasHit()) {
      m_lastPickDist *= (rayCallback.m_closestHitFraction - 0.2);
      rayTo = transform.getOrigin() + targetVec * m_lastPickDist;
    } else if (m_lastPickDist < 3.0) {
      m_lastPickDist += dt * 0.5;
    }
    
    m_pickConstraint->setPivotB(rayTo);
  }
}

