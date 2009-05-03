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

#include <boost/bind.hpp>

using namespace IID;

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
    }
    
    /**
     * Keyboard event handler.
     */
    void keyboardEvent(KeyboardEvent *ev)
    {
      if (ev->isSpecial()) {
        // Handle camera movements
        switch (ev->key()) {
          case EventDispatcher::Up: m_camera->walk(0.2); break;
          case EventDispatcher::Down: m_camera->walk(-0.2); break;
          case EventDispatcher::Left: m_camera->rotate(0.0, 2.0, 0.0); break;
          case EventDispatcher::Right: m_camera->rotate(0.0, -2.0, 0.0); break;
        }
      } else if (ev->key() == 27) {
        // Escape pressed, let's exit
        exit(0);
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
      CompositeMesh *level = m_storage->get<CompositeMesh>("/Models/level");
      Texture *brick = m_storage->get<Texture>("/Textures/Brick/rough_dark");
      Texture *carpet = m_storage->get<Texture>("/Textures/Carpet/plush_forest");
      Texture *stone = m_storage->get<Texture>("/Textures/Stone/vein_gray");
      Texture *metal = m_storage->get<Texture>("/Textures/Metal/rusted");
      
      SceneNode *lnode = m_scene->createNodeFromStorage(level);
      lnode->setShader(shader);
      lnode->child("object1")->setTexture(brick);
      lnode->child("object2")->setTexture(metal);
      lnode->child("object3")->setTexture(carpet);
      lnode->child("object4")->setTexture(stone);
      lnode->setOrientation(
        AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitY()) *
        AngleAxisf(1.25*M_PI, Vector3f::UnitZ())
      );
      m_scene->attachNode(lnode);
      
      /*SceneNode *node3 = m_scene->createNodeFromStorage(mesh2);
      node3->setShader(shader2);
      node3->setOrientation(
        AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
        AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
      );
      m_scene->attachNode(node3);
      
      SceneNode *node4 = m_scene->createNodeFromStorage(mesh2, "r3-d2");
      node4->setShader(shader2);
      node4->setPosition(0.9, 0., 0.);
      node4->setOrientation(
        AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
        AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
      );
      m_scene->attachNode(node4);
      
      SceneNode *node5 = m_scene->createNodeFromStorage(mesh2, "r4-d2");
      node5->setShader(shader2);
      node5->setPosition(-0.9, 0., 0.);
      node5->setOrientation(
        AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
        AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
      );
      m_scene->attachNode(node5);
      
      SceneNode *node6 = m_scene->createNodeFromStorage(mesh2, "r5-d2");
      node6->setShader(shader2);
      node6->setPosition(1.8, 0., 0.);
      node6->setOrientation(
        AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
        AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
      );
      m_scene->attachNode(node6);
      
      SceneNode *node7 = m_scene->createNodeFromStorage(mesh2, "r6-d2");
      node7->setShader(shader2);
      node7->setPosition(-1.8, 0., 0.);
      node7->setOrientation(
        AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
        AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
        AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
      );
      m_scene->attachNode(node7);*/
      
      /*RendrableNode *node = static_cast<RendrableNode*>(m_scene->createNodeFromStorage(mesh));
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
      node->attachChild(node2);*/
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
};

int main()
{
  Game game;
  game.prepare();
  game.start();
  
  return 0;
}
