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

class btTriangleIndexVertexArray;

namespace IID {

class Scene;
class Octree;
class OctreeNode;
class StateBatcher;
class Texture;
class Shader;
class Material;
class Player;
class LightManager;

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
     */
    SceneNode(const std::string &name, SceneNode *parent = 0);
    
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
     * Returns the specified child node.
     *
     * @param name Child name
     * @return A valid SceneNode instance or NULL if there is none
     */
    SceneNode *child(const std::string &name);
    
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
     * Returns the node's world coordinates.
     *
     * @return World coordinates vector
     */
    Vector3f getWorldPosition() const;
    
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
    const Transform3f &worldTransform() const { return m_worldTransform; }
    
    /**
     * Returns this node's world orientation.
     */
    Quaternionf worldOrientation() const { return m_worldOrientation; }
    
    /**
     * Returns this node's bounding box in world coordinates.
     */
    const AxisAlignedBox &getBoundingBox() const { return m_worldBounds; }
    
    /**
     * Returns this node's dimensions (= bounding box in local coordinates).
     */
    const AxisAlignedBox &getLocalBoundingBox() const { return m_localBounds; }
    
    /**
     * Renders this node if it is rendrable.
     *
     * @param batcher State batcher that holds the render queue
     */
    virtual void render(StateBatcher *batcher);
    
    /**
     * Sets this node's octree node.
     */
    void setOctreeNode(OctreeNode *node);
    
    /**
     * Returns reference to this node's octree node.
     */
    OctreeNode *getOctreeNode() const { return m_octreeNode; }
    
    /**
     * Set node's texture.
     *
     * @param texture Texture instance
     */
    virtual void setTexture(Texture *texture);
    
    /**
     * Sets node's shader.
     *
     * @param shader Shader instance
     */
    virtual void setShader(Shader *shader);
    
    /**
     * Sets node's material.
     *
     * @param material Material instance
     */
    virtual void setMaterial(Material *material);
    
    /**
     * Sets the static hint for this scene node. This is just a hint for
     * some other geometry-baking methods that this node is static and
     * its position/orientation will never change. Chaning this node's
     * position/orientation will still work, but will produce undefined
     * results in dynamics simulation etc.
     *
     * @param value True to set static hint, false to unset
     */
    void setStaticHint(bool value);
    
    /**
     * Gets the node's sound player.
     *
     * @return Node's requested player object
     */
    Player *getSoundPlayer(const std::string &playerName);
    
    /**
     * Register a player.
     *
     * @param playerName The name used for retrieving this player
     * @param player Player instance
     */
    void registerSoundPlayer(const std::string &playerName, Player *player);
    
    /**
     * Performs batching of static geometry currently on the scene.
     *
     * @param triangles An array holding all the vertices
     */
    virtual void batchStaticGeometry(btTriangleIndexVertexArray *triangles);
    
    /**
     * Moves this node so it is attached directly to the root scene
     * node, but preserving world position and orientation.
     */
    void separateNodeFromParent();
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
     * Performs additional updates.
     */
    virtual void updateNodeSpecific();
    
    /**
     * Updates scene pointer from parent node.
     */
    virtual void updateSceneFromParent();
    
    /**
     * Removes this node from the scene graph.
     */
    virtual void clearConnectionToScene();
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
    
    // Octree linkage
    OctreeNode *m_octreeNode;
    Octree *m_octree;
protected:
    // Scene associated with this node
    Scene *m_scene;
    LightManager *m_lightManager;
    
    // Transformations
    Transform3f m_worldTransform;
    Vector3f m_localPosition;
    Vector3f m_worldPosition;
    Quaternionf m_localOrientation;
    Quaternionf m_worldOrientation;
    
    // Bounding box
    AxisAlignedBox m_localBounds;
    AxisAlignedBox m_worldBounds;
    
    // Sound players associated with this node
    boost::unordered_map<std::string, Player*> m_players;
    
    // Flags
    bool m_inheritOrientation;
    bool m_dirty;
    bool m_static;
};

}

#endif
