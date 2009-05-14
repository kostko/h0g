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
    
    enum Extent {
      Null,
      Finite,
      Infinite
    };
    
    /**
     * Class constructor.
     */
    AxisAlignedBox()
      : m_extent(Null)
    {}
    
    /**
     * Class constructor.
     */
    AxisAlignedBox(const AxisAlignedBox &box)
      : m_extent(Null)
    {
      if (box.isNull())
        m_extent = Null;
      else if (box.isInfinite())
        m_extent = Infinite;
      else
        setBounds(box.m_min, box.m_max);
    }
    
    /**
     * Class constructor.
     */
    AxisAlignedBox(const Vector3f &min, const Vector3f &max)
    {
      setBounds(min, max);
    }
    
    /**
     * Returns true if the bounding box is null.
     */
    bool isNull() const
    {
      return m_extent == Null;
    }
    
    /**
     * Returns true if the bounding box is infinite.
     */
    bool isInfinite() const
    {
      return m_extent == Infinite;
    }
    
    /**
     * Returns true if the point is inside the box.
     */
    bool contains(Vector3f p) const
    {
      if(p.x() > m_max.x() || 
         p.y() > m_max.y() ||
         p.z() > m_max.z() ||
         p.x() < m_min.x() || 
         p.y() < m_min.y() ||
         p.z() < m_min.z()  )
      {
        return false;
      }
      
      return true;
    }
    
    /**
     * Sets bounding box bounds.
     *
     * @param min Minimum corner
     * @param max Maximum corner
     */
    void setBounds(const Vector3f &min, const Vector3f &max)
    {
      m_min = min;
      m_max = max;
      m_radius = ((m_max - m_min) * 0.5).norm();
      m_extent = Finite;
    }
    
    /**
     * Sets bounding box minimum corner.
     *
     * @param x X component
     * @param y Y component
     * @param z Z component
     */
    void setMinimum(float x, float y, float z)
    {
      m_min << x, y, z;
      m_radius = ((m_max - m_min) * 0.5).norm();
    }
    
    /**
     * Returns bounding box minimum corner.
     */
    const Vector3f& getMinimum() const
    {
      return m_min;
    }
    
    /**
     * Sets bounding box maximum corner.
     *
     * @param x X component
     * @param y Y component
     * @param z Z component
     */
    void setMaximum(float x, float y, float z)
    {
      m_max << x, y, z;
      m_radius = ((m_max - m_min) * 0.5).norm();
    }
    
    /**
     * Returns bounding box maximum corner.
     */
    const Vector3f& getMaximum() const
    {
      return m_max;
    }
    
    /**
     * Merges the given point into the bounding box.
     */
    inline void merge(const Vector3f &point)
    {
      switch (m_extent) {
        case Null: {
          // We are null, so we become the point
          setBounds(point, point);
          return;
        }
        case Finite: {
          // We are finite, so we expand to contain the point
          for (int i = 0; i < 3; i++) {
            if (point[i] > m_max[i])
              m_max[i] = point[i]; 
            
            if (point[i] < m_min[i])
              m_min[i] = point[i];
          }
          return;
        }
        case Infinite: {
          // We are infinite, so we can grow no larger
          return;
        }
      }
    }
    
    /**
     * Merges the given bounding box into this bounding box.
     */
    void merge(const AxisAlignedBox &box)
    {
      if (box.m_extent == Null || m_extent == Infinite) {
        return;
      } else if (box.m_extent == Infinite) {
        m_extent = Infinite;
      } else if (m_extent == Null) {
        setBounds(box.m_min, box.m_max);
      } else {
        // Actual merge
        Vector3f min = m_min;
        Vector3f max = m_max;
        
        for (int i = 0; i < 3; i++) {
          if (box.m_max[i] > max[i])
            max[i] = box.m_max[i]; 
          
          if (box.m_min[i] < min[i])
            min[i] = box.m_min[i];
        }
        
        setBounds(min, max);
      }
    }
    
    /**
     * Performs an affine transformation on the bounding box.
     *
     * @param transform Transformation descriptor
     */
    inline void transformAffine(const Transform3f &transform)
    {
      if (m_extent != Finite)
        return;
      
      Vector3f center = getCenter();
      Vector3f halfSize = getHalfSize();
      
      Matrix4f m = transform.matrix();
      Vector3f newCenter = transform * center;
      Vector3f newHalfSize(
        std::abs(m(0, 0)) * halfSize[0] + std::abs(m(0, 1)) * halfSize[1] + std::abs(m(0, 2)) * halfSize[2],
        std::abs(m(1, 0)) * halfSize[0] + std::abs(m(1, 1)) * halfSize[1] + std::abs(m(1, 2)) * halfSize[2],
        std::abs(m(2, 0)) * halfSize[0] + std::abs(m(2, 1)) * halfSize[1] + std::abs(m(2, 2)) * halfSize[2]
      );
      
      setBounds(newCenter - newHalfSize, newCenter + newHalfSize);
    }
    
    /**
     * Sets corner coordinates.
     *
     * @param corners An array of 8 vectors where corners will be saved
     */
    inline void getCorners(Vector3f *corners) const
    {
      corners[0] = m_min;
      corners[1] << m_min[0], m_max[1], m_min[2];
      corners[2] << m_max[0], m_max[1], m_min[2];
      corners[3] << m_max[0], m_min[1], m_min[2];
      
      corners[4] = m_max;
      corners[5] << m_min[0], m_max[1], m_max[2];
      corners[6] << m_min[0], m_min[1], m_max[2];
      corners[7] << m_max[0], m_min[1], m_max[2];
    }
    
    /**
     * Returns bounding box geometric center.
     */
    Vector3f getCenter() const
    {
      return (m_max + m_min) * 0.5;
    }
    
    /**
     * Returns bounding box size.
     */
    Vector3f getSize() const
    {
      switch (m_extent) {
        case Null: return Vector3f(0, 0, 0);
        case Finite: return m_max - m_min;
        case Infinite: return Vector3f(
          std::numeric_limits<float>::infinity(),
          std::numeric_limits<float>::infinity(),
          std::numeric_limits<float>::infinity()
        );
      }
    }
    
    /**
     * Returns bound box half size.
     */
    Vector3f getHalfSize() const
    {
      switch (m_extent) {
        case Null: return Vector3f(0, 0, 0);
        case Finite: return (m_max - m_min) * 0.5;
        case Infinite: return Vector3f(
          std::numeric_limits<float>::infinity(),
          std::numeric_limits<float>::infinity(),
          std::numeric_limits<float>::infinity()
        );
      }
    }
    
    /**
     * Returns the bounding sphere radius for this AABB.
     */
    float getRadius() const
    {
      return m_radius;
    }
protected:
    Vector3f m_min;
    Vector3f m_max;
    Extent m_extent;
    
    // Bounding sphere radius
    float m_radius;
};

}

#endif
