/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef HOG_AI_H
#define HOG_AI_H

#include "scene/node.h"

#define GRID_WIDTH 10
#define GRID_HEIGHT 10

using namespace IID;

/** 
 * Represents a body in the map.
 */
class MapBody {
friend class AIController;
public:
    /**
     * Dynamic bodies trigger updates while static bodies 
     * don't (and they don't need notifications about changes).
     */
    enum Type {
      Dynamic,
      Static
    };
  
    /**
     * Class constructor.
     */
    MapBody(SceneNode *n, Type type);
    
    /**
     * Class destructor.
     */
    ~MapBody();
private:
    /**
     * Set position.
     *
     * @param idx Cell index
     */
    void setPosition(Vector2i idx);
    
    /**
     * Get position.
     *
     * @return Cell index
     */
    Vector2i getPosition();
    
    // Body type
    Type m_type;
    SceneNode *m_sceneNode;
    
    // Position on the grid map
    Vector2i m_currCell;
};

/**
 * The AIController can provide support for various 
 * decisions the game entities make. Such as
 * "what is the best way to get to (x,y)?".
 */
class AIController {
public:
    /**
     * Possible cell states.
     */
    enum CellState {
      Empty,
      Full
    };
  
    /**
     * Class constructor.
     */
    AIController(float mapHeight, float mapWidth);
    
    /**
     * Class destructor.
     */
    ~AIController();
    
    /**
     * Add a map body.
     *
     * @param body A map body instance
     */
    void addMapBody(MapBody *body);
    
    /**
     * Remove a map body.
     *
     * @param body Map body instance to remove
     */
    void removeMapBody(MapBody *body);
    
    /**
     * Returns the direction which the originator must follow to get to the target.
     *
     * @param origin MapBody instance of the origin
     * @param target MapBody instance of the target
     */
    Vector3f getDirection(MapBody *origin, MapBody *target);
    
    /**
     * Checks if any of the dynamic bodies have
     * migrated to new cells.
     */
    void updateMap();
private:
    
    /**
     * Calculates the proper cell for the given X, Y coordinates.
     *
     * @param x X coordinate
     * @param y Y coordinate
     */
    Vector2i convert(float x, float y);
  
    // List of dynamic bodies
    std::list<MapBody*> m_dynamic;
    
    // Grid of cells
    MatrixXi m_grid;
    
    // Cell dimension
    float m_dx;
    float m_dy;
};

#endif
