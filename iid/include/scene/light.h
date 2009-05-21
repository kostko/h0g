/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_LIGHT_H
#define IID_SCENE_LIGHT_H

#include "globals.h"
#include "scene/node.h"

#include <vector>

namespace IID {

class StateBatcher;
class LightManager;

/**
 * A light node is a node that represents a scene light.
 */
class Light : public SceneNode {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    /**
     * Valid light types.
     */
    enum Type {
      PointLight,
      SpotLight
    };
    
    /**
     * Class constructor.
     *
     * @param name Node name
     * @param parent Parent node
     */
    Light(const std::string &name, SceneNode *parent = 0);
    
    /**
     * Class destructor.
     */
    ~Light();
    
    /**
     * Sets the light type.
     *
     * @param type Light type
     */
    void setType(Type type);
    
    /**
     * Returns the light type.
     */
    Type type() const { return m_type; }
    
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
protected:
    /**
     * Performs additional updates.
     */
    void updateNodeSpecific();
    
    /**
     * Updates scene pointer from parent node.
     */
    void updateSceneFromParent();
    
    /**
     * Removes this node from the scene graph.
     */
    void clearConnectionToScene();
private:
    // Light properties
    Vector4f m_ambient;
    Vector4f m_diffuse;
    Vector4f m_specular;
    float m_attConst;
    float m_attLin;
    float m_attQuad;
    
    // Light type
    Type m_type;
};

// A list of lights
typedef std::vector<Light*> LightList;

}

#endif
