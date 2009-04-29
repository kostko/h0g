/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_NODE_H
#define IID_SCENE_NODE_H

#include "scene/aabb.h"
#include "globals.h"

#include <list>

namespace IID {

class Scene;

/**
 * A scene node represents an object in the scene graph.
 */
class SceneNode {
friend class Scene;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    /**
     * Class constructor.
     *
     * @param name Node name
     * @param parent Parent node
     * @param scene Scene instance
     */
    SceneNode(const std::string &name, SceneNode *parent = 0, Scene *scene = 0);
    
    /**
     * Class destructor.
     */
    virtual ~SceneNode();
    
    /**
     * Returns node's name.
     */
    std::string getName() const { return m_name; }
    
    /**
     * Attaches a child node to this node.
     *
     * @param child Child node to attach
     */
    void attachChild(SceneNode *child);
    
    /**
     * Detaches a child node from this node.
     *
     * @param child Child node to detach
     */
    void detachChild(SceneNode *child);
    
    /**
     * Detaches a child node from this node.
     *
     * @param name Child node name
     */
    void detachChild(const std::string &name);
    
    /**
     * Sets node's position relative to the parent.
     *
     * @param x X component
     * @param y Y component
     * @param z Z component
     */
    void setPosition(float x, float y, float z);
    
    /**
     * Sets node's position relative to the parent.
     *
     * @param pos Position vector
     */
    virtual void setPosition(Vector3f pos);
    
    /**
     * Sets node's orientation.
     *
     * @param w W coefficient
     * @param x X coefficient
     * @param y Y coefficient
     * @param z Z coefficient
     */
    void setOrientation(float w, float x, float y, float z);
    
    /**
     * Sets node's orientation.
     *
     * @param orientation A quaternion representing the orientation
     */
    virtual void setOrientation(Quaternionf orientation);
    
    /**
     * Should this node inherit parent's orientation.
     *
     * @param value True to inherit, false otherwise
     */
    void setInheritOrientation(bool value);
    
    /**
     * Mark this node and all children as out of date and in need of
     * transformation updates.
     *
     * @param updateParent Should parent update be forced
     */
    void needUpdate(bool updateParent = false);
    
    /**
     * Returns true if this node has unupdated changes.
     */
    bool isDirty() const { return m_dirty; }
    
    /**
     * Returns this node's world transformation.
     */
    Transform3f worldTransform() const { return m_worldTransform; }
    
    /**
     * Renders this node if it is rendrable.
     */
    virtual void render();
protected:
    /**
     * Performs transformation updates.
     */
    virtual void update(bool updateChildren, bool parentHasChanged);
    
    /**
     * Child update request.
     *
     * @param child Child node that is requesting an update
     * @param updateParent Should parent update be forced
     */
    void requestUpdate(SceneNode *child, bool updateParent);
    
    /**
     * Performs bound updates.
     */
    void updateBounds();
    
    /**
     * Updates scene pointer from parent node.
     */
    void updateSceneFromParent();
private:
    // Parent node and children list
    SceneNode *m_parent;
    boost::unordered_map<std::string, SceneNode*> m_children;
    
    // Naming
    std::string m_name;
    
    // Updates
    bool m_needParentUpdate;
    bool m_needChildUpdate;
    bool m_parentNotified;
    std::list<SceneNode*> m_childrenToUpdate;
protected:
    // Scene associated with this node
    Scene *m_scene;
    
    // Transformations
    Transform3f m_worldTransform;
    Vector3f m_localPosition;
    Vector3f m_worldPosition;
    Quaternionf m_localOrientation;
    Quaternionf m_worldOrientation;
    
    // Bounding box
    AxisAlignedBox m_localBounds;
    AxisAlignedBox m_worldBounds;
    
    // Flags
    bool m_inheritOrientation;
    bool m_dirty;
    
    // TODO Modifiers
    // std::list<Modifier*> m_modifiers;
};

}

#endif
