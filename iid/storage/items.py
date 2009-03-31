#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
from OpenGL.GL import *
from OpenGL.GLU import *
import logging
import re

# IID imports
from iid.storage.base import Item, Container
from iid.scene import Entity

# Logger for this module
logger = logging.getLogger(__name__)

class BasicModel(Item):
  """
  Represents a loadable 3D model. Polygons are represented using
  a numpy array for efficient storage.
  """
  vertices = None
  polygons = None
  textureMap = None
  hints = None
  polygonMaterial = None    # Maps a material to a certain polygon
  
  # Model dimensions (for bounding box)
  dimensions = None
  mind = None
  maxd = None
  
  # OpenGL model identifier
  __modelId = None
  
  def __init__(self, storage, itemId, parent = None):
    """
    Class constructor.
    """
    super(BasicModel, self).__init__(storage, itemId, parent)
    if parent != None and isinstance(parent, CompositeModel):
      # Inherit parent's hints
      self.hints = parent.hints
    else:
      self.hints = {}
  
  def prepare(self):
    """
    Prepares the model for drawing.
    
    @return: OpenGL list identifier
    """
    if self.__modelId:
      return self.__modelId
    
    # Prepare materials (this generates the call lists)
    if self.polygonMaterial != None:
      for p in self.polygonMaterial:
        p.prepare()
    
    self.__modelId = glGenLists(1)
    glNewList(self.__modelId, GL_COMPILE)
    glPushMatrix()
    
    # Load object's vertices and texture mapping
    if 'scaling' in self.hints:
      # If scaling hints have been given, let's scale the model
      glScalef(*self.hints['scaling'])
    
    if len(self.polygons):
      # There are actual polygons (=faces)
      glBegin(GL_TRIANGLES)
      
      for i, p in enumerate(self.polygons):
        # Prepare materials
        if self.polygonMaterial[i]:
          glCallList(self.polygonMaterial[i].prepare())
        
        glTexCoord2fv(self.textureMap[p[0]])
        glVertex3fv(self.vertices[p[0]])
        
        glTexCoord2fv(self.textureMap[p[1]])
        glVertex3fv(self.vertices[p[1]])
        
        glTexCoord2fv(self.textureMap[p[2]])
        glVertex3fv(self.vertices[p[2]])
      
      glEnd()
    
    glPopMatrix()
    glEndList()
        
    return self.__modelId
  
  def destroy(self):
    """
    Destroys the given model.
    """
    glDeleteLists(self.__modelId, 1)

class BasicTexture(Item):
  """
  Represents a loadable texture. The actual texture is represented
  using a numpy array.
  """
  width = 0
  height = 0
  components = 0
  format = None
  data = None
  
  # OpenGL texture identifier
  __textureId = None
  
  def prepare(self):
    """
    Prepares the texture for drawing on the specified scene by
    mapping it into OpenGL.
    
    @return: OpenGL texture identifier
    """
    if self.__textureId:
      return self.__textureId
    
    self.__textureId = glGenTextures(1)
    
    # Load into OpenGL
    glBindTexture(GL_TEXTURE_2D, self.__textureId)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST)
    
    gluBuild2DMipmaps(GL_TEXTURE_2D, self.components, self.width, self.height, self.format, GL_UNSIGNED_BYTE, self.data)
    
    return self.__textureId
  
  def destroy(self):
    """
    Destroys the given texture.
    """
    glDeleteTextures([self.__textureId])

class BasicMaterial(Item):
  """
  Represents one material definition.
  """
  # Defaults
  ambient = (0.2,0.2,0.2)
  diffuse = (0.8,0.8,0.8)
  specular = (1.0,1.0,1.0)
  shininess = 0.0
  
  # OpenGL texture identifier
  __materialId = None
  
  def prepare(self):
    """
    Generates a list for this type of material.
    
    @return: OpenGL texture identifier
    """
    if self.__materialId:
      return self.__materialId
    
    self.__materialId = glGenLists(1)
    glNewList(self.__materialId, GL_COMPILE)
    glMaterialfv(GL_FRONT, GL_AMBIENT, self.ambient)
    glMaterialfv(GL_FRONT, GL_DIFFUSE, self.diffuse)
    glMaterialfv(GL_FRONT, GL_SPECULAR, self.specular)
    glMaterialfv(GL_FRONT, GL_SHININESS, self.shininess)
    glEndList()
    
    return self.__materialId

class BasicScript(Item):
  pass

class BasicEntity(Item):
  pass

class BasicMap(Item):
  """
  A map contains scene properties (physical constants, ...), script references,
  a scene description and signal connections.
  """
  properties = None
  controller = None
  scene = None
  
  class EntityDescriptor(object):
    """
    Entity descriptor.
    """
    objectId = None
    model = None
    texture = None
    entityClass = None
    match = None
    properties = None
    
    # Entity hierarchy
    parent = None
    children = None
    
    def __init__(self):
      """
      Class constructor.
      """
      self.properties = {}
      self.children = []
  
  def load(self, scene):
    """
    Loads map components into the scene.
    """
    logger.info("Loading map '%s' into current scene..." % self.itemId)
    
    # Setup scene properties
    scene.physicalWorld.setGravity((0, self.properties['gravity'], 0))
    
    # Create entities
    for entity in self.scene:
      logger.info("Spawning entity class '%s' (objectId = '%s')." % (entity.entityClass.__name__, entity.objectId))
      self.__createEntity(scene, entity)
    
    logger.info("Map loaded!")
  
  def __createEntity(self, scene, entity):
    """
    Spawn entity and its subentities.
    
    @param scene: Scene instance
    @param entity: An entity descriptor
    """
    e = entity.entityClass(scene, entity.objectId, entity.model, entity.texture)
    e.setVisible(True)
    
    if 'pos' in entity.properties:
      e.setCoordinates(*entity.properties['pos'])
    
    if 'rot' in entity.properties:
      e.setRotation(*entity.properties['rot'])
    
    # Add subentities when model has them
    if 'children' in entity.model.__dict__:
      rpl = (
        ('.', r'\.'),
        ('?', '.'),
        ('*', '.*?')
      )
      regexps = []
      for subentity in entity.children:
        for s, r in rpl:
          subentity.match = subentity.match.replace(s, r)
        
        regexps.append((re.compile(subentity.match), subentity))
      
      for objectId, model in e.model.children.iteritems():
        for regexp, subentity in regexps:
          if regexp.match(objectId):
            se = subentity.entityClass(scene, objectId, model, subentity.texture, e)
            break
        else:
          # No regular expression has matched, default to Entity with no textures
          se = Entity(scene, objectId, model, None, e)
        
        se.setVisible(True)
    
    scene.registerObject(e)

class CompositeModel(Container):
  """
  'Virtual model' for storing models consisting 
  of multiple objects. 
  """
  hints = None
  # Model dimensions (for bounding box)
  dimensions = None
  mind = None
  maxd = None
  
  def __init__(self, storage, containerId, parent = None):
    """
    Class constructor.
    
    @param storage: A valid item storage
    @param itemId: Unique container identifier
    @param parent: Parent container
    """
    super(CompositeModel, self).__init__(storage, containerId, parent)
    self.hints = {}
  
  def prepare(self):
    """
    This object cannot be prepared so it returns None. It only calculates
    bounding box dimensions of this composite model.
    """
    mind = [None, None, None]
    maxd = [None, None, None]
    
    def check_dimensions(d):
      for i in xrange(3):
        if mind[i] is None or d[i] < mind[i]:
          mind[i] = d[i]
        
        if maxd[i] is None or d[i] > maxd[i]:
          maxd[i] = d[i]
    
    for model in self.children.values():
      model.prepare()
      
      check_dimensions(model.mind)
      check_dimensions(model.maxd)
    
    # Set model dimensions
    self.mind, self.maxd = mind, maxd
    self.dimensions = [maxd[0] - mind[0], maxd[1] - mind[1], maxd[2] - mind[2]]
    
    # Apply scaling factors
    if 'scaling' in self.hints:
      for i in xrange(3):
        self.dimensions[i] *= self.hints['scaling'][i]
    
    return None
