/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_VIEWTRANSFORM_H
#define IID_SCENE_VIEWTRANSFORM_H

#include "globals.h"

namespace IID {

/**
 * Represents the view transformation.
 */
class ViewTransform {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    /**
     * Class constructor.
     */
    ViewTransform()
    {
      loadIdentity();
    }
    
    /**
     * Sets this transformation to the identity matrix.
     */
    void loadIdentity()
    {
      m_transform = Matrix4f::Identity();
    }
    
    /**
     * Performs a view transformation.
     */
    void lookAt(const Vector3f &eye, const Vector3f &center, const Vector3f &up)
    {
      Vector3f forward = (center - eye).normalized();
      Vector3f side = forward.cross(up).normalized();
      Vector3f upy = side.cross(forward);
      Matrix4f m;
      m << side[0], upy[0], -forward[0], 0.,
           side[1], upy[1], -forward[1], 0.,
           side[2], upy[2], -forward[2], 0.,
           0.,      0.,     0.,          1.;
      m_transform = m_transform * m;
      m_transform = m_transform.translate(-eye);
    }
    
    /**
     * Returns the actual transformation matrix.
     */
    Transform3f transform() { return m_transform; }
private:
    Transform3f m_transform;
};

}

#endif
