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

// Scene
#include "scene/scene.h"
#include "scene/node.h"
#include "scene/camera.h"

// Events
#include "events/dispatcher.h"
#include "events/keyboard.h"

// Bullet dynamics
#include <btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btTriangleShape.h>

#include <boost/bind.hpp>

using namespace IID;

class CharacterState {
public:
    enum Movement {
      FORWARD = 1,
      BACKWARD = 2,
      LEFT = 4,
      RIGHT = 8
    };
    
    // Movement bitfield
    int movement;
    
    // Jump
    bool jump;
    
    // Hover
    bool hover;
    
    /**
     * Class constructor.
     */
    CharacterState()
      : movement(0),
        jump(false),
        hover(false)
    {}
};

class Robot : public btActionInterface {
public:
    /**
     * Robot's motion state.
     */
    class MotionState : public btMotionState {
    public:
        /**
         * Class constructor.
         *
         * @param initial Initial world transformation
         * @param node Robot's scene node
         */
        MotionState(const btTransform &initial, SceneNode *node)
          : m_sceneNode(node),
            m_initial(initial)
        {
        }
        
        /**
         * Returns the initial world transformation.
         */
        void getWorldTransform(btTransform &worldTransform) const
        {
          worldTransform = m_initial;
        }
        
        /**
         * Propagate world transformation to the scene graph.
         *
         * @param worldTransform World transformation
         */
        void setWorldTransform(const btTransform &worldTransform)
        {
          btQuaternion rot = worldTransform.getRotation();
          btVector3 pos = worldTransform.getOrigin();
          m_sceneNode->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
          m_sceneNode->setPosition(pos.x(), pos.y(), pos.z());
        }
    private:
        SceneNode *m_sceneNode;
        btTransform m_initial;
    };
    
    /**
     * Class constructor.
     */
    Robot(btDynamicsWorld *world, Scene *scene, Storage *storage)
    {
      CompositeMesh *robotMesh = storage->get<CompositeMesh>("/Models/r2-d2");
      Shader *shader = storage->get<Shader>("/Shaders/material");
      
      // Create the robot's scene node
      m_sceneNode = scene->createNodeFromStorage(robotMesh);
      m_sceneNode->setShader(shader);
      m_sceneNode->setPosition(-1., 0., -8.25);
      m_sceneNode->setOrientation(
        AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
        AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
      );
      scene->attachNode(m_sceneNode);
      scene->update();
      
      // Create the robot's physical shape and body
      Vector3f hs = robotMesh->getAABB().getHalfSize();
      m_shape = new btCylinderShapeZ(btVector3(hs[0], hs[1], hs[2]));
      float mass = 80.0;
      btVector3 localInertia(0, 0, 0);
      m_shape->calculateLocalInertia(mass, localInertia);
      
      btTransform startTransform;
      startTransform.setFromOpenGLMatrix(m_sceneNode->worldTransform().data());
      m_motionState = new MotionState(startTransform, m_sceneNode);
      
      // We have to disable deactivation (so we can control the body when it stops) and set
      // the angular factor to zero, so our robot can't fall
      btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_motionState, m_shape, localInertia);
      m_body = new btRigidBody(cInfo);
      m_body->setActivationState(DISABLE_DEACTIVATION);
      m_body->setAngularFactor(0.0);
      world->addRigidBody(m_body);
      world->addAction(this);
      m_world = world;
      
      // Setup some attributes
      m_turnAngle = 0.0;
      m_maxLinearVelocity = 3.0;
      m_walkVelocity = 8.0;
      m_turnVelocity = 3.0;
    }
    
    /**
     * This method gets called on every step of the simulation and is responsible for
     * maintaining control over the robot.
     *
     * @param world World instance
     * @param dt Delta time
     */
    void updateAction(btCollisionWorld *world, btScalar dt)
    {
      btTransform transform = m_body->getCenterOfMassTransform();
      
      // Handle rotation
      if (m_state.movement & CharacterState::LEFT)
        transform.setRotation(transform.getRotation() * btQuaternion(btVector3(0.0, 0.0, 1.0), dt * m_turnVelocity));
      
      if (m_state.movement & CharacterState::RIGHT)
        transform.setRotation(transform.getRotation() * btQuaternion(btVector3(0.0, 0.0, 1.0), -dt * m_turnVelocity));
      
      // Handle walking
      btVector3 linearVelocity = m_body->getLinearVelocity();
      float speed = linearVelocity.length();
      
      btVector3 forwardVec = transform.getBasis() * btVector3(0.0, -1.0, 0.0);
      btVector3 walkDirection(0.0, 0.0, 0.0);
      float walkSpeed = m_walkVelocity * dt;
      forwardVec.normalize();
      
      if (m_state.movement & CharacterState::FORWARD)
        walkDirection += forwardVec;
      if (m_state.movement & CharacterState::BACKWARD)
        walkDirection -= forwardVec;
      
      // Check for hover mode
      if (m_state.hover) {
        m_hoverDeltaTime += dt;
        if (m_hoverDeltaTime > 0.05) {
          // Check distance to floor, so we can perform height corrections
          float floorDistance = getFloorDistance();
          float mdist = 1.2;
          m_hoverDeltaTime = 0;
          
          if (floorDistance < mdist) {
            linearVelocity += (transform.getBasis() * btVector3(0.0, 0.0, 60.0)) * walkSpeed;
          }
        }
      }
      
      if (!(m_state.movement & (CharacterState::FORWARD | CharacterState::BACKWARD)) && !m_state.hover) {
        // Not being moved, slow down gradually
        linearVelocity *= 0.2;
        m_body->setLinearVelocity(linearVelocity);
      } else {
        // We are in transit
        if (m_state.hover) {
          linearVelocity *= 0.2;
          walkSpeed *= 5;
        }
        
        if (speed < m_maxLinearVelocity) {
          btVector3 velocity = linearVelocity + walkDirection * walkSpeed;
          m_body->setLinearVelocity(velocity);
        }
      }
      
      // Update transformations
      m_body->getMotionState()->setWorldTransform(transform);
      m_body->setCenterOfMassTransform(transform);
    }
    
    /**
     * Returns the distance between the robot and the floor. This is done
     * via ray tests.
     */
    float getFloorDistance()
    {
      class ClosestNotMe : public btCollisionWorld::ClosestRayResultCallback {
      public:
          ClosestNotMe(btRigidBody *me)
            : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)),
              m_me(me)
          {}
          
          btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace)
          {
            if (rayResult.m_collisionObject == m_me)
              return 1.0;
            
            return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
          }
      private:
          btRigidBody *m_me;
      };
      
      float distance = 5.0;
      btTransform transform = m_body->getCenterOfMassTransform();
      btVector3 source = transform.getOrigin();
      btVector3 target = source + distance * (transform.getBasis() * btVector3(0.0, 0.0, -1.0)).normalized();
      
      ClosestNotMe rayCallback(m_body);
      rayCallback.m_closestHitFraction = 1.0;
      m_world->rayTest(source, target, rayCallback);
      if (rayCallback.hasHit()) {
        return rayCallback.m_closestHitFraction * distance;
      } else {
        return distance;
      }
    }
    
    /**
     * Debug drawer (required by btActionInterface).
     */
    void debugDraw(btIDebugDraw *drawer)
    {}
    
    /**
     * Enables or disables hover mode.
     */
    void hover()
    {
      m_state.hover = !m_state.hover;
      
      if (m_state.hover)
        m_hoverDeltaTime = 0.0;
    }
    
    /**
     * Enables or disables forward-vector movement.
     */
    void forward(bool value)
    {
      if (value)
        m_state.movement |= CharacterState::FORWARD;
      else
        m_state.movement &= ~CharacterState::FORWARD;
    }
    
    /**
     * Enables or disables backward-vector movement.
     */
    void backward(bool value)
    {
      if (value)
        m_state.movement |= CharacterState::BACKWARD;
      else
        m_state.movement &= ~CharacterState::BACKWARD;
    }
    
    /**
     * Enables or disables left-vector movement.
     */
    void left(bool value)
    {
      if (value)
        m_state.movement |= CharacterState::LEFT;
      else
        m_state.movement &= ~CharacterState::LEFT;
    }
    
    /**
     * Enables or disables right-vector movement.
     */
    void right(bool value)
    {
      if (value)
        m_state.movement |= CharacterState::RIGHT;
      else
        m_state.movement &= ~CharacterState::RIGHT;
    }
private:
    SceneNode *m_sceneNode;
    btCollisionShape *m_shape;
    btRigidBody *m_body;
    MotionState *m_motionState;
    btDynamicsWorld *m_world;
    
    // Hover stuff
    float m_hoverDeltaTime;
    
    // Controller stuff
    float m_turnAngle;
    float m_maxLinearVelocity;
    float m_walkVelocity;
    float m_turnVelocity;
    
    // Character state
    CharacterState m_state;
};

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
          
          // Exit when escape is pressed
          case 27: exit(0);
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
