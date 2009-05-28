/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/compositemesh.h"
#include "storage/mesh.h"

#include <bullet/BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>

#include <boost/foreach.hpp>

namespace IID {

CompositeMesh::CompositeMesh(Storage *storage, const std::string &itemId, Item *parent)
  : Item(storage, "CompositeMesh", itemId, "", parent)
{
}

void CompositeMesh::setBounds(const Vector3f &min, const Vector3f &max)
{
  m_boundAABB = AxisAlignedBox(min, max);
}

void CompositeMesh::getConvexHullShape(btConvexHullShape *shape)
{
  // Get convex shapes from submeshes
  btConvexHullShape *tmp = new btConvexHullShape();
  typedef std::pair<std::string, Item*> Child;
  BOOST_FOREACH(Child c, *children()) {
    static_cast<Mesh*>(c.second)->getConvexHullShape(tmp);
  }
  
  // Simplify the shape
  btShapeHull *hull = new btShapeHull(tmp);
  hull->buildHull(tmp->getMargin());
  for (int i = 0; i < hull->numVertices(); i++) {
    shape->addPoint(hull->getVertexPointer()[i]);
  }
  
  delete hull;
  delete tmp;
}

Item *CompositeMeshFactory::create(Storage *storage, const std::string &itemId, Item *parent)
{
  return new CompositeMesh(storage, itemId, parent);
}

}
