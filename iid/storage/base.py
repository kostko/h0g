#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
import logging
import os

# IID imports
from iid.exceptions import *

# Logger for this module
logger = logging.getLogger(__name__)

class Item(object):
  """
  An abstract class that represents items.
  """
  storage = None
  itemId = None
  parent = None
  
  def __init__(self, storage, itemId, parent = None):
    """
    Class constructor.
    
    @param storage: A valid item storage
    @param itemId: Unique item identifier
    @param parent: Parent item
    """
    self.storage = storage
    self.itemId = itemId
    self.parent = parent
  
  def resolveFullPath(self, path = ""):
    """
    Returns the items full filesystem path.
    
    @param path: Item-relative path to resolve
    """
    fullItemPath = [path]
    parent = self.parent
    while parent is not None:
      if not parent.path:
        logger.error("Physical files not allowed in virtual containers!")
        raise PhysicalFilesNotAllowedInVirtualContainer
      
      fullItemPath.insert(0, parent.path)
      parent = parent.parent
    
    return os.path.join(*fullItemPath)

class Container(Item):
  """
  Item container.
  """
  children = None
  path = ""
  
  def __init__(self, storage, containerId, parent = None):
    """
    Class constructor.
    
    @param storage: A valid item storage
    @param itemId: Unique container identifier
    @param parent: Parent container
    """
    super(Container, self).__init__(storage, containerId, parent)
    self.children = {}
