/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_OCTREE_H
#define IID_SCENE_OCTREE_H

#include "globals.h"
#include "scene/aabb.h"

namespace IID {

class OctreeNode;
class SceneNode;

/**
 * A simple loose octree implementation for frustum culling purpuses.
 */
class Octree {
public:
    /**
     * Class constructor.
     */
    Octree();
    
    /**
     * Class destructor.
     */
    ~Octree();
    
    /**
     * Adds a node into this octree. Note that you should not need to call this
     * method manually as it gets called on SceneNode bound updates.
     *
     * @param node Scene node to add
     */
    void addNode(SceneNode *node, OctreeNode *oc = 0, int depth = 0);
    
    /**
     * Updates the specified scene node. This is called whenever the node's
     * bounds move.
     *
     * @param node Scene node to update
     */
    void updateNode(SceneNode *node);
    
    /**
     * Removes a node from the octree.
     *
     * @param node Scene node to remove
     */
    void removeNode(SceneNode *node);
protected:
    /**
     * Returns true when node's box is contained in the specified octree node
     * bounding box.
     *
     * @param node Node's bounding box
     * @param oc Octree node bounding box
     * @return True if box is contained inside oc, false otherwise
     */
    bool isNodeInBox(const AxisAlignedBox &node, const AxisAlignedBox &oc) const;
private:
    OctreeNode *m_root;
    int m_maxDepth;
};

}

#endif
