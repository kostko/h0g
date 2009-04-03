#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
import logging
import traceback

# Logger for this module
logger = logging.getLogger(__name__)

class EventType:
  """
  Valid event types.
  """
  Invalid = 0
  Keyboard = 1
  MouseMove = 2
  MousePress = 3

class Event(object):
  """
  An abstract event that can be dispatched.
  """
  eventType = EventType.Invalid
  
  def __init__(self, eventType):
    """
    Class constructor.
    
    @param eventType: A valid event type
    """
    self.eventType = eventType

class KeyboardEvent(Event):
  """
  A keyboard event.
  """
  key = None
  special = False
  
  def __init__(self, key, special = False):
    """
    Class constructor.
    
    @param key: Key code that was pressed
    @param special: True if the key was special
    """
    super(KeyboardEvent, self).__init__(EventType.Keyboard)
    self.key = key
    self.special = special

class MouseMoveEvent(Event):
  """
  A mouse move event.
  """
  x = None
  y = None
  dx = None
  dy = None
  
  # For tracking movement deltas
  __ox = 0
  __oy = 0
  
  def __init__(self, x, y):
    """
    Class constructor.
    
    @param x: X coordinate
    @param y: Y coordinate
    """
    super(MouseMoveEvent, self).__init__(EventType.MouseMove)
    self.x = x
    self.y = y
    self.dx = x - MouseMoveEvent.__ox
    self.dy = y - MouseMoveEvent.__oy
    MouseMoveEvent.__ox = x
    MouseMoveEvent.__oy = y

class MousePressEvent(Event):
  """
  A mouse press event.
  """
  x = None
  y = None
  button = None
  state = None
  
  def __init__(self, x, y, button, state):
    """
    Class constructor.
    
    @param x: X coordinate
    @param y: Y coordinate
    @param button: Button that was pressed
    @param state: Button state
    """
    super(MousePressEvent, self).__init__(EventType.MousePress)
    self.x = x
    self.y = y
    self.button = button
    self.state = state

class Signalizable(object):
  """
  All objects that want to emit signals should mix-in this class.
  """
  __subscribers = None
  
  def __init__(self):
    """
    Class constructor.
    """
    self.__subscribers = {}
  
  def emit(self, name, *args, **kwargs):
    """
    Emits a signal / notifies all subscribers.
    """
    for subscriber in self.__subscribers.get(name, []):
      subscriber(*args, **kwargs)
  
  def subscribe(self, name, callback):
    """
    Connects a signal to a receiver.
    
    @param name: Signal name
    @param callback: A valid callback
    """
    self.__subscribers.setdefault(name, []).append(callback)

class EventDispatcher(object):
  """
  IID event dispatcher. This only dispatches to direct event
  subscribers which are then responsible for further dispatching
  when needed.
  """
  __subscribers = None
  __dispatcher = None
  
  def __init__(self):
    """
    Class constructor.
    """
    self.__subscribers = {}
    EventDispatcher.__dispatcher = self
  
  @staticmethod
  def getDispatcher():
    """
    Returns the active dispatcher instance.
    """
    return EventDispatcher.__dispatcher
  
  def subscribe(self, eventType, handler):
    """
    Subscribes to an event.
    
    @param eventType: Event type
    @param handler: A callable event handler
    """
    self.__subscribers.setdefault(eventType, []).append(handler)
  
  def unsubscribe(self, eventType, handler):
    """
    Unsubscribes from an event.
    
    @param eventType: Event type
    @param handler: A callable event handler
    """
    try:
      self.__subscribers.get(eventType, []).remove(handler)
    except ValueError:
      pass
  
  def dispatchToSubscribers(self, event):
    """
    Dispatches an event to subscribers.
    
    @param event: A valid Event instance
    """
    # NOTE: We have to work on a copy of the subscribers list! Otherwise if this list
    #       is modified while dispatching an event lockups will ocurr!
    for subscriber in self.__subscribers.get(event.eventType, [])[:]:
      try:
        subscriber(event)
      except SystemExit:
        raise
      except:
        logger.warning("Unhandled exception in event dispatcher!")
        logger.warning(traceback.format_exc())
  
  def eventKeyboardInput(self, key, x, y):
    """
    Process keyboard input event.
    """
    self.dispatchToSubscribers(KeyboardEvent(key))
  
  def eventMouseMoveInput(self, x, y):
    """
    Process mouse move event.
    """
    self.dispatchToSubscribers(MouseMoveEvent(x, y))
  
  def eventMousePressInput(self, button, state, x, y):
    """
    Process mouse press event.
    """
    self.dispatchToSubscribers(MousePressEvent(x, y, button, state))
