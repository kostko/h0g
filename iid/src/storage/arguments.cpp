/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/arguments.h"
#include "storage/storage.h"
#include "storage/importers/base.h"
#include "exceptions.h"

#include <iostream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace IID {

void argumentLoadFile(Storage *storage, Item *item, StringMap arguments)
{
  if (arguments.find("loader") == arguments.end())
    throw Exception("Missing 'loader' parameter for load_file!");
  
  if (arguments.find("path") == arguments.end())
    throw Exception("Missing 'path' parameter for load_file!");
  
  Importer *importer = storage->importer(arguments["loader"]);
  if (!importer)
    throw Exception("No importer named '" + arguments["loader"] + "' can be found!");
  
  // Determine object path
  fs::path path(arguments["path"]);
  Item *x = item->parent();
  while (x) {
    path = fs::path(x->getPath()) / path;
    x = x->parent();
  }
  
  importer->load(storage, item, path.string());
}

void argumentAttribute(Storage *storage, Item *item, StringMap arguments)
{
  if (arguments.find("name") == arguments.end())
    throw Exception("Missing 'name' parameter for storage item attribute!");
  
  std::string name = arguments["name"];
  arguments.erase("name");
  item->setAttribute(name, arguments);
}

}

