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

# Setup logger
logging.basicConfig(level = logging.DEBUG,
                    format = '%(asctime)s %(levelname)-8s [%(name)s] %(message)s',
                    datefmt = '%a, %d %b %Y %H:%M:%S')

# Setup IID context
c = Context()
c.loadConfiguration()
c.initAll()

cam = Camera(c.scene, 'camera')
cam.setCoordinates(0, 0, 200)
cam.setRotation(0, 0, 0)
c.scene.registerObject(cam)

light1 = Light(c.scene, 'light1')
light1.setCoordinates(5, 5, 0, 1)
c.scene.registerObject(light1)

c.scene.cull = False
c.scene.showBoundingBoxes = True
c.scene.showSubentityBoxes = False

# Load a map
c.storage['/Maps/first'].load(c.scene)

win1 = Window(c.gui, caption = "Hello World")
win1.setPosition(50, 50)
win1.setSize(600, 300)

win2 = Window(c.gui, caption = "Another")
win2.setPosition(600, 510)
win2.setSize(200, 200)

# Exit on key-press
c.events.subscribe(EventType.Keyboard, lambda event: sys.exit(0))

c.execute()