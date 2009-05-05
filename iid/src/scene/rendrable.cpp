/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/rendrable.h"
#include "scene/scene.h"
#include "renderer/statebatcher.h"
#include "storage/mesh.h"
#include "storage/texture.h"
#include "storage/shader.h"
#include "storage/material.h"
#include "drivers/base.h"

// Bullet
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>

namespace IID {

RendrableNode::RendrableNode(const std::string &name, SceneNode *parent)
  : SceneNode(name, parent),
    m_mesh(0),
    m_texture(0),
    m_shader(0),
    m_material(0),
    m_showBoundingBox(false),
    m_staticGeomMesh(0)
{
}

RendrableNode::~RendrableNode()
{
  if (m_staticGeomMesh) {
    delete m_staticGeomMesh->m_vertexBase;
    delete m_staticGeomMesh;
  }
}

void RendrableNode::setMesh(Mesh *mesh)
{
  m_mesh = mesh;
  m_localBounds = mesh->getAABB();
}

void RendrableNode::setTexture(Texture *texture)
{
  m_texture = texture;
  SceneNode::setTexture(texture);
}

void RendrableNode::setShader(Shader *shader)
{
  m_shader = shader;
  SceneNode::setShader(shader);
}

void RendrableNode::setMaterial(Material *material)
{
  m_material = material;
  SceneNode::setMaterial(material);
}

void RendrableNode::setShowBoundingBox(bool value)
{
  m_showBoundingBox = value;
}

void RendrableNode::batchStaticGeometry(btTriangleIndexVertexArray *triangles)
{
  SceneNode::batchStaticGeometry(triangles);
  
  // Add transformed vertices
  if (m_static && m_mesh) {
    m_staticGeomMesh = new btIndexedMesh();
    m_staticGeomMesh->m_numVertices = m_mesh->vertexCount();
    m_staticGeomMesh->m_numTriangles = m_mesh->indexCount();
    m_staticGeomMesh->m_triangleIndexBase = (unsigned char*) m_mesh->indices();
    
    // Transform original vertices using node's world transformation
    float *orig = m_mesh->vertices();
    float *vertices = new float[3 * m_mesh->vertexCount()];
    for (int i = 0; i < m_mesh->vertexCount(); i++) {
      Vector3f p = m_worldTransform * Vector3f(orig + 3*i);
      vertices[3*i] = p[0];
      vertices[3*i + 1] = p[1];
      vertices[3*i + 2] = p[2];
    }
    
    m_staticGeomMesh->m_vertexBase = (unsigned char*) vertices;
    triangles->addIndexedMesh(*m_staticGeomMesh);
  }
}

void RendrableNode::render(StateBatcher *batcher)
{
  batcher->add(
    m_shader,
    m_mesh,
    m_texture,
    m_material,
    m_worldTransform
  );
  
  // Also add a bounding box if set
  /*if (m_showBoundingBox) {
    if (!m_bbMesh) {
      // Generate a bounding box mesh
      Vector3f corners[8];
      m_worldBounds.getCorners(corners);
      
      float vertices[27] = {
        corners[0][0], corners[0][1], corners[0][2],
        corners[1][0], corners[1][1], corners[1][2],
        corners[2][0], corners[2][1], corners[2][2],
        corners[3][0], corners[3][1], corners[3][2],
        corners[4][0], corners[4][1], corners[4][2],
        corners[5][0], corners[5][1], corners[5][2],
        corners[6][0], corners[6][1], corners[6][2],
        corners[7][0], corners[7][1], corners[7][2]
      };
      
      unsigned int indices[24] = {
        0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
        6, 7, 7, 4, 0, 6, 1, 5, 2, 4, 3, 7
      };
      
      m_bbMesh = new Mesh(m_mesh->storage());
      m_bbMesh->setMesh(
        8,
        24,
        (unsigned char*) &vertices,
        0,
        0,
        (unsigned char*) &indices,
        Driver::Lines
      );
    }
    
    batcher->add(
      m_mesh->storage()->get<Shader>("/Shaders/debug"),
      m_bbMesh,
      0,
      0,
      Transform3f(Matrix4f::Identity())
      //m_worldTransform
    );
  }*/
}

}
