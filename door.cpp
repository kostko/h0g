/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "door.h"
#include "motionstate.h"

// IID includes
#include "storage/storage.h"
#include "storage/compositemesh.h"
#include "storage/texture.h"
#include "storage/shader.h"
#include "storage/font.h"
#include "storage/sound.h"
#include "scene/scene.h"
#include "context.h"

#include "gui/manager.h"
#include "gui/windows.h"
#include "gui/window.h"
#include "gui/button.h"
#include "gui/font.h"
#include "events/keyboard.h"

#include "drivers/openal.h"

#include <boost/lexical_cast.hpp>
#include <boost/signal.hpp>
#include <boost/bind.hpp>

using namespace IID;

/**
 * A simple keypad where a code has to be entered before proceeding.
 */
class Keypad : public GUI::Window {
public:
    /**
     * Class constructor.
     */
    Keypad(Context *context, const std::string &code)
      : GUI::Window(context->getGuiManager()->getWindowManager()),
        m_code(code)
    {
      // Set position and size
      int centerX = getParent()->getWidth() / 2;
      int centerY = getParent()->getHeight() / 2;
      
      setPosition(centerX - 100, centerY - 100);
      setSize(200, 200);
      
      // Prepare font
      Font *font = context->storage()->get<Font>("/Fonts/verdana");
      
      // Create our buttons
      int x = 10;
      int y = 10;
      for (int i = 0; i < 9; i++) {
        GUI::Button *b = new GUI::Button(this);
        b->setPosition(x, y);
        b->setSize(50, 50);
        b->setFont(GUI::Font(font));
        b->setText(boost::lexical_cast<std::string>(i + 1));
        b->signalClicked.connect(boost::bind(&Keypad::keypadPressed, this, i + 1));
        x += 65;
        if (x >= 200) {
          x = 10;
          y += 65;
        }
        
        m_buttons[i] = b;
      }
      
      // Setup sound effects
      m_soundAccessDenied = new OpenALPlayer();
      m_soundAccessDenied->setMode(Player::Once);
      m_soundAccessDenied->queue(context->storage()->get<Sound>("/Sounds/wrong_code"));
      
      m_soundKeyPress = new OpenALPlayer();
      m_soundKeyPress->setMode(Player::Once);
      m_soundKeyPress->queue(context->storage()->get<Sound>("/Sounds/key_press"));
      
      m_soundDoorOpening = new OpenALPlayer();
      m_soundDoorOpening->setMode(Player::Once);
      m_soundDoorOpening->queue(context->storage()->get<Sound>("/Sounds/door_opening"));
    }
    
    /**
     * Shows the keypad entry window.
     */
    void show()
    {
      m_entered = "";
      setVisible(true);
      grabKeyboard();
    }
    
    /**
     * Handle keypad button press.
     */
    void keypadPressed(int key)
    {
      m_soundKeyPress->play();
      
      m_entered += boost::lexical_cast<std::string>(key);
      if (m_entered == m_code) {
        // A correct code has been entered
        m_soundDoorOpening->play();
        signalCodeAccepted();
        setVisible(false);
        ungrabKeyboard();
      } else if (m_entered.size() >= m_code.size()) {
        m_entered = "";
        m_soundAccessDenied->play();
      }
    }
public:
    // Signals
    boost::signal<void ()> signalCodeAccepted;
protected:
    /**
     * Process local key press event.
     *
     * @param event Event instance
     */
    bool keyPressEvent(KeyboardEvent *event)
    {
      if (event->key() == 27) {
        // Escape has been pressed, close this window and release grab
        setVisible(false);
        ungrabKeyboard();
      }
      
      return true;
    }
private:
    GUI::Button *m_buttons[9];
    std::string m_code;
    std::string m_entered;
    
    // Sounds effects
    Player *m_soundAccessDenied;
    Player *m_soundKeyPress;
    Player *m_soundDoorOpening;
};

SlidingDoor::SlidingDoor(IID::Context *context)
  : Entity(context, "door"),
    m_state(Closed)
{
  btDynamicsWorld *world = context->getDynamicsWorld();
  Scene *scene = context->scene();
  Storage *storage = context->storage();
  
  CompositeMesh *door = storage->get<CompositeMesh>("/Models/door");
  Texture *doorTexture = storage->get<Texture>("/Textures/door");
  Shader *shader = storage->get<Shader>("/Shaders/general");
  
  // Create the door scene node
  m_sceneNode = scene->createNodeFromStorage(door);
  m_sceneNode->setShader(shader);
  m_sceneNode->setTexture(doorTexture);
  m_sceneNode->setPosition(40.086, 1.925, 0.0);
  scene->attachNode(m_sceneNode);
  
  // Create the kinematic object
  Vector3f hs = door->getAABB().getHalfSize();
  m_shape = new btBoxShape(btVector3(hs[0], hs[1], hs[2]));
  m_motionState = new EntityMotionState(m_sceneNode);
  m_slideLimit = hs[2]*2.0 + 0.01;
  m_slideStart = 0.0;
  
  btRigidBody::btRigidBodyConstructionInfo cInfo(0.0, m_motionState, m_shape, btVector3(0, 0, 0));
  m_body = new btRigidBody(cInfo);
  m_body->setCollisionFlags(m_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
  world->addRigidBody(m_body);
  world->addAction(this);
  m_world = world;
  
  // Create the door keypad
  m_keypad = new Keypad(context, "42");
  m_keypad->setVisible(false);
  m_keypad->signalCodeAccepted.connect(boost::bind(&SlidingDoor::codeAccepted, this));
  
  // Entity setup
  setCollisionObject(m_body);
}

void SlidingDoor::updateAction(btCollisionWorld *world, btScalar dt)
{
  switch (m_state) {
    case Opening: {
      Vector3f pos = m_sceneNode->getPosition();
      pos -= Vector3f(0.0, 0.0, 0.40) * dt;
      
      if (m_slideStart - pos[2] < m_slideLimit) {
        m_sceneNode->setPosition(pos);
      } else {
        m_state = Open;
        m_body->forceActivationState(WANTS_DEACTIVATION);
      }
      break;
    }
    default: break;
  }
}

void SlidingDoor::open()
{
  if (m_state != Closed && m_state != Closing)
    return;
  
  m_state = Opening;
  m_body->setActivationState(DISABLE_DEACTIVATION);
}

void SlidingDoor::close()
{
  if (m_state != Open && m_state != Opening)
    return;
  
  m_state = Closing;
  m_body->setActivationState(DISABLE_DEACTIVATION);
}

void SlidingDoor::codeAccepted()
{
  // A correct code has been entered, open the door
  open();
}

void SlidingDoor::trigger(Entity *entity, TriggerType type)
{
  if (type == PickTrigger) {
    // Someone clicked on us, display keypad entry form if not yet open
    if (m_state == Closed)
      m_keypad->show();
  }
}
