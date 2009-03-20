#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#

# IID imports
from iid.events import Signalizable

class EntityBehaviour(Signalizable):
  """
  Represents entity's behaviour (aka. the brain).
  """
  entity = None
  
  def __init__(self, entity):
    """
    Class constructor.
    
    @param entity: A valid Entity instance
    """
    self.entity = entity
    entity.behaviour = self
  
  def update(self):
    """
    This method gets called on every step of simulation.
    """
    pass
  
  def prepare(self):
    """
    This method gets called when the scene loads.
    """
    pass
  
  def collision(self, entity):
    """
    This method gets called when bound entity collides with another
    entity.
    
    @param entity: The other entity
    """
    pass
