/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */

#include "toad.h"
#include "robot.h"
#include "motionstate.h"
#include "ai.h"

// IID includes
#include "context.h"
#include "storage/mesh.h"
#include "storage/compositemesh.h"
#include "storage/texture.h"
#include "storage/shader.h"
#include "drivers/openal.h"

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>

using namespace IID;

Toad::Toad(const Vector3f &pos, IID::Context *context, Robot *target, AIController *ai)
  : Enemy("toad", context, target, ai),
    m_damage(0),
    m_hopInterval(0.8 + rand()/(float)RAND_MAX),
    m_hopDeltaTime(0),
    m_croakInterval(2.0 + rand()/(float)RAND_MAX),
    m_croakDeltaTime(0),
    m_death(false)
{
  btDynamicsWorld *world = context->getDynamicsWorld();
  Scene *scene = context->scene();
  Storage *storage = context->storage();
  
  CompositeMesh *frogMesh = storage->get<CompositeMesh>("/Models/toad");
  //Texture *frogTexture = storage->get<Texture>("/Textures/toad");
  Shader *shader = storage->get<Shader>("/Shaders/material");
  
  m_sceneNode = scene->createNodeFromStorage(frogMesh, "Toad" + boost::lexical_cast<std::string>(m_enemyId++));
  m_sceneNode->setShader(shader);
  //m_sceneNode->setTexture(frogTexture);
  m_sceneNode->setPosition(pos);
  m_sceneNode->setOrientation(
    AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
    AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
  scene->attachNode(m_sceneNode);
  scene->update();
  
  // Create the toad's physical shape and body
  Vector3f hs = frogMesh->getAABB().getHalfSize();
  btConvexHullShape *shape = new btConvexHullShape();
  frogMesh->getConvexHullShape(shape);
  m_shape = shape;

  float mass = 10.0;
  btVector3 localInertia(0, 0, 0);
  m_shape->calculateLocalInertia(mass, localInertia);
  
  // Create motion state
  btTransform startTransform;
  startTransform.setFromOpenGLMatrix(m_sceneNode->worldTransform().data());
  m_motionState = new EntityMotionState(startTransform, m_sceneNode);
  
  // Construct the rigid body that holds the toad
  btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_motionState, m_shape, localInertia);
  m_body = new btRigidBody(cInfo);
  m_body->setAngularFactor(0.0);
  m_body->setActivationState(DISABLE_DEACTIVATION);
  m_world->addRigidBody(m_body);
  m_world->addAction(this);
  
  // AI setup
  m_mapBody = new MapBody(m_sceneNode, MapBody::Dynamic);
  // m_ai->addMapBody(m_mapBody);
  
  // Prepare some sounds and a sound player for croaking
  m_sounds["croak"] = storage->get<Sound>("/Sounds/croak");
  m_sceneNode->registerSoundPlayer("CroakPlayer", new OpenALPlayer());
  m_sceneNode->getSoundPlayer("CroakPlayer")->queue(m_sounds["croak"]);
  
  // Entity setup
  setCollisionObject(m_body);
}

Toad::~Toad()
{
}

void Toad::updateAction(btCollisionWorld *world, btScalar dt)
{
  btTransform transform = m_body->getCenterOfMassTransform();
  
  if (m_death) {
    // The toad has croaked
    transform.setRotation(transform.getRotation() * btQuaternion(btVector3(0.0, 1.0, 0.0), M_PI));
    m_body->applyImpulse(btVector3(0, 70, 0), btVector3(0,0,0));
    
    // Do this "animation" only once
    m_death = false;
  }
  
  m_hopDeltaTime += dt;
  m_croakDeltaTime += dt;
  
  // Animate only live toads
  if (m_hopDeltaTime > m_hopInterval && isAlive()) 
  {
    // Correct the toad's direction
    Vector3f d = m_ai->getDirection(m_mapBody, m_target->getMapBody());
    
    // Ignore the y coordinate, as we only need the 2D projection
    btVector3 dir(d.x(), 0.0f, d.z());
    
    btVector3 forwardVec = transform.getBasis() * btVector3(0.0, -1.0, 0.0);
    
    // Get the needed correction angle
    float fi = forwardVec.angle(dir);
    
    // This if statement is a hack.
    // Prevents the case of setting the rotation when fi is NaN.
    if (fi >= 0) {
      transform.setRotation(transform.getRotation() * btQuaternion(btVector3(0.0, 0.0, 1.0), fi));
    }
    
    m_body->applyImpulse(btVector3(-15*forwardVec.getX(), 35, -15*forwardVec.getZ()), btVector3(0,0,0));
    m_hopDeltaTime = 0;
  }
  
  // See it is time to make an annoying croak
  if (m_croakDeltaTime > m_croakInterval && isAlive())
  {
    m_sceneNode->getSoundPlayer("CroakPlayer")->play();
    m_croakDeltaTime = 0;
  }
  
  // Update transformations
  m_body->getMotionState()->setWorldTransform(transform);
  m_body->setCenterOfMassTransform(transform);
}

void Toad::debugDraw(btIDebugDraw *drawer)
{
}

void Toad::setPosition(Vector3f position)
{
  m_sceneNode->setPosition(position);
  m_scene->update();
}

void Toad::takeDamage(double dmg)
{
  m_life -= dmg;
  if (m_life <= 0) {
    // The toad is dead. 
    m_death = true;
  }
}
