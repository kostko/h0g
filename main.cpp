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
#include "storage/font.h"

// Scene
#include "scene/scene.h"
#include "scene/node.h"
#include "scene/rendrable.h"
#include "scene/camera.h"
#include "scene/light.h"
#include "scene/geometrymeta.h"

// Events
#include "events/dispatcher.h"
#include "events/keyboard.h"
#include "events/mouse.h"

// Entities
#include "entities/triggers.h"
#include "entities/entity.h"

// Audio support
#include "drivers/openal.h"

// GUI
#include "gui/manager.h"
#include "gui/windows.h"
#include "gui/window.h"
#include "gui/button.h"
#include "gui/font.h"
#include "gui/image.h"

// Gamestate
#include "gamestate.h"

// Bullet dynamics
#include <btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btTriangleShape.h>
#include <bullet/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.h>
#include <bullet/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.h>

#include <boost/bind.hpp>

// Hog includes
#include "motionstate.h"
#include "robot.h"
#include "crate.h"
#include "toad.h"
#include "ai.h"
#include "door.h"

using namespace IID;

// Thresholds for proper normal handling on collisions
#define POINT_ON_LINE_THRESHOLD 0.00001
#define INTERNAL_ANGLE_THRESHOLD 0.000001

/**
 * Checks if a point lies on a line according to POINT_ON_LINE_THRESHOLD.
 *
 * @param point Point coordinates
 * @param a Line start point
 * @param b Line end point
 * @return True if point lies on a line, false otherwise
 */
bool isPointOnLine(const btVector3 &point, const btVector3 &a, const btVector3 &b)
{
  float u = ((point.x() - a.x()) * (b.x() - a.x()) + (point.y() - a.y()) * (b.y() - a.y()) + (point.z() - a.z()) * (b.z() - a.z())) / (b - a).length2();
  btVector3 t(a.x() + u*(b.x() - a.x()), a.y() + u*(b.y() - a.y()), a.z() + u*(b.z() - a.z()));
  return (t - point).length() < POINT_ON_LINE_THRESHOLD;
}

/**
 * Fix triangle mesh normals.
 */
void bulletContactAddedCallbackObj(btManifoldPoint &cp, const btCollisionObject *colObj, const btCollisionObject *colObjOther, int partId, int index)
{
  const btCollisionShape *shape = colObj->getCollisionShape();

  if (shape->getShapeType() != TRIANGLE_SHAPE_PROXYTYPE)
    return;
  
  const btTriangleShape *tshape = static_cast<const btTriangleShape*>(colObj->getCollisionShape());
  const btCollisionShape *parent = colObj->getRootCollisionShape();
  
  if (parent == NULL || parent->getShapeType() != MULTIMATERIAL_TRIANGLE_MESH_PROXYTYPE)
    return;
  
  // Acquire edge angle information as we are going to check if edges are internal based on angles
  btMultimaterialTriangleMeshShape *pshape = (btMultimaterialTriangleMeshShape*) parent;
  const GeometryMetadata::FaceInfo *props = static_cast<const GeometryMetadata::FaceInfo*>(pshape->getMaterialProperties(partId, index));
  
  btTransform orient = colObj->getWorldTransform();
  orient.setOrigin(btVector3(0.0f, 0.0f, 0.0f));

  btVector3 v1 = tshape->m_vertices1[0];
  btVector3 v2 = tshape->m_vertices1[1];
  btVector3 v3 = tshape->m_vertices1[2];
  
  // Check first edge
  if (isPointOnLine(cp.m_localPointB, v1, v2) && props->edgeAngles[0] > INTERNAL_ANGLE_THRESHOLD)
    return;
  
  // Check second edge
  if (isPointOnLine(cp.m_localPointB, v2, v3) && props->edgeAngles[1] > INTERNAL_ANGLE_THRESHOLD)
    return;
  
  // Check third edge
  if (isPointOnLine(cp.m_localPointB, v3, v1) && props->edgeAngles[2] > INTERNAL_ANGLE_THRESHOLD)
    return;
  
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
bool bulletContactAddedCallback(btManifoldPoint &cp,
                                const btCollisionObject *colObj0, int partId0, int index0,
                                const btCollisionObject *colObj1, int partId1, int index1)
{
  bulletContactAddedCallbackObj(cp, colObj0, colObj1, partId0, index0);
  bulletContactAddedCallbackObj(cp, colObj1, colObj0, partId1, index1);
  
  return true;
}

// External declaration for contact added callback
extern ContactAddedCallback gContactAddedCallback;

/**
 * This game state handles in-game menu display.
 */
class MenuState : public GameState {
public:
    /**
     * Class constructor.
     *
     * @param context Engine context
     */
    MenuState(Context *context)
      : GameState("menu", context),
        m_context(context)
    {
    }
    
    /**
     * This method is called when this game state is entered.
     *
     * @param fromState State from which we have entered
     */
    void enter(const std::string &fromState)
    {
      m_inGame = (fromState == "play");
      
      Font *font = m_context->storage()->get<Font>("/Fonts/verdana");
      Texture *logoImage = m_context->storage()->get<Texture>("/GUI/Images/logo");
      
      // Create game menu
      GUI::WindowManager *wm = m_context->getGuiManager()->getWindowManager();
      wm->setBackgroundColor(GUI::Color(0, 0, 0, 100));
      
      int centerX = wm->getWidth() / 2;
      int centerY = wm->getHeight() / 2;
      
      m_logo = new GUI::Image(wm);
      m_logo->setPosition(centerX - 144, 100);
      m_logo->setSize(329, 214);
      m_logo->setTexture(logoImage);
      
      m_menuWindow = new GUI::Window(wm);
      m_menuWindow->setPosition(centerX - 100, centerY - 55);
      m_menuWindow->setSize(200, 110);
      
      GUI::Button *startGame = new GUI::Button(m_menuWindow);
      startGame->setPosition(10, 10);
      startGame->setSize(180, 40);
      startGame->setFont(GUI::Font(font));
      startGame->setText(m_inGame ? "Resume game" : "Start game");
      startGame->signalClicked.connect(boost::bind(&MenuState::startGameClicked, this));
      
      GUI::Button *quitGame = new GUI::Button(m_menuWindow);
      quitGame->setPosition(10, 60);
      quitGame->setSize(180, 40);
      quitGame->setFont(GUI::Font(font));
      quitGame->setText("Quit");
      quitGame->signalClicked.connect(boost::bind(&MenuState::quitClicked, this));
      
      // Handle escape key
      EventDispatcher *eventDispatcher = m_context->getEventDispatcher();
      m_keyboardEvents = eventDispatcher->signalKeyboard.connect(boost::bind(&MenuState::keyboardEvent, this, _1));
      m_ignoreKeyboardEvent = true;
    }
    
    /**
     * Keyboard event handler.
     */
    void keyboardEvent(KeyboardEvent *ev)
    {
      if (m_ignoreKeyboardEvent && m_inGame) {
        m_ignoreKeyboardEvent = false;
        return;
      }
      
      // First check if GUI will handle keyboard events
      if (m_context->getGuiManager()->handleKeyboard(ev))
        return;
      
      if (!ev->isReleased() && !ev->isSpecial() && ev->key() == 27) {
        if (m_inGame) {
          getGsm()->transitionDown();
        } else {
          delete m_context;
          exit(0);
        }
      }
    }
    
    /**
     * Called when start game/resume game button has been clicked.
     */
    void startGameClicked()
    {
      getGsm()->transitionTo("play");
    }
    
    /**
     * Called when the quit button has been clicked.
     */
    void quitClicked()
    {
      delete m_context;
      exit(0);
    }
    
    /**
     * This method is called when this game state is left behind.
     *
     * @param toState State to which we are going
     */
    void leave(const std::string &toState)
    {
      // Remove keyboard handler
      m_keyboardEvents.disconnect();
      
      // Reset transparent background
      GUI::WindowManager *wm = m_context->getGuiManager()->getWindowManager();
      wm->setBackgroundColor(GUI::Color(0, 0, 0, 0));
      
      // Remove menu window
      m_menuWindow->reparent(0);
      delete m_menuWindow;
      
      // Remove logo
      m_logo->reparent(0);
      delete m_logo;
    }
private:
    Context *m_context;
    GUI::Window *m_menuWindow;
    GUI::Image *m_logo;
    boost::signals::connection m_keyboardEvents;
    bool m_inGame;
    bool m_ignoreKeyboardEvent;
};

/**
 * This game state handles the actual gameplay.
 */
class PlayState : public GameState {
public:
    /**
     * Class constructor.
     *
     * @param context Engine context
     */
    PlayState(Context *context)
      : GameState("play", context),
        m_context(context),
        m_scene(context->scene()),
        m_storage(context->storage()),
        m_camera(0),
        m_soundPlayer(0),
        m_robot(0),
        m_staticGeometry(0),
        m_staticGeometryMeta(0),
        m_staticShape(0),
        m_ai(0)
    {
    }
    
    /**
     * This method is called when this game state is entered.
     *
     * @param fromState State from which we have entered
     */
    void enter(const std::string &fromState)
    {
      // Create our game scene
      m_camera = new Camera(m_scene);
      m_camera->setLag(20);
      m_camera->setZoom(Vector3f(-3.6, 2., 3.6));
      m_camera->setListener(new OpenALListener());
      m_scene->setCamera(m_camera);

      m_camera->lookAt(
        Vector3f(0.09511, 2.944, 0.0),
        Vector3f(3.0, 2.0, 0.0),
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
      lnode->child("object0")->setTexture(carpet);
      lnode->child("object1")->setTexture(brick);
      lnode->child("object2")->setTexture(stone);
      lnode->child("object3")->setTexture(metal);
      m_scene->attachNode(lnode);
      
      // Generate static geometry shape
      btVector3 aabbMin(-100, -100, -100), aabbMax(100, 100, 100);
      m_scene->update();
      m_staticGeometry = new btTriangleIndexVertexMaterialArray();
      m_scene->getRootNode()->batchStaticGeometry(m_staticGeometry);
      m_staticGeometryMeta = new GeometryMetadata(m_staticGeometry);
      m_staticShape = new btMultimaterialTriangleMeshShape(m_staticGeometry, true, aabbMin, aabbMax);
      
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
      
      // Create the sliding door
      new SlidingDoor(m_context);
      
      // Create some crates
      new Crate(Vector3f(26.0, 1.475, 0.0), m_context->getDynamicsWorld(), m_scene, m_storage);
      new Crate(Vector3f(26.0, 2.225, 0.0), m_context->getDynamicsWorld(), m_scene, m_storage);
      new Crate(Vector3f(26.0, 1.475, 1.0), m_context->getDynamicsWorld(), m_scene, m_storage);
      
      // AI setup
      // FIXME proper width & height
      m_ai = new AIController(100.0, 100.0);
      
      // Create the robot
      m_robot = new Robot(m_context, m_camera, m_ai);
      m_context->getTriggerManager()->setPickOwner(m_robot);
      
      // Create some enemies
      new Toad(Vector3f(34.0, 0.55, 6.5), m_context, m_robot, m_ai);
      new Toad(Vector3f(34.0, 0.55, 3.0), m_context, m_robot, m_ai);
      new Toad(Vector3f(34.0, 0.55, 0.0), m_context, m_robot, m_ai);
      new Toad(Vector3f(34.0, 0.55, -3.0), m_context, m_robot, m_ai);
      new Toad(Vector3f(34.0, 0.55, -6.5), m_context, m_robot, m_ai);
      
      // Init some background music
      m_soundPlayer = new OpenALPlayer();
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
      
      // Subscribe to events
      EventDispatcher *eventDispatcher = m_context->getEventDispatcher();
      m_keyboardEvents = eventDispatcher->signalKeyboard.connect(boost::bind(&PlayState::keyboardEvent, this, _1));
    }
    
    /**
     * Keyboard event handler.
     */
    void keyboardEvent(KeyboardEvent *ev)
    {
      // First check if GUI will handle keyboard events
      if (m_context->getGuiManager()->handleKeyboard(ev))
        return;
      
      if (!ev->isSpecial()) {
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
          
          // Display game menu when escape is pressed
          case 27: {
            if (!ev->isReleased())
              getGsm()->transitionTo("menu", true);
            break;
          }
        }
      }
    }
    
    /**
     * This method is called when this game state has been put on
     * the stack. The state is still active and will receive updates.
     *
     * @param state State that has now become active
     */
    void stack(const std::string &state)
    {
      m_keyboardEvents.block();
    }
    
    /**
     * This method is called when this game state has been popped
     * out of the stack and is now the current state.
     *
     * @param state State that has been active before
     */
    void unstack(const std::string &state)
    {
      m_keyboardEvents.unblock();
    }
    
    /**
     * This method is called when this game state is left behind.
     *
     * @param toState State to which we are going
     */
    void leave(const std::string &toState)
    {
      delete m_staticGeometryMeta;
      delete m_staticGeometry;
      delete m_robot;
      delete m_soundPlayer;
      delete m_ai;
      
      m_staticGeometryMeta = 0;
      m_staticGeometry = 0;
      m_robot = 0;
      m_soundPlayer = 0;
      m_ai = 0;
    }
private:
    Context *m_context;
    Scene *m_scene;
    Storage *m_storage;
    Camera *m_camera;
    
    // Ambiental player
    Player *m_soundPlayer;
    
    // Actors
    Robot *m_robot;
    
    // Phsyics
    btTriangleIndexVertexMaterialArray *m_staticGeometry;
    GeometryMetadata *m_staticGeometryMeta;
    btBvhTriangleMeshShape *m_staticShape;
    
    // AI
    AIController *m_ai;
    
    // Events
    boost::signals::connection m_keyboardEvents;
};

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
      
      // Subscribe to events
      EventDispatcher *eventDispatcher = m_context->getEventDispatcher();
      eventDispatcher->signalMousePress.connect(boost::bind(&Game::mousePressEvent, this, _1));
      eventDispatcher->signalMouseRelease.connect(boost::bind(&Game::mouseReleaseEvent, this, _1));
      eventDispatcher->signalMouseMove.connect(boost::bind(&Game::mouseMoveEvent, this, _1));
      
      // Setup on contact added callback
      gContactAddedCallback = bulletContactAddedCallback;
    }
    
    /**
     * Mouse button press event handler.
     */
    void mousePressEvent(MouseEvent *ev)
    {
      // First check if the GUI will handle mouse press, if not use it for picking objects
      if (!m_context->getGuiManager()->handleMousePress(ev))
        m_context->getTriggerManager()->dispatchPickEvent(ev->x(), ev->y());
    }
    
    /**
     * Mouse button release event handler.
     */
    void mouseReleaseEvent(MouseEvent *ev)
    {
      m_context->getGuiManager()->handleMouseRelease(ev);
    }
    
    /**
     * Mouse move event handler.
     */
    void mouseMoveEvent(MouseEvent *ev)
    {
      m_context->getGuiManager()->handleMouseMove(ev);
    }
    
    /**
     * Prepares the scene.
     */
    void prepare()
    {
      // Configure scene perspective
      m_context->scene()->setPerspective(45., 1024., 768., 0.1, 100.0);
      
      // Configure game states
      GameStateManager *gsm = m_context->getGameStateManager();
      gsm->addState(new MenuState(m_context));
      gsm->addState(new PlayState(m_context));
      gsm->transitionTo("menu");
    }
    
    /**
     * Enter the main engine loop.
     */
    void start()
    {
      m_context->start();
    }
private:
    Context *m_context;
};

int main()
{
  Game game;
  game.prepare();
  game.start();
  
  return 0;
}
