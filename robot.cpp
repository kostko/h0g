/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "robot.h"
#include "motionstate.h"
#include "weapon.h"

// Storage
#include "storage/mesh.h"
#include "storage/compositemesh.h"
#include "storage/texture.h"
#include "storage/shader.h"
#include "storage/sound.h"

// Audio support
#include "drivers/openal.h"

using namespace IID;

Robot::Robot(btDynamicsWorld *world, Scene *scene, Storage *storage)
  : m_weaponIdx(0),
    m_weapon(0)
{
  CompositeMesh *robotMesh = storage->get<CompositeMesh>("/Models/r2-d2");
  Shader *shader = storage->get<Shader>("/Shaders/material");
  
  // Create the robot's scene node
  m_sceneNode = scene->createNodeFromStorage(robotMesh);
  m_sceneNode->setShader(shader);
  m_sceneNode->setPosition(-1., 0., -8.25);
  m_sceneNode->setOrientation(
    AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
    AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
  
  m_sceneNode->registerSoundPlayer("TauntPlayer", new OpenALPlayer());
  m_sceneNode->registerSoundPlayer("ThrustersPlayer", new OpenALPlayer());
  scene->attachNode(m_sceneNode);
  scene->update();
  
  // Create the robot's physical shape and body
  Vector3f hs = robotMesh->getAABB().getHalfSize();
  m_shape = new btBoxShape(btVector3(hs[0], hs[1], hs[2]));
  float mass = 80.0;
  btVector3 localInertia(0, 0, 0);
  m_shape->calculateLocalInertia(mass, localInertia);
  
  btTransform startTransform;
  startTransform.setFromOpenGLMatrix(m_sceneNode->worldTransform().data());
  m_motionState = new EntityMotionState(startTransform, m_sceneNode);
  
  // We have to disable deactivation (so we can control the body when it stops) and set
  // the angular factor to zero, so our robot can't roll over
  btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_motionState, m_shape, localInertia);
  m_body = new btRigidBody(cInfo);
  m_body->setActivationState(DISABLE_DEACTIVATION);
  m_body->setAngularFactor(0.0);
  world->addRigidBody(m_body);
  world->addAction(this);
  m_world = world;
  
  // Setup some attributes
  m_turnAngle = 0.0;
  m_maxLinearVelocity = 3.0;
  m_walkVelocity = 8.0;
  m_turnVelocity = 3.0;
  
  // Create our weapons
  m_weaponInventory.push_back(0);
  m_weaponInventory.push_back(new RocketLauncher(this, world, scene, storage));
  
  // Add some sounds from storage
  OpenALPlayer *player;
  m_sounds["Taunt"] = storage->get<Sound>("/Sounds/r2-sound1");
  m_sceneNode->getSoundPlayer("TauntPlayer")->queue(m_sounds["Taunt"]);
  
  m_sounds["Thrusters"] = storage->get<Sound>("/Sounds/r2-thrusters");
  m_sceneNode->getSoundPlayer("ThrustersPlayer")->setMode(Player::Looped);
  m_sceneNode->getSoundPlayer("ThrustersPlayer")->queue(m_sounds["Thrusters"]);
}

void Robot::updateAction(btCollisionWorld *world, btScalar dt)
{
  btTransform transform = m_body->getCenterOfMassTransform();
  
  // Handle rotation
  if (m_state.movement & CharacterState::LEFT)
    transform.setRotation(transform.getRotation() * btQuaternion(btVector3(0.0, 0.0, 1.0), dt * m_turnVelocity));
  
  if (m_state.movement & CharacterState::RIGHT)
    transform.setRotation(transform.getRotation() * btQuaternion(btVector3(0.0, 0.0, 1.0), -dt * m_turnVelocity));
  
  // Handle walking
  btVector3 linearVelocity = m_body->getLinearVelocity();
  float speed = linearVelocity.length();
  
  btVector3 forwardVec = transform.getBasis() * btVector3(0.0, -1.0, 0.0);
  btVector3 walkDirection(0.0, 0.0, 0.0);
  float walkSpeed = m_walkVelocity * dt;
  forwardVec.normalize();
  
  if (m_state.movement & CharacterState::FORWARD)
    walkDirection += forwardVec;
  if (m_state.movement & CharacterState::BACKWARD)
    walkDirection -= forwardVec;
  
  // Check for hover mode
  if (m_state.hover) {
    m_hoverDeltaTime += dt;
    if (m_hoverDeltaTime > 0.05) {
      // Check distance to floor, so we can perform height corrections
      float floorDistance = getFloorDistance();
      float mdist = 1.2;
      m_hoverDeltaTime = 0;
      
      if (floorDistance < mdist) {
        linearVelocity += (transform.getBasis() * btVector3(0.0, 0.0, 60.0)) * walkSpeed;
      }
    }
  }
  
  if (!(m_state.movement & (CharacterState::FORWARD | CharacterState::BACKWARD)) && !m_state.hover) {
    // Not being moved, slow down gradually
    linearVelocity *= 0.2;
    m_body->setLinearVelocity(linearVelocity);
  } else {
    // We are in transit
    if (m_state.hover) {
      linearVelocity *= 0.2;
      walkSpeed *= 5;
    }
    
    if (speed < m_maxLinearVelocity) {
      btVector3 velocity = linearVelocity + walkDirection * walkSpeed;
      m_body->setLinearVelocity(velocity);
    }
  }
  
  // Update transformations
  m_body->getMotionState()->setWorldTransform(transform);
  m_body->setCenterOfMassTransform(transform);
}

float Robot::getFloorDistance()
{
  class ClosestNotMe : public btCollisionWorld::ClosestRayResultCallback {
  public:
      ClosestNotMe(btRigidBody *me)
        : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)),
          m_me(me)
      {}
      
      btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace)
      {
        if (rayResult.m_collisionObject == m_me)
          return 1.0;
        
        return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
      }
  private:
      btRigidBody *m_me;
  };
  
  float distance = 5.0;
  btTransform transform = m_body->getCenterOfMassTransform();
  btVector3 source = transform.getOrigin();
  btVector3 target = source + distance * (transform.getBasis() * btVector3(0.0, 0.0, -1.0)).normalized();
  
  ClosestNotMe rayCallback(m_body);
  rayCallback.m_closestHitFraction = 1.0;
  m_world->rayTest(source, target, rayCallback);
  if (rayCallback.hasHit()) {
    return rayCallback.m_closestHitFraction * distance;
  } else {
    return distance;
  }
}

void Robot::debugDraw(btIDebugDraw *drawer)
{
}

void Robot::hover()
{
  OpenALPlayer *player = static_cast<OpenALPlayer*>( m_sceneNode->getSoundPlayer("ThrustersPlayer") );
  m_state.hover = !m_state.hover;
  
  if (m_state.hover) {
    player->play();
    m_hoverDeltaTime = 0.0;
  }
  else {
    player->stop();
  }
}

void Robot::forward(bool value)
{
  if (value)
    m_state.movement |= CharacterState::FORWARD;
  else
    m_state.movement &= ~CharacterState::FORWARD;
}

void Robot::backward(bool value)
{
  if (value)
    m_state.movement |= CharacterState::BACKWARD;
  else
    m_state.movement &= ~CharacterState::BACKWARD;
}

void Robot::left(bool value)
{
  if (value)
    m_state.movement |= CharacterState::LEFT;
  else
    m_state.movement &= ~CharacterState::LEFT;
}

void Robot::right(bool value)
{
  if (value)
    m_state.movement |= CharacterState::RIGHT;
  else
    m_state.movement &= ~CharacterState::RIGHT;
}

btRigidBody *Robot::getBody() const
{
  return m_body;
}

SceneNode *Robot::getSceneNode() const
{
  return m_sceneNode;
}

void Robot::taunt()
{
  OpenALPlayer *player = static_cast<OpenALPlayer*>( m_sceneNode->getSoundPlayer("TauntPlayer") );
  player->play();
}

void Robot::switchWeapon()
{
  m_weaponIdx = (m_weaponIdx + 1) % m_weaponInventory.size();
  
  if (m_weapon)
    m_weapon->unequip();
 
  m_weapon = m_weaponInventory[m_weaponIdx];
  if (m_weapon)
    m_weapon->equip();
}
