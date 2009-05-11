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

Weapon::Weapon()
  : m_sceneNode(0),
    m_body(0),
    m_motionState(0)
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
{
  CompositeMesh *weaponMesh = storage->get<CompositeMesh>("/Models/rocketlauncher");
  Texture *texture = storage->get<Texture>("/Textures/rocketlauncher");
  Shader *shader = storage->get<Shader>("/Shaders/general");
  
  // Get robot's dimensions to properly attach the weapon
  Vector3f halfSize = robot->getSceneNode()->getBoundingBox().getHalfSize();
  
  // Create the robot's scene node
  m_sceneNode = scene->createNodeFromStorage(weaponMesh);
  m_sceneNode->setShader(shader);
  m_sceneNode->setTexture(texture);
  m_sceneNode->setPosition(-halfSize[0], 0.0, 0.);
  m_sceneNode->setOrientation(
    AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
    AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
  robot->getSceneNode()->attachChild(m_sceneNode);
  scene->update();
}

void RocketLauncher::fire()
{
}
