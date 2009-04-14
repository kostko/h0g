#
# Infinite Improbability Drive test script
#
import logging
import sys

# IID imports
from iid.context import Context
from iid.scene import Entity, PhysicalEntity, Camera, Light
from iid.events import EventType
from iid.gui.window import Window
from iid.gui.style import WidgetStyle

# Setup logger
logging.basicConfig(level = logging.DEBUG,
                    format = '%(asctime)s %(levelname)-8s [%(name)s] %(message)s',
                    datefmt = '%a, %d %b %Y %H:%M:%S')

# Setup IID context
c = Context()
c.loadConfiguration()
c.initAll()

cam = Camera(c.scene, 'camera')
cam.setCoordinates(0, 0, 6)
cam.lookAt(0, 0, 0)
c.scene.registerObject(cam)

light1 = Light(c.scene, 'light1')
light1.setCoordinates(0, 10, 10, 1)
c.scene.registerObject(light1)

c.scene.cull = False
c.scene.showBoundingBoxes = True
c.scene.showSubentityBoxes = False

# Load a map
c.storage['/Maps/first'].load(c.scene)

# Setup a widget style
style = WidgetStyle()
style.generalFont = c.storage['/Fonts/verdana']
c.gui.setWidgetStyle(style)

# Create some windows
win1 = Window(c.gui, caption = "Hello World")
win1.setPosition(50, 50)
win1.setSize(600, 300)

win2 = Window(c.gui, caption = "Another")
win2.setPosition(600, 510)
win2.setSize(200, 200)

# Exit on key-press
c.events.subscribe(EventType.Keyboard, lambda event: sys.exit(0))

#
# Sound test script
#
from iid.behaviour import EntityBehaviour
class R2Behaviour(EntityBehaviour):
  def collision(self, entity):
    self.entity.player.queue(c.storage['/Audio/r2-sound1'].source)
    self.entity.player.eos_action = "pause"
    self.entity.player.play()
  def update(self):
    self.entity.player.dispatch_events()
b = R2Behaviour(c.scene.objects['r2'])
c.scene.behaviours['r2-behaviour'] = b
#
# End test script
#

c.execute()