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
class Camera;

/**
 * A structure used to describe scene perspective.
 */
struct ScenePerspective {
    float fov;
    float ratio;
    float near;
    float far;
};

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
     * @param name Wanted scene node name
     * @return Root scene node or NULL
     */
    SceneNode *createNodeFromStorage(Item *mesh, const std::string &name = "");
    
    /**
     * Attaches a node directly under the root of the scene graph.
     *
     * @param node Node to attach
     */
    void attachNode(SceneNode *node);
    
    /**
     * Sets scene active camera.
     *
     * @param camera A valid Camera instance
     */
    void setCamera(Camera *camera);
    
    /**
     * Sets scene perspective.
     *
     * @param fov Field of view angle
     * @param ratio Viewport aspect ratio
     * @param near Distance from the viewer to the near clipping plane
     * @param far Distance from the viewer to the far clipping plane
     */
    void setPerspective(float fov, float ratio, float near, float far);
    
    /**
     * Returns current perspective properties.
     */
    const ScenePerspective &getPerspective() const;
    
    /**
     * Returns the currently active camera.
     */
    Camera *getCamera() const { return m_camera; }
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
    
    // Camera describing the current viewpoint
    Camera *m_camera;
    
    // Current perspective
    ScenePerspective m_perspective;
};

}

#endif
