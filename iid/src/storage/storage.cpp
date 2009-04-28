/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/storage.h"
#include "storage/importers/base.h"
#include "context.h"
#include "logger.h"
#include "exceptions.h"

// TinyXML parser
#include "tinyxml/ticpp.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace IID {

Item::Item(Storage *storage, const std::string &type, const std::string &itemId, const std::string &path, Item *parent)
  : m_itemId(itemId),
    m_type(type),
    m_path(path),
    m_parent(parent),
    m_storage(storage)
{
  if (parent) {
    // Perform item registration in parent child list
    parent->m_children[itemId] = this;
  }
}

Item::~Item()
{
  typedef std::pair<std::string, Item*> Child;
  BOOST_FOREACH(Child child, m_children) {
    delete child.second;
  }
}

std::string Item::getId() const
{
  return m_itemId;
}

std::string Item::getType() const
{
  return m_type;
}

std::string Item::getPath() const
{
  return m_path;
}

Item *Item::parent()
{
  return m_parent;
}

void Item::setAttribute(const std::string &name, StringMap value)
{
  m_attributes[name] = value;
}

StringMap Item::getAttribute(const std::string &name)
{
  return m_attributes[name];
}

bool Item::hasAttribute(const std::string &name)
{
  return m_attributes.find(name) != m_attributes.end();
}

Item *Item::child(const std::string &itemId)
{
  if (m_children.find(itemId) == m_children.end())
    return 0;
  
  return m_children.at(itemId);
}

Storage::Storage(Context *context)
  : m_context(context),
    m_logger(context->logger("iid.storage")),
    m_root(new Item(this, "root"))
{
}

Storage::~Storage()
{
  // Delete all registred type factories
  typedef std::pair<std::string, ItemFactory*> Child;
  BOOST_FOREACH(Child child, m_types) {
    delete child.second;
  }
  
  // Delete all registred importers
  typedef std::pair<std::string, Importer*> ImporterChild;
  BOOST_FOREACH(ImporterChild child, m_importers) {
    delete child.second;
  }
  
  delete m_root;
  delete m_logger;
}

void Storage::load()
{
  m_logger->info("Loading data storage manifest file...");
  
  try {
    ticpp::Document manifest("manifest.xml");
    manifest.LoadFile();
    std::string version;
    
    if (manifest.FirstChildElement()->Value() != "manifest") {
      m_logger->error("Invalid manifest file format!");
      return;
    }
    
    manifest.FirstChildElement()->GetAttribute("version", &version);
    if (version != "1.0") {
      m_logger->error("Unsupported manifest version!");
      return;
    }
    
    m_logger->info("Got IID manifest file, version " + version + ".");
    
    // Parse containers
    loadContainer(m_root, manifest.FirstChildElement());
  } catch (ticpp::Exception &e) {
    // Failed to open the manifest file
    m_logger->error("Unable to open 'manifest.xml' storage descriptor!");
  }
  
  m_logger->info("Storage system initialized.");
}

void Storage::loadContainer(Item *parent, ticpp::Element *element)
{
  ticpp::Iterator<ticpp::Element> child;
  for (child = child.begin(element); child != child.end(); child++) {
    if ((*child).Value() == "container") {
      // Another container
      Item *container = new Item(
        this,
        "Item",
        (*child).GetAttributeOrDefault("id", ""),
        (*child).GetAttributeOrDefault("path", ""),
        parent
      );
      
      m_logger->info("Creating a container '" + container->getId() + "' (path = " + container->getPath() + ").");
      
      // Recursively parse containers
      loadContainer(container, &*child);
    } else if ((*child).Value() == "item") {
      // Process arguments
      std::string itemType = (*child).GetAttributeOrDefault("type", "");
      if (m_types.find(itemType) == m_types.end()) {
        // Invalid item type
        m_logger->error("Unknown storage item type '" + itemType + "'!");
        return;
      }
      
      Item *item = m_types[itemType]->create(
        this,
        (*child).GetAttributeOrDefault("name", ""),
        parent
      );
      
      m_logger->info("Creating an item '" + item->getId() + "' (container = " + parent->getId() + ").");
      
      // Process arguments
      ticpp::Iterator<ticpp::Element> arg;
      for (arg = arg.begin(&*child); arg != arg.end(); arg++) {
        std::string argName = (*arg).Value();
        if (m_args.find(argName) == m_args.end()) {
          // Missing argument handler
          m_logger->error("Missing argument handler for '" + argName + "'!");
          return;
        }
        
        // Parse attributes
        StringMap attrs;
        ticpp::Iterator<ticpp::Attribute> attr;
        for (attr = attr.begin(&*arg); attr != attr.end(); attr++) {
          attrs[(*attr).Name()] = (*attr).Value();
        }
        
        StorageArgumentHandler handler = m_args[argName];
        try {
          handler(this, item, attrs);
        } catch (const Exception &e) {
          m_logger->error(e.getMessage());
          return;
        }
      }
    } else {
      m_logger->error("Invalid element detected in manifest XML: " + (*child).Value());
      return;
    }
  }
}

void Storage::registerType(const std::string &type, ItemFactory *factory)
{
  m_types[type] = factory;
  m_logger->info("Registred type '" + type + "' into the storage system.");
}

void Storage::registerArgument(const std::string &name, StorageArgumentHandler handler)
{
  m_args[name] = handler;
  m_logger->info("Registred argument handler for '" + name + "'.");
}

void Storage::registerImporter(const std::string &name, Importer *importer)
{
  m_importers[name] = importer;
  m_logger->info("Registred importer '" + name + "'.");
}

Item *Storage::get(const std::string &path)
{
  Item *item = m_root;
  std::vector<std::string> parts;
  boost::split(parts, path, boost::is_any_of("/"));
  
  for (std::vector<std::string>::iterator i = parts.begin(); i != parts.end(); i++) {
    if (!(*i).length())
      continue;
    
    item = item->child(*i);
    if (!item)
      return 0;
  }
  
  return item;
}

Importer *Storage::importer(const std::string &name)
{
  if (m_importers.find(name) == m_importers.end())
    return 0;
  
  return m_importers[name];
}

}

