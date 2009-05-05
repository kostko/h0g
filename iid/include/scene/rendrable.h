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
class RendrableNode : public SceneNode {
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
};

}

#endif
