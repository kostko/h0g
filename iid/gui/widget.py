#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
from OpenGL.GL import *
from OpenGL.GLUT import *
import logging

# IID imports
from iid.events import Signalizable, EventType

# Logger for this module
logger = logging.getLogger(__name__)

class Widget(Signalizable):
  """
  A paintable GUI object.
  """
  parent = None
  children = None
  
  # Widget coordinates and size
  x = 0
  y = 0
  width = 0
  height = 0
  
  # Other properties
  alpha = 1.0
  visible = True
  
  # Widget that currently has focus
  focusedChild = None
  
  def __init__(self, parent, insertChild = True):
    """
    Class constructor.
    
    @param parent: A parent widget
    """
    super(Widget, self).__init__()
    self.parent = parent
    self.children = []
    
    if parent and insertChild and isinstance(parent, Widget):
      parent.children.append(self)
    
    logger.debug("Created widget '%s' with parent '%s'." % (self.__class__.__name__, self.parent.__class__.__name__))
  
  def setPosition(self, x, y):
    """
    Changes widget's position.
    """
    self.x, self.y = x, y
    self.emit("Widget.moved", x, y)
  
  def setSize(self, width, height):
    """
    Changes widget's size.
    """
    self.width, self.height = width, height
    self.emit("Widget.resized", width, height)
  
  def containsCoordinates(self, x, y):
    """
    Returns true if this widget contains the following parent-relative
    coordinates.
    
    @param x: Parent-relative X coordinate
    @param y: Parent-relative Y coordinate
    """
    return self.visible and (self.x <= x <= self. x + self.width) and (self.y <= y <= self.y + self.height)
  
  def setFocus(self, widget = None):
    """
    Sets the focus to a widget.
    """
    if self.focusedChild is not None and self.focusedChild == widget:
      return
    
    self.parent.setFocus(self)
    
    if widget:
      self.focusedChild = widget
    
    # Emit a signal
    self.emit("Widget.focused")
  
  def eventMouseMove(self, event):
    """
    Called when a mouse event ocurrs on this widget. Should return True when
    event should be propagated to child widgets.
    
    @param event: A valid MouseMoveEvent instance
    """
    return True
  
  def eventMousePress(self, event):
    """
    Called when a mouse event ocurrs on this widget. Should return True when
    event should be propagated to child widgets.
    
    @param event: A valid MousePressEvent instance
    """
    return True
  
  def eventMouse(self, event):
    """
    Called when a mouse event ocurrs on this widget. Should return True when
    event should be propagated to child widgets.
    
    @param event: A valid MouseEvent instance
    """
    if event.eventType == EventType.MouseMove:
      return self.eventMouseMove(event)
    elif event.eventType == EventType.MousePress:
      # Widgets get focus on press events by default
      if event.state == GLUT_DOWN:
        self.setFocus()
      
      return self.eventMousePress(event)
  
  def eventKeyPressed(self, event):
    """
    Called when a key press event ocurrs on this widget. Should return True
    when event should be propagated to child widgets.
    
    @param event: A valid KeyboardEvent instance
    """
    return True
  
  def event(self, event):
    """
    Called when an event ocurrs.
    
    @param event: A valid Event instance
    """
    if event.eventType in (EventType.MouseMove, EventType.MousePress):
      event.x -= self.x
      event.y -= self.y
      
      # Do local mouse press processing
      if self.eventMouse(event):
        # Only continue when handler returns True, otherwise this event is
        # considered handled by the current widget
        for widget in self.children:
          if widget.containsCoordinates(event.x, event.y):
            widget.event(event)
            break
    elif event.eventType == EventType.Keyboard:
      # Do local key press processing first
      if self.eventKeyPressed(event) and self.focusedChild and self.focusedChild.visible:
        # Only continue when handler returns True, otherwise this event is
        # considered handled by the current widget
        self.focusedChild.event(event)
  
  def paint(self):
    """
    This method should be overridden by individual widgets to actually
    paint via OpenGL.
    """
    pass
  
  def _paint(self):
    """
    Paints ourselves and then all children.
    """
    self.paint()
    
    glPushMatrix()
    glTranslatef(self.x, self.y, 0)
    
    for child in self.children:
      if child.visible:
        child._paint()
    
    glPopMatrix()
