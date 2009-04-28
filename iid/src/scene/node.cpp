/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/node.h"

#include <boost/foreach.hpp>

namespace IID {

typedef std::pair<std::string, SceneNode*> Child;

SceneNode::SceneNode(SceneNode *parent, Scene *scene)
  : m_parent(parent),
    m_scene(scene),
    m_dirty(true)
{
  if (m_parent) {
    m_parent->attachChild(this);
    m_scene = m_parent->m_scene;
  }
}

SceneNode::~SceneNode()
{
  BOOST_FOREACH(Child child, m_children) {
    delete child.second;
  }
}

void SceneNode::setName(const std::string &name)
{
  if (m_parent) {
    m_parent->detachChild(this);
    m_name = name;
    m_parent->attachChild(this);
  } else {
    m_name = name;
  }
}
    
void SceneNode::attachChild(SceneNode *child)
{
  if (m_children.find(child->getName()) != m_children.end())
    return;
  
  m_children[child->getName()] = child;
  child->m_scene = m_scene;
}

void SceneNode::detachChild(SceneNode *child)
{
  m_children.erase(child->getName());
}
    
void SceneNode::detachChild(const std::string &name)
{
  m_children.erase(name);
}
    
void SceneNode::setPosition(float x, float y, float z)
{
  setPosition(Vector3f(x, y, z));
}

void SceneNode::setPosition(Vector3f pos)
{
  m_position = pos;
  m_dirty = true;
}

void SceneNode::setOrientation(float w, float x, float y, float z)
{
  setOrientation(Quaternionf(w, x, y, z));
}

void SceneNode::setOrientation(Quaternionf orientation)
{
  m_orientation = orientation;
  m_dirty = true;
}

void SceneNode::update()
{
  if (!m_dirty)
    return;
  
  m_localTransform.setIdentity();
  m_localTransform.rotate(m_orientation);
  m_worldBounds = m_localBounds;
  m_worldBounds.transform(m_localTransform);
  m_localTransform.translate(m_position);
  
  if (!m_parent) {
    m_worldTransform = m_localTransform;
    m_worldPosition = m_position;
  } else {
    m_worldTransform = m_parent->m_worldTransform * m_localTransform;
    m_worldPosition = m_parent->m_worldPosition + m_position;
  }
  
  m_worldBounds.translate(m_worldPosition);
  
  // Update all child nodes
  BOOST_FOREACH(Child child, m_children) {
    child.second->update();
    m_worldBounds.extend(child.second->m_worldBounds);
  }
  
  m_dirty = false;
}

void SceneNode::setInheritOrientation(bool value)
{
  m_inheritOrientation = value;
}

void SceneNode::render()
{
}

}
