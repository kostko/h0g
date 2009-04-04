import pyglet.media as Media
# IID imports
from iid.storage.base import Item

class BasicSound(Item):
  """
  Represents a playable sound which is decoded only once.
  """
  source = None
  
  def __init__(self, storage, itemId, parent = None):
    """
    Class constructor.
    """
    super(BasicSound, self).__init__(storage, itemId, parent)

class StreamedSound(BasicSound):
  """
  Represents a playable sound streamed from a file.
  These sounds can be queued on only one player simultaneously.
  """
  pass