#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
import xml.etree.cElementTree as XMLTree
import logging
import os
import inspect

# IID imports
from iid.storage.base import Item, Container
from iid.storage.items import BasicModel, BasicTexture, BasicScript, BasicEntity, BasicMap, CompositeModel, Shader
from iid.storage.opcodes import ArgumentOpcodes
from iid.exceptions import *

# Logger for this module
logger = logging.getLogger(__name__)

class ItemStorage(object):
  """
  An addressable item storage that can store different kinds of
  items (models, textures, audio, ...). Items are ordered in a
  hierarchy that can be addressed via paths.
  """
  __dataDirectory = None
  __rootContainer = None
  
  def __init__(self, dataDirectory):
    """
    Class constructor.
    
    @param dataDirectory: A directory that contains the actual items.
    """
    self.__dataDirectory = dataDirectory
    self.__rootContainer = Container(self, '')
    self.__rootContainer.path = dataDirectory
    
    # Read item manifest and load the items
    self.__load()
  
  def __load(self):
    """
    Loads all items into memory by parsing the manifest and invoking
    all specified loaders while building a hierarchy. Any errors will
    abort the loading process.
    """
    logger.info("Loading data storage manifest file...")
    
    xml = XMLTree.parse(os.path.join(self.__dataDirectory, "manifest.xml"))
    manifest = xml.getroot()
    if manifest.tag != "manifest":
      raise CorruptedManifest
    
    version = manifest.attrib['version']
    logger.info("Got IID manifest file, version %s." % version)
    
    if version == "1.0":
      # Parse the manifest file
      self.__loadContainer(manifest, self.__rootContainer)
    else:
      logger.error("Unsupported manifest file version!")
      raise InvalidManifestVersion
    
    logger.info("manifest.xml loaded!")
  
  def __loadContainer(self, containerNode, parent):
    """
    Loads data from a container node.
    """
    for child in containerNode:
      if child.tag == "container":
        # Create a new subcontainer
        container = self.createContainer(
          child.attrib['id'],
          child.attrib['path'] if 'path' in child.attrib else '',
          parent
        )
        
        # Parse container contents
        self.__loadContainer(child, container)
      elif child.tag == "item":
        # Parse arguments
        arguments = []
        for argNode in child:
          arguments.append((argNode.tag, argNode.attrib))
        
        # Create the actual item
        self.createItem(
          child.attrib['name'],
          child.attrib['type'],
          parent,
          arguments
        )
      else:
        raise CorruptedManifest
  
  def createContainer(self, name, path, parent = None):
    """
    Creates a new container item.
    
    @param name: Container name
    @param path: Container filesystem path (optional)
    @param parent: Parent container
    @return: The newly created container
    """
    logger.debug("Creating a container '%s' (path = %s)." % (name, path))
    
    # Create the container instance
    container = Container(self, name, parent)
    container.path = path
    parent.children[name] = container
    return container
  
  def createItem(self, name, type, parent, arguments = None):
    """
    Creates a new item.
    
    @return: The newly created item
    """
    logger.debug("Creating an item '%s' (container = %s)." % (name, parent.itemId))
    
    # Create item instance
    try:
      itemClass = globals()[type]
      if not issubclass(itemClass, Item) or itemClass == Container:
        raise KeyError
      
      item = itemClass(self, name, parent)
    except KeyError:
      logger.error("Unsupported item type '%s'!" % type)
      raise InvalidItemType
    
    # Process arguments
    if arguments:
      for arg in arguments:
        ArgumentOpcodes.call(arg, item)
    
    # Insert into hierarchy
    parent.children[name] = item
    return item
  
  def __getitem__(self, path):
    """
    Retrieves an item from the item store. If the item does not
    exist, KeyError is raised.
    
    @param path: A valid item path
    @return: An item instance
    """
    container = self.__rootContainer
    path = path.split("/")
    
    for atom in path[1:]:
      if atom not in container.children:
        raise KeyError
      
      container = container.children[atom]
    
    return container
