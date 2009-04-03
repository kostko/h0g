#
# Infinite Improbability Drive test script
#
import logging
import sys

# IID imports
from iid.context import Context
from iid.scene import Entity, PhysicalEntity, Camera, Light
from iid.events import EventType

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

# Load a map
c.storage['/Maps/first'].load(c.scene)

# Exit on key-press
c.events.subscribe(EventType.Keyboard, lambda event: sys.exit(0))

c.execute()