#!/usr/bin/python
#
# This file is part of h0g.
#
# Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
# Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
#
from zipfile import ZipFile
import sys

if len(sys.argv) != 3:
  print "Usage: ./sketchup2iid.py <file.kmz> <file.dae>"
  print "Hint: KMZ file can be obtained using File -> Export -> 3D model."
  exit(1)

archive = sys.argv[1]
output = sys.argv[2]

try:
  file = ZipFile(archive, 'r')
  for filename in file.namelist():
    if filename[-4:] == '.dae':
      f = open(output, 'w')
      f.write(file.read(filename))
      f.close()
      break
  else:
    file.close()
    raise Exception

  file.close()
except:
  print "Error: Not a valid KMZ archive!"
  exit(1)

