/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_GAMESTATE_H
#define IID_GAMESTATE_H

#include "globals.h"

#include <list>

namespace IID {

class Context;
class GameStateManager;

/**
 * This class represents a game state.
 */
class GameState {
public:
    /**
     * Class constructor.
     *
     * @param name Unique state name
     * @param context Engine context
     */
    GameState(const std::string &name, Context *context);
    
    /**
     * Class destructor.
     */
    virtual ~GameState() {}
    
    /**
     * Returns this state's name.
     */
    std::string getName() const;
    
    /**
     * Returns the game state manager instance.
     */
    GameStateManager *getGsm() const;
    
    /**
     * This method is called when this game state is entered.
     *
     * @param fromState State from which we have entered
     */
    virtual void enter(const std::string &fromState) {}
    
    /**
     * This method is called when this game state is left behind.
     *
     * @param toState State to which we are going
     */
    virtual void leave(const std::string &toState) {}
    
    /**
     * This method is called when this game state has been put on
     * the stack. The state is still active and will receive updates.
     *
     * @param state State that has now become active
     */
    virtual void stack(const std::string &state) {}
    
    /**
     * This method is called when this game state has been popped
     * out of the stack and is now the current state.
     *
     * @param state State that has been active before
     */
    virtual void unstack(const std::string &state) {}
    
    /**
     * This method is called on every frame when the state is active.
     */
    virtual void update(float dt) {}
private:
    std::string m_name;
    GameStateManager *m_gsm;
};

/**
 * Game state manager is a simple manager of game states. It supports state
 * registrations, transition handling and update propagation.
 */
class GameStateManager {
public:
    /**
     * Class constructor.
     */
    GameStateManager();
    
    /**
     * Registers a new state.
     *
     * @param state Game state instance
     */
    void addState(GameState *state);
    
    /**
     * Transitions to another state.
     *
     * @param name Name of the state to transition into
     * @param stack Should the current state be put on the stack
     */
    void transitionTo(const std::string &name, bool stack = false);
    
    /**
     * Transitions to the state down the stack. If the stack is empty, this
     * method does nothing.
     */
    void transitionDown();
    
    /**
     * Propagates updates to currently active states.
     *
     * @param dt Time delta
     */
    void update(float dt);
private:
    boost::unordered_map<std::string, GameState*> m_states;
    std::list<GameState*> m_stateStack;
    GameState *m_currentState;
};

}

#endif
