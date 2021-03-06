/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_STORAGE_COMPOSITEMESH_H
#define IID_STORAGE_COMPOSITEMESH_H

#include "storage.h"
#include "scene/aabb.h"

class btConvexHullShape;

namespace IID {

/**
 * This class is just a simple container for other Mesh instances that are part
 * of the same imported file. It behaves identically to the standard Item, the
 * only difference is the type 'CompositeMesh'.
 */
class CompositeMesh : public Item {
public:
    /**
     * Class constructor.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param path Item path
     * @param parent Optional parent instance
     */
    CompositeMesh(Storage *storage, const std::string &itemId = "", Item *parent = 0);
    
    /**
     * Specifies mesh boundaries.
     *
     * @param min Minimum AABB corner
     * @param max Maximum AABB corner
     */
    void setBounds(const Vector3f &min, const Vector3f &max);
    
    /**
     * Returns the axis aligned bounding box of this 3D mesh.
     */
    AxisAlignedBox getAABB() const { return m_boundAABB; }
    
    /**
     * Builds a convex hull shape out of this composite mesh.
     *
     * @param shape Destination hull shape
     */
    void getConvexHullShape(btConvexHullShape *shape);
private:
    // Boundaries
    AxisAlignedBox m_boundAABB;
};

/**
 * A factory of CompositeMesh items.
 */
class CompositeMeshFactory : public ItemFactory {
public:
    /**
     * Create a new CompositeMesh storage item.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param parent Parent item
     * @return A valid Mesh item
     */
    Item *create(Storage *storage, const std::string &itemId, Item *parent);
};

}

#endif
