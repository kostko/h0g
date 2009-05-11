/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "weapon.h"
#include "robot.h"

using namespace IID;

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
  // TODO
  /*CompositeMesh *weaponMesh = storage->get<CompositeMesh>("/Models/rocketlauncher");
  Texture *texture = storage->get<Texture>("/Textures/rocketlauncher");
  Shader *shader = storage->get<Shader>("/Shaders/general");
  
  // Create the robot's scene node
  m_sceneNode = scene->createNodeFromStorage(weaponMesh);
  m_sceneNode->setShader(shader);
  m_sceneNode->setTexture(texture);
  m_sceneNode->setPosition(0., 0., 0.);
  m_sceneNode->setOrientation(
    AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
    AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
  robot->getSceneNode()->attachNode(m_sceneNode);
  scene->update();*/
}

void RocketLauncher::fire()
{
}
