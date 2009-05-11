/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "context.h"
#include "logger.h"

// Storage
#include "storage/storage.h"
#include "storage/arguments.h"
#include "storage/mesh.h"
#include "storage/compositemesh.h"
#include "storage/texture.h"
#include "storage/shader.h"
#include "storage/material.h"
#include "storage/sound.h"

// Importers
#include "storage/importers/image.h"
#include "storage/importers/3ds.h"
#include "storage/importers/collada.h"
#include "storage/importers/glsl.h"
#include "storage/importers/audio.h"

// Scene
#include "scene/scene.h"
#include "scene/viewtransform.h"
#include "renderer/statebatcher.h"

// Drivers
#include "drivers/opengl.h"
#include "drivers/openal.h"

// Events
#include "events/dispatcher.h"

// Bullet dynamics
#include <btBulletDynamicsCommon.h>

// XXX FIXME TODO DEBUG
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <iostream>

namespace IID {

static Context *gContext = 0;

Context::Context()
  : m_logger(new Logger("iid.context")),
    m_storage(new Storage(this)),
    m_debug(false)
{
  gContext = this;

  // Initialize the OpenGL driver
  m_driver = new OpenGLDriver();
  m_driver->init();
  
  // Init sound support on the default device
  m_soundContext = new OpenALContext();
  m_soundContext->init("Default");
  
  // Initialize the event dispatcher
  m_eventDispatcher = new EventDispatcher(this);
  m_driver->setEventDispatcher(m_eventDispatcher);
  
  // Register basic item types into the storage subsystem
  registerBasicStorageTypes();
  
  // Register basic item argument handlers
  registerBasicArgumentHandlers();
  
  // Register basic importers
  registerBasicImporters();
  
  // Create the scene
  m_scene = new Scene(this);
  
  // Initialize physics
  initPhysics();
}

Context::~Context()
{
  delete m_soundContext;
  delete m_driver;
  delete m_eventDispatcher;
  delete m_logger;
  
  // Delete physics stuff
  delete m_dynamicsWorld;
  delete m_solver;
  delete m_broadphase;
  delete m_dispatcher,
  delete m_collisionConfiguration;
}

void Context::initPhysics()
{
  m_logger->info("Initializing Bullet Dynamics physics engine...");
  
  // Create collision configuration
  m_collisionConfiguration = new btDefaultCollisionConfiguration();
  
  // Use default collision dispatcher
  m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
  m_broadphase = new btDbvtBroadphase();
  
  // Use default constraint solver
  btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();
  m_solver = solver;
  
  // Create the dynamics world
  m_dynamicsWorld = new btDiscreteDynamicsWorld(
    m_dispatcher,
    m_broadphase,
    m_solver,
    m_collisionConfiguration
  );
  
  m_logger->info("Physics initialized.");
}

void Context::moveAndDisplay()
{
  // Get delta time in microseconds and reset timer
  float dt = m_clock.getTimeMicroseconds();
  m_clock.reset();
  
  // Limit framerate to 120 fps max
  if (dt < 8333)
    usleep((useconds_t) (8333 - dt));
  
  // Step the world
  m_dynamicsWorld->stepSimulation(dt * 0.000001f, 7);
  
  // Render the scene
  display();
}
    
void Context::display()
{
  m_frameCounter++;
  float dt = m_frameClock.getTimeMilliseconds();
  if (dt > 10000) {
    std::cout << "fps = " << (1000.*m_frameCounter / dt) << std::endl;
    m_frameCounter = 0;
    m_frameClock.reset();
  }
  
  m_driver->clear();
  
  // Propagate scene node updates
  m_scene->update();
  
  if (m_debug) {
    m_driver->applyModelViewTransform(m_scene->viewTransform()->transform().data());
    m_dynamicsWorld->debugDrawWorld();
  } else {
    // Perform culling and fill the render queue
    m_scene->render();
    
    // Actually render from the render queue
    m_scene->stateBatcher()->render();
  }
  
  m_driver->swap();
}

static void idleCb()
{
  gContext->moveAndDisplay();
}

static void displayCb()
{
  gContext->display();
}

void Context::start()
{
  // XXX FIXME TODO DEBUG
  glutDisplayFunc(displayCb);
  glutIdleFunc(idleCb);
  
  m_frameClock.reset();
  m_frameCounter = 0;
  
  // Setup debug drawer
  m_dynamicsWorld->setDebugDrawer(m_driver->getDebugBulletDynamicsDrawer());
  
  m_driver->processEvents();
}

Logger *Context::logger(const std::string &module)
{
  return new Logger(module);
}

void Context::registerBasicStorageTypes()
{
  m_logger->info("Registering basic storage item types...");
  
  // Register some basic types that are implemented inside IID
  m_storage->registerType("Texture", new TextureFactory());
  m_storage->registerType("Mesh", new MeshFactory());
  m_storage->registerType("CompositeMesh", new CompositeMeshFactory());
  m_storage->registerType("Shader", new ShaderFactory());
  m_storage->registerType("Material", new MaterialFactory());
  m_storage->registerType("Sound", new SoundFactory());
  
  m_logger->info("Type registration completed.");
}

void Context::registerBasicArgumentHandlers()
{
  m_logger->info("Registering basic argument handlers...");
  
  // Register some basic argument handlers that are implemented inside IID
  m_storage->registerArgument("load_file", &argumentLoadFile);
  m_storage->registerArgument("attribute", &argumentAttribute);
  
  m_logger->info("Argument handler registration completed.");
}

void Context::registerBasicImporters()
{
  m_logger->info("Registering basic importers...");
  
  // Register some basic importers that are implemented inside IID
  m_storage->registerImporter("iid.ImageImporter", new ImageImporter(this));
  m_storage->registerImporter("iid.3DSMeshImporter", new ThreeDSMeshImporter(this));
  m_storage->registerImporter("iid.ColladaMeshImporter", new ColladaMeshImporter(this));
  m_storage->registerImporter("iid.GLSLImporter", new GLSLImporter(this));
  m_storage->registerImporter("iid.SoundImporter", new SoundImporter(this));
  
  m_logger->info("Importer registration completed.");
}

void Context::init()
{
  // Load storage items from the manifest file
  m_storage->load();
  m_logger->info("IID initialized and ready.");
}

}
