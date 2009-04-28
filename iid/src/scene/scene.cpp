/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "scene/scene.h"
#include "scene/node.h"
#include "scene/viewtransform.h"
#include "renderer/statebatcher.h"
#include "context.h"

namespace IID {

Scene::Scene(Context *context)
  : m_context(context),
    m_root(new SceneNode(0, this)),
    m_stateBatcher(new StateBatcher(this)),
    m_viewTransform(new ViewTransform())
{
}

}
