/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_H
#define IID_SCENE_H

#include "globals.h"

namespace IID {

class Context;
class SceneNode;
class StateBatcher;
class ViewTransform;
class Item;
class Octree;

/**
 * Represents the 3D scene.
 */
class Scene {
public:
    /**
     * Class constructor.
     *
     * @param context IID engine context
     */
    Scene(Context *context);
    
    /**
     * Class destructor.
     */
    ~Scene();
    
    /**
     * Returns the current engine context.
     */
    Context *context() { return m_context; }
    
    /**
     * Returns the state batcher instance.
     */
    StateBatcher *stateBatcher() const { return m_stateBatcher; }
    
    /**
     * Returns the view transformation instance.
     */
    ViewTransform *viewTransform() const { return m_viewTransform; }
    
    /**
     * Returns the octree associated with this scene.
     */
    Octree *getOctree() const { return m_octree; }
    
    /**
     * Perform scene graph updates that have been previously queued
     * when nodes were allowed to move. Must be called after dynamics
     * simulation step.
     */
    void update();
    
    /**
     * Renders all visible objects on the scene.
     */
    void render();
    
    /**
     * Creates a scene node hierarchy from the specified storage item
     * and returns the root node. When an invalid storage item is
     * specified, this method returns NULL.
     *
     * @param mesh Storage item that must be a valid Mesh/CompositeMesh
     * @return Root scene node or NULL
     */
    SceneNode *createNodeFromStorage(Item *mesh);
    
    /**
     * Attaches a node directly under the root of the scene graph.
     *
     * @param node Node to attach
     */
    void attachNode(SceneNode *node);
private:
    Context *m_context;
    
    // Scene view transformation
    ViewTransform *m_viewTransform;

    // Root scene node
    SceneNode *m_root;
    
    // Render batcher
    StateBatcher *m_stateBatcher;
    
    // Octree
    Octree *m_octree;
};

}

#endif
