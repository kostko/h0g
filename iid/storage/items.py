#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
from OpenGL.GL import *
from OpenGL.GLU import *

# IID imports
from iid.storage.base import Item, Container

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
  
  # OpenGL model identifier
  __modelId = None
  
  def __init__(self, storage, itemId, parent = None):
    """
    Class constructor.
    """
    super(BasicModel, self).__init__(storage, itemId, parent)
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
    
    # Load object's vertices and texture mapping
    if 'scaling' in self.hints:
      # If scaling hints have been given, let's scale the model
      glScalef(*self.hints['scaling'])
    
    mind = [None, None, None]
    maxd = [None, None, None]
    
    def check_dimensions(d):
      for i in xrange(3):
        if mind[i] is None or d[i] < mind[i]:
          mind[i] = d[i]
        
        if maxd[i] is None or d[i] > maxd[i]:
          maxd[i] = d[i]
    
    glBegin(GL_TRIANGLES)
    
    for i, p in zip(xrange(len(self.polygons)), self.polygons):
      
      # Prepare materials
      if self.polygonMaterial[i]:
        glCallList(self.polygonMaterial[i].prepare())
      
      glTexCoord2fv(self.textureMap[p[0]])
      glVertex3fv(self.vertices[p[0]])
      check_dimensions(self.vertices[p[0]])
      
      glTexCoord2fv(self.textureMap[p[1]])
      glVertex3fv(self.vertices[p[1]])
      check_dimensions(self.vertices[p[1]])
      
      glTexCoord2fv(self.textureMap[p[2]])
      glVertex3fv(self.vertices[p[2]])
      check_dimensions(self.vertices[p[2]])
    
    glEnd()
    glEndList()
    
    # Set model dimensions
    self.dimensions = [maxd[0] - mind[0], maxd[1] - mind[1], maxd[2] - mind[2]]
    if 'scaling' in self.hints:
      for i in xrange(3):
        self.dimensions[i] *= self.hints['scaling'][i]
    
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
  pass

class CompositeModel(Container):
  """
  'Virtual model' for storing models consisting 
  of multiple objects. 
  """
  pass