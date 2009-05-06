/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/compositemesh.h"

namespace IID {

CompositeMesh::CompositeMesh(Storage *storage, const std::string &itemId, Item *parent)
  : Item(storage, "CompositeMesh", itemId, "", parent)
{
}

void CompositeMesh::setBounds(const Vector3f &min, const Vector3f &max)
{
  m_boundAABB = AxisAlignedBox(min, max);
}

Item *CompositeMeshFactory::create(Storage *storage, const std::string &itemId, Item *parent)
{
  return new CompositeMesh(storage, itemId, parent);
}

}
