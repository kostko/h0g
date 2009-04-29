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

SceneNode::SceneNode(const std::string &name, SceneNode *parent, Scene *scene)
  : m_parent(parent),
    m_name(name),
    m_scene(scene),
    m_worldTransform(Matrix4f::Identity()),
    m_localPosition(0, 0, 0),
    m_worldPosition(0, 0, 0),
    m_localOrientation(Quaternionf::Identity()),
    m_worldOrientation(Quaternionf::Identity()),
    m_needParentUpdate(false),
    m_needChildUpdate(false),
    m_parentNotified(false),
    m_inheritOrientation(true),
    m_dirty(false)
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

void SceneNode::updateSceneFromParent()
{
  m_scene = m_parent->m_scene;
  
  BOOST_FOREACH(Child child, m_children) {
    child.second->updateSceneFromParent();
  }
}

void SceneNode::attachChild(SceneNode *child)
{
  if (m_children.find(child->getName()) != m_children.end())
    return;
  
  m_children[child->getName()] = child;
  child->m_scene = m_scene;
  child->m_parent = this;
  child->m_parentNotified = false;
  child->needUpdate();
  child->updateSceneFromParent();
}

void SceneNode::detachChild(SceneNode *child)
{
  m_children.erase(child->getName());
  needUpdate();
}
    
void SceneNode::detachChild(const std::string &name)
{
  m_children.erase(name);
  needUpdate();
}
    
void SceneNode::setPosition(float x, float y, float z)
{
  setPosition(Vector3f(x, y, z));
}

void SceneNode::setPosition(Vector3f pos)
{
  m_localPosition = pos;
  needUpdate();
}

void SceneNode::setOrientation(float w, float x, float y, float z)
{
  setOrientation(Quaternionf(w, x, y, z));
}

void SceneNode::setOrientation(Quaternionf orientation)
{
  m_localOrientation = orientation;
  needUpdate();
}

void SceneNode::needUpdate(bool updateParent)
{
  m_needParentUpdate = true;
  m_needChildUpdate = true;
  m_dirty = true;
  
  if (m_parent && (!m_parentNotified || updateParent)) {
    m_parent->requestUpdate(this, updateParent);
    m_parentNotified = true;
  }
  
  m_childrenToUpdate.clear();
}

void SceneNode::requestUpdate(SceneNode *child, bool updateParent)
{
  // Since we are going to update everything, just return
  if (m_needChildUpdate)
    return;
  
  m_childrenToUpdate.push_back(child);
  if (m_parent && (!m_parentNotified || updateParent)) {
    m_parent->requestUpdate(this, updateParent);
    m_parentNotified = true;
  }
}

void SceneNode::update(bool updateChildren, bool parentHasChanged)
{
  m_parentNotified = false;
  
  if (!updateChildren && !m_needParentUpdate && !m_needChildUpdate && !parentHasChanged)
    return;
  
  if (m_needParentUpdate || parentHasChanged) {
    // Update transformations from parent
    if (m_parent) {
      // Check for orientation inheritance
      if (m_inheritOrientation) {
        m_worldOrientation = m_parent->m_worldOrientation * m_localOrientation;
      } else {
        m_worldOrientation = m_localOrientation;
      }
      
      m_worldPosition = m_parent->m_worldOrientation * m_localPosition;
      m_worldPosition += m_parent->m_worldPosition;
    } else {
      m_worldPosition = m_localPosition;
      m_worldOrientation = m_localOrientation;
    }
    
    // Update transformations
    m_worldTransform.setIdentity();
    m_worldTransform.translate(m_worldPosition);
    m_worldTransform.rotate(m_worldOrientation);
    
    m_needParentUpdate = false;
    m_dirty = false;
  }
  
  if (m_needChildUpdate || parentHasChanged) {
    // We are updating all the children
    BOOST_FOREACH(Child child, m_children) {
      child.second->update(true, true);
    }
  } else {
    // Only update queued children
    BOOST_FOREACH(SceneNode *child, m_childrenToUpdate) {
      child->update(true, false);
    }
  }
  
  m_childrenToUpdate.clear();
  m_needChildUpdate = false;
  
  // Update boundaries
  updateBounds();
}

void SceneNode::updateBounds()
{
  m_worldBounds = m_localBounds;
  m_worldBounds.transformAffine(m_worldTransform);
  
  // Merge all child nodes
  BOOST_FOREACH(Child child, m_children) {
    m_worldBounds.merge(child.second->m_worldBounds);
  }
}

void SceneNode::setInheritOrientation(bool value)
{
  m_inheritOrientation = value;
}

void SceneNode::render()
{
}

}
