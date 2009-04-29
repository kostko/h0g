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

// Importers
#include "storage/importers/image.h"
#include "storage/importers/3ds.h"
#include "storage/importers/glsl.h"

// Scene
#include "scene/scene.h"
// FIXME view transform here just for debug
#include "scene/viewtransform.h"
#include "scene/node.h"
#include "scene/rendrable.h"
#include "renderer/statebatcher.h"

// Drivers
#include "drivers/opengl.h"

// XXX FIXME TODO DEBUG
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <iostream>

namespace IID {

static Context *gContext = 0;

Context::Context()
  : m_logger(new Logger("iid.context")),
    m_storage(new Storage(this))
{
  gContext = this;
  
  // Initialize the OpenGL driver
  m_driver = new OpenGLDriver();
  m_driver->init();
  
  // Register basic item types into the storage subsystem
  registerBasicStorageTypes();
  
  // Register basic item argument handlers
  registerBasicArgumentHandlers();
  
  // Register basic importers
  registerBasicImporters();
  
  // Create the scene
  m_scene = new Scene(this);
}

Context::~Context()
{
  delete m_driver;
  delete m_logger;
}

Driver *Context::driver()
{
  return m_driver;
}

Scene *Context::scene()
{
  return m_scene;
}

static void idleCb()
{
  usleep(20000);
  glutPostRedisplay();
}

static void displayCb()
{
  gContext->displayCallback();
}

void Context::displayCallback()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_scene->viewTransform()->loadIdentity();
  m_scene->viewTransform()->lookAt(
    Vector3f(0., 0., 6.),
    Vector3f(0., 0., 0.),
    Vector3f(0., 1., 0.)
  );
  
  m_scene->update();
  m_scene->render();
  m_scene->stateBatcher()->render();
  
  m_driver->swap();
}

void Context::start()
{
  glClearColor(0, 0, 0, 0);
  glClearDepth(1);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
  glViewport(0, 0, 1024, 768);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45., 1024. / 768., 0.1, 3000000.0);
  
  glMatrixMode(GL_MODELVIEW);
  
  // XXX FIXME TODO DEBUG
  glutDisplayFunc(displayCb);
  glutIdleFunc(idleCb);
  
  // Create a test scene
  Shader *shader = m_storage->get<Shader>("/Shaders/general");
  Mesh *mesh = m_storage->get<Mesh>("/Models/spaceship");
  Texture *texture = m_storage->get<Texture>("/Textures/spaceship");
  
  RendrableNode *node = static_cast<RendrableNode*>(m_scene->createNodeFromStorage(mesh));
  node->setTexture(texture);
  node->setShader(shader);
  node->setOrientation(
    AngleAxisf(0.25*M_PI, Vector3f::UnitX()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
  m_scene->attachNode(node);
  
  RendrableNode *node2 = static_cast<RendrableNode*>(m_scene->createNodeFromStorage(mesh));
  node2->setTexture(texture);
  node2->setShader(shader);
  node2->setPosition(0., 0., 1.);
  node2->setOrientation(
    AngleAxisf(0.0*M_PI, Vector3f::UnitX()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.5*M_PI, Vector3f::UnitZ())
  );
  node->attachChild(node2);
  
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
  m_storage->registerImporter("iid.GLSLImporter", new GLSLImporter(this));
  
  m_logger->info("Importer registration completed.");
}

void Context::init()
{
  // Load storage items from the manifest file
  m_storage->load();
  m_logger->info("IID initialized and ready.");
}

}
