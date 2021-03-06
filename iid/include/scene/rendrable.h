/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_RENDRABLENODE_H
#define IID_SCENE_RENDRABLENODE_H

#include "globals.h"
#include "scene/node.h"
#include "renderer/rendrable.h"

class btIndexedMesh;

namespace IID {

class Mesh;
class Texture;
class Shader;
class Material;
class StateBatcher;

/**
 * A rendrable node is a scene node that can be rendered.
 */
class RendrableNode : public SceneNode, public Rendrable {
public:
    /**
     * Class constructor.
     *
     * @param name Node name
     * @param parent Parent node
     */
    RendrableNode(const std::string &name, SceneNode *parent = 0);
    
    /**
     * Class destructor.
     */
    ~RendrableNode();
    
    /**
     * Set node's 3D mesh.
     *
     * @param mesh Mesh instance
     */
    void setMesh(Mesh *mesh);
    
    /**
     * Set node's texture.
     *
     * @param texture Texture instance
     */
    void setTexture(Texture *texture);
    
    /**
     * Sets node's shader.
     *
     * @param shader Shader instance
     */
    void setShader(Shader *shader);
    
    /**
     * Sets node's material.
     *
     * @param material Material instance
     */
    void setMaterial(Material *material);
    
    /**
     * Returns this rendrable's mesh.
     */
    Mesh *getMesh() const { return m_mesh; }
    
    /**
     * Returns this rendrable's texture.
     */
    Texture *getTexture() const { return m_texture; }
    
    /**
     * Returns this rendrable's shader.
     */
    Shader *getShader() const { return m_shader; }
    
    /**
     * Returns this rendrable's material.
     */
    Material *getMaterial() const { return m_material; }
    
    /**
     * Sets status of displaying this object's bounding box.
     *
     * @param value AABB display status
     */
    void setShowBoundingBox(bool value);
    
    /**
     * Performs batching of static geometry currently on the scene.
     *
     * @param triangles An array holding all the vertices
     */
    void batchStaticGeometry(btTriangleIndexVertexArray *triangles);
    
    /**
     * Returns this node's world transformation. This needs to be here because
     * the Rendrable interface requires worldTransform to be implemented.
     */
    const Transform3f &worldTransform() const { return m_worldTransform; }
    
    /**
     * Returns the light list of affecting lights.
     */
    const LightList &getLights() const;
    
    /**
     * Renders this node.
     *
     * @param batcher State batcher that holds the render queue
     */
    void render(StateBatcher *batcher);
private:
    // Resources used for rendering this node
    Mesh *m_mesh;
    Texture *m_texture;
    Shader *m_shader;
    Material *m_material;

    // Bounding box display
    bool m_showBoundingBox;
    
    // Indexed mesh for static geometry batching
    btIndexedMesh *m_staticGeomMesh;
    
    // Lighting
    unsigned long m_lightVersionCounter;
    mutable LightList m_affectingLights;
};

}

#endif
