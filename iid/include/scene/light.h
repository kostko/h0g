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
      DirectionalLight,
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
    Type getType() const { return m_type; }
    
    /**
     * Sets this light's visibility.
     *
     * @param value True for visible, false for hidden
     */
    void setVisible(bool value);
    
    /**
     * Returns true if this light is visible.
     */
    bool isVisible() const { return m_visible; }
    
    /**
     * Returns the squared distance between the light and the specified
     * point.
     *
     * @param point A vector representing point coordinates
     */
    float getSquaredDistanceTo(const Vector3f &point) const;
    
    /**
     * Returns the last computed distance.
     */
    float getLastSquaredDistance() const { return m_lastDistance; }
    
    /**
     * Sets the light's diffuse color.
     *
     * @param r Red component
     * @param g Green component
     * @param b Blue component
     */
    void setDiffuseColor(float r, float g, float b);
    
    /**
     * Returns the light's diffuse color.
     */
    Vector3f getDiffuseColor() const { return m_diffuse; }
    
    /**
     * Sets the light's specular color.
     *
     * @param r Red component
     * @param g Green component
     * @param b Blue component
     */
    void setSpecularColor(float r, float g, float b);
    
    /**
     * Returns the light's specular color.
     */
    Vector3f getSpecularColor() const { return m_specular; }
    
    /**
     * Sets light's attenuation.
     *
     * @param range Attenuation range
     * @param constant Constant attenuation
     * @param linear Linear attenuation
     * @param quadratic Quadratic attenuation
     */
    void setAttenuation(float range, float constant, float linear, float quadratic);
    
    /**
     * Returns the light's constant attenuation factor.
     */
    float getConstantAttenuation() const { return m_attConst; }
    
    /**
     * Returns the light's linear attenuation factor.
     */
    float getLinearAttenuation() const { return m_attLin; }
    
    /**
     * Returns the light's quadratic attenuation factor.
     */
    float getQuadraticAttenuation() const { return m_attQuad; }
    
    /**
     * Returns this light's attenuation range.
     */
    float getAttenuationRange() const { return m_range; }
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
    Vector3f m_diffuse;
    Vector3f m_specular;
    float m_attConst;
    float m_attLin;
    float m_attQuad;
    float m_range;
    
    // Distance to last object
    mutable float m_lastDistance;
    
    // Light type
    Type m_type;
    
    // Visibility
    bool m_visible;
};

// A list of lights
typedef std::vector<Light*> LightList;

}

#endif
