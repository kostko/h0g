#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
import numpy
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import ode
import logging
import time
import traceback

# IID imports
from iid.exceptions import *
from iid.behaviour import EntityBehaviour
from iid.frustum import Frustum

# Logger for this module
logger = logging.getLogger(__name__)

class SceneObject(object):
  """
  Represents an abstract object that can be rendered by the scene.
  """
  objectId = ""
  scene = None
  
  # Object hierarchy
  parent = None
  children = None
  
  # Object attributes
  visible = False
  static = True
  
  # Transformations (when not using a physical body)
  coordinates = None
  rotation = None
  rotationMatrix = None
  scaling = None
  
  def __init__(self, scene, objectId, parent = None):
    """
    Class constructor.
    
    @param scene: A valid Scene instance
    @param objectId: Unique object identifier
    @param parent: Parent object if this is a subobject
    """
    self.scene = scene
    self.objectId = objectId
    self.parent = parent
    self.children = {}
    
    self.coordinates = numpy.zeros(3)
    self.rotation = numpy.zeros(3)
    self.scaling = numpy.ones(3)
    self.__prepareRotationMatrix()
    
    # Insert us into hierarchy
    if self.parent:
      self.parent.children[objectId] = self
  
  def isChild(self):
    """
    Returns true if this object is not a top-level object, but
    a subobject for some other object.
    """
    return self.parent is not None
  
  def mapCoordsToParent(self, coords):
    """
    Maps coordinates in object coordinate system to world coordinate
    system.
    
    @param coords: A valid numpy array of coordinates
    """
    if not self.isChild():
      return coords
    
    return self.parent.mapCoordsToParent(self.parent.coordinates + coords)
  
  def setCoordinates(self, x, y, z):
    """
    Changes object's coordinates.
    """
    self.coordinates[0:3] = [x, y, z]
  
  def setRotation(self, x, y, z):
    """
    Changes object's rotation.
    """
    self.rotation[0:3] = [x, y, z]
    self.__prepareRotationMatrix()
  
  def setScaling(self, x, y, z):
    """
    Set object's scaling.
    """
    self.scaling[0:3] = [x, y, z]
  
  def rotateX(self, phi):
    """
    Rotate the object on X-axis.
    
    @param phi: Rotation angle
    """
    self.rotation[0] = phi
    self.__prepareRotationMatrix()
  
  def rotateY(self, phi):
    """
    Rotate the object on Y-axis.
    
    @param phi: Rotation angle
    """
    self.rotation[1] = phi
    self.__prepareRotationMatrix()
  
  def rotateZ(self, phi):
    """
    Rotate the object on Z-axis.
    
    @param phi: Rotation angle
    """
    self.rotation[2] = phi
    self.__prepareRotationMatrix()
  
  def setVisible(self, visible):
    """
    Sets object's visibility.
    """
    self.visible = visible
  
  def prepare(self):
    """
    Should prepare the object for rendering.
    """
    for obj in self.children.values():
      obj.prepare()
  
  def render(self):
    """
    Should render the given object.
    """
    for obj in self.children.values():
      if obj.visible:
        obj.render()
  
  def __prepareRotationMatrix(self):
    """
    Prepares the object's rotation matrix.
    """
    x, y, z = self.rotation
    
    # Calculate the rotation matrix
    cx, cy, cz = numpy.cos([numpy.pi * x/180., numpy.pi * y/180., numpy.pi * z/180.])
    sx, sy, sz = numpy.sin([numpy.pi * x/180., numpy.pi * y/180., numpy.pi * z/180.])
    
    self.rotationMatrix = (
      cx*cz - sx*cy*sz, -sx*cz - cx*cy*sz, sy*sz,
      cx*sz + sx*cy*cz, -sx*sz + cx*cy*cz, -sy*cz,
      sx*sy,            cx*sy,             cy
    )

class Entity(SceneObject):
  """
  Represents a model that can be drawn by the scene. This entity does
  not participate in neither physical simulation nor collision detection.
  """
  model = None
  texture = None
  shader = None
  
  # Entity behaviour
  behaviour = None
  
  # OpenGL list identifier
  textureId = None
  listId = None
  
  def __init__(self, scene, objectId, model, texture, parent = None):
    """
    Class constructor.
    
    @param scene: A valid Scene instance
    @param objectId: Unique object identifier
    @param model: A valid model
    @param texture: A valid texture
    @param parent: Parent object if this is a subobject
    """
    super(Entity, self).__init__(scene, objectId, parent)
    self.model = model
    self.texture = texture
  
  def setShader(self, shader):
    """
    Sets a GLSL shader for this entity.
    """
    self.shader = shader
  
  def prepare(self):
    """
    Prepares the model's vertices/textures so they can be rendered.
    """
    # First prepare all subentities
    super(Entity, self).prepare()
    
    # Prepare this entity texture and model
    self.textureId = self.texture.prepare() if self.texture else None
    self.listId = self.model.prepare()
  
  def render(self):
    """
    Renders the model by first transforming model coordinates to
    scene coordinates and then pushing model's vertices via
    calls to OpenGL.
    """
    x, y, z = self.coordinates
    R = self.rotationMatrix
    M = [
      R[0], R[3], R[6], 0.,
      R[1], R[4], R[7], 0.,
      R[2], R[5], R[8], 0.,
      x, y, z, 1.0
    ]
    
    glPushMatrix()
    glMultMatrixd(M)
    
    if self.shader:
      self.shader.activate()
    
    # Render all subentities
    super(Entity, self).render()
    
    # Add texture when requested
    if self.textureId is not None:
      glBindTexture(GL_TEXTURE_2D, self.textureId)
    
    # Execute precompiled OpenGL commands
    if self.listId:
      glCallList(self.listId)
    
    if self.shader:
      self.shader.deactivate()
    
    if self.scene.showBoundingBoxes and (self.scene.showSubentityBoxes or self.parent == None):
      self.__drawBoundingBox()
    
    glPopMatrix()
    
  def __drawBoundingBox(self):
    """
    Draws a box around the model.
    """
    glScalef(*self.model.dimensions)
    glMaterialfv(GL_FRONT, GL_EMISSION, (1.0, 0.0, 0.0))
    glutWireCube(1.0)

class PhysicalEntity(Entity):
  """
  Represents an entity that follows the laws of simulated
  physics.
  """
  body = None
  
  def __init__(self, scene, objectId, model, texture, parent = None):
    """
    Class constructor.
    
    @param scene: A valid Scene instance
    @param objectId: Unique object identifier
    @param model: A valid model
    @param texture: A valid texture
    @param parent: Parent object if this is a subobject
    """
    super(PhysicalEntity, self).__init__(scene, objectId, model, texture, parent)
    self.prepare()
    self.body = self.preparePhysicalModel()
  
  def setCoordinates(self, x, y, z):
    """
    Changes object's coordinates.
    """
    super(PhysicalEntity, self).setCoordinates(x, y, z)
    self.body.setPosition(self.mapCoordsToParent(self.coordinates))
  
  def setRotation(self, x, y, z):
    """
    Changes object's rotation.
    """
    super(PhysicalEntity, self).setRotation(x, y, z)
    self.body.setRotation(self.rotationMatrix)
  
  def setVisible(self, visible):
    """
    Sets object's visibility.
    """
    super(PhysicalEntity, self).setVisible(visible)
    if visible:
      self.body.enable()
    else:
      self.body.disable()
  
  def setDensity(self, density):
    """
    Sets entity's density.
    """
    if not isinstance(self.body, ode.Body):
      return
    
    lx, ly, lz = self.model.dimensions
    M = self.body.getMass()
    M.setBox(density, lx, ly, lz)
    self.body.setMass(M)
  
  def setMass(self, mass):
    """
    Sets entity's mass.
    """
    if not isinstance(self.body, ode.Body):
      return
    
    lx, ly, lz = self.model.dimensions
    M = self.body.getMass()
    M.setBoxTotal(mass, lx, ly, lz)
    self.body.setMass(M)
  
  def render(self):
    """
    Renders the model by first transforming model coordinates to
    scene coordinates and then pushing model's vertices via
    calls to OpenGL.
    """
    self.updateScenePosition()
    super(PhysicalEntity, self).render()
  
  def updateScenePosition(self):
    """
    Just updates the object's parameters.
    """
    self.coordinates = self.body.getPosition()
    self.rotationMatrix = self.body.getRotation()
  
  def preparePhysicalModel(self):
    """
    Should prepare the physical model. Default just represents all
    models as boxes. When using subentities this method should connect
    all subentities together via joints when needed.
    
    @return: A valid ode.Body instance
    """
    lx, ly, lz = self.model.dimensions
    body = ode.Body(self.scene.physicalWorld)
    M = ode.Mass()
    M.setBox(10.0, lx, ly, lz)
    body.setMass(M)
    
    # Create a box geom for collision detection
    geom = ode.GeomBox(self.scene.space, lengths = (lx, ly, lz))
    geom.sceneObject = self
    geom.setBody(body)
    
    return body

class StaticObstacle(PhysicalEntity):
  """
  Represents an entity that participates in collision detection but not
  in physical simulation (so it is static).
  """
  def preparePhysicalModel(self):
    """
    This actually returns a geometry object, since methods match.
    """
    lx, ly, lz = self.model.dimensions
    geom = ode.GeomBox(self.scene.space, lengths = (lx, ly, lz))
    geom.sceneObject = self
    
    return geom

class Camera(SceneObject):
  """
  Represents the camera.
  """
  frustum = None
  visible = True
  
  # Direction of the camera
  __direction = numpy.array([0.0, 0.0, -1.0])
  # Up vector (default)
  __up = numpy.array([0.0, 1.0, 0.0])
  
  def setCoordinates(self, x, y, z):
    """
    Changes camera's coordinates.
    """
    super(Camera, self).setCoordinates(x, y, z)
    self.__updateFrustum()
  
  def setRotation(self, x, y, z):
    """
    Changes camera's rotation.
    """
    super(Camera, self).setRotation(x, y, z)
    self.__updateFrustum()
    self.__rotateDirectionVector(x, y, z)
  
  def rotateX(self, phi):
    """
    Rotate the camera on X-axis.
    
    @param phi: Rotation angle
    """
    super(Camera, self).rotateX(phi)
    self.__updateFrustum()
    self.__rotateDirectionVector(phi, 0, 0)
  
  def rotateY(self, phi):
    """
    Rotate the camera on Y-axis.
    
    @param phi: Rotation angle
    """
    super(Camera, self).rotateY(phi)
    self.__updateFrustum()
    self.__rotateDirectionVector(0, phi, 0)
  
  def rotateZ(self, phi):
    """
    Rotate the camera on Z-axis.
    
    @param phi: Rotation angle
    """
    super(Camera, self).rotateZ(phi)
    self.__updateFrustum()
    self.__rotateDirectionVector(0, 0, phi)
    
  def setFrustum(self, frustum):
    """
    Set the camera's frustum.
    """
    self.frustum = frustum
    self.__updateFrustum()
  
  def __updateFrustum(self):
    """
    Updates the frustum each time the camera's
    position or orientation changes.
    """
    if self.frustum:
      # Calculate the center of the scene (where the cam is directed)
      center = self.__direction + self.coordinates
      # Update frustum
      self.frustum.setCamDef(
        self.coordinates,
        center,
        self.__up
      )
    
  def __rotateDirectionVector(self, x, y, z):
    """
    Properly transform direction vector.
    """
    # Calculate the rotation matrix
    cx, cy, cz = numpy.cos([numpy.pi * x/180., numpy.pi * y/180., numpy.pi * z/180.])
    sx, sy, sz = numpy.sin([numpy.pi * x/180., numpy.pi * y/180., numpy.pi * z/180.])
    
    rotationMatrix = numpy.matrix(
      [[cx*cz - sx*cy*sz, -sx*cz - cx*cy*sz, sy*sz],
      [cx*sz + sx*cy*cz, -sx*sz + cx*cy*cz, -sy*cz],
      [sx*sy,            cx*sy,             cy]]
    )
    
    pos = self.coordinates
    direction = self.__direction
    center = pos + direction
    # Rotate
    center = numpy.dot(center, rotationMatrix).tolist()[0]
    direction = center - pos
    self.__direction = direction / numpy.linalg.norm(direction)
  
  def render(self):
    """
    Move the camera to proper position.
    """
    glMatrixMode(GL_MODELVIEW)
    glRotatef(self.rotation[0], 1, 0, 0)
    glRotatef(360.0 - self.rotation[1], 0, 1, 0)
    glRotatef(self.rotation[2], 0, 0, 1)
    glTranslatef(
      -self.coordinates[0],
      -self.coordinates[1],
      -self.coordinates[2]
    )

class Light(SceneObject):
  """
  A light source that can be added to a scene.
  
  TODO: one million missing possible settings of a light.
  """
  __lightNumber = None
  __globalLights = GL_LIGHT0-1
  
  ambient = (0.2, 0.2, 0.2, 0.0)
  diffuse = (0.8, 0.8, 0.8, 0.0)
  specular = (1.0, 1.0, 1.0, 0.0)
  constantAttenuation = 1.0
  linearAttenuation = 0.0
  quadraticAttenuation = 0.0
  
  def __init__(self, scene, objectId):
    super(Light, self).__init__(scene, objectId)
    Light.__globalLights += 1   # Increment the number of all lights
    self.__lightNumber = Light.__globalLights;
    
  def prepare(self):
    """
    Prepare the lights.
    """
    self.setVisible(True)
    
  def render(self):
    """
    'Render' the light.
    """
    glLight(self.__lightNumber, GL_POSITION, self.coordinates)
    glLight(self.__lightNumber, GL_AMBIENT, self.ambient)
    glLight(self.__lightNumber, GL_DIFFUSE, self.diffuse)
    glLight(self.__lightNumber, GL_SPECULAR, self.diffuse)
    glLightf(self.__lightNumber, GL_CONSTANT_ATTENUATION , self.constantAttenuation)
    glLightf(self.__lightNumber, GL_LINEAR_ATTENUATION , self.linearAttenuation)
    glLightf(self.__lightNumber, GL_QUADRATIC_ATTENUATION , self.quadraticAttenuation)
    
  def setVisible(self, visible):
    """
    Set the light's visibility.
    """
    self.visible = visible
    if not visible:
      glDisable(self.__lightNumber)
    else:
      glEnable(self.__lightNumber)  
    
  def setCoordinates(self, x, y, z, a):
    """
    Changes object's coordinates.
    """
    self.coordinates = [x, y, z, a]
  
  def setAmbient(self, x, y, z, a):
    """
    Change ambient parameters.
    """
    self.ambient = [x, y, z, a]
    
  def setDiffuse(self, x, y, z, a):
    """
    Change diffuse parameters.
    """
    self.diffuse = [x, y, z, a]
    
  def setSpecular(self, x, y, z, a):
    """
    Change specular parameters.
    """
    self.specular = [x, y, z, a]
    
  def setConstantAttenuation(self, constant):
    """
    Set the constant attenuation value.
    """
    self.constantAttenuation = constant
    
  def setLinearAttenuation(self, linear):
    """
    Set the linear attenuation value.
    """
    self.linearAttenuation = linear
    
  def setQuadraticAttenuation(self, quadratic):
    """
    Set the quadratic attenuation value.
    """
    self.quadraticAttenuation = quadratic
 
class Scene(object):
  """
  The scene is an object container that renders those objects to
  the screen by calling their respective render methods. It has
  its own coordinate system that corresponds to OpenGL's internal
  coordinate system.
  """
  # Viewpoint information
  angle = 0
  width = 0
  height = 0
  nearDistance = 0
  farDistance = 0
  
  objects = None
  camera = None
  lights = None
  frustum = None

  # Entity brain container
  behaviours = None
  
  # Settings
  cull = False
  showBoundingBoxes = False
  showSubentityBoxes = True
  
  # Physical world
  physicalWorld = None
  space = None
  contactGroup = None
  lastTime = 0
  
  def __init__(self):
    """
    Class constructor.
    """
    self.objects = {}
    self.behaviours = {}
    self.lights = {}
    
    # Create ODE physical world
    self.physicalWorld = ode.World()
    self.physicalWorld.setGravity((0, -9.81, 0))
    self.physicalWorld.setERP(0.8)
    self.physicalWorld.setCFM(1E-5)

    self.space = ode.Space()
    self.contactGroup = ode.JointGroup()
  
  def registerObject(self, obj):
    """
    Registers a new object with the scene.
    """
    if isinstance(obj, Camera):
      self.camera = obj
      return
    
    if isinstance(obj, Light):
      self.lights[obj.objectId] = obj
      return
    
    self.objects[obj.objectId] = obj
  
  def unregisterObject(self, obj):
    """
    Removes an existing object from the scene.
    """
    if isinstance(obj, Camera):
      self.camera = None
      return
    
    if isinstance(obj, Light):
      try:
        del self.lights[obj.objectId]
      except KeyError:
        raise SceneObjectNotFound
      return
    
    try:
      del self.objects[obj.objectId]
    except KeyError:
      raise SceneObjectNotFound
  
  def registerBehaviour(self, behaviour):
    """
    Registres a new entity's behaviour. Entity must previously be a
    part of the scene.
    
    @param behaviour: A valid EntityBehaviour (or subclass) instance
    """
    self.behaviours[behaviour.entity] = behaviour
  
  def getObjectByName(self, name):
    """
    Returns a top-level object identified by its name.
    """
    return self.objects.get[name]
  
  def prepare(self):
    """
    Prepare all objects.
    """
    logger.info("Preparing scene objects, stand by...")
    
    # Prepare frustum and set it to the camera
    self.frustum = Frustum(self.angle, float(self.width) / float(self.height), self.nearDistance, self.farDistance)
    self.camera.setFrustum(self.frustum)
    
    # Prepare lights
    glEnable(GL_LIGHTING)
    for light in self.lights.values():
      light.prepare()
    
    glEnable(GL_TEXTURE_2D)
    glShadeModel(GL_SMOOTH)
    glClearColor(0, 0, 0, 0)
    glClearDepth(1)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
    
    glViewport(0, 0, self.width, self.height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(self.angle, float(self.width) / float(self.height), self.nearDistance, self.farDistance)
    
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    
    # Prepare all objects that have been loaded into the scene
    for obj in self.objects.values():
      try:
        obj.prepare()
        
        # Prepare entity's brain if it has one
        if 'behaviour' in obj.__dict__ and obj.behaviour is not None:
          obj.behaviour.prepare()
      except:
        logger.error("Unhandled exception while preparing an object!")
        logger.error(traceback.format_exc())
        sys.exit(1)
    
    logger.info("Scene preparation completed! Let's render some stuff.")
  
  def render(self):
    """
    Renders all visible objects to the scene.
    """
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    
    # Render the camera first
    if self.camera:
      self.camera.render()
      
    # Render the lights
    for light in self.lights.values():
      light.render()
      
    # Determine visible object by frustum culling
    if self.cull and self.camera.frustum:
      self.__cull(self.objects)
    
    # Now render all other (visible) objects
    for obj in self.objects.values():
      if obj.visible:
        try:
          obj.render()
        except:
          logger.error("Unhandled exception while rendering an object!")
          logger.error(traceback.format_exc())
          sys.exit(1)
      elif isinstance(obj, PhysicalEntity):
        # Just update the position (needed for further frustum checks)
        obj.updateScenePosition()
    
    # TODO: Now render any GUI elements
    
    glutSwapBuffers()

  def __nearCallback(self, args, g1, g2):
    """
    Callback function for collision detection.
    """
    contacts = ode.collide(g1, g2)
    if contacts:
      # Invoke entity's behaviour class with proper arguments
      entity1 = g1.sceneObject
      entity2 = g2.sceneObject
      
      if entity1 and entity1.behaviour:
        entity1.behaviour.collision(entity2)
      
      if entity2 and entity2.behaviour:
        entity2.behaviour.collision(entity1)
    
    world, contactGroup = args
    for c in contacts:
      # TODO: Bounce/MU should be per-object configurable
      c.setBounce(0.2)
      c.setMu(5000)
      
      # Create a new contact joint
      j = ode.ContactJoint(world, contactGroup, c)
      j.attach(g1.getBody(), g2.getBody())
      
  def __cull(self, objects):
    """
    Check for visible objects.
    """
    level = objects.values()
    while len(level):
      children = []
      
      for obj in level:
        if obj.parent:
          # Calculate coordinates relative to the parent
          coordinates = obj.model.center + obj.parent.coordinates
        else:
          coordinates = obj.coordinates
          
        objSphere = (coordinates, obj.model.radius)
        state = self.frustum.sphereInFrustum(*objSphere)
        if state == Frustum.OUTSIDE:
          obj.visible = False
        else:
          obj.visible = True
          if obj.children:
            children += obj.children.values()  # Check the sub entities only if the parent is in the frustum
            
      level = children # Next check the visibility of children of visible objects
  
  def update(self):
    """
    Process any collisions and dynamic physical simulation.
    """
    t = 0.02 - (time.time() - self.lastTime)
    if t > 0:
      time.sleep(t)
    
    # Wakup all behaviour classes
    for behaviour in self.behaviours.values():
      try:
        behaviour.update()
      except:
        logger.error("Unhandled exception while processing entity behaviour!")
        logger.error(traceback.format_exc())
        sys.exit(1)
    
    glutPostRedisplay()
    
    # Perform physical simulation
    for i in xrange(2):
      self.space.collide((self.physicalWorld, self.contactGroup), self.__nearCallback)
      self.physicalWorld.step(0.02 / 2)
      self.contactGroup.empty()
    
    self.lastTime = time.time()

  def listVisible(self, event):
    """
    List all at this moment visible objects.
    """
    def vis(objects):
      l = []
      for obj in objects:
        if obj.visible:
          l.append(obj.model.itemId+"::%s" % vis(obj.children.values()))
      return l
    
    visible = vis(self.objects.values())
    logger.debug("All visible objects: %s" % visible)