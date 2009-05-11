/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
 
#include "drivers/base.h"
#include "drivers/openal.h"
#include "storage/sound.h"
#include "exceptions.h"

// OpenAL and ALUT headers
#include <AL/alut.h>

#include <iostream>

namespace IID {

OpenALPlayer::OpenALPlayer()
{
     // Try to generate a source
     alGenSources(1, m_source);
     
     // Clear error
     if (alGetError() != AL_NO_ERROR) {
        throw Exception("OpenAL: Couldn't create a source.");
     }
}

OpenALPlayer::~OpenALPlayer()
{
    alDeleteSources(1, m_source);
}

void OpenALPlayer::queue(Sound *sound)
{
    OpenALBuffer *buffer = static_cast<OpenALBuffer*>(sound->getSoundBuffer());
    
    alSourceQueueBuffers(
        *m_source,
        1,
        (ALuint*)buffer->getBufferId()
    );
    
    // Clear error
    if (alGetError() != AL_NO_ERROR) {
        throw Exception("OpenAL: Error queueing sound: "+ sound->getId() +".");
    }
}

void OpenALPlayer::unqueue(Sound *sound) 
{
    OpenALBuffer *buffer = static_cast<OpenALBuffer*>(sound->getSoundBuffer());
    alSourceUnqueueBuffers(
        *m_source,
        1,
        (ALuint*)buffer->getBufferId()
    );
    
    // Clear error.
    alGetError();
}

void OpenALPlayer::play()
{
    // If play is called when the source is already playing
    // then the current buffer is restarted - which is bad.
    if (!isPlaying()) {
        alSourcePlay(*m_source);
    }
}

void OpenALPlayer::stop()
{
    alSourceStop(*m_source);
}

bool OpenALPlayer::isPlaying()
{
    ALint state;
    alGetSourcei(*m_source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

void OpenALPlayer::setPosition(const float *position)
{
    alSourcefv(
        *m_source,
        AL_POSITION,
        (ALfloat*) position
    );
}

void OpenALPlayer::setMode(PlayMode mode)
{
    switch (mode) {
        // Set proper mode
        case Looped:
            alSourcei(*m_source, AL_LOOPING, AL_TRUE);
            break;
            
        case Once:
            alSourcei(*m_source, AL_LOOPING, AL_FALSE);
    }
}

void OpenALPlayer::setGain(float value)
{
    alSourcef(
        *m_source,
        AL_GAIN,
        (ALfloat) value
    );
}

OpenALBuffer::OpenALBuffer(const std::string &fileName)
  : SoundBuffer(fileName)
{
}

OpenALBuffer::~OpenALBuffer()
{
    alDeleteBuffers(1, m_buffer);
}

void OpenALBuffer::init() 
{
    // Try to generate a source
    alGenBuffers(1, m_buffer);
    
    *m_buffer = alutCreateBufferFromFile(m_fileName.c_str());
    
    // Clear error
    if (alGetError() != AL_NO_ERROR) {
        throw Exception("OpenAL: Couldn't create a buffer.");
    }
}

ALuint *OpenALBuffer::getBufferId()
{
    return m_buffer;
}

void OpenALListener::setPosition(const float *position)
{
    alListenerfv(
        AL_POSITION,
        (ALfloat*) position
    );
}

void OpenALListener::setGain(float value)
{
    alListenerf(
        AL_GAIN,
        (ALfloat) value
    );
}

void OpenALListener::setOrientation(const float *forward, const float *up)
{
    float orientation[6] = {
        forward[0],
        forward[1],
        forward[2],
        up[0],
        up[1],
        up[2]
    };
    
    alListenerfv(AL_ORIENTATION, orientation);
}

OpenALContext::OpenALContext()
{
}

OpenALContext::~OpenALContext()
{
    // Disable context
    alcMakeContextCurrent(NULL);
    
    // Release context
    alcDestroyContext(m_context);
    
    // Close device
    alcCloseDevice(m_device);
    alutExit();
}

void OpenALContext::init(const std::string &deviceName)
{
    // Init with no context
    alutInitWithoutContext (NULL, NULL);
    
    // Try to open the specified device
    if (deviceName == "Default") {
        m_device = alcOpenDevice(NULL);
    } else {
        m_device = alcOpenDevice(deviceName.c_str());
    }
    
    // Success?
    if (m_device == NULL) {
        throw Exception("OpenAL: Error opening the specified device "+ deviceName +" for creating a context.");
    }
    
    // Create context and make it current
    m_context = alcCreateContext(m_device, NULL);
    alcMakeContextCurrent(m_context);
    
    if (alGetError() != AL_NO_ERROR) {
        throw Exception("OpenAL: Could not create a context.");
    }
}

}
