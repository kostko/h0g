/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "crate.h"
#include "motionstate.h"

// Storage
#include "storage/mesh.h"
#include "storage/compositemesh.h"
#include "storage/texture.h"
#include "storage/shader.h"
#include "storage/sound.h"

// Scene
#include "scene/scene.h"

#include <boost/lexical_cast.hpp>

using namespace IID;

int Crate::m_crateId = 0;

Crate::Crate(const Vector3f &pos, btDynamicsWorld *world, IID::Scene *scene, IID::Storage *storage)
{
  Shader *shader = storage->get<Shader>("/Shaders/general");
  CompositeMesh *crateMesh = storage->get<CompositeMesh>("/Models/crate");
  Texture *crateTexture = storage->get<Texture>("/Textures/crate");
  
  // Create scene node
  m_sceneNode = scene->createNodeFromStorage(crateMesh, "crate" + boost::lexical_cast<std::string>(m_crateId++));
  m_sceneNode->setTexture(crateTexture);
  m_sceneNode->setShader(shader);
  m_sceneNode->setPosition(pos);
  scene->attachNode(m_sceneNode);
  scene->update();
  
  // Create the crate's physical shape and body
  Vector3f hs = crateMesh->getAABB().getHalfSize();
  m_shape = new btBoxShape(btVector3(hs[0], hs[1], hs[2]));
  float mass = 10.0;
  btVector3 localInertia(0, 0, 0);
  m_shape->calculateLocalInertia(mass, localInertia);
  
  // Create motion state
  m_motionState = new EntityMotionState(m_sceneNode);
  
  // Construct the rigid body that holds the weapon
  btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_motionState, m_shape, localInertia);
  m_body = new btRigidBody(cInfo);
  world->addRigidBody(m_body);
}
    
Crate::~Crate()
{
}
