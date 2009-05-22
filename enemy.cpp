/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "enemy.h"
#include "ai.h"

// IID includes
#include "context.h"

using namespace IID;

int Enemy::m_enemyId = 0;

Enemy::Enemy(const std::string &type, Context *context, Robot *target, AIController *ai)
  : Entity(context, type),
    m_world(context->getDynamicsWorld()),
    m_target(target),
    m_life(100),
    m_mapBody(0),
    m_scene(context->scene()),
    m_ai(ai)
{
}

MapBody *Enemy::getMapBody() const
{
  return m_mapBody;
}
