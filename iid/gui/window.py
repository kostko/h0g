#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
from OpenGL.GL import *
from OpenGL.GLUT import *

# IID imports
from iid.gui.widget import Widget
from iid.events import EventDispatcher, EventType

class WindowState:
  Normal = 0
  InMove = 1

class WindowFlags:
  Border = 1
  Titlebar = 2

class Titlebar(Widget):
  """
  Window's titlebar widget.
  """
  text = ""
  
  def __init__(self, parent):
    """
    Class constructor.
    """
    super(Titlebar, self).__init__(parent, insertChild = False)
    
    # Subscribe to some signals
    self.parent.subscribe("Widget.resized", self.windowResized)
    self.parent.subscribe("Widget.moved", self.windowMoved)
    self.subscribe("Widget.focused", self.parent.setFocus)
    
    # Position the titlebar
    self.windowResized(self.parent.width, self.parent.height)
    self.windowMoved(self.parent.x, self.parent.y)
  
  def windowResized(self, width, height):
    """
    Window resize callback.
    """
    self.setSize(width, 20)
  
  def windowMoved(self, x, y):
    """
    Window move callback.
    """
    self.setPosition(x, y - self.height)
  
  def eventMousePress(self, event):
    """
    Process window movements.
    """
    if event.button == GLUT_LEFT_BUTTON and event.state == GLUT_DOWN:
      self.parent.startMove()
    elif event.button == GLUT_LEFT_BUTTON and event.state == GLUT_UP:
      self.parent.endMove()
    
    return True
  
  def paint(self):
    """
    The titlebar.
    """
    if self.parent.state == WindowState.InMove:
      glColor4f(0.62, 0.05, 1.0, 0.9 if self.parent.active else 0.75)
    else:
      glColor4f(0.62, 0.05, 1.0, 0.7 if self.parent.active else 0.55)
    
    glBegin(GL_QUADS)
    glVertex2i(self.x, self.y)
    glVertex2i(self.x + self.width, self.y)
    glVertex2i(self.x + self.width, self.y + self.height)
    glVertex2i(self.x, self.y + self.height)
    glEnd()
    
    # TODO: Draw text

class Window(Widget):
  """
  A window is a window.
  """
  caption = ""
  state = WindowState.Normal
  flags = 0
  active = False
  
  # Subwidgets (set by the window manager)
  titlebar = None
  
  def __init__(self, parent, caption = "Untitled", flags = WindowFlags.Border | WindowFlags.Titlebar):
    """
    Class constructor.
    
    @param parent: The parent WindowManager instance
    @param caption: Window caption
    """
    super(Window, self).__init__(parent)
    self.caption = caption
    self.flags = flags
    
    # Register the window with the window manager
    parent.registerWindow(self)
  
  def startMove(self):
    """
    Start window move.
    """
    self.state = WindowState.InMove
      
    # We also need global mouse move events so we can follow the mouse while
    # the window is moving (otherwise a quick movement might loose focus)
    EventDispatcher.getDispatcher().subscribe(EventType.MouseMove, self.eventMouseMoveGlobal)
    EventDispatcher.getDispatcher().subscribe(EventType.MousePress, self.eventMousePress)
  
  def endMove(self):
    """
    End window move.
    """
    self.state = WindowState.Normal
      
    # Unsubscribe from special events
    EventDispatcher.getDispatcher().unsubscribe(EventType.MouseMove, self.eventMouseMoveGlobal)
    EventDispatcher.getDispatcher().unsubscribe(EventType.MousePress, self.eventMousePress)
  
  def eventMousePress(self, event):
    """
    Move the window when ALT key is pressed.
    """
    if event.button == GLUT_LEFT_BUTTON and event.state == GLUT_DOWN and glutGetModifiers() == GLUT_ACTIVE_CTRL:
      self.startMove()
      return False
    elif event.button == GLUT_LEFT_BUTTON and event.state == GLUT_UP:
      self.endMove()
    
    return True
  
  def eventMouseMoveGlobal(self, event):
    """
    Process window movements.
    """
    if self.state == WindowState.InMove:
      # Change window position
      self.setPosition(
        self.x + event.dx,
        self.y + event.dy
      )
    
    return True
  
  def setCaption(self, caption):
    """
    Sets window's caption.
    """
    self.caption = caption
  
  def paint(self):
    """
    Paints this window to the screen.
    """
    glColor4f(0.12, 0.48, 1.0, 0.4 if self.active else 0.30)
    glBegin(GL_QUADS)
    glVertex2i(self.x, self.y)
    glVertex2i(self.x + self.width, self.y)
    glVertex2i(self.x + self.width, self.y + self.height)
    glVertex2i(self.x, self.y + self.height)
    glEnd()
