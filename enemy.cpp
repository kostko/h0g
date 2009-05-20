/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */

#include "enemy.h"
#include "ai.h"

int Enemy::m_enemyId = 0;

Enemy::Enemy(btDynamicsWorld *world, IID::Scene *scene, Robot *target, AIController *ai)
  : m_world(world),
    m_target(target),
    m_life(100),
    m_mapBody(0),
    m_scene(scene),
    m_ai(ai)
{
}

MapBody *Enemy::getMapBody() const
{
  return m_mapBody;
}
