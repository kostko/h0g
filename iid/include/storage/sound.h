/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_STORAGE_SOUND_H
#define IID_STORAGE_SOUND_H

#include "storage.h"

namespace IID {

class SoundBuffer;

class Sound : public Item {
public:
    
    enum SoundType {
        Effect,
        Ambiental
    };
    
    /**
     * Class constructor.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param path Item path
     * @param parent Optional parent instance
     */
    Sound(Storage *storage, const std::string &itemId, Item *parent);
    
    /**
     * Class destructor.
     */
    ~Sound();
    
    /**
     * Set the buffer holding the sound.
     *
     * @param sound The sound buffer holding the sound
     */
    void setSoundBuffer(SoundBuffer *sound);
    
    /**
     * Set the sound type.
     *
     * @param type One of the valid Types
     */
    void setType(const std::string &type);
    
    /**
     * Get the buffer holding the sound.
     *
     * @return Buffer object
     */
    SoundBuffer *getSoundBuffer();
    
    /**
     * Get the sound type.
     *
     * @return One of the valid Types
     */
    SoundType getType();
    
private:
    SoundBuffer *m_soundBuffer;
    
    // The sound type
    SoundType m_type;
};

/**
 * A factory of Sound items.
 */
class SoundFactory : public ItemFactory {
    public:
    /**
     * Create a new Sound storage item.
     *
     * @param storage Storage instance
     * @param itemId Item identifier
     * @param parent Parent item
     * @return A valid Sound item
     */
    Item *create(Storage *storage, const std::string &itemId, Item *parent);
};

}

#endif
