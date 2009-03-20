#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
from OpenGL.GL import *
from OpenGL.GLU import *

# IID imports
from iid.storage.base import Item

class BasicModel(Item):
  """
  Represents a loadable 3D model. Polygons are represented using
  a numpy array for efficient storage.
  """
  vertices = None
  polygons = None
  textureMap = None
  hints = None
  
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
    
    for p in self.polygons:
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

class BasicScript(Item):
  pass

class BasicEntity(Item):
  pass

class BasicMap(Item):
  pass
