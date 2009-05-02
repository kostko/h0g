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
#include "storage/texture.h"
#include "storage/shader.h"

// Scene
#include "scene/scene.h"
#include "scene/node.h"
#include "scene/camera.h"

using namespace IID;

int main()
{
  Context *context = new IID::Context();
  context->init();
  
  Scene *scene = context->scene();
  Storage *storage = context->storage();
  
  // Prepare the scene
  scene->setPerspective(45., 1024. / 768., 0.1, 100.0);
  
  // Create a test scene
  Camera *camera = new Camera(scene);
  scene->setCamera(camera);
  camera->lookAt(
    Vector3f(0., 0., 6.),
    Vector3f(0., 0., 0.),
    Vector3f(0., 1., 0.)
  );
  
  Shader *shader = storage->get<Shader>("/Shaders/general");
  Mesh *mesh = storage->get<Mesh>("/Models/spaceship");
  Texture *texture = storage->get<Texture>("/Textures/spaceship");
  
  Shader *shader2 = storage->get<Shader>("/Shaders/material");
  Mesh *mesh2 = storage->get<Mesh>("/Models/r2-d2");
  
  SceneNode *node3 = scene->createNodeFromStorage(mesh2);
  node3->setShader(shader2);
  node3->setOrientation(
    AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
    AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
  scene->attachNode(node3);
  
  SceneNode *node4 = scene->createNodeFromStorage(mesh2, "r3-d2");
  node4->setShader(shader2);
  node4->setPosition(0.9, 0., 0.);
  node4->setOrientation(
    AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
    AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
  scene->attachNode(node4);
  
  SceneNode *node5 = scene->createNodeFromStorage(mesh2, "r4-d2");
  node5->setShader(shader2);
  node5->setPosition(-0.9, 0., 0.);
  node5->setOrientation(
    AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
    AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
  scene->attachNode(node5);
  
  SceneNode *node6 = scene->createNodeFromStorage(mesh2, "r5-d2");
  node6->setShader(shader2);
  node6->setPosition(1.8, 0., 0.);
  node6->setOrientation(
    AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
    AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
  scene->attachNode(node6);
  
  SceneNode *node7 = scene->createNodeFromStorage(mesh2, "r6-d2");
  node7->setShader(shader2);
  node7->setPosition(-1.8, 0., 0.);
  node7->setOrientation(
    AngleAxisf(0.5*M_PI, Vector3f::UnitX()) *
    AngleAxisf(1.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
  scene->attachNode(node7);
  
  /*RendrableNode *node = static_cast<RendrableNode*>(scene->createNodeFromStorage(mesh));
  node->setTexture(texture);
  node->setShader(shader);
  node->setOrientation(
    AngleAxisf(0.25*M_PI, Vector3f::UnitX()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitZ())
  );
  scene->attachNode(node);
  
  RendrableNode *node2 = static_cast<RendrableNode*>(scene->createNodeFromStorage(mesh));
  node2->setTexture(texture);
  node2->setShader(shader);
  node2->setPosition(0., 0., 1.);
  node2->setOrientation(
    AngleAxisf(0.0*M_PI, Vector3f::UnitX()) *
    AngleAxisf(0.0*M_PI, Vector3f::UnitY()) *
    AngleAxisf(0.5*M_PI, Vector3f::UnitZ())
  );
  node->attachChild(node2);*/
  
  // Enter the render loop
  context->start();
  return 0;
}
