/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "door.h"
#include "motionstate.h"

// IID includes
#include "storage/storage.h"
#include "storage/compositemesh.h"
#include "storage/texture.h"
#include "storage/shader.h"
#include "scene/scene.h"
#include "context.h"

using namespace IID;

SlidingDoor::SlidingDoor(IID::Context *context)
  : Entity(context, "door"),
    m_state(Closed)
{
  btDynamicsWorld *world = context->getDynamicsWorld();
  Scene *scene = context->scene();
  Storage *storage = context->storage();
  
  CompositeMesh *door = storage->get<CompositeMesh>("/Models/door");
  Texture *doorTexture = storage->get<Texture>("/Textures/door");
  Shader *shader = storage->get<Shader>("/Shaders/general");
  
  // Create the door scene node
  m_sceneNode = scene->createNodeFromStorage(door);
  m_sceneNode->setShader(shader);
  m_sceneNode->setTexture(doorTexture);
  m_sceneNode->setPosition(40.086, 1.925, 0.0);
  scene->attachNode(m_sceneNode);
  
  // Create the kinematic object
  Vector3f hs = door->getAABB().getHalfSize();
  m_shape = new btBoxShape(btVector3(hs[0], hs[1], hs[2]));
  m_motionState = new EntityMotionState(m_sceneNode);
  m_slideLimit = hs[2]*2.0 + 0.01;
  m_slideStart = 0.0;
  
  btRigidBody::btRigidBodyConstructionInfo cInfo(0.0, m_motionState, m_shape, btVector3(0, 0, 0));
  m_body = new btRigidBody(cInfo);
  m_body->setCollisionFlags(m_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
  world->addRigidBody(m_body);
  world->addAction(this);
  m_world = world;
  
  // Entity setup
  setCollisionObject(m_body);
}

void SlidingDoor::updateAction(btCollisionWorld *world, btScalar dt)
{
  switch (m_state) {
    case Opening: {
      Vector3f pos = m_sceneNode->getPosition();
      pos -= Vector3f(0.0, 0.0, 0.40) * dt;
      
      if (m_slideStart - pos[2] < m_slideLimit) {
        m_sceneNode->setPosition(pos);
      } else {
        m_state = Open;
        m_body->forceActivationState(WANTS_DEACTIVATION);
      }
      break;
    }
    default: break;
  }
}

void SlidingDoor::open()
{
  if (m_state != Closed && m_state != Closing)
    return;
  
  m_state = Opening;
  m_body->setActivationState(DISABLE_DEACTIVATION);
}

void SlidingDoor::close()
{
  if (m_state != Open && m_state != Opening)
    return;
  
  m_state = Closing;
  m_body->setActivationState(DISABLE_DEACTIVATION);
}

void SlidingDoor::trigger(Entity *entity, TriggerType type)
{
  if (type == PickTrigger) {
    std::cout << "clicked the door" << std::endl;
    open();
  }
}
