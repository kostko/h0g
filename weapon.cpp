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

using namespace IID;

Weapon::Weapon(Robot *robot, Scene *scene)
  : m_sceneNode(0),
    m_body(0),
    m_motionState(0),
    m_robot(robot),
    m_scene(scene)
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
  // TODO
}

void Weapon::down()
{
  // TODO
}

RocketLauncher::RocketLauncher(Robot *robot, btDynamicsWorld *world, Scene *scene, Storage *storage)
  : Weapon(robot, scene)
{
  CompositeMesh *weaponMesh = storage->get<CompositeMesh>("/Models/rocketlauncher");
  Texture *texture = storage->get<Texture>("/Textures/rocketlauncher");
  Shader *shader = storage->get<Shader>("/Shaders/general");
  
  // Get robot's dimensions to properly attach the weapon
  Vector3f robotHalfSize = robot->getSceneNode()->getLocalBoundingBox().getHalfSize();
  Vector3f hs = weaponMesh->getAABB().getHalfSize();
  
  // Create the weapon's scene node
  m_sceneNode = scene->createNodeFromStorage(weaponMesh);
  m_sceneNode->setShader(shader);
  m_sceneNode->setTexture(texture); // 0.023
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
  
  // We have to disable deactivation (so we can control the body when it stops)
  btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_motionState, m_shape, localInertia);
  m_body = new btRigidBody(cInfo);
  m_body->setActivationState(DISABLE_DEACTIVATION);
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
  m_constraint->setLimit(0.0, M_PI*2.0);
  
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
}
