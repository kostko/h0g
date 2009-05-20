/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */

#include <math.h>
#include "ai.h"

#include <boost/foreach.hpp>

MapBody::MapBody(SceneNode *n, Type type)
  : m_sceneNode(n),
    m_type(type)
{
}

MapBody::~MapBody()
{
}

void MapBody::setPosition(Vector2i idx)
{
  m_currCell = idx;
}

Vector2i MapBody::getPosition()
{
  return m_currCell;
}

AIController::AIController(float mapWidth, float mapHeight)
{
  // Make a grid of empty cells
  m_grid = MatrixXi::Constant(GRID_WIDTH, GRID_HEIGHT, Empty);
  // Setup some map properties
  m_dx = mapWidth / GRID_WIDTH;
  m_dy = mapHeight / GRID_HEIGHT;
}

AIController::~AIController()
{
}

void AIController::addMapBody(MapBody *body)
{
  Vector3f pos = body->m_sceneNode->getWorldPosition();
  Vector2i gridPos = convert(pos.x(), pos.y());
  
  // Set the location on the map by converting the node's 
  // world position to cell indices.
  body->setPosition(gridPos);
  
  if (body->m_type == MapBody::Dynamic) {
    // Add this body to the dynamic bodies list
    m_dynamic.push_back(body);
  }
  
  // Mark the proper cell in the grid as full
  m_grid(gridPos.x(), gridPos.y()) = Full;
}

void AIController::removeMapBody(MapBody *body)
{
  Vector2i gridPos = body->getPosition();
  
  if (body->m_type == MapBody::Dynamic) {
    // We must also remove the body from the dynamic body list
    m_dynamic.remove(body);
  }
  
  // Mark the proper cell in the grid as empty
  m_grid(gridPos.x(), gridPos.y()) = Empty;
}

Vector2i AIController::convert(float x, float y)
{
  int i = floor(x / GRID_WIDTH);
  int j = floor(y / GRID_HEIGHT);
  
  // Check for proper bounds
  if ( i >= GRID_WIDTH || j >= GRID_HEIGHT )
  {
    // FIXME logger
    std::cout << "Body out of bounds!" << std::endl;
    return Vector2i(0, 0);
  }
  
  return Vector2i(i, j);
}

Vector3f AIController::getDirection(MapBody *origin, MapBody *target)
{
  Vector3f originPos = origin->m_sceneNode->getWorldPosition();
  Vector3f targetPos = target->m_sceneNode->getWorldPosition();
  
  return originPos - targetPos;
}

void AIController::updateMap()
{
  BOOST_FOREACH(MapBody *body, m_dynamic)
  {
    Vector3f pos = body->m_sceneNode->getWorldPosition();
    Vector2i gridPos = convert(pos.x(), pos.y());
    
    if (body->getPosition() != gridPos)
    {
      // If the body has migrated to a new cell update its position
      body->setPosition(gridPos);
    }
  }
  
  // Do path updates
  // TODO
}
