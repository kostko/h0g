/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "weapon.h"
#include "robot.h"
#include "motionstate.h"

// Storage
#include "storage/mesh.h"
#include "storage/compositemesh.h"
#include "storage/texture.h"
#include "storage/shader.h"
#include "storage/sound.h"
#include "entities/entity.h"
#include "context.h"

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
    m_targetAngle(0.0)
{
}

Weapon::~Weapon()
{
  delete m_body;
  delete m_sceneNode;
  delete m_motionState;
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
  if (m_targetAngle - 0.1 < 0.0)
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
      
      // TODO Attach a particle emitter for missile's exhaust
      /*m_exhaust = new ParticleEmitter("Exhaust", 50, m_sceneNode, scene);
      m_exhaust->setTexture(storage->get<Texture>("/Textures/particle"));
      m_exhaust->setPosition(0, 0, 0);
      m_exhaust->setOrientation(
        AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
        AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
      );
      m_exhaust->setGravity(0.0, 10.0, 0.0);
      m_exhaust->setBounds(1.0, 1.0, 1.0);
      
      std::vector<Vector3f> colors;
      colors.push_back(Vector3f(1, 0.6, 0));
      colors.push_back(Vector3f(0.9, 0.55, 0));
      colors.push_back(Vector3f(1, 0.65, 0));
      m_exhaust->setColors(colors);
      
      m_exhaust->init();
      m_sceneNode->attachChild(m_exhaust);*/
      
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
    }
    
    /**
     * Class destructor.
     */
    ~Rocket()
    {
      delete m_motionState;
      delete m_sceneNode;
      //delete m_exhaust;
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
      //m_exhaust->start();
      //m_exhaust->show(true);
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
      
      // TODO render explosion
      
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
    ParticleEmitter *m_exhaust;
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
  btDynamicsWorld *world = context->getDynamicsWorld();
  Scene *scene = context->scene();
  Storage *storage = context->storage();
  
  CompositeMesh *weaponMesh = storage->get<CompositeMesh>("/Models/rocketlauncher");
  Texture *texture = storage->get<Texture>("/Textures/rocketlauncher");
  Shader *shader = storage->get<Shader>("/Shaders/general");
  
  // Get robot's dimensions to properly attach the weapon
  Vector3f robotHalfSize = robot->getSceneNode()->getLocalBoundingBox().getHalfSize();
  Vector3f hs = weaponMesh->getAABB().getHalfSize();
  
  // Create the weapon's scene node
  m_sceneNode = scene->createNodeFromStorage(weaponMesh);
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
  m_world = world;
}

void RocketLauncher::equip()
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

void RocketLauncher::unequip()
{
  m_scene->detachNode(m_sceneNode);
  m_world->removeConstraint(m_constraint);
  m_world->removeRigidBody(m_body);
  delete m_constraint;
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
  : Weapon(robot, context)
{
  btDynamicsWorld *world = context->getDynamicsWorld();
  Scene *scene = context->scene();
  Storage *storage = context->storage();
  
  CompositeMesh *weaponMesh = storage->get<CompositeMesh>("/Models/gravitygun");
  Texture *texture = storage->get<Texture>("/Textures/gravitygun");
  Shader *shader = storage->get<Shader>("/Shaders/general");
  
  // Get robot's dimensions to properly attach the weapon
  Vector3f robotHalfSize = robot->getSceneNode()->getLocalBoundingBox().getHalfSize();
  Vector3f hs = weaponMesh->getAABB().getHalfSize();
  
  // Create the weapon's scene node
  m_sceneNode = scene->createNodeFromStorage(weaponMesh);
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
  m_world = world;
}

void GravityGun::equip()
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

void GravityGun::unequip()
{
  m_scene->detachNode(m_sceneNode);
  m_world->removeConstraint(m_constraint);
  m_world->removeRigidBody(m_body);
  delete m_constraint;
}

void GravityGun::fire()
{
  btTransform transform = m_body->getCenterOfMassTransform();
  btVector3 targetVec = transform.getBasis() * btVector3(0.0, -1.0, 0.0);
  targetVec.normalize();
  
  // TODO
}

