#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
from OpenGL.GL import *
from OpenGL.GLUT import *
import FTGL

# IID imports
from iid.gui.widget import Widget

class Label(Widget):
  """
  A label for displaying text.
  """
  text = ""
  
  # Properties
  __font = None
  __fontSize = 14
  __color = (1., 1., 1., 1.)
  
  def __init__(self, parent):
    """
    Class constructor.
    """
    super(Label, self).__init__(parent)
    self.setFont(self.getStyle().generalFont)
  
  def setFont(self, font, size = 14):
    """
    Set label's font.
    
    @param font: A valid TrueTypeFont instance
    @param size: Wanted font size
    """
    self.__font = font.getSize(size)
    self.__fontSize = size
  
  def setColor(self, color):
    """
    Set label's color.
    """
    self.__color = color
  
  def paint(self):
    """
    Output text.
    """
    if not self.__font:
      return
    
    # Set proper text color
    glColor4fv(self.__color)
    
    # NOTE: Glyphs are drawn in some reversed coordinate system so rotation and
    #       negative positions are needed.
    
    glEnable(GL_TEXTURE_2D)
    glPushMatrix()
    glRotatef(180., 1, 0, 0)
    glTranslatef(self.x, -self.y - self.__fontSize, 0)
    self.__font.Render(self.text)
    glPopMatrix()
    glDisable(GL_TEXTURE_2D)
