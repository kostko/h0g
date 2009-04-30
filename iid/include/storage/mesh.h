/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_STORAGE_MESH_H
#define IID_STORAGE_MESH_H

#include "storage.h"
#include "scene/aabb.h"
#include "drivers/base.h"

namespace IID {

class DVertexBuffer;
class Driver;

/**
 * This class represents a 3D mesh object backed by a VBO.
 */
class Mesh : public Item {
public:
    /**
     * Class constructor.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param path Item path
     * @param parent Optional parent instance
     */
    Mesh(Storage *storage, const std::string &itemId = "", Item *parent = 0);
    
    /**
     * Class destructor.
     */
    ~Mesh();
    
    /**
     * Specifies a mesh (this creates vertex buffers). Vertices and indices arrays
     * should NOT be freed after this function is done, since they are used internaly.
     * They will be freed when mesh object is destroyed.
     *
     * @param vertexCount Number of vertices
     * @param indexCount Number of indices
     * @param vertices Vertex data
     * @param normals Normal data
     * @param tex Texture coordinate data
     * @param indices Index data
     */
    void setMesh(int vertexCount, int indexCount, unsigned char *vertices, unsigned char *normals,
                 unsigned char *tex, unsigned char *indices, Driver::DrawPrimitive primitive = Driver::Triangles);
    
    /**
     * Specifies mesh boundaries.
     *
     * @param min Minimum AABB corner
     * @param max Maximum AABB corner
     */
    void setBounds(const Vector3f &min, const Vector3f &max);
    
    /**
     * Returns the axis aligned bounding box of this 3D mesh.
     */
    AxisAlignedBox getAABB() const { return m_boundAABB; }
    
    /**
     * Binds associated index and vertex buffers. This also configures shader
     * attributes.
     */
    void bind() const;
    
    /**
     * Unbinds associated index and vertex buffers.
     */
    void unbind() const;
    
    /**
     * Draws the mesh.
     */
    void draw() const;
    
    /**
     * Returns number of vertices in this mesh.
     */
    int vertexCount() const { return m_vertexCount; }
    
    /**
     * Returns number of indices in this mesh.
     */
    int indexCount() const { return m_indexCount; }
    
    /**
     * Returns a vertex list associated with this mesh.
     */
    float *vertices() const { return m_rawVertices; }
    
    /**
     * Returns an index list associated with this mesh.
     */
    unsigned int *indices() const { return m_rawIndices; }
private:
    // Vertex buffers
    DVertexBuffer *m_attributes;
    DVertexBuffer *m_indices;
    Driver *m_driver;
    
    // Vertex and index count
    int m_vertexCount;
    int m_indexCount;
    float *m_rawVertices;
    unsigned int *m_rawIndices;
    
    // Boundaries
    AxisAlignedBox m_boundAABB;
    
    // Primitive type
    Driver::DrawPrimitive m_primitive;
};

/**
 * A factory of Mesh items.
 */
class MeshFactory : public ItemFactory {
public:
    /**
     * Create a new Mesh storage item.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param parent Parent item
     * @return A valid Mesh item
     */
    Item *create(Storage *storage, const std::string &itemId, Item *parent);
};

}

#endif
