/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/camera.h"
#include "scene/scene.h"
#include "scene/viewtransform.h"

namespace IID {

Camera::Camera(Scene *scene)
  : m_scene(scene)
{
}

Camera::~Camera()
{
}

void Camera::setCamInternals(float angle, float ratio, float nearDist, float farDist)
{
  m_angle = angle;
  m_ratio = ratio;
  m_nearDist = nearDist;
  m_farDist = farDist;
  
  // Recompute plane dimensions
  float tang = (float) std::tan(0.5 * M_PI * angle / 180.0);
  m_nearHeight = nearDist * tang;
  m_nearWidth = m_nearHeight * ratio;
  m_farHeight = farDist * tang;
  m_farWidth = m_farHeight * ratio;
}

void Camera::lookAt(const Vector3f &eye, const Vector3f &center, const Vector3f &up)
{
  Vector3f z = (eye - center).normalized();
  Vector3f x = up.cross(z).normalized();
  Vector3f y = z.cross(x);
  
  // Compute centers of near and far planes
  Vector3f nearCenter = eye - z * m_nearDist;
  Vector3f farCenter = eye - z * m_farDist;
  
  // Define each plane's point and normal
  m_planes[Near] = Plane(-z, nearCenter);
  m_planes[Far] = Plane(z, farCenter);
  
  Vector3f aux = ((nearCenter + y * m_nearHeight) - eye).normalized();
  Vector3f norm = aux.cross(x);
  m_planes[Top] = Plane(norm, nearCenter + y * m_nearHeight);
  
  aux = ((nearCenter - y * m_nearHeight) - eye).normalized();
  norm = x.cross(aux);
  m_planes[Bottom] = Plane(norm, nearCenter - y * m_nearHeight);
  
  aux = ((nearCenter - x * m_nearWidth) - eye).normalized();
  norm = aux.cross(y);
  m_planes[Left] = Plane(norm, nearCenter - x * m_nearWidth);
  
  aux = ((nearCenter + x * m_nearWidth) - eye).normalized();
  norm = y.cross(aux);
  m_planes[Right] = Plane(norm, nearCenter + x * m_nearWidth);
  
  // Update local stuff
  m_eye = eye;
  m_center = center;
  m_up = up;
  
  // Don't forget to update the actual view transformation
  m_scene->viewTransform()->loadIdentity();
  m_scene->viewTransform()->lookAt(eye, center, up);
  m_viewTransform = m_scene->viewTransform()->transform();
}

void Camera::walk(float distance)
{
  Vector3f x = (m_center - m_eye).normalized() * distance;
  lookAt(m_eye + x, m_center + x, m_up);
}

void Camera::rotate(float x, float y, float z)
{
  Quaternionf rot;
  Vector3f rad = Vector3f(x, y, z) * M_PI / 180.0;
  rot = AngleAxisf(rad[0], Vector3f::UnitX()) *
        AngleAxisf(rad[1], Vector3f::UnitY()) *
        AngleAxisf(rad[2], Vector3f::UnitZ());
  
  lookAt(m_eye, rot * (m_center - m_eye) + m_eye, rot * m_up);
}

void Camera::render()
{
  // Just use the previously cached transformation
  m_scene->viewTransform()->setTransform(m_viewTransform);
}

Camera::Position Camera::containsPoint(const Vector3f &point) const
{
  for (int i = 0; i < 6; i++) {
    if (m_planes[i].signedDistance(point) < 0)
      return Outside;
  }
  
  return Inside;
}

Camera::Position Camera::containsSphere(const Vector3f &pos, float radius) const
{
  float distance;
  
  for (int i = 0; i < 6; i++) {
    distance = m_planes[i].signedDistance(pos);
    
    if (distance < -radius)
      return Outside;
    else if (std::abs(distance) < radius)
      return Intersect;
  }
  
  return Inside;
}

Camera::Position Camera::containsBox(const AxisAlignedBox &box) const
{
  Vector3f corners[8];
  int totalIn;
  box.getCorners(corners);
  
  // Test all 8 corners against the 6 sides; if all points are behind
  // one specific plane, we are out; if we are in with all the points,
  // then we are fully in
  for (int i = 0; i < 6; i++) {
    int inCount = 8;
    int pointIn = 1;
    
    for (int j = 0; j < 8; j++) {
      if (m_planes[i].signedDistance(corners[j]) < 0) {
        pointIn = 0;
        inCount--;
      }
    }
    
    // All points outside of plane i ?
    if (inCount == 0)
      return Outside;
    
    totalIn += pointIn;
  }
  
  // If all are in, we are in
  if (totalIn == 6)
    return Inside;
  
  return Intersect;
}

}
