/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "motionstate.h"

using namespace IID;

EntityMotionState::EntityMotionState(const btTransform &initial, SceneNode *node)
  : m_sceneNode(node),
    m_initial(initial),
    m_useInitial(true)
{
}

EntityMotionState::EntityMotionState(SceneNode *node)
  : m_sceneNode(node),
    m_useInitial(false)
{
}

void EntityMotionState::setInitialTransform(const btTransform &initial)
{
  m_initial = initial;
  m_useInitial = true;
}

void EntityMotionState::getWorldTransform(btTransform &worldTransform) const
{
  if (m_useInitial)
    worldTransform = m_initial;
  else
    worldTransform.setFromOpenGLMatrix(m_sceneNode->worldTransform().data());
}

void EntityMotionState::setWorldTransform(const btTransform &worldTransform)
{
  btQuaternion rot = worldTransform.getRotation();
  btVector3 pos = worldTransform.getOrigin();
  m_sceneNode->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
  m_sceneNode->setPosition(pos.x(), pos.y(), pos.z());
}
