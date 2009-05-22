/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/node.h"
#include "scene/scene.h"
#include "scene/octree.h"
#include "scene/lightmanager.h"
#include "drivers/openal.h"

#include <boost/foreach.hpp>

namespace IID {

typedef std::pair<std::string, SceneNode*> Child;
typedef std::pair<std::string, Player*> PlayerPair;

SceneNode::SceneNode(const std::string &name, SceneNode *parent)
  : m_parent(parent),
    m_name(name),
    m_scene(0),
    m_lightManager(0),
    m_worldTransform(Matrix4f::Identity()),
    m_localPosition(0, 0, 0),
    m_worldPosition(0, 0, 0),
    m_localOrientation(Quaternionf::Identity()),
    m_worldOrientation(Quaternionf::Identity()),
    m_needParentUpdate(false),
    m_needChildUpdate(false),
    m_parentNotified(false),
    m_inheritOrientation(true),
    m_dirty(false),
    m_octreeNode(0),
    m_octree(0),
    m_static(false)
{
  if (m_parent) {
    m_parent->attachChild(this);
  }
}

SceneNode::~SceneNode()
{
  BOOST_FOREACH(Child child, m_children) {
    delete child.second;
  }
  BOOST_FOREACH(PlayerPair player, m_players) {
    delete player.second;
  }
}

void SceneNode::updateSceneFromParent()
{
  if (!m_parent->m_scene)
    return;
  
  m_scene = m_parent->m_scene;
  m_octree = m_scene->getOctree();
  m_lightManager = m_scene->getLightManager();
  
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

void SceneNode::clearConnectionToScene()
{
  if (m_octree)
    m_octree->removeNode(this);
  
  m_scene = 0;
  m_lightManager = 0;
  
  BOOST_FOREACH(Child child, m_children) {
    child.second->clearConnectionToScene();
  }
}

void SceneNode::detachChild(SceneNode *child)
{
  child->clearConnectionToScene();
  child->m_parent = 0;
  
  m_children.erase(child->getName());
  
  // Erase from update pending list if present to prevent resurecting the item on update
  std::list<SceneNode*>::iterator it = std::find(m_childrenToUpdate.begin(), m_childrenToUpdate.end(), child);
  if (it != m_childrenToUpdate.end())
    m_childrenToUpdate.erase(it);
}

SceneNode *SceneNode::child(const std::string &name)
{
  if (m_children.find(name) == m_children.end())
    return 0;
  
  return m_children[name];
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

Vector3f SceneNode::getWorldPosition() const
{
  return m_worldPosition;
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
    
    // Perform node-specific updates
    updateNodeSpecific();
    
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
}

void SceneNode::updateNodeSpecific()
{
  m_worldBounds = m_localBounds;
  m_worldBounds.transformAffine(m_worldTransform);
  
  // Update octree here
  m_octree->updateNode(this);
  
  // Update all registered player's position
  BOOST_FOREACH(PlayerPair player, m_players) {
    player.second->setPosition(m_worldPosition.data());
  }
}

void SceneNode::setOctreeNode(OctreeNode *node)
{
  m_octreeNode = node;
}

void SceneNode::setInheritOrientation(bool value)
{
  m_inheritOrientation = value;
}

void SceneNode::setTexture(Texture *texture)
{
  // Apply texture to all child nodes
  BOOST_FOREACH(Child child, m_children) {
    child.second->setTexture(texture);
  }
}

void SceneNode::setShader(Shader *shader)
{
  // Apply shader to all child nodes
  BOOST_FOREACH(Child child, m_children) {
    child.second->setShader(shader);
  }
}

void SceneNode::setMaterial(Material *material)
{
  // Apply material to all child nodes
  BOOST_FOREACH(Child child, m_children) {
    child.second->setMaterial(material);
  }
}

void SceneNode::setStaticHint(bool value)
{
  m_static = value;
  
  // Apply static flag to all child nodes
  BOOST_FOREACH(Child child, m_children) {
    child.second->setStaticHint(value);
  }
}

Player *SceneNode::getSoundPlayer(const std::string &playerName)
{
  return m_players[playerName];
}

void SceneNode::registerSoundPlayer(const std::string &playerName, Player *player)
{
  m_players[playerName] = player;
}

void SceneNode::batchStaticGeometry(btTriangleIndexVertexArray *triangles)
{
  BOOST_FOREACH(Child child, m_children) {
    child.second->batchStaticGeometry(triangles);
  }
}

void SceneNode::separateNodeFromParent()
{
  if (!m_scene)
    return;
  
  // First ensure that we have up-to-date world transformations
  m_scene->update();
  
  // Now switch local position/orientation with world ones
  m_localPosition = m_worldPosition;
  m_localOrientation = m_worldOrientation;
  
  // Detach from parent and attach to root
  Scene *scene = m_scene;
  m_parent->detachChild(this);
  scene->attachNode(this);
}

void SceneNode::render(StateBatcher *batcher)
{
}

}
