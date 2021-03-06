/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/camera.h"
#include "scene/scene.h"
#include "scene/viewtransform.h"
#include "drivers/base.h"

namespace IID {

Camera::Camera(Scene *scene)
  : m_scene(scene),
    m_listener(0),
    m_lag(20),
    m_zoom(0.,0.,0.)
{
}

Camera::~Camera()
{
}

void Camera::setCamInternals(float angle, float width, float height, float nearDist, float farDist)
{
  m_screenWidth = width;
  m_screenHeight = height;
  m_angle = angle;
  m_ratio = width / height;
  m_nearDist = nearDist;
  m_farDist = farDist;
  
  // Recompute plane dimensions
  float tang = (float) std::tan(0.5 * M_PI * angle / 180.0);
  m_nearHeight = nearDist * tang;
  m_nearWidth = m_nearHeight * m_ratio;
  m_farHeight = farDist * tang;
  m_farWidth = m_farHeight * m_ratio;
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

  // Update sound listener properties
  if (m_listener) {
    m_listener->setPosition( eye.data() );
    m_listener->setOrientation( m_center.data(), m_up.data() );
  }
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

void Camera::setupViewTransform()
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

void Camera::setListener(Listener *listener)
{
    m_listener = listener;
}

Listener *Camera::getListener()
{
    return m_listener;
}

void Camera::setLag(int lag)
{
  m_lag = lag;
}

void Camera::setZoom(const Vector3f &zoom)
{
  m_zoom = zoom;
}

void Camera::appendTrajectoryPoint(const Vector3f &trajectoryPoint)
{
  TrajectoryPoint p;
  p.point = trajectoryPoint;
  p.relative = true;
  
  m_trajectory.push(p);
}

void Camera::appendTrajectoryPoint(const Vector3f &trajectoryPoint, const Vector3f &eye)
{
  TrajectoryPoint p;
  p.point = trajectoryPoint;
  p.eye = eye;
  p.relative = false;
  
  m_trajectory.push(p);
}

void Camera::nextTrajectoryPoint()
{
  // Check if the trajectory queue is sufficiently large
  if (m_trajectory.size() > m_lag) {
    TrajectoryPoint next = m_trajectory.front();
    
    if (next.relative) {
      lookAt(next.point + m_zoom, next.point, m_up);
    } else {
      lookAt(next.eye, next.point, m_up);
    }
    
    // Get rid of the currently set point
    m_trajectory.pop();
  }
}

Vector3f Camera::rayTo(int x, int y)
{
  Vector3f rayFrom = m_eye;
  Vector3f rayForward = (m_center - m_eye).normalized() * m_farDist;
  Vector3f rightOffset;
  Vector3f vertical = m_up;
  Vector3f horizontal = rayForward.cross(vertical).normalized();
  vertical = horizontal.cross(rayForward).normalized();
  horizontal *= 2.0 * m_farHeight;
  vertical *= 2.0 * m_farHeight;
  
  if (m_ratio < 1.0)
    horizontal /= m_ratio;
  else
    vertical *= m_ratio;
  
  Vector3f rayToCenter = rayFrom + rayForward;
  Vector3f dHor = horizontal * 1.0/m_screenWidth;
  Vector3f dVert = vertical * 1.0/m_screenHeight;
  
  Vector3f rayToTarget = rayToCenter - 0.5 * horizontal + 0.5 * vertical;
  rayToTarget += x * dHor;
  rayToTarget -= y * dVert;
  
  return rayToTarget;
}

}
