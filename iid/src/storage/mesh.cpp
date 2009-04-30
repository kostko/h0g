/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/mesh.h"
#include "drivers/base.h"
#include "context.h"

#include <iostream>
#define RECORD_SIZE 32

namespace IID {

Mesh::Mesh(Storage *storage, const std::string &itemId, Item *parent)
  : Item(storage, "Mesh", itemId, "", parent),
    m_attributes(0),
    m_indices(0),
    m_rawVertices(0),
    m_rawIndices(0),
    m_primitive(Driver::Triangles)
{
}

Mesh::~Mesh()
{
  delete m_rawVertices;
  delete m_rawIndices;
  delete m_attributes;
  delete m_indices;
}

void Mesh::setMesh(int vertexCount, int indexCount, unsigned char *vertices, unsigned char *normals,
                   unsigned char *tex, unsigned char *indices, Driver::DrawPrimitive primitive)
{
  // Copy raw vertices
  m_rawVertices = new float[vertexCount * 3];
  memcpy(m_rawVertices, vertices, sizeof(float) * vertexCount * 3);
  
  m_rawIndices = new unsigned int[indexCount];
  memcpy(m_rawIndices, indices, sizeof(unsigned int) * indexCount);
  
  // Combine everything into one big array
  unsigned char *attributes = new unsigned char[vertexCount * RECORD_SIZE];
  for (int i = 0; i < vertexCount; i++) {
    memcpy(attributes + i*32, vertices + i*12, 12);
    
    if (normals)
      memcpy(attributes + i*32 + 12, normals + i*12, 12);
    else
      memset(attributes + i*32 + 12, 0, 12);
    
    if (tex)
      memcpy(attributes + i*32 + 24, tex + i*8, 8);
    else
      memset(attributes + i*32 + 24, 0, 8);
  }
  
  m_primitive = primitive;
  m_driver = m_storage->context()->driver();
  m_vertexCount = vertexCount;
  m_indexCount = indexCount;
  m_attributes = m_driver->createVertexBuffer(
    vertexCount * RECORD_SIZE,
    attributes,
    DVertexBuffer::StaticDraw,
    DVertexBuffer::VertexArray
  );
  m_indices = m_driver->createVertexBuffer(
    indexCount * 4,
    indices,
    DVertexBuffer::StaticDraw,
    DVertexBuffer::ElementArray
  );
  
  delete attributes;
}

void Mesh::setBounds(const Vector3f &min, const Vector3f &max)
{
  m_boundAABB = AxisAlignedBox(min, max);
}

void Mesh::bind() const
{
  m_attributes->bind();
  m_indices->bind();
  
  DShader *shader = m_driver->currentShader();
  if (shader) {
    shader->bindAttributePointer("Vertex", 3, RECORD_SIZE, 0);
    shader->bindAttributePointer("Normal", 3, RECORD_SIZE, 12);
    shader->bindAttributePointer("TexCoord", 2, RECORD_SIZE, 24);
  }
}

void Mesh::unbind() const
{
  m_attributes->unbind();
  m_indices->unbind();
}

void Mesh::draw() const
{
  m_driver->drawElements(m_indexCount, 0, m_primitive);
}

Item *MeshFactory::create(Storage *storage, const std::string &itemId, Item *parent)
{
  return new Mesh(storage, itemId, parent);
}

}
