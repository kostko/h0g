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
  Vector3f robotHalfSize = robot->getSceneNode()->getBoundingBox().getHalfSize();
  Vector3f halfSize = weaponMesh->getAABB().getHalfSize();
  
  // Create the robot's scene node
  m_sceneNode = scene->createNodeFromStorage(weaponMesh);
  m_sceneNode->setShader(shader);
  m_sceneNode->setTexture(texture);
  m_sceneNode->setPosition(-robotHalfSize[0], 0.0, 0.);
  m_sceneNode->setOrientation(
    AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
    AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
}

void RocketLauncher::equip()
{
  m_robot->getSceneNode()->attachChild(m_sceneNode);
  m_scene->update();
}

void RocketLauncher::unequip()
{
  m_robot->getSceneNode()->detachChild(m_sceneNode);
}

void RocketLauncher::fire()
{
}
