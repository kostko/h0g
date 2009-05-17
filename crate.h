/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef HOG_CRATE_H
#define HOG_CRATE_H

// IID includes
#include "scene/node.h"
#include "storage/storage.h"

// Bullet dynamics
#include <btBulletDynamicsCommon.h>

class EntityMotionState;

/**
 * A simple crate that can be pushed around.
 */
class Crate {
public:
    /**
     * Class constructor.
     *
     * @param pos Initial position in world coordinates
     * @param world Dynamics world
     * @param scene Scene instance
     * @param storage Storage
     */
    Crate(const Vector3f &pos, btDynamicsWorld *world, IID::Scene *scene, IID::Storage *storage);
    
    /**
     * Class destructor.
     */
    ~Crate();
private:
    static int m_crateId;
    IID::SceneNode *m_sceneNode;
    btRigidBody *m_body;
    btCollisionShape *m_shape;
    EntityMotionState *m_motionState;    
};

#endif
