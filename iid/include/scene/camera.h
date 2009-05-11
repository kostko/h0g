/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_CAMERA_H
#define IID_SCENE_CAMERA_H

#include "globals.h"
#include "scene/aabb.h"

namespace IID {

class Scene;
class Listener;

// Plane is just a simple hyperplane
typedef Hyperplane<float, 3> Plane;

/**
 * This class represents a camera (aka view-frustum). It contains some
 * convenience functions to manipulate the camera with ease and also to
 * perform checks against the frustum for culling.
 */
class Camera {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    /**
     * Human-readable plane indices into m_planes array.
     */
    enum {
      Top = 0,
      Bottom,
      Left,
      Right,
      Near,
      Far
    };
    
    /**
     * Possible positions for frustum checks.
     */
    enum Position {
      Outside = 0,
      Intersect,
      Inside
    };
    
    /**
     * Class constructor.
     */
    Camera(Scene *scene);
    
    /**
     * Class destructor.
     */
    ~Camera();
    
    /**
     * "Renders" the camera by applying a view transform.
     */
    void render();
    
    /**
     * Setup camera parameters.
     */
    void setCamInternals(float angle, float ratio, float nearDist, float farDist);
    
    /**
     * Setup camera viewpoint.
     */
    void lookAt(const Vector3f &eye, const Vector3f &center, const Vector3f &up);
    
    /**
     * "Walks" the camera forward or backward.
     *
     * @param distance Distance to walk
     */
    void walk(float distance);
    
    /**
     * Rotate camera for specific angles.
     *
     * @param x Rotation around X-axis
     * @param y Rotation around Y-axis
     * @param z Rotation around Z-axis
     */
    void rotate(float x, float y, float z);
    
    /**
     * Returns intersection status of a sphere with the frustum.
     *
     * @param pos Position vector
     * @param radius Sphere radius
     * @return A valid Position
     */
    Position containsSphere(const Vector3f &pos, float radius) const;
    
    /**
     * Returns intersection status of an axis aligned box with the frustum.
     *
     * @param box Axis aligned bounding box
     * @return A valid Position
     */
    Position containsBox(const AxisAlignedBox &box) const;
    
    /**
     * Returns intersection status of a point with the frustum.
     *
     * @param point Position vector
     * @return A valid Position
     */
    Position containsPoint(const Vector3f &point) const;
    
    /**
     * Set the listener instance. 
     *
     * The place of the sound listener in space is represented 
     * by the camera's "eye" coordinates.
     *
     * @param listener A valid listener instance.
     */
    void setListener(Listener *listener);
    
    /**
     * Get the listener instance.
     */
    Listener *getListener();
    
private:
    // Scene instance
    Scene *m_scene;
    
    // Listener instance
    Listener *m_listener;
    
    // Active transform
    Vector3f m_eye;
    Vector3f m_center;
    Vector3f m_up;
    Transform3f m_viewTransform;
    
    // Our frustum planes
    Plane m_planes[6];
    
    // Camera description
    float m_nearDist, m_farDist, m_ratio, m_angle;
    float m_nearWidth, m_nearHeight, m_farWidth, m_farHeight;
};

}

#endif
