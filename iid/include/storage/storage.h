/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_STORAGE_H
#define IID_STORAGE_H

#include <string>
#include <boost/function.hpp>

#include "globals.h"

namespace ticpp {
  class Element;
}

namespace IID {

class Item;
class Storage;
class Importer;
class Context;
class Logger;

/**
 * An abstract class for storage items.
 */
class Item {
public:
    /**
     * Class constructor.
     *
     * @param storage Storage instance
     * @param type Item type
     * @param itemId Item identifier
     * @param path Item path
     * @param parent Optional parent instance
     */
    Item(Storage *storage, const std::string &type = "Item", const std::string &itemId = "",
         const std::string &path = "", Item *parent = 0);
    
    /**
     * Class destructor.
     */
    virtual ~Item();
    
    /**
     * Returns storage item identifier.
     */
    std::string getId() const;
    
    /**
     * Returns item's type.
     */
    std::string getType() const;
    
    /**
     * Returns parent-relative filesystem path.
     */
    std::string getPath() const;
    
    /**
     * Sets an item attribute.
     *
     * @param name Attribute name
     * @param value Attribute value
     */
    void setAttribute(const std::string &name, StringMap value);
    
    /**
     * Returns true if item has the specified attribute set.
     *
     * @param name Attribute name
     * @return True if the attribute is set, false otherwise
     */
    bool hasAttribute(const std::string &name);
    
    /**
     * Returns an item attribute value.
     *
     * @param name Attribute name
     * @return Attribute value
     */
    StringMap getAttribute(const std::string &name);
    
    /**
     * Returns parent item.
     */
    Item *parent();
    
    /**
     * Returns the storage instance that contains this item.
     */
    Storage *storage() const { return m_storage; }
    
    /**
     * Returns the specified child item.
     *
     * @param itemId Child item identifier
     * @return A valid Item instance or NULL when no child by this name
     *         can be found
     */
    Item *child(const std::string &itemId);
    
    /**
     * Returns this item's children list.
     */
    boost::unordered_map<std::string, Item*> *children() { return &m_children; }
protected:
    // Storage instance
    Storage *m_storage;
private:
    // Item identifier, type, path and parent
    std::string m_itemId;
    std::string m_type;
    std::string m_path;
    Item *m_parent;
    
    // Child items
    boost::unordered_map<std::string, Item*> m_children;
    
    // Item attributes
    boost::unordered_map<std::string, StringMap> m_attributes;
};

/**
 * An abstract class for item factories - that is classes that generate
 * items of the specified class.
 */
class ItemFactory {
public:
    /**
     * Actual item factories must reimplement this method to return a
     * valid Item instance.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param parent Parent item
     * @return A valid Item descendant instance
     */
    virtual Item *create(Storage *storage, const std::string &itemId, Item *parent) = 0;
};

// Argument handler function
typedef boost::function<void (Storage *storage, Item *item, StringMap arguments)> StorageArgumentHandler;

/**
 * Represents the IID hierarchical store.
 */
class Storage {
public:
    /**
     * Class constructor.
     *
     * @param context A valid IID context
     */
    Storage(Context *context);
    
    /**
     * Class destructor.
     */
    ~Storage();
    
    /**
     * Returns the current engine context.
     */
    Context *context() { return m_context; }
    
    /**
     * Load the manifest XML file and create the storage tree.
     */
    void load();
    
    /**
     * Registers an item type.
     *
     * @param type Type identifier
     * @param factory Factory class
     */
    void registerType(const std::string &type, ItemFactory *factory);
    
    /**
     * Registers an argument handler. Argument handlers are responsible for
     * performing actions specified in the manifest descriptor file such as
     * load_file etc.
     *
     * @param name Argument name
     * @param handler Handler callback function
     */
    void registerArgument(const std::string &name, StorageArgumentHandler handler);
    
    /**
     * Registers an importer. Importers are responsible for loading the actual
     * content into storage items.
     *
     * @param name Importer name
     * @param importer A valid Importer descendant instance
     */
    void registerImporter(const std::string &name, Importer *importer);
    
    /**
     * Returns an item identified by path or NULL when an item
     * cannot be found.
     *
     * @param path Item path in the virtual hierarchy
     * @return A valid Item descendant pointer or NULL
     */
    template<typename T>
    T *get(const std::string &path) { return static_cast<T*>(get(path)); }
    
    /**
     * Returns an item identified by path or NULL when an item
     * cannot be found.
     *
     * @param path Item path in the virtual hierarchy
     * @return A valid Item pointer or NULL
     */
    Item *get(const std::string &path);
    
    /**
     * Returns a valid importer or NULL when there is none registred under
     * specified name.
     *
     * @param name Importer name
     * @return A valid Importer instance or NULL
     */
    Importer *importer(const std::string &name);
protected:
    /**
     * Loads the specified container element.
     */
    void loadContainer(Item *item, ticpp::Element *element);
private:
    // IID context pointer
    Context *m_context;
    Logger *m_logger;
    
    // Root item
    Item *m_root;
    
    // Registred types
    boost::unordered_map<std::string, ItemFactory*> m_types;
    
    // Registred argument handlers
    boost::unordered_map<std::string, StorageArgumentHandler> m_args;
    
    // Registred importers
    boost::unordered_map<std::string, Importer*> m_importers;
};

}

#endif
