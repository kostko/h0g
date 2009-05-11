/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
 
#ifndef IID_DRIVERS_OPENAL_H
#define IID_DRIVERS_OPENAL_H

#include <AL/alut.h>
#include "drivers/base.h"

namespace IID {

class Sound;

class OpenALPlayer : public Player {
public:    
    /**
     * Class constructor.
     */
    OpenALPlayer();
    
    /**
    * Class destructor.
    */
    ~OpenALPlayer();
    
    /**
     * Init function.
     */
    void init(const std::string &deviceName);
    
    /** 
     * Queue a sound.
     */ 
    void queue(Sound *sound);
    
    /** 
     * Unqueue a sound.
     */ 
    void unqueue(Sound *sound);
    
    /**
     * Play the queued sounds.
     */
    void play();
    
    /**
     * Stop playing.
     */
    void stop();
    
    /**
     * Returns true if the player is playing anything.
     */
    bool isPlaying();
    
    /**
    * Set player position in space.
    */
    void setPosition(const float *position);
    
    /**
    * Set master gain.
    */
    void setGain(float value);
    
    /**
     * Set the mode of playing.
     */
    void setMode(PlayMode mode);
    
private:
    ALuint m_source[1];
};
  
class OpenALBuffer : public SoundBuffer {
public:
    /**
     * Class constructor.
     */
    OpenALBuffer(const std::string &fileName);
    
    /**
     * Class destructor.
     */
    ~OpenALBuffer();

    /**
     * Do the actual file read.
     */
    void init();
    
    /**
     * Get the buffer id.
     */
    ALuint *getBufferId();
  
private:
    ALuint m_buffer[1];
};

class OpenALListener : public Listener {
public:
  
    /**
    * Set position in space.
    */
    void setPosition(const float *position);
    
    /**
     * Set position in space.
     */
    void setOrientation(const float *forward, const float *up);
    
    /**
    * Set master gain.
    */
    void setGain(float value);
};

class OpenALContext : public SoundContext {
public:
 
    /**
     * Class constructor.
     */
    OpenALContext();
    
    /**
     * Class destructor.
     */
    ~OpenALContext();
    
    /**
     * Init context.
     */
    void init(const std::string &deviceName);
  
private:
    ALCdevice *m_device;
    ALCcontext *m_context;
};

}

#endif
