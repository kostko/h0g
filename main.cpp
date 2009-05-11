/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "context.h"

// Storage
#include "storage/storage.h"
#include "storage/mesh.h"
#include "storage/compositemesh.h"
#include "storage/texture.h"
#include "storage/shader.h"
#include "storage/sound.h"

// Scene
#include "scene/scene.h"
#include "scene/node.h"
#include "scene/camera.h"

// Events
#include "events/dispatcher.h"
#include "events/keyboard.h"

// Audio support
#include "drivers/openal.h"

// Bullet dynamics
#include <btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btTriangleShape.h>

#include <boost/bind.hpp>

// Hog includes
#include "motionstate.h"
#include "robot.h"

using namespace IID;

/**
 * Fix triangle mesh normals.
 */
void bulletContactAddedCallbackObj(btManifoldPoint& cp, const btCollisionObject* colObj, int, int)
{
  const btCollisionShape *shape = colObj->getCollisionShape();

  if (shape->getShapeType() != TRIANGLE_SHAPE_PROXYTYPE)
    return;
  
  const btTriangleShape *tshape = static_cast<const btTriangleShape*>(colObj->getCollisionShape());
  const btCollisionShape *parent = colObj->getRootCollisionShape();
  
  if (parent == NULL || parent->getShapeType() != TRIANGLE_MESH_SHAPE_PROXYTYPE)
    return;

  btTransform orient = colObj->getWorldTransform();
  orient.setOrigin(btVector3(0.0f, 0.0f, 0.0f));

  btVector3 v1 = tshape->m_vertices1[0];
  btVector3 v2 = tshape->m_vertices1[1];
  btVector3 v3 = tshape->m_vertices1[2];

  btVector3 normal = (v2-v1).cross(v3-v1);

  normal = orient * normal;
  normal.normalize();

  btScalar dot = normal.dot(cp.m_normalWorldOnB);
  btScalar magnitude = cp.m_normalWorldOnB.length();
  normal *= dot > 0 ? magnitude : -magnitude;

  cp.m_normalWorldOnB = normal;
}

/**
 * A contact added callback handler for static geometry mesh. This custom callback
 * fixes problems with spurious collisions when crossing trimesh boundaries.
 *
 * Also see:
 *   http://code.google.com/p/bullet/issues/detail?id=27
 */
bool bulletContactAddedCallback(btManifoldPoint& cp,
                                const btCollisionObject* colObj0, int partId0, int index0,
                                const btCollisionObject* colObj1, int partId1, int index1)
{
  bulletContactAddedCallbackObj(cp, colObj0, partId0, index0);
  bulletContactAddedCallbackObj(cp, colObj1, partId1, index1);
  
  return true;
}

// External declaration for contact added callback
extern ContactAddedCallback gContactAddedCallback;

class Game {
public:
    /**
     * Class constructor.
     */
    Game()
    {
      // Initialize stuff
      m_context = new Context();
      m_context->init();
      
      // Save pointers for easier reference
      m_scene = m_context->scene();
      m_storage = m_context->storage();
      m_eventDispatcher = m_context->getEventDispatcher();
      
      // Subscribe to events
      m_eventDispatcher->signalKeyboard.connect(boost::bind(&Game::keyboardEvent, this, _1));
      
      // Setup on contact added callback
      gContactAddedCallback = bulletContactAddedCallback;
    }
    
    ~Game()
    {
      delete m_staticGeometry;
      delete m_robot;
    }
    
    /**
     * Keyboard event handler.
     */
    void keyboardEvent(KeyboardEvent *ev)
    {
      if (ev->isSpecial()) {
        if (!ev->isReleased()) {
          // Handle camera movements
          switch (ev->key()) {
            case EventDispatcher::Up: m_camera->walk(0.2); break;
            case EventDispatcher::Down: m_camera->walk(-0.2); break;
            case EventDispatcher::Left: m_camera->rotate(0.0, 2.0, 0.0); break;
            case EventDispatcher::Right: m_camera->rotate(0.0, -2.0, 0.0); break;
          }
        }
      } else {
        switch (ev->key()) {
          case 'w': m_robot->forward(!ev->isReleased()); break;
          case 's': m_robot->backward(!ev->isReleased()); break;
          case 'a': m_robot->left(!ev->isReleased()); break;
          case 'd': m_robot->right(!ev->isReleased()); break;
          case 'h': {
            if (!ev->isReleased())
              m_robot->hover();
            break;
          }
          // Toggle debug mode with 'p'
          case 'p': {
            if (!ev->isReleased())
              m_context->setDebug(!m_context->isDebug());
            break;
          }
          case 't': {
            // Taunt the enemy by squeaking
            m_robot->taunt();
            break;
          }
          
          case 9: {
            // Switch current weapon the weapon
            if (!ev->isReleased())
              m_robot->switchWeapon();
            break;
          }
          
          // Exit when escape is pressed
          case 27: {
              delete m_context;
              delete m_scene;
              delete m_storage;
              exit(0);
          }
        }
      }
    }
    
    /**
     * Prepares the scene.
     */
    void prepare()
    {
      // Prepare the scene
      m_scene->setPerspective(45., 1024. / 768., 0.1, 100.0);
      
      // Create a test scene
      m_camera = new Camera(m_scene);
      m_camera->setListener(new OpenALListener());
      m_scene->setCamera(m_camera);
      // FIXME this camera position is hacky:-)
      float x = -4.25;
      m_camera->lookAt(
        Vector3f(0., 0., x),
        Vector3f(-1., 0., x - 4.),
        Vector3f(0., 1., 0.)
      );
      
      Shader *shader = m_storage->get<Shader>("/Shaders/general");
      Mesh *mesh = m_storage->get<Mesh>("/Models/spaceship");
      Texture *texture = m_storage->get<Texture>("/Textures/spaceship");
      
      Shader *shader2 = m_storage->get<Shader>("/Shaders/material");
      CompositeMesh *mesh2 = m_storage->get<CompositeMesh>("/Models/r2-d2");
      
      // Level stuff
      CompositeMesh *level = m_storage->get<CompositeMesh>("/Levels/first");
      Texture *brick = m_storage->get<Texture>("/Textures/Brick/rough_dark");
      Texture *carpet = m_storage->get<Texture>("/Textures/Carpet/plush_forest");
      Texture *stone = m_storage->get<Texture>("/Textures/Stone/vein_gray");
      Texture *metal = m_storage->get<Texture>("/Textures/Metal/rusted");
      
      SceneNode *lnode = m_scene->createNodeFromStorage(level);
      lnode->setStaticHint(true);
      lnode->setShader(shader);
      // FIXME naming of objects must be handled differently
      lnode->child("object1")->setTexture(metal);
      lnode->child("object2")->setTexture(brick);
      lnode->child("object3")->setTexture(carpet);
      //lnode->child("object4")->setTexture(stone);
      lnode->setOrientation(
        AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitY()) *
        AngleAxisf(1.25*M_PI, Vector3f::UnitZ())
      );
      m_scene->attachNode(lnode);
      
      // Generate static geometry shape
      btVector3 aabbMin(-100, -100, -100), aabbMax(100, 100, 100);
      m_scene->update();
      m_staticGeometry = new btTriangleIndexVertexArray();
      m_scene->getRootNode()->batchStaticGeometry(m_staticGeometry);
      m_staticShape = new btBvhTriangleMeshShape(m_staticGeometry, true, aabbMin, aabbMax);
      
      // Load static geometry into the physics engine
      btTransform startTransform;
      startTransform.setIdentity();
      startTransform.setOrigin(btVector3(0, 0, 0));
      
      btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);
      btRigidBody::btRigidBodyConstructionInfo cInfo(0.0, motionState, m_staticShape, btVector3(0, 0, 0));
      btRigidBody *body = new btRigidBody(cInfo);
      m_context->getDynamicsWorld()->addRigidBody(body);
      body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
      body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
      
      // Create the robot
      m_robot = new Robot(m_context->getDynamicsWorld(), m_scene, m_storage);
    }
    
    /**
     * Enter the render loop.
     */
    void start()
    {
      m_context->start();
    }
private:
    Context *m_context;
    Scene *m_scene;
    Storage *m_storage;
    Camera *m_camera;
    EventDispatcher *m_eventDispatcher;
    
    // Actors
    Robot *m_robot;
    
    // Phsyics
    btTriangleIndexVertexArray *m_staticGeometry;
    btBvhTriangleMeshShape *m_staticShape;
};

int main()
{
  Game game;
  game.prepare();
  game.start();
  
  return 0;
}
