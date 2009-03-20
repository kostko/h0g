#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
import Image
from OpenGL.GL import *
import logging
import os
import struct
import numpy

# IID imports
from iid.storage.items import BasicTexture, BasicModel
from iid.exceptions import *

# Logger for this module
logger = logging.getLogger(__name__)

class Importer(object):
  """
  An abstract importer.
  """
  filename = None
  
  def __init__(self, filename):
    """
    Class constructor.
    
    @param filename: Filename to import
    """
    self.filename = filename
  
  def load(self, item):
    """
    Imports the file into specified item.
    """
    pass

class PILTextureImporter(Importer):
  """
  Importer for image textures using the PIL module.
  """
  image = None
  
  def load(self, item):
    """
    Imports the texture into specified item.
    
    @param item: A valid BasicTexture instance
    """
    if not isinstance(item, BasicTexture):
      logger.error("Can only load textures into BasicTexture type items!")
      raise ItemTypeMismatch
    
    # Convert the image to RGB, make image the valid size for
    # OpenGL (if it isn't) and create a valid storable texture
    try:
      self.image = Image.open(self.filename)
    except IOError:
      logger.error("Texture data file '%s' not found!" % self.filename)
      raise ItemFileNotFound
    
    self.__ensureRGB()
    item.width, item.height = self.image.size
    item.components, item.format = self.__getCompFormat()
    item.data = self.image.tostring("raw", self.image.mode, 0, -1)
  
  def __ensureRGB(self):
    """
    Convert the image into RGB mode when needed.
    """
    if self.image.mode == 'P':
      # Image is in palletted mode
      self.image = self.image.convert('RGB')
  
  def __getCompFormat(self):
    """
    Returns image component count and OpenGL format.
    """
    if self.image.mode == "RGB":
      length = 3
      format = GL_RGB
    elif self.image.mode in ("RGBA", "RGBX"):
      length = 4
      format = GL_RGBA
    elif self.image.mode == "L":
      length = 1
      format = GL_LUMINANCE
    elif self.image.mode == "LA":
      length = 2
      format = GL_LUMINANCE_ALPHA
    else:
      raise ImageFormatError
    
    return length, format

class SimpleVertexImporter(Importer):
  """
  Simple ASCII format vertex importer.
  """
  pass

class ThreeDSModelImporter(Importer):
  """
  3DS model importer.
  """
  def load(self, item):
    """
    Imports the 3DS model into specified item. Based on format description
    that can be found on:
      http://www.spacesimulator.net/tut4_3dsloader.html
      
    @param item: A valid BasicModel instance
    """
    if not isinstance(item, BasicModel):
      logger.error("Can only load 3D models into BasicModel type items!")
      raise ItemTypeMismatch
    
    try:
      f = open(self.filename, 'rb')
    except IOError:
      logger.error("3DS model data file '%s' not found!" % self.filename)
      raise ItemFileNotFound
    
    # Get file size
    f.seek(0, os.SEEK_END)
    size = f.tell()
    f.seek(0, os.SEEK_SET)
    
    while f.tell() < size:
      chunkId, chunkLen = struct.unpack('<HI', f.read(6))
      
      if chunkId == 0x4d4d:
        # Skip main chunk header
        pass
      elif chunkId == 0x3d3d:
        # Skip 3D editor chunk header
        pass
      elif chunkId == 0x4000:
        # Process object chunk - read object name (actually skip it)
        while f.read(1) != '\x00':
          pass
      elif chunkId == 0x4100:
        # Skip triangular mesh chunk header
        pass
      elif chunkId == 0x4110:
        # Process vertices list
        vertexCount = struct.unpack('<H', f.read(struct.calcsize('<H')))[0]
        item.vertices = numpy.empty((vertexCount, 3))
        item.textureMap = numpy.empty((vertexCount, 2))
        
        for i in xrange(vertexCount):
          item.vertices[i] = struct.unpack('<fff', f.read(struct.calcsize('<fff')))
      elif chunkId == 0x4120:
        # Process polygon list
        polygonCount = struct.unpack('<H', f.read(struct.calcsize('<H')))[0]
        item.polygons = numpy.empty((polygonCount, 3))
        
        for i in xrange(polygonCount):
          a, b, c, flags = struct.unpack('<HHHH', f.read(struct.calcsize('<HHHH')))
          item.polygons[i] = a, b, c
      elif chunkId == 0x4140:
        # Process texture mapping data
        mapCount = struct.unpack('<H', f.read(struct.calcsize('<H')))[0]
        
        for i in xrange(mapCount):
          item.textureMap[i] = struct.unpack('<ff', f.read(struct.calcsize('<ff')))
      else:
        f.seek(chunkLen - 6, os.SEEK_CUR)
    
    f.close()
    
    # Output loaded model statistics
    logger.debug('Loaded 3DS model with %d vertices and %d polygons.' % (vertexCount, polygonCount))

class MapImporter(Importer):
  """
  Map XML descriptor importer.
  """
  pass
