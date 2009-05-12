/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef HOG_MOTIONSTATE_H
#define HOG_MOTIONSTATE_H

// IID includes
#include "scene/node.h"

// Bullet dynamics
#include <btBulletDynamicsCommon.h>

/**
 * Entity motion state.
 */
class EntityMotionState : public btMotionState {
public:
    /**
     * Class constructor.
     *
     * @param initial Initial world transformation
     * @param node Entity scene node
     */
    EntityMotionState(const btTransform &initial, IID::SceneNode *node);
    
    /**
     * Class constructor.
     *
     * @param node Entity scene node
     */
    EntityMotionState(IID::SceneNode *node);
    
    /**
     * Sets the initial world transformation.
     *
     * @param initial Initial world transformation
     */
    void setInitialTransform(const btTransform &initial);
    
    /**
     * Returns the initial world transformation.
     */
    void getWorldTransform(btTransform &worldTransform) const;
    
    /**
     * Propagate world transformation to the scene graph.
     *
     * @param worldTransform World transformation
     */
    void setWorldTransform(const btTransform &worldTransform);
private:
    IID::SceneNode *m_sceneNode;
    btTransform m_initial;
    bool m_useInitial;
};

#endif
