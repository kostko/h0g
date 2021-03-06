/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_CONTEXT_H
#define IID_CONTEXT_H

#include "globals.h"
#include "timing.h"

#include <string>

class btBroadphaseInterface;
class btCollisionDispatcher;
class btConstraintSolver;
class btDefaultCollisionConfiguration;
class btDynamicsWorld;

namespace IID {

class Logger;
class Storage;
class Driver;
class Scene;
class EventDispatcher;
class SoundContext;
class TriggerManager;
class GameStateManager;

namespace GUI {
  class Manager;
}

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
    Driver *driver() const { return m_driver; }
    
    /**
     * Returns the currently used scene instance.
     */
    Scene *scene() const { return m_scene; }
    
    /**
     * Returns the currently used storage instance.
     */
    Storage *storage() const { return m_storage; }
    
    /**
     * Returns a logger instance for the specified module. Logger must be
     * manually deleted when not needed anymore.
     */
    Logger *logger(const std::string &module);
    
    /**
     * Returns the Bullet Dynamics world associated with this context.
     */
    btDynamicsWorld *getDynamicsWorld() const { return m_dynamicsWorld; }
    
    /**
     * Returns the event dispatcher instance.
     */
    EventDispatcher *getEventDispatcher() const { return m_eventDispatcher; }
    
    /**
     * Returns the trigger manager instance.
     */
    TriggerManager *getTriggerManager() const { return m_triggerManager; }
    
    /**
     * Returns the GUI manager instance.
     */
    GUI::Manager *getGuiManager() const { return m_guiManager; }
    
    /**
     * Returns the game state manager instance.
     */
    GameStateManager *getGameStateManager() const { return m_gameStateManager; }
    
    /**
     * Sets debugging flag.
     *
     * @param value True to enable debug mode, false otherwise
     */
    void setDebug(bool value) { m_debug = value; }
    
    /**
     * Returns true if debug mode is currently set.
     */
    bool isDebug() const { return m_debug; }
    
    /**
     * Returns the viewport dimensions.
     */
    Vector2i getViewportDimensions() const;
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
    
    /**
     * Initializes bullet dynamics engine.
     */
    void initPhysics();
public:
    /**
     * Performs world dynamics calculation and then renders the world.
     */
    void moveAndDisplay();
    
    /**
     * Renders the world.
     */
    void display();
private:
    // Local logger instance
    Logger *m_logger;
    
    // Local item storage instance
    Storage *m_storage;
    
    // Scene instance
    Scene *m_scene;
    
    // Driver
    Driver *m_driver;
    
    // Sound context
    SoundContext *m_soundContext;
    
    // Clock
    Clock m_clock;
    Clock m_frameClock;
    int m_frameCounter;
    
    // Bullet dynamics stuff
    btBroadphaseInterface *m_broadphase;
    btCollisionDispatcher *m_dispatcher;
    btConstraintSolver *m_solver;
    btDefaultCollisionConfiguration *m_collisionConfiguration;
    btDynamicsWorld *m_dynamicsWorld;
    
    // Event dispatcher
    EventDispatcher *m_eventDispatcher;
    
    // Trigger manager
    TriggerManager *m_triggerManager;
    
    // GUI manager
    GUI::Manager *m_guiManager;
    
    // Game state manager
    GameStateManager *m_gameStateManager;
    
    // Debugging mode
    bool m_debug;
    
    // Viewport dimensions
    Vector2i m_viewportDimensions;
};

}

#endif
