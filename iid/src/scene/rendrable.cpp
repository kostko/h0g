/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/rendrable.h"
#include "scene/scene.h"
#include "scene/lightmanager.h"
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
    m_staticGeomMesh->m_numTriangles = m_mesh->indexCount() / 3;
    m_staticGeomMesh->m_triangleIndexBase = (unsigned char*) m_mesh->indices();
    m_staticGeomMesh->m_triangleIndexStride = 3*sizeof(unsigned int);
    
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
    m_staticGeomMesh->m_vertexStride = 3*sizeof(float);
    triangles->addIndexedMesh(*m_staticGeomMesh);
  }
}

const LightList &RendrableNode::getLights() const
{
  if (m_lightManager) {
    // Check whether our light cache is up to date
    if (m_lightManager->getLightVersionCounter() != m_lightVersionCounter) {
      m_lightManager->computeAffectingLights(m_affectingLights, m_worldPosition, m_localBounds.getRadius());
    }
  }
  
  return m_affectingLights;
}

void RendrableNode::render(StateBatcher *batcher)
{
  batcher->addToQueue(this);
}

}
