#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
import logging

# IID imports
from iid.exceptions import *
from iid.importers import Importer
from iid.storage.items import BasicModel, BasicTexture, CompositeModel

# Logger for this module
logger = logging.getLogger(__name__)

class ArgumentOpcodes(object):
  """
  A simple opcode dispatcher for manifest item descriptors.
  """
  __opcodes = {}
  
  @staticmethod
  def register(name, opcode):
    """
    Registers an opcode.
    """
    ArgumentOpcodes.__opcodes[name] = opcode
  
  @staticmethod
  def call(args, item):
    """
    Calls an opcode.
    """
    opcode, data = args
    
    try:
      logger.debug('%s(%s)' % (opcode, repr(data)))
      opcode = ArgumentOpcodes.__opcodes[opcode]
      opcode(item, data)
    except KeyError:
      logger.error("Invalid argument opcode '%s'!" % opcode)
      raise InvalidArgumentOpcode

# Opcodes
def load_file(item, args):
  """
  Loads a file by using a specific importer class.
  """
  if 'loader' not in args or 'path' not in args:
    logger.error("Invalid opcode arguments for load_file (loader and path are required)!")
    raise InvalidOpcodeArguments
  
  try:
    loader = args['loader']
    module = loader[0:loader.rfind('.')]
    className = loader[loader.rfind('.') + 1:]
    m = __import__(module, globals(), locals(), [className], -1)
    cls = m.__dict__[className]
  except KeyError:
    logger.error("Item importer '%s' not found!" % args['loader'])
    raise ItemImporterNotFound
  
  if not issubclass(cls, Importer):
    logger.error("Specified importer class is not subclass of Importer!")
    raise InvalidOpcodeArguments
  
  importer = cls(item.resolveFullPath(args['path']))
  
  # Determine sub-object type (if composite)
  if args.has_key("subtype"):
    subtype = globals()[args['subtype']]
    importer.load(item, subtype)
  else:
    importer.load(item) 

def scale_factor(item, args):
  """
  Sets scaling hint to a model.
  """
  if 'x' not in args or 'y' not in args or 'z' not in args:
    logger.error("Invalid opcode arguments for scale factor (x, y and z are required)!")
    raise InvalidOpcodeArguments
  
  if not (isinstance(item, BasicModel) or isinstance(item, CompositeModel)):
    logger.error("Scaling factors can only be applied to models!")
    raise InvalidOpcodeArguments

  try:
    if isinstance(item, BasicModel):
      item.hints['scaling'] = (
        float(args['x']),
        float(args['y']),
        float(args['z'])
      )
    elif isinstance(item, CompositeModel):
      # Set scaling to all children
      for child in item.children:
        child.hints['scaling'] = (
          float(args['x']),
          float(args['y']),
          float(args['z'])
        )
  except ValueError:
    logger.error("Invalid opcode arguments for scale factor (x, y and z must be floats)!")
    raise InvalidOpcodeArguments

def model(item, args):
  """
  Assigns model to an entity.
  """
  pass

def script(item, args):
  """
  Assigns script to an entity.
  """
  pass

# Register some basic opcodes
ArgumentOpcodes.register('load_file', load_file)
ArgumentOpcodes.register('scale_factor', scale_factor)
ArgumentOpcodes.register('model', model)
ArgumentOpcodes.register('script', script)
