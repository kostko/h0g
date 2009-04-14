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
import pyglet.media as Media
import FTGL

# This seems to be somewhat system dependent (?):
try:
  from PIL import Image
except:
  import Image

# IID imports
from iid.exceptions import *
from iid.storage.items import BasicTexture, BasicModel, BasicMaterial, CompositeModel, BasicMap, Shader, TrueTypeFont
from iid.sound import BasicSound, StreamedSound
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
        currentObject.normals = numpy.zeros((vertexCount, 3))
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
      elif chunkId == 0xA050:
        # Transparency
        materials[currentMaterial].transparency = self.__readDoubleByte(f)[0]
      elif chunkId == 0xA084:
        # Self illuminations (emission)
        materials[currentMaterial].emission = self.__readDoubleByte(f)[0]
      else:
        f.seek(chunkLen - 6, os.SEEK_CUR)
        
      # TODO: transparency and shininess !
       
    # Now set proper material for each polygon of each object and compute normals
    if basic:
      self.__prepareMaterialMap(currentObject, materials)
      self.__computeMeshNormals(currentObject)
    else:
      for child in item.children.values():
        self.__prepareMaterialMap(child, materials)
        self.__computeMeshNormals(child)
    
    f.close()
    
    # Translate model to 'true' center
    self.__moveToCenter(item)

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
  
  def __readDoubleByte(self, f):
    """
    Reads two bytes and returns them in a pair.
    """
    # Move forward
    chunkId, chunkLen = struct.unpack('<HI', f.read(6))
    if chunkId == 0x0030:
      a, b = struct.unpack('<bb', f.read(struct.calcsize('<bb')))
      
    return (a, b)
  
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
  
  def __computeMeshNormals(self, item):
    """
    Computes vertex normals by doing the average.
    """
    tmp = numpy.zeros((len(item.vertices), 1))
    
    for i, p in enumerate(item.polygons):
      side0 = item.vertices[p[0]] - item.vertices[p[1]]
      side1 = item.vertices[p[2]] - item.vertices[p[1]]
      
      normal = numpy.cross(side1, side0)
      n = numpy.linalg.norm(normal)
      if n < 0.0001:
        normal = numpy.array([0., 0., 0.])
      else:
        normal = normal / n
      
      for i in xrange(3):
        item.normals[p[i]] += normal
        tmp[p[i]] += 1
    
    # Compute average
    for i in xrange(len(tmp)):
      if not tmp[i]:
        continue
      
      item.normals[i] /= tmp[i]
      n = numpy.linalg.norm(item.normals[i])
      if n < 0.0001:
        item.normals[i] = numpy.array([0., 0., 0.])
      else:
        item.normals[i] = item.normals[i] / n
    
    del tmp
  
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
    
  def __moveToCenter(self, item):
    """
    Finds the geometric center of the model and translates
    the model so that (0,0,0) is in the geometric center.
    """
    logger.info("Moving model %s's vertices to true center..." % item.itemId)
    
    mind = [None, None, None]
    maxd = [None, None, None]
    
    def check_dimensions(d, min=mind, max=maxd):
      for i in xrange(3):
        if min[i] is None or d[i] < min[i]:
          min[i] = d[i]
        
        if max[i] is None or d[i] > max[i]:
          max[i] = d[i]
          
    def set_center(obj):
      # Calculate geometric center
      lx, ly, lz = [maxd[0] - mind[0], maxd[1] - mind[1], maxd[2] - mind[2]]
      cx, cy, cz = (mind[0] + lx / 2, 
                    mind[1] + ly / 2, 
                    mind[2] + lz / 2)
      
      for i in xrange(len(obj.vertices)):
        x, y, z = obj.vertices[i]
        obj.vertices[i] = x - cx, y - cy, z - cz
      
      if 'scaling' in obj.hints:
        for i in xrange(3):
          obj.dimensions[i] *= obj.hints['scaling'][i]
          
    def set_box(obj, min = mind, max = maxd):
      # Set model dimensions
      obj.mind, obj.maxd = min, max
      obj.dimensions = [max[0] - min[0], max[1] - min[1], max[2] - min[2]]
      lx, ly, lz = obj.dimensions
      center = numpy.array(
        [min[0] + lx / 2, 
         min[1] + ly / 2, 
         min[2] + lz / 2]
      )
      
      # Make a copy for radius calculation
      dimensions = [d for d in obj.dimensions]
      if 'scaling' in obj.hints:
        for i in xrange(3):
          center[i] *= obj.hints['scaling'][i]
          dimensions[i] *= obj.hints['scaling'][i]
      
      # Calculate radius for bounding sphere
      obj.radius = numpy.linalg.norm(numpy.array(dimensions) / 2)
      obj.center = center
    
    if isinstance(item, BasicModel):
      for vertex in item.vertices:
        check_dimensions(vertex)
      set_box(item)
      set_center(item)
      
    else: # Composite model
      for child in item.children.values():
        localMind = [None, None, None]
        localMaxd = [None, None, None]
        
        for vertex in child.vertices:
          check_dimensions(vertex)                        # Update global extremes
          check_dimensions(vertex, localMind, localMaxd)  # Update local extremes
          
        set_box(child, localMind, localMaxd)              # Calculate box dimensions based on local extremes
      
      # Translate vertices relative to the global center
      for child in item.children.values():
        set_center(child)

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
      
      self.__getCoordsProperty(entity, 'pos', e)
      self.__getCoordsProperty(entity, 'rot', e)
      self.__getFloatProperty(entity, 'density', e)
      self.__getFloatProperty(entity, 'mass', e)
      self.__getStorageProperty(entity, 'use_shader', e, item.storage, 'Shader')
      
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
  
  def __getCoordsProperty(self, node, prop, entity):
    """
    Initializes entity's properties from the given XML node.
    """
    x = node.find("./%s" % prop)
    if x is not None:
      try:
        entity.properties[prop] = (
          float(x.findtext("./x")),
          float(x.findtext("./y")),
          float(x.findtext("./z"))
        )
      except (ValueError, TypeError):
        logger.error("Invalid entity property value specified for '%s'!" % prop)
        raise ItemTypeMismatch
  
  def __getStorageProperty(self, node, prop, entity, storage, className):
    """
    Initializes entity's properties from the given XML node.
    """
    x = node.findtext("./%s" % prop)
    if x is not None:
      try:
        item = storage[x]
        if item.__class__.__name__ != className:
          logger.error("Invalid item type specified for entity property '%s'!" % prop)
          raise ItemTypeMismatch
        
        entity.properties[prop] = item
      except KeyError:
        logger.error("Invalid entity property value specified for '%s'!" % prop)
        raise ItemTypeMismatch
  
  def __getFloatProperty(self, node, prop, entity):
    """
    Initializes entity's properties from the given XML node.
    """
    x = node.findtext("./%s" % prop)
    if x is not None:
      try:
        entity.properties[prop] = float(x)
      except ValueError:
        logger.error("Invalid entity property value specified for '%s'!" % prop)
        raise ItemTypeMismatch
  
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

class GLSLImporter(Importer):
  """
  GLSL shader source importer.
  """
  def load(self, item):
    """
    Imports the shader source into specified item.

    @param item: A valid Shader instance
    """
    if not isinstance(item, Shader):
      logger.error("Can only load GLSL sources into Shader type items!")
      raise ItemTypeMismatch
    
    try:
      f = open(self.filename, 'r')
    except IOError:
      logger.error("GLSL source data file '%s' not found!" % self.filename)
      raise ItemFileNotFound
    
    currentShader = None
    
    for line in f:
      line = line.strip()
      if line == '[Vertex_Shader]':
        currentShader = 'vertex'
        item.vertexShaderSource = ''
        continue
      elif line in ('[Fragment_Shader]', '[Pixel_Shader]'):
        currentShader = 'fragment'
        item.fragmentShaderSource = ''
        continue
      
      if currentShader == 'vertex':
        item.vertexShaderSource += line + '\n'
      elif currentShader == 'fragment':
        item.fragmentShaderSource += line + '\n'
    
    f.close()
    
    if not currentShader:
      logger.error('Missing shader declarations in GLSL source file!')
      raise MissingShaderDeclaration

class SoundImporter(Importer):
  """
  Sound file importer.
  """
  def load(self, item):
    """
    Imports a sound into the item.
    """
    if not isinstance(item, BasicSound):
      logger.error("Can only load sounds into BasicSound (and its subclasses) type items!")
      raise ItemTypeMismatch
    
    # Stream sounds for StreamedSound type items
    type = item.__class__ == StreamedSound
    item.source = Media.load(self.filename, streaming=type)
    logger.info("Loaded sound '%s'" % item.itemId)

class FontImporter(Importer):
  """
  Font importer.
  """
  def load(self, item):
    """
    Imports a TrueType font into the item.
    """
    if not isinstance(item, TrueTypeFont):
      logger.error("Can only load sounds into BasicSound (and its subclasses) type items!")
      raise ItemTypeMismatch
    
    try:
      for size in (12, 14, 16):
        font = FTGL.TextureFont(self.filename)
        font.UseDisplayList(True)
        font.FaceSize(size, 75)
        item.sizes[size] = font
    except:
      logger.error("Font data file '%s' not found!" % self.filename)
      raise ItemFileNotFound
    
    logger.info("Loaded TrueType font '%s' via FTGL." % item.itemId)
