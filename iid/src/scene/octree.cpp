/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/octree.h"
#include "scene/node.h"
#include "scene/camera.h"
#include "renderer/statebatcher.h"

#include <boost/foreach.hpp>

namespace IID {

class OctreeNode {
friend class Octree;
public:
    OctreeNode(OctreeNode *parent = 0)
      : m_parent(parent),
        m_numNodes(0)
    {
      // Initialize children
      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
          for (int k = 0; k < 2; k++) {
            m_children[i][j][k] = 0;
          }
        }
      }
    }
    
    ~OctreeNode()
    {
      // Free children
      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
          for (int k = 0; k < 2; k++) {
            delete m_children[i][j][k];
          }
        }
      }
    }
    
    void getChildIndexes(const AxisAlignedBox &box, int *x, int *y, int *z)
    {
      Vector3f max = m_box.getMaximum();
      Vector3f min = box.getMinimum();
      Vector3f center = m_box.getCenter();
      Vector3f otherCenter = box.getCenter();
      
      if (otherCenter[0] > center[0])
        *x = 1;
      else
        *x = 0;
      
      if (otherCenter[1] > center[1])
        *y = 1;
      else
        *y = 0;
      
      if (otherCenter[2] > center[2])
        *z = 1;
      else
        *z = 0;
    }
    
    bool isTwiceSize(const AxisAlignedBox &box) const
    {
      if (box.isInfinite())
        return false;
      
      Vector3f size = box.getSize();
      return (
        (size[0] <= m_halfSize[0]) &&
        (size[1] <= m_halfSize[1]) &&
        (size[2] <= m_halfSize[2])
      );
    }
    
    void getCullBounds(AxisAlignedBox *box) const
    {
      box->setBounds(m_box.getMinimum() - m_halfSize, m_box.getMaximum() + m_halfSize);
    }
    
    void incrNodes()
    {
      m_numNodes++;
      if (m_parent)
        m_parent->incrNodes();
    }
    
    void decrNodes()
    {
      m_numNodes--;
      if (m_parent)
        m_parent->decrNodes();
    }
private:
    OctreeNode *m_parent;
    AxisAlignedBox m_box;
    int m_numNodes;
    Vector3f m_halfSize;
    
    // Children list
    OctreeNode *m_children[2][2][2];
    
    // Scene node list
    std::list<SceneNode*> m_nodes;
};

Octree::Octree()
  : m_root(new OctreeNode()),
    m_maxDepth(8)
{
  m_root->m_box = AxisAlignedBox(
    Vector3f(-1000, -1000, -1000),
    Vector3f(1000, 1000, 1000)
  );
  m_root->m_halfSize = m_root->m_box.getHalfSize();
}

Octree::~Octree()
{
  delete m_root;
}

void Octree::addNode(SceneNode *node, OctreeNode *oc, int depth)
{
  if (!oc)
    oc = m_root;
  
  const AxisAlignedBox &box = node->getBoundingBox();
  if (depth < m_maxDepth && oc->isTwiceSize(box)) {
    int x, y, z;
    oc->getChildIndexes(box, &x, &y, &z);
    
    if (oc->m_children[x][y][z] == 0) {
      // No child node available yet
      oc->m_children[x][y][z] = new OctreeNode(oc);
      const Vector3f &ocMin = oc->m_box.getMinimum();
      const Vector3f &ocMax = oc->m_box.getMaximum();
      const Vector3f &ocCenter = oc->m_box.getCenter();
      Vector3f min, max;
      
      if (x == 0) {
        min[0] = ocMin[0];
        max[0] = ocCenter[0];
      } else {
        min[0] = ocCenter[0];
        max[0] = ocMax[0];
      }
      
      if (y == 0) {
        min[1] = ocMin[1];
        max[1] = ocCenter[1];
      } else {
        min[1] = ocCenter[1];
        max[1] = ocMax[1];
      }
      
      if (z == 0) {
        min[2] = ocMin[2];
        max[2] = ocCenter[2];
      } else {
        min[2] = ocCenter[2];
        max[2] = ocMax[2];
      }
      
      oc->m_children[x][y][z]->m_box.setBounds(min, max);
      oc->m_children[x][y][z]->m_halfSize = (max - min) * 0.5;
    }
    
    addNode(node, oc->m_children[x][y][z], ++depth);
  } else {
    // Node should go into this node
    oc->incrNodes();
    oc->m_nodes.push_back(node);
    node->setOctreeNode(oc);
  }
}

bool Octree::isNodeInBox(const AxisAlignedBox &node, const AxisAlignedBox &oc) const
{
  if (node.isNull() || oc.isNull())
    return false;
  
  if (oc.isInfinite())
    return true;
  
  // Since this is a loose octree, only compare centers
  Vector3f center = node.getCenter();
  Vector3f min = oc.getMinimum();
  Vector3f max = oc.getMaximum();
  
  if (!((max.cwise() > center).all() && (min.cwise() < center).all()))
    return false;
  
  // Check to make sure node's AABB is not large enough to require being moved
  // up into the parent
  Vector3f ocSize = max - min;
  Vector3f nodeSize = node.getSize();
  return (nodeSize.cwise() < ocSize).all();
}

void Octree::updateNode(SceneNode *node)
{
  const AxisAlignedBox &box = node->getBoundingBox();
  if (box.isNull() || !m_root)
    return;
  
  OctreeNode *oc = node->getOctreeNode();
  if (!oc) {
    // No octree node assigned
    if (!isNodeInBox(box, m_root->m_box)) {
      m_root->incrNodes();
      m_root->m_nodes.push_back(node);
      node->setOctreeNode(m_root);
    } else {
      addNode(node, m_root);
    }
  } else if (!isNodeInBox(box, oc->m_box)) {
    // Node has moved outside its box
    removeNode(node);
    
    // If outside the octree, put into root node
    if (!isNodeInBox(box, m_root->m_box)) {
      m_root->incrNodes();
      m_root->m_nodes.push_back(node);
      node->setOctreeNode(m_root);
    } else {
      addNode(node, m_root);
    }
  }
}

void Octree::removeNode(SceneNode *node)
{
  if (m_root)
    return;
  
  OctreeNode *oc = node->getOctreeNode();
  if (oc) {
    // Erase the node from list (this list contains a small amount of nodes)
    oc->decrNodes();
    oc->m_nodes.erase(
      std::find(oc->m_nodes.begin(), oc->m_nodes.end(), node)
    );
    
    // Set octree node pointer to NULL
    node->setOctreeNode(0);
  }
}

void Octree::walkAndCull(Camera *camera, StateBatcher *batcher, OctreeNode *oc,
                         bool fullyVisible)
{
  if (!oc)
    oc = m_root;
  
  // If there are no nodes, don't go down there
  if (oc->m_numNodes == 0)
    return;
  
  Camera::Position pos = Camera::Outside;
  if (fullyVisible) {
    pos = Camera::Inside;
  } else if (oc == m_root) {
    pos = Camera::Intersect;
  } else {
    AxisAlignedBox box;
    oc->getCullBounds(&box);
    
    // First test the sphere, then test the full AABB
    pos = camera->containsSphere(box.getCenter(), box.getRadius());
    if (pos == Camera::Intersect)
      pos = camera->containsBox(box);
  }
  
  // If octree node is visible or it is a root node
  if (pos != Camera::Outside) {
    // Add objects to the render queue
    bool vis = true;
    
    BOOST_FOREACH(SceneNode *node, oc->m_nodes) {
      // If this octree node is partially visible, manually cull all
      // scene nodes attached to this level
      if (pos == Camera::Intersect) {
        AxisAlignedBox box = node->getBoundingBox();
        Camera::Position npos = camera->containsSphere(box.getCenter(), box.getRadius());
        if (npos == Camera::Intersect)
          npos = camera->containsBox(box);
        
        vis = (npos != Camera::Outside);
      }
      
      if (vis) {
        // Push the node to the render queue if it's visible
        node->render(batcher);
      }
    }
    
    // Recurse to other octree nodes
    OctreeNode *child;
    fullyVisible = (pos == Camera::Inside);
    
    if ((child = oc->m_children[0][0][0]) != 0)
      walkAndCull(camera, batcher, child, fullyVisible);
    
    if ((child = oc->m_children[0][0][1]) != 0)
      walkAndCull(camera, batcher, child, fullyVisible);
    
    if ((child = oc->m_children[0][1][0]) != 0)
      walkAndCull(camera, batcher, child, fullyVisible);
    
    if ((child = oc->m_children[0][1][1]) != 0)
      walkAndCull(camera, batcher, child, fullyVisible);
    
    if ((child = oc->m_children[1][0][0]) != 0)
      walkAndCull(camera, batcher, child, fullyVisible);
    
    if ((child = oc->m_children[1][0][1]) != 0)
      walkAndCull(camera, batcher, child, fullyVisible);
    
    if ((child = oc->m_children[1][1][0]) != 0)
      walkAndCull(camera, batcher, child, fullyVisible);
    
    if ((child = oc->m_children[1][1][1]) != 0)
      walkAndCull(camera, batcher, child, fullyVisible);
  }
}

}
