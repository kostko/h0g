/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_IMPORTERS_MESH_H
#define IID_IMPORTERS_MESH_H

#include "storage/importers/base.h"
#include "globals.h"

#include <string>

namespace IID {

class Item;
class Storage;

/**
 * An abstract mesh importer that defines some convenience functions for
 * other importers to use.
 */
class MeshImporter : public Importer {
public:
    /**
     * Class constructor.
     *
     * @param context A valid IID context
     */
    MeshImporter(Context *context);
    
    /**
     * Loads a 3DS mesh file into the respective Mesh/CompositeMesh item.
     *
     * @param storage Item's storage
     * @param item Item to load the data into
     * @param filename Already resolved filename
     */
    virtual void load(Storage *storage, Item *item, const std::string &filename) = 0;
protected:
    /**
     * Computes vertex normals.
     *
     * @param vertexCount Vertex count
     * @param faceCount Face count
     * @param vertices Vertex list
     * @param faces Face list
     * @return List of normals (caller is responsible for freeing this)
     */
    float *computeNormals(int vertexCount, int faceCount, float *vertices, unsigned int *faces) const;
    
    /**
     * Scales the 3D mesh.
     *
     * @param factorX X-axis scale factor
     * @param factorY Y-axis scale factor
     * @param factorZ Z-axis scale factor
     * @param vertexCount Vertex count
     * @param vertices Vertex list
     */
    void scaleMesh(float factorX, float factorY, float factorZ, int vertexCount, float *vertices) const;
    
    /**
     * Translates vertices for some vector.
     *
     * @param vector Translation vector
     * @param vertexCount Vertex count
     * @param vertices Vertex list
     */
    void translateMesh(const Vector3f &vector, int vertexCount, float *vertices) const;
    
    /**
     * Calculates mesh geometric center.
     *
     * @param mind Minimum dimensions
     * @param maxd Maximum dimensions
     * @return Geometric center
     */
    Vector3f geometricCenter(const Vector3f &mind, const Vector3f &maxd) const;
};

}

#endif
