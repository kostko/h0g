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
cam.setCoordinates(0, 2, 400)
cam.setRotation(0, 0, 0)
c.scene.registerObject(cam)

light1 = Light(c.scene, 'light1')
light1.setCoordinates(5, 5, 0, 1)
c.scene.registerObject(light1)

# Add some objects to the scene
model = c.storage['/Models/r2-d2/R2 Head b']
texture = None

#obj = PhysicalEntity(c.scene, 'test', c.storage['/Models/spaceship'], c.storage['/Textures/spaceship'])
obj = PhysicalEntity(c.scene, model.itemId, model, texture)
obj.setCoordinates(0, 0, 0)
obj.setVisible(True)
c.scene.registerObject(obj)

# Exit on key-press
c.events.subscribe(EventType.Keyboard, lambda event: sys.exit(0))

c.execute()
