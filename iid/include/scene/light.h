/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_LIGHTNODE_H
#define IID_SCENE_LIGHTNODE_H

#include "globals.h"
#include "scene/node.h"

namespace IID {

class StateBatcher;

/**
 * A light node is a node that represents a scene light.
 */
class LightNode : public SceneNode {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    /**
     * Class constructor.
     *
     * @param parent Parent node
     */
    LightNode(SceneNode *parent = 0);
    
    /**
     * Class destructor.
     */
    ~LightNode();
    
    /**
     * Configures light properties.
     *
     * @param ambient Ambient component
     * @param diffuse Diffuse component
     * @param specular Specular component
     * @param attConst Constant attenuation
     * @param attLin Linear attenuation
     * @param attQuad Quadratic attenuation
     */
    void setProperties(const Vector4f &ambient, const Vector4f &diffuse, const Vector4f &specular,
                       float attConst, float attLin, float attQuad);
    
    /**
     * Renders this node.
     */
    void render();
private:
    // Resources used for rendering this node
    Vector4f m_ambient;
    Vector4f m_diffuse;
    Vector4f m_specular;
    float m_attConst;
    float m_attLin;
    float m_attQuad;
    
    // Pointer to state batcher to avoid lookups
    StateBatcher *m_stateBatcher;
};

}

#endif
