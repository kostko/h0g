#
# This file is part of the Infinite Improbability Drive.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#

class SceneObjectNotFound(Exception):
  pass

class ItemStorageException(Exception):
  pass

class InvalidManifestVersion(ItemStorageException):
  pass

class CorruptedManifest(ItemStorageException):
  pass

class InvalidItemType(ItemStorageException):
  pass

class InvalidArgumentOpcode(ItemStorageException):
  pass

class ItemImporterNotFound(ItemStorageException):
  pass

class InvalidOpcodeArguments(ItemStorageException):
  pass

class ImporterException(ItemStorageException):
  pass

class PhysicalFilesNotAllowedInVirtualContainer(ItemStorageException):
  pass

class ItemTypeMismatch(ImporterException):
  pass

class ItemFileNotFound(ImporterException):
  pass

class ImageFormatError(ImporterException):
  pass

class MissingShaderDeclaration(ImporterException):
  pass
