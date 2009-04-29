/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/scene.h"
#include "scene/node.h"
#include "scene/rendrable.h"
#include "scene/viewtransform.h"
#include "scene/octree.h"
#include "renderer/statebatcher.h"
#include "storage/storage.h"
#include "storage/mesh.h"
#include "context.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace IID {

Scene::Scene(Context *context)
  : m_context(context),
    m_root(new SceneNode("root", 0, this)),
    m_stateBatcher(new StateBatcher(this)),
    m_viewTransform(new ViewTransform()),
    m_octree(new Octree())
{
}

Scene::~Scene()
{
  delete m_octree;
  delete m_root;
  delete m_viewTransform;
  delete m_stateBatcher;
}

void Scene::update()
{
  // All actual updating is handled by individual nodes. When their position
  // and orientation change, they signal parents that they will have to be
  // updated. So all we have to do is initiate updates from root. If nothing
  // has actually moved, this will do nothing at all.
  m_root->update(true, false);
}

void Scene::render()
{
  // FIXME this is a stupid renderer and should be replaced by an octree
  //       that performs frustum culling
  std::list<SceneNode*> n;
  n.push_back(m_root);
  
  while (!n.empty()) {
    SceneNode *node = n.front();
    n.pop_front();
    node->render();
    
    typedef std::pair<std::string, SceneNode*> Child;
    BOOST_FOREACH(Child child, node->m_children) {
      n.push_back(child.second);
    }
  }
}

void Scene::attachNode(SceneNode *node)
{
  m_root->attachChild(node);
}

SceneNode *Scene::createNodeFromStorage(Item *mesh)
{
  if (mesh->getType() == "Mesh") {
    // A simple mesh will result in creation of a single rendrable node
    // that will be returned
    RendrableNode *node = new RendrableNode(mesh->getId());
    node->setMesh(static_cast<Mesh*>(mesh));
    return node;
  } else if (mesh->getType() == "CompositeMesh") {
    // A composite mesh will result in creation of a grouping SceneNode
    // that will contain all child nodes with relative position hints
    SceneNode *group = new SceneNode(mesh->getId());
    
    typedef std::pair<std::string, Item*> Child;
    BOOST_FOREACH(Child child, *mesh->children()) {
      if (child.second->getType() != "Mesh")
        continue;
      
      RendrableNode *node = new RendrableNode(child.second->getId(), group);
      node->setMesh(static_cast<Mesh*>(child.second));
      
      if (child.second->hasAttribute("Mesh.RelativePosition")) {
        StringMap relative = child.second->getAttribute("Mesh.RelativePosition");
        node->setPosition(
          boost::lexical_cast<float>(relative["x"]),
          boost::lexical_cast<float>(relative["y"]),
          boost::lexical_cast<float>(relative["z"])
        );
      }
    }
  } else {
    return 0;
  }
}

}
