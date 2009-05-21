/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_SCENE_PARTICLES_H
#define IID_SCENE_PARTICLES_H

#include "scene/node.h"
#include <vector>

// Some hacky random functions
#define RANDOM_SPEED() float((rand()%50)-26.0f)
#define RANDOM_COLOR() ((float) random() / 1) * 0.5f + 0.5f
#define RANDOM_FADE() float(rand()%100) / 1000.0f+0.003f
#define RANDOM_TIME() RANDOM_SPEED()

namespace IID {

/**
 * A structure representing one particle.
 */
struct Particle {
    bool m_active;
    float m_life;
    float m_fade;
    float *m_position;
    float *m_rgba;
    Vector3f m_velocity;
    Vector3f m_gravity;
};

/**
 * A node emitting particles.
 */
class ParticleEmitter : public SceneNode {
public:
    /**
     * Class constructor.
     *
     * @param name Node name
     * @param parent Parent node
     */
    ParticleEmitter(const std::string &name, int maxParticles, SceneNode *parent = 0);
    
    /**
     * Class destructor.
     */
    ~ParticleEmitter();
    
    /**
     * Initialize the particle emitter.
     *
     * This sets all the initial properties of all the
     * particles that take place in the simulation.
     */
    void init();
    
    /**
     * Start the simulation.
     */
    void start();
    
    /**
     * Stop the simulation.
     */
    void stop();
    
    /**
     * Show the particles on the scene or not.
     */
    void show(bool value);
    
    /**
     * Move one step forward in the simulation of particles.
     *
     * This calculates the new parameters (position, color, speed, ...) of 
     * all the particles in the system.
     */
    void animate();
    
    /**
     * Set the particle texture.
     *
     * @param texture Texture object
     */
    void setTexture(Texture *texture);
    
    /**
     * Set the particle shader.
     *
     * @param shader Shader object
     */
    void setShader(Shader *shader);
    
    /**
     * Set the gravity - determines the direction
     * of the particle tail.
     *
     * @param gx X component
     * @param gy Y component
     * @param gz Z component
     */
    void setGravity(float gx, float gy, float gz);
    
    /**
     * Set the emitter's bounds. The particle system is bounded
     * to [-width/2, width/2]x[-breadth/2, breadth/2]x[-height/2, height/2].
     *
     * @param width Box width
     * @param breadth Box breadth
     * @param height Box height
     */
    void setBounds(float width, float breadth, float height);
    
    /**
     * Returns all the particles to (0,0,0)
     */
    void explode();
    
    /**
     * Renders this node.
     *
     * @param batcher State batcher that holds the render queue
     */
    void render(StateBatcher *batcher);
    
    /**
     * Set color array.
     *
     * @param colors A list of available colors
     */
    void setColors(std::vector<Vector3f> colors);
private:
    /**
     * Reset's all the particles' parameters.
     */
    void reset(Particle *p);
    
    // If true the simulation is continued
    bool m_animate;
    bool m_render;
    
    // Size of the vertex array
    int m_maxParticles;
    float m_slowdown;
    float m_zoom;
    Vector3f m_gravity;
    Particle *m_particles;

    // Data
    float *m_vertices;
    float *m_colors;
    
    // Particle texture
    Texture *m_texture;
    
    // Particle shader
    Shader *m_shader;
    
    // Color list
    std::vector<Vector3f> m_colorList;
    
    // Bounding box display
    bool m_showBoundingBox;
};

}

#endif
