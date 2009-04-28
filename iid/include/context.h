/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_CONTEXT_H
#define IID_CONTEXT_H

#include <string>

namespace IID {

class Logger;
class Storage;
class Driver;
class Scene;

/**
 * The Infinite Improbability Drive context object links all aspects of
 * the engine together. It must be created before any other parts can be
 * used.
 */
class Context {
public:
    /**
     * Class constructor.
     */
    Context();
    
    /**
     * Class destructor.
     */
    ~Context();
    
    /**
     * This method initializes the engine and must be called.
     */
    void init();
    
    /**
     * Start the engine event loop. This method will not return.
     */
    void start();
    
    /**
     * Returns the currently used driver.
     */
    Driver *driver();
    
    /**
     * Returns the currently used scene instance.
     */
    Scene *scene();
    
    /**
     * Returns a logger instance for the specified module. Logger must be
     * manually deleted when not needed anymore.
     */
    Logger *logger(const std::string &module);
protected:
    /**
     * Registers basic storage types.
     */
    void registerBasicStorageTypes();
    
    /**
     * Registers basic storage argument handlers.
     */
    void registerBasicArgumentHandlers();
    
    /**
     * Registers basic importers.
     */
    void registerBasicImporters();
public:    
    void displayCallback();
private:
    // Local logger instance
    Logger *m_logger;
    
    // Local item storage instance
    Storage *m_storage;
    
    // Scene instance
    Scene *m_scene;
    
    // Driver
    Driver *m_driver;
};

}

#endif
