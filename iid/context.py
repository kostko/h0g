#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
import logging
import ConfigParser as configparser
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import sys

# IID imports
from iid.scene import Scene
from iid.storage.itemstorage import ItemStorage
from iid.events import EventDispatcher
from iid.exceptions import *

# Logger for this module
logger = logging.getLogger(__name__)

class Context(object):
  """
  IID engine context - holds the storage system and the scene
  together.
  """
  scene = None
  storage = None
  events = None
  config = None
  
  def initAll(self, title = 'Infinite Improbability Drive', dataDirectory = '.'):
    """
    Initializes all subsystems in the proper order.
    """
    self.initOpenGL(title)
    self.initStorage(dataDirectory)
    self.initEvents()
    self.initScene()
  
  def initStorage(self, dataDirectory = '.'):
    """
    Initializes the storage subsystem.
    """
    logger.info("Initializing data storage...")
    
    try:
      self.storage = ItemStorage(dataDirectory)
    except ItemStorageException:
      sys.exit(1)
    
    logger.info("Data storage init done.")
  
  def initScene(self):
    """
    Initializes the scene.
    """
    logger.info("Initializing the scene...")
    self.scene = Scene()
    logger.info("Scene init done.")
  
  def initEvents(self):
    """
    Initializes the event dispatcher.
    """
    logger.info("Initializing the event dispatcher...")
    self.events = EventDispatcher()
    logger.info("Event dispatcher init done.")
  
  def initOpenGL(self, title = 'Infinite Improbability Drive'):
    """
    Initializes basic OpenGL window via GLUT.
    """
    logger.info("Initializing OpenGL subsystem...")
    glutInit()
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH)
    
    glutInitWindowPosition(0, 0)
    glutInitWindowSize(
      self.config.getint('Window', 'width'),
      self.config.getint('Window', 'height')
    )
    glutCreateWindow(title)
    logger.info("OpenGL subsystem init done.")
  
  def loadConfiguration(self, filename = 'iid.cfg'):
    """
    Loads engine configuration.
    """
    logger.info("Loading engine configuration from '%s'..." % filename)
    self.config = configparser.RawConfigParser()
    self.config.read(filename)
    
    if not self.config.has_section('Window'):
      self.config.add_section('Window')
      self.config.set('Window', 'width', '1024')
      self.config.set('Window', 'height', '768')
    
    logger.info("Engine configuration loaded.")
  
  def saveConfiguration(self, filename = 'iid.cfg'):
    """
    Saves engine configuration.
    """
    self.config.write(filename)
  
  def execute(self):
    """
    Executes the main event loop and starts the engine.
    """
    logger.info("Preparing to enter the GLUT event loop...")
    
    # Register callbacks
    glutDisplayFunc(self.__displayCallback)
    glutIdleFunc(self.__updateCallback)
    glutKeyboardFunc(self.events.eventKeyboardInput)
    glutMouseFunc(self.events.eventMouseClickInput)
    glutPassiveMotionFunc(self.events.eventMouseMoveInput)
    
    # Prepare the scene
    self.scene.width = self.config.getint('Window', 'width')
    self.scene.height = self.config.getint('Window', 'height')
    # FIXME This should be done on map load
    self.scene.prepare()
    
    # Enter GLUT main loop
    logger.info("Event loop entered.")
    glutMainLoop()
  
  def __displayCallback(self):
    """
    Render the scene here.
    """
    self.scene.render()
  
  def __updateCallback(self):
    """
    Do event processing and scene calculations here.
    """
    self.scene.update()
