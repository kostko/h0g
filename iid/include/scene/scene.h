/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_H
#define IID_SCENE_H

#include "globals.h"

namespace IID {

class Context;
class SceneNode;
class StateBatcher;
class ViewTransform;

/**
 * Represents the 3D scene.
 */
class Scene {
public:
    /**
     * Class constructor.
     *
     * @param context IID engine context
     */
    Scene(Context *context);
    
    /**
     * Returns the current engine context.
     */
    Context *context() { return m_context; }
    
    /**
     * Returns the state batcher instance.
     */
    StateBatcher *stateBatcher() const { return m_stateBatcher; }
    
    /**
     * Returns the view transformation instance.
     */
    ViewTransform *viewTransform() { return m_viewTransform; }
private:
    Context *m_context;
    
    // Scene view transformation
    ViewTransform *m_viewTransform;

    // Root scene node
    SceneNode *m_root;
    
    // Render batcher
    StateBatcher *m_stateBatcher;
};

}

#endif
