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
     * Renders this node.
     */
    void render();
private:
    // Resources used for rendering this node
    Mesh *m_mesh;
    Texture *m_texture;
    Shader *m_shader;
    Material *m_material;
    
    // Pointer to state batcher to avoid lookups
    StateBatcher *m_stateBatcher;
};

}

#endif
