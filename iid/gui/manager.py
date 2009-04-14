#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
from OpenGL.GL import *
from OpenGL.GLU import *
import logging

# IID imports
from iid.events import EventDispatcher, EventType
from iid.gui.window import Titlebar, WindowFlags

# Logger for this module
logger = logging.getLogger(__name__)

class WindowManager(object):
  """
  Window manager is the top level manager for GUI events.
  """
  context = None
  windows = None
  widgetStyle = None
  
  # Current instance
  __manager = None
  
  def __init__(self, context):
    """
    Class constructor.
    
    @param context: A valid IID Context instance
    """
    WindowManager.__manager = self
    self.context = context
    self.context.scene.windowManager = self
    self.windows = []
    
    # Register signal handlers
    context.events.subscribe(EventType.MouseMove, self.event)
    context.events.subscribe(EventType.MousePress, self.event)
  
  @staticmethod
  def getManager():
    """
    Returns the current WindowManager instance.
    """
    return WindowManager.__manager
  
  def setWidgetStyle(self, style):
    """
    Sets a widget style to use.
    """
    self.widgetStyle = style
  
  def setFocus(self, window):
    """
    Sets the window that currently has focus by decreasing order
    to all windows previously above this window and setting this
    window's order to maximum value.
    """
    if len(self.windows):
      self.windows[0].active = False
      self.windows[0].emit("Window.lostFocus")
    
    try:
      self.windows.remove(window)
    except ValueError:
      pass
    
    self.windows.insert(0, window)
    window.active = True
    window.emit("Window.gotFocus")
  
  def registerWindow(self, window):
    """
    Registers a new window with this window manager.
    
    @param window: A valid Window instance
    """
    self.setFocus(window)
    
    if window.flags & WindowFlags.Titlebar:
      # Generate a titlebar
      window.titlebar = Titlebar(window)
  
  def event(self, event):
    """
    Event handler (dispatch events to widgets).
    """
    if event.eventType in (EventType.MouseMove, EventType.MousePress):
      # Dispatch to proper widget
      for window in self.windows:
        # Window titlebars are separate entities, since they are drawn
        # by the window manager and are not part of a window
        if window.titlebar.containsCoordinates(event.x, event.y):
          window.titlebar.event(event)
          break
        
        if window.containsCoordinates(event.x, event.y):
          window.event(event)
          break
      else:
        # No window accepted mouse press event, active window looses focus
        if len(self.windows) and event.eventType == EventType.MousePress:
          self.windows[0].active = False
          self.windows[0].emit("Window.lostFocus")
    elif event.eventType == EventType.Keyboard:
      # Get currently focused widget
      if len(self.windows) and self.windows[0].active:
        self.windows[0].event(event)
  
  def render(self):
    """
    Emits paint events to all visible top-level windows in reverse
    order (so windows below everything get painted first).
    """
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    
    glDisable(GL_DEPTH_TEST)
    glDisable(GL_LIGHTING)
    glDisable(GL_TEXTURE_2D)
    
    glMatrixMode(GL_PROJECTION)
    glPushMatrix()
    glLoadIdentity()
    gluOrtho2D(0, self.context.scene.width, 0, self.context.scene.height)
    glScalef(1, -1, 1)
    glTranslatef(0, -self.context.scene.height, 0)
    glMatrixMode(GL_MODELVIEW)
    glPushMatrix()
    glLoadIdentity()
    
    for window in self.windows[::-1]:
      if window.visible:
        self.__paintWindow(window)
    
    glPopMatrix()
    glMatrixMode(GL_PROJECTION)
    glPopMatrix()
    glMatrixMode(GL_MODELVIEW)
    
    glDisable(GL_BLEND)
    glEnable(GL_DEPTH_TEST)
    glEnable(GL_LIGHTING)
    glEnable(GL_TEXTURE_2D)
  
  def __paintWindow(self, window):
    """
    Paints a window to the screen.
    """
    if window.titlebar.visible:
      window.titlebar._paint()
    
    window._paint()
    
