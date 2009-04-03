#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
import math
from numpy import ndarray, dot, cross
from numpy.linalg import norm

class Frustum(object):
  """
  Represents the view frustum.
  """
  # States
  INSIDE, INTERSECT, OUTSIDE = xrange(3)
  
  # Plane dimensions
  nearWidth = None
  nearHeight = None
  farWidth = None
  farHeight = None
  
  # Perspective information
  angle = None
  ratio = None
  nearDistance = None
  farDistance = None
  
  # All six planes defining the frustum space
  planes = {}
  
  def __init__(self, angle, ratio, nearDistance, farDistance):
    """
    Create a frustum.
    """
    self.setCamInternals(angle, ratio, nearDistance, farDistance)
    self.planes['top'] = Plane()
    self.planes['bottom'] = Plane()
    self.planes['left'] = Plane()
    self.planes['right'] = Plane()
    self.planes['near'] = Plane()
    self.planes['far'] = Plane()
  
  def setCamInternals(self, angle, ratio, nearDistance, farDistance):
    """
    Takes the same arguments as gluPerspective(..).
    Must be called each time the perspective changes.
    """
    self.angle = angle
    self.ratio = ratio
    self.nearDistance = nearDistance
    self.farDistance = farDistance
    
    # Re-compute plane dimensions
    tang = math.tan(0.5 * math.radians(angle))
    self.nearHeight = nearDistance * tang
    self.nearWidth = ratio * self.nearHeight
    self.farHeight = farDistance * tang
    self.farWidth = ratio * self.farHeight
    
  def setCamDef(self, camPos, camTo, up):
    """
    Takes the same parameters as gluLookAt(..). This method must be
    called each time the camera's position or orientation changes.
    Each parameter must be a numpy ndarray.
    
    @param camPos: Position of the camera
    @param camTo: The point to where the camera is pointing
    @param up: The up vector
    """
    # Get Z axis of camera
    z = camPos - camTo
    z = z / norm(z)
    # Get X axis of camera
    x = cross(up, z)
    x = x / norm(x)
    # Get Y axis of camera
    y = cross(z, x)
    # Compute centers of near & far planes
    nearCenter = camPos - z * self.nearDistance
    farCenter = camPos - z * self.farDistance
    # Define/change each plane's point and normal
    self.planes['near'].setNormalAndPoint(-z, nearCenter)
    self.planes['far'].setNormalAndPoint(z, farCenter)
    
    aux = (nearCenter + y * self.nearHeight) - camPos
    aux = aux / norm(aux)
    normal = cross(aux, x)
    self.planes['top'].setNormalAndPoint(normal, nearCenter + y * self.nearHeight)
  
    aux = (nearCenter - y * self.nearHeight) - camPos
    aux = aux / norm(aux)
    normal = cross(x, aux)
    self.planes['bottom'].setNormalAndPoint(normal, nearCenter - y * self.nearHeight)
    
    aux = (nearCenter - x * self.nearWidth) - camPos
    aux = aux / norm(aux)
    normal = cross(aux, y)
    self.planes['left'].setNormalAndPoint(normal, nearCenter - x * self.nearWidth)
    
    aux = (nearCenter + x * self.nearWidth) - camPos
    aux = aux / norm(aux)
    normal = cross(y, aux)
    self.planes['right'].setNormalAndPoint(normal, nearCenter + x * self.nearWidth)
    
  def pointInFrustum(self, point):
    """
    Test if a point is inside the frustum.
    
    @return: INSIDE if the point is inside the frustum, otherwise OUTSIDE
    """
    for plane in self.planes.values():
      if plane.distance(p) < 0:  # The point is outside
        return Frustum.OUTSIDE
    
    return Frustum.INSIDE
  
  def sphereInFrustum(self, point, radius):
    """
    Test if a sphere is inside the frustum.
    
    @return: INSIDE if the sphere is inside, OUTSIDE if the sphere 
    is outside and INTERSECT if it intersects the frustum
    """
    state = Frustum.INSIDE
    for plane in self.planes.values():
      d = plane.distance(point)
      if d < -radius:
        return Frustum.OUTSIDE
      elif d < radius:
        state = Frustum.INTERSECT
    
    return state
  
  def boxInFrustum(self, box):
    """
    Test if a box is inside the frustum. The box should be a
    list of 8 points.
    
    @return: INSIDE if the box is inside, OUTSIDE if the box 
    is outside and INTERSECT if it intersects the frustum
    """
    state = Frustum.INSIDE
    for plane in self.planes.values():
      outside, inside = 0, 0   # Corners out, corners in counters
      
      for corner in box:
        if inside != 0 and outside != 0:
          # The box has corners in and out of the frustum
          break
        if plane.distance(corner) < 0:
          outside = outside + 1
        else:
          inside = inside + 1 
          
      # All corners are out
      if not inside:
        return Frustum.OUTSIDE
      # Some in some out
      elif outside:
        state = Frustum.INTERSECT
    
    return state
  
  
class Plane(object):
  """
  Represents a plane by defining a point and a normal.
  """
  normal = None
  point = None
  D = None
  
  def __init__(self, point = None, normal = None):
    self.point = point
    self.normal = normal
    if point and normal:
      self.D = -dot(self.normal, point)
    
  def setNormalAndPoint(self, normal, point):
    """
    Set the normal and point of this plane.
    """
    self.normal = normal
    self.point = point
    self.D = -dot(self.normal, point)
    
  def distance(self, point):
    """
    @return: The SIGNED distance of the point to this plane.
    """
    return dot(self.normal, point) + self.D