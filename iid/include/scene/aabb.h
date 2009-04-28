/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_AABB_H
#define IID_SCENE_AABB_H

#include "globals.h"

namespace IID {

class AxisAlignedBox {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    /**
     * Class constructor.
     */
    AxisAlignedBox()
      : m_box(AlignedBox<float, 3>())
    {}
    
    /**
     * Class constructor.
     */
    AxisAlignedBox(const AxisAlignedBox &other)
      : m_box(AlignedBox<float, 3>(other.m_box))
    {}
    
    /**
     * Class constructor.
     */
    AxisAlignedBox(const Vector3f &min, const Vector3f &max)
      : m_box(AlignedBox<float, 3>(min, max))
    {}
    
    /**
     * Translates this AABB for the given vector.
     */
    void translate(const Vector3f &vec)
    {
      m_box = m_box.translate(vec);
    }
    
    /**
     * Transforms this AABB with the given transformation.
     */
    void transform(const Transform3f &transform)
    {
      Vector3f oldMin, oldMax, currentCorner;
      
      // Save old values
      oldMin = m_box.min();
      oldMax = m_box.max();
      
      // Transform corners
      currentCorner = oldMin;
      m_box = m_box.extend(transform * currentCorner);
      
      currentCorner[2] = oldMax[2];
      m_box = m_box.extend(transform * currentCorner);
      
      currentCorner[1] = oldMax[1];
      m_box = m_box.extend(transform * currentCorner);
      
      currentCorner[2] = oldMin[2];
      m_box = m_box.extend(transform * currentCorner);
      
      currentCorner[0] = oldMax[0];
      m_box = m_box.extend(transform * currentCorner);
      
      currentCorner[2] = oldMax[2];
      m_box = m_box.extend(transform * currentCorner);
      
      currentCorner[1] = oldMin[1];
      m_box = m_box.extend(transform * currentCorner);
      
      currentCorner[2] = oldMin[2];
      m_box = m_box.extend(transform * currentCorner);
    }
    
    /**
     * Extends this AABB.
     */
    void extend(const AxisAlignedBox &box)
    {
      m_box = m_box.extend(box.m_box);
    }
protected:
    AlignedBox<float, 3> m_box;
};

}

#endif
