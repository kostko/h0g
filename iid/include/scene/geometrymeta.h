/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_GEOMETRYMETA_H
#define IID_SCENE_GEOMETRYMETA_H

#include "globals.h"

#include <BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.h>
#include <BulletCollision/CollisionShapes/btMaterial.h>

#include <list>
#include <vector>

class btMaterialProperties;

namespace IID {

/**
 * This class is used to hold static geometry metadata needed for proper
 * collision detection. It provides information about triangle connectivity
 * and edge angles (this can be used to identify internal trimesh edges and
 * process collision contacts accordingly).
 */
class GeometryMetadata {
public:
    /**
     * A simple structure for holding face metadata.
     */
    class FaceInfo : public btMaterial {
    public:
        float edgeAngles[3];
    };
    
    /**
     * Class constructor.
     *
     * @param geometry Properly batched static trimesh geometry
     */
    GeometryMetadata(btTriangleIndexVertexMaterialArray *geometry);

    /**
     * Class destructor.
     */
    ~GeometryMetadata();
protected:
    /**
     * A helper method that processes mesh connectivity.
     *
     * @param mesh A valid indexed mesh pointer
     */
    void processConnectivity(btIndexedMesh *mesh);
    
    /**
     * A helper method that determines "edge angles".
     *
     * @param mesh A valid indexed mesh pointer
     */
    void processEdgeAngles(btIndexedMesh *mesh);
private:
    // Geometry store
    btTriangleIndexVertexMaterialArray *m_geometry;
    
    // Data for edge angle processing
    typedef std::vector<float> Vertex;
    typedef std::pair<Vertex, Vertex> Edge;
    typedef std::pair<btIndexedMesh*, int> Triangle;
    typedef boost::unordered_map<Edge, std::list<Triangle> > ConnectivityMap;
    typedef boost::unordered_map<Triangle, Vector3f> NormalMap;
    ConnectivityMap m_connectivity;
    NormalMap m_normals;
    std::list<btMaterialProperties*> m_faceInfos;
};

}

#endif
