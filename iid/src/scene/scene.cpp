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
#include "scene/camera.h"
#include "renderer/statebatcher.h"
#include "storage/storage.h"
#include "storage/mesh.h"
#include "context.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#define USE_FRUSTUM_CULLING

namespace IID {

Scene::Scene(Context *context)
  : m_context(context),
    m_root(new SceneNode("root", 0, this)),
    m_stateBatcher(new StateBatcher(this)),
    m_viewTransform(new ViewTransform()),
    m_octree(new Octree()),
    m_camera(0)
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
  // When no camera is currently active, show nothing at all
  if (!m_camera)
    return;
  
  // First render the camera to apply a proper view transform
  m_camera->render();
  
  // Then perform view frustum culling and add all nodes to the state
  // batcher render queue for rendering
#ifdef USE_FRUSTUM_CULLING
  m_octree->walkAndCull(m_camera, m_stateBatcher);
#else
  std::list<SceneNode*> n;
  n.push_back(m_root);
  
  while (!n.empty()) {
    SceneNode *node = n.front();
    n.pop_front();
    node->render(m_stateBatcher);
  
    typedef std::pair<std::string, SceneNode*> Child;
    BOOST_FOREACH(Child child, node->m_children) {
      n.push_back(child.second);
    }
  }
#endif
}

void Scene::attachNode(SceneNode *node)
{
  m_root->attachChild(node);
}

SceneNode *Scene::createNodeFromStorage(Item *mesh, const std::string &name)
{
  if (mesh->getType() == "Mesh") {
    // A simple mesh will result in creation of a single rendrable node
    // that will be returned
    RendrableNode *node = new RendrableNode(name.empty() ? mesh->getId() : name);
    node->setMesh(static_cast<Mesh*>(mesh));
    return node;
  } else if (mesh->getType() == "CompositeMesh") {
    // A composite mesh will result in creation of a grouping SceneNode
    // that will contain all child nodes with relative position hints
    SceneNode *group = new SceneNode(name.empty() ? mesh->getId() : name);
    
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
    
    return group;
  } else {
    return 0;
  }
}

void Scene::setCamera(Camera *camera)
{
  m_camera = camera;
  
  // Update camera configuration to match current perspective settings
  m_camera->setCamInternals(
    m_perspective.fov,
    m_perspective.ratio,
    m_perspective.near,
    m_perspective.far
  );
}

void Scene::setPerspective(float fov, float ratio, float near, float far)
{
  m_perspective.fov = fov;
  m_perspective.ratio = ratio;
  m_perspective.near = near;
  m_perspective.far = far;
  
  // If a camera is set, update it as well
  if (m_camera)
    m_camera->setCamInternals(fov, ratio, near, far);
  
  // Update the projection matrix
  Matrix4f m;
  float f = 1. / std::tan(0.5 * M_PI * fov / 180.);
  float nfd = near - far;
  m << f / ratio, 0, 0,                  0,
       0,         f, 0,                  0,
       0,         0, (far + near) / nfd, (2 * far * near) / nfd,
       0,         0, -1,                 0;
  
  m_context->driver()->applyProjectionTransform(m.data());
}

const ScenePerspective &Scene::getPerspective() const
{
  return m_perspective;
}

}
