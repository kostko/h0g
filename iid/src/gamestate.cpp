/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "gamestate.h"
#include "context.h"

#include <boost/foreach.hpp>

namespace IID {

GameState::GameState(const std::string &name, Context *context)
  : m_name(name),
    m_gsm(context->getGameStateManager())
{
}

std::string GameState::getName() const
{
  return m_name;
}

GameStateManager *GameState::getGsm() const
{
  return m_gsm;
}

GameStateManager::GameStateManager()
  : m_currentState(0)
{
}

void GameStateManager::addState(GameState *state)
{
  m_states[state->getName()] = state;
}

void GameStateManager::transitionTo(const std::string &name, bool stack)
{
  if (m_states.find(name) == m_states.end())
    return;
  
  // Check if we are stacked and are actually transitioning down the stack
  if (!m_stateStack.empty() && m_stateStack.front() == m_states[name]) {
    transitionDown();
    return;
  }
  
  if (stack) {
    // Stack states (don't leave the old state)
    std::string lastState;
    if (m_currentState) {
      m_currentState->stack(name);
      m_stateStack.push_back(m_currentState);
      lastState = m_currentState->getName();
    }
    
    m_currentState = m_states[name];
    m_currentState->enter(lastState);
  } else {
    // No stacking, leave current state, enter new state
    std::string lastState;
    if (m_currentState) {
      m_currentState->leave(name);
      lastState = m_currentState->getName();
    }
    
    // Also leave all states that are on the stack
    while (!m_stateStack.empty()) {
      GameState *state = m_stateStack.back();
      m_stateStack.pop_back();
      state->leave(name);
    }
    
    m_currentState = m_states[name];
    m_currentState->enter(lastState);
  }
}

void GameStateManager::transitionDown()
{
  if (m_stateStack.empty())
    return;
  
  GameState *state = m_stateStack.back();
  m_stateStack.pop_back();
  
  m_currentState->leave(state->getName());
  state->unstack(m_currentState->getName());
  m_currentState = state;
}

void GameStateManager::update(float dt)
{
  if (m_currentState)
    m_currentState->update(dt);
  
  // Also update all stuff left in the stack
  BOOST_FOREACH(GameState *state, m_stateStack) {
    state->update(dt);
  }
}

}
