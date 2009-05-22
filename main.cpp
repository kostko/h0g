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
#include "storage/material.h"
#include "storage/compositemesh.h"
#include "storage/texture.h"
#include "storage/shader.h"
#include "storage/sound.h"

// Scene
#include "scene/scene.h"
#include "scene/node.h"
#include "scene/camera.h"
#include "scene/light.h"

// Events
#include "events/dispatcher.h"
#include "events/keyboard.h"
#include "events/mouse.h"

// Entities
#include "entities/triggers.h"

// Audio support
#include "drivers/openal.h"

// Bullet dynamics
#include <btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btTriangleShape.h>

#include <boost/bind.hpp>

// Hog includes
#include "motionstate.h"
#include "robot.h"
#include "crate.h"
#include "toad.h"
#include "ai.h"

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
      m_eventDispatcher->signalMousePress.connect(boost::bind(&Game::mousePressEvent, this, _1));
      
      // Setup on contact added callback
      gContactAddedCallback = bulletContactAddedCallback;
      
      // Create a player object
      m_soundPlayer = new OpenALPlayer();
    }
    
    ~Game()
    {
      delete m_staticGeometry;
      delete m_robot;
    }
    
    /**
     * Mouse event handler.
     */
    void mousePressEvent(MousePressEvent *ev)
    {
      m_context->getTriggerManager()->dispatchPickEvent(ev->x(), ev->y());
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
          case 'q': {
            // Raise weapon
            if (!ev->isReleased())
              m_robot->weaponUp();
            break;
          }
          case 'e': {
            // Lower weapon
            if (!ev->isReleased())
              m_robot->weaponDown();
            break;
          }
          
          case 9: {
            // Switch current weapon
            if (!ev->isReleased())
              m_robot->switchWeapon();
            break;
          }
          
          case ' ': {
            // Fire current weapon
            if (!ev->isReleased())
              m_robot->weaponFire();
            break;
          }
          
          case 'm': {
            // Toggle background music on/off
            if (!ev->isReleased()) {
              if (m_soundPlayer->isPlaying())
                m_soundPlayer->stop();
              else
                m_soundPlayer->play();
            }
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
      m_scene->setPerspective(45., 1024., 768., 0.1, 100.0);
      
      // Create a test scene
      m_camera = new Camera(m_scene);
      m_camera->setLag(20);
      m_camera->setZoom(Vector3f(-3.6, 2., 3.6));
      m_camera->setListener(new OpenALListener());
      m_scene->setCamera(m_camera);

      m_camera->lookAt(
        Vector3f(-14., 0., 14.),
        Vector3f(-11., 0., 11.),
        Vector3f(0., 1., 0.)
      );
      
      // Level stuff
      Shader *shader = m_storage->get<Shader>("/Shaders/general");
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
      
      // Create some crates
      new Crate(Vector3f(1.0697, -1.475, -8.06514), m_context->getDynamicsWorld(), m_scene, m_storage);
      new Crate(Vector3f(1.0697, -0.725, -8.06514), m_context->getDynamicsWorld(), m_scene, m_storage);
      
      // AI setup
      // FIXME proper width & height
      m_ai = new AIController(100.0, 100.0);
      
      // Create the robot
      m_robot = new Robot(m_context, m_camera, m_ai);
      m_context->getTriggerManager()->setPickOwner(m_robot);
      
      // Create some enemies
      new Toad(Vector3f(6.9, -0.55, -14.38), m_context, m_robot, m_ai);
      new Toad(Vector3f(10.4, -0.55, -13.42), m_context, m_robot, m_ai);
      new Toad(Vector3f(12.44, -0.55, -8.981), m_context, m_robot, m_ai);
      new Toad(Vector3f(14.89, -0.55, -7.0), m_context, m_robot, m_ai);
      
      // Init some background music
      m_soundPlayer->setMode(Player::Looped);
      m_soundPlayer->queue(m_storage->get<Sound>("/Sounds/zair"));
      m_soundPlayer->play();
      
      // Let there be light
      Light *light = new Light("light");
      light->setType(Light::PointLight);
      light->setPosition(0., 0., -8.25);
      light->setAttenuation(10.0, 1.0, 0.0, 0.0);
      light->setDiffuseColor(0.8, 0.8, 0.8);
      light->setSpecularColor(1.0, 0.2, 0.0);
      // FIXME we need proper shaders before we can use lighting, uncomment this when ready
      //m_scene->attachNode(light);
      m_scene->setAmbientLight(0.2, 0.2, 0.2);
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
    
    // Ambiental player
    Player *m_soundPlayer;
    
    // Actors
    Robot *m_robot;
    
    // Phsyics
    btTriangleIndexVertexArray *m_staticGeometry;
    btBvhTriangleMeshShape *m_staticShape;
    
    // AI
    AIController *m_ai;
};

int main()
{
  Game game;
  game.prepare();
  game.start();
  
  return 0;
}
