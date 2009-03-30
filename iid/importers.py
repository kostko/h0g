#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
from OpenGL.GL import *
import logging
import numpy
import os
import struct
import xml.etree.cElementTree as XMLTree

# This seems to be somewhat system dependent (?):
try:
  from PIL import Image
except:
  import Image

# IID imports
from iid.exceptions import *
from iid.storage.items import BasicTexture, BasicModel, BasicMaterial, CompositeModel, BasicMap
from iid.scene import Entity

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
  def load(self, item, subtype = BasicModel):
    """
    Imports the 3DS model into specified item. Based on format description
    that can be found on:
      http://www.spacesimulator.net/tut4_3dsloader.html

    @param item: A valid BasicModel or CompositeModel instance
    """
    basic = False          # True if expecting one object
    currentObject = None   # Currently parsed object
    materials = {}         # All materials
    
    # Some statistics
    totalVertices = 0
    totalPolygons = 0
    
    if not isinstance(item, (BasicModel, CompositeModel)):
      logger.error("Can only load 3D models into CompositeModel or BasicModel type items!")
      raise ItemTypeMismatch
    if isinstance(item, BasicModel):
      basic = True
    
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
        # Process object chunk - read object name
        name = self.__readAsciiz(f)
        # Check for 'unsaved' stuff (materials, ...)
        if basic:
          currentObject = item
        else:
          # Create new child
          currentObject = subtype(item.storage, name, item)
          item.children[name] = currentObject
          
        currentObject.materialMap = {}  # Save material mappings per object
        logger.debug("Created new object '%s'" % name)
      elif chunkId == 0x4100:
        # Skip triangular mesh chunk header
        pass
      elif chunkId == 0x4110:
        # Process vertices list
        vertexCount = struct.unpack('<H', f.read(struct.calcsize('<H')))[0]
        currentObject.vertices = numpy.empty((vertexCount, 3))
        currentObject.textureMap = numpy.empty((vertexCount, 2))
        totalVertices += vertexCount

        for i in xrange(vertexCount):
          currentObject.vertices[i] = struct.unpack('<fff', f.read(struct.calcsize('<fff')))
      elif chunkId == 0x4120:
        # Process polygon list
        polygonCount = struct.unpack('<H', f.read(struct.calcsize('<H')))[0]
        currentObject.polygons = numpy.empty((polygonCount, 3))
        totalPolygons += polygonCount

        for i in xrange(polygonCount):
          a, b, c, flags = struct.unpack('<HHHH', f.read(struct.calcsize('<HHHH')))
          currentObject.polygons[i] = a, b, c
      elif chunkId == 0x4140:
        # Process texture mapping data
        mapCount = struct.unpack('<H', f.read(struct.calcsize('<H')))[0]

        for i in xrange(mapCount):
          currentObject.textureMap[i] = struct.unpack('<ff', f.read(struct.calcsize('<ff')))
      elif chunkId == 0x4130:
        # Faces material definition
        # Read name
        name = self.__readAsciiz(f)
        # Add entry
        currentObject.materialMap[name] = []
        # Get number
        faceCount = struct.unpack('<h', f.read(struct.calcsize('<h')))[0]
        # List faces 
        for i in xrange(faceCount):
          faceIndex = struct.unpack('<h', f.read(struct.calcsize('<h')))[0]
          currentObject.materialMap[name].append(faceIndex)     # Register polygon to be drawn by this material
          
        logger.debug("Registered %d polygons with material '%s'" % (faceCount, name))
      elif chunkId == 0xAFFF:
        # Material block - skip
        pass
      elif chunkId == 0xA000:
        # Material name
        currentMaterial = self.__readAsciiz(f)
        materials[currentMaterial] = BasicMaterial(item.storage, currentMaterial, item)
      elif chunkId == 0xA010:
        # Ambient
        materials[currentMaterial].ambient = self.__readRgb(f)
      elif chunkId == 0xA020:
        # Diffuse
        materials[currentMaterial].diffuse = self.__readRgb(f)
      elif chunkId == 0xA030:
        # Specular
        materials[currentMaterial].specular = self.__readRgb(f)
      else:
        f.seek(chunkLen - 6, os.SEEK_CUR)
        
      # TODO: transparency and shininess !
       
    # Now set proper material for each polygon of each object
    if basic:
      self.__prepareMaterialMap(currentObject, materials)
    else:
      for child in item.children.values():
        self.__prepareMaterialMap(child, materials)
    
    f.close()

    # Output loaded model statistics
    logger.debug('Loaded 3DS model with %d vertices and %d polygons.' % (totalVertices, totalPolygons))
    
  def __readAsciiz(self, f):
    """
    Reads a null terminated string from file f.
    
    @return: The string
    """
    name, ch = "", ""
    while ch != '\x00':
      name += ch
      ch = f.read(1)
    return name
  
  def __readRgb(self, f):
    """
    Reads the RGB values following a certain
    material characteristic.
    
    @return: Triplet of RGB floats
    """
    # Move forward and get RGB type
    chunkId, chunkLen = struct.unpack('<HI', f.read(6))
    if chunkId == 0x0010:  # 3x float format
      rgb = struct.unpack('<fff', f.read(struct.calcsize('<fff')))
    elif chunkId == 0x0011:  # 24 bit format
      rgb = struct.unpack('<BBB', f.read(struct.calcsize('<BBB')))
      rgb = tuple([float(x)/255 for x in rgb])  # Convert to floats
      
    return rgb

  def __prepareMaterialMap(self, item, materials):
    """
    Prepares a map: polygon number -> the material with which
    the polygon must be drawn.
    """
    try:
      defaultMaterial = materials[" -- default --"]   # Check if default material is defined
    except:
      defaultMaterial = None
    
    if item.polygons == None:
      # No polygons to map
      return 
    
    polygonCount = len(item.polygons)
    polygonMaterial = [defaultMaterial]*polygonCount
    for material, polygons in item.materialMap.items():
      for p in polygons:
        polygonMaterial[p] = materials[material]      # Set reference to proper material object

    # Set to item
    item.polygonMaterial = polygonMaterial

class MapImporter(Importer):
  """
  Map XML descriptor importer.
  """
  def load(self, item):
    """
    Imports the map into specified item.

    @param item: A valid BasicMap instance
    """
    if not isinstance(item, BasicMap):
      logger.error("Can only load maps into BasicMap type items!")
      raise ItemTypeMismatch
    
    xml = XMLTree.parse(self.filename)
    m = xml.getroot()
    if m.tag != "map":
      logger.error("Invalid map XML file!")
      raise ItemFileNotFound
    
    # Parse map properties
    item.properties = {
      'gravity' : float(m.findtext("./properties/physics/gravity"))
    }
    
    # TODO Get map controller script
    # TODO Get imports
    # XXX need scripting support first
    scripts = {}
    
    # Parse scene descriptor
    item.scene = []
    anonymousId = 0
    for entity in m.findall("./scene/entity"):
      # Create main entity descriptor
      e = BasicMap.EntityDescriptor()
      if 'id' in entity.attrib:
        e.objectId = entity.attrib['id']
      else:
        e.objectId = "anonymous_%d" % anonymousId
        anonymousId += 1
      
      if 'model' in entity.attrib:
        e.model = item.storage[entity.attrib['model']]
        if not isinstance(e.model, (BasicModel, CompositeModel)):
          logger.error("Invalid entity model specified (not a BasicModel/CompositeModel subclass!")
          raise ItemTypeMismatch
      
      if 'texture' in entity.attrib:
        e.texture = item.storage[entity.attrib['texture']]
        if not isinstance(e.texture, BasicTexture):
          logger.error("Invalid entity texture specified (not a BasicTexture subclass!")
          raise ItemTypeMismatch
      
      e.entityClass = self.__loadEntityClass(entity.attrib.get('class', 'iid.scene.Entity'), scripts)
      
      pos = entity.find("./pos")
      if pos is not None:
        try:
          e.properties['pos'] = (
            float(pos.findtext("./x")),
            float(pos.findtext("./y")),
            float(pos.findtext("./z"))
          )
        except (ValueError, TypeError):
          logging.error("Invalid entity coordinates specified!")
          raise ItemTypeMismatch
      
      rot = entity.find("./rot")
      if rot is not None:
        try:
          e.properties['rot'] = (
            float(rot.findtext("./x")),
            float(rot.findtext("./y")),
            float(rot.findtext("./z"))
          )
        except (ValueError, TypeError):
          logging.error("Invalid entity rotation specified!")
          raise ItemTypeMismatch
      
      # Add subentity descriptors (if any)
      if isinstance(e.model, CompositeModel):
        for subentity in entity.findall("./subentity"):
          se = BasicMap.EntityDescriptor()
          se.parent = e
          
          texture = subentity.findtext("./texture")
          if texture:
            se.texture = item.storage[texture]
            if not isinstance(e.texture, BasicTexture):
              logger.error("Invalid sub-entity texture specified (not a BasicTexture subclass!")
              raise ItemTypeMismatch
          
          entityClass = subentity.findtext("./class")
          if entityClass:
            se.entityClass = self.__loadEntityClass(entityClass, scripts)
          
          se.match = subentity.attrib.get('match', '*')
          e.children.append(se)
      
      item.scene.append(e)
    
    # Parse signal/slot connections
    # TODO implement with scripts
  
  def __loadEntityClass(self, classPath, scripts):
    """
    Loads an entity class.
    
    @param classPath: A valid entity class path
    @param scripts: A list of loaded scripts
    """
    if '.' in classPath:
      # Module path
      try:
        module = classPath[0:classPath.rfind('.')]
        className = classPath[classPath.rfind('.') + 1:]
        m = __import__(module, globals(), locals(), [className], -1)
        cls = m.__dict__[className]
      except KeyError:
        logger.error("Entity class '%s' not found!" % classPath)
        raise ItemImporterNotFound
    else:
      # Just a class name, should be already available
      # TODO implement with scripts
      cls = None
    
    if not issubclass(cls, Entity):
      logger.error("Specified entity class is not a subclass of Entity!")
      raise ItemTypeMismatch
    
    return cls
