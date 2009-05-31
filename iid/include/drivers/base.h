/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_DRIVERS_BASE_H
#define IID_DRIVERS_BASE_H

#include <string>

#include "scene/light.h"

class btIDebugDraw;

namespace IID {

class AbstractEventDispatcher;
class Sound;
class ParticleEmitter;

/**
 * An abstract texture handle.
 */
class DTexture {
public:
    /**
     * Texture formats.
     */
    enum Format {
      Texture1D,
      Texture2D,
      Texture3D
    };
    
    /**
     * Pixel format.
     */
    enum PixelFormat {
      RGB,
      RGBA
    };
    
    /**
     * Valid parameters.
     */
    enum Parameter {
      WrapS,
      WrapT,
      MagFilter,
      MinFilter
    };
    
    /**
     * Valid values.
     */
    enum Value {
      Repeat,
      Linear,
      LinearMipmapNearest
    };
    
    /**
     * Class constructor.
     *
     * @param format Texture format
     */
    DTexture(Format format);
    
    /**
     * Class destructor.
     */
    virtual ~DTexture() {};
    
    /**
     * Set texture parameter.
     *
     * @param param Parameter type
     * @param value Parameter value
     */
    virtual void setParameter(Parameter param, Value value) = 0;
    
    /**
     * Set texture parameter.
     *
     * @param param Parameter type
     * @param value Parameter value
     */
    virtual void setParameter(Parameter param, float value) = 0;
    
    /**
     * Bind texture.
     *
     * @param textureUnit Texture unit to bind to
     */
    virtual void bind(int textureUnit) = 0;
    
    /**
     * Unbind texture.
     *
     * @param textureUnit Texture unit to unbind from
     */
    virtual void unbind(int textureUnit) = 0;
    
    /**
     * Builds 2D mipmaps for the specified image and uploads it to the GPU.
     *
     * @param components Number of color components
     * @param width Image width
     * @param height Image height
     * @param data Image data
     */
    virtual void buildMipMaps2D(int components, int width, int height, PixelFormat format, unsigned char *data) = 0;
protected:
    // Texture format
    Format m_format;
};

/**
 * An abstract vertex buffer object.
 */
class DVertexBuffer {
public:
    /**
     * VBO targets.
     */
    enum Target {
      VertexArray,
      ElementArray
    };
    
    /**
     * VBO usage hint.
     */
    enum UsageHint {
      StreamDraw,
      StreamRead,
      StreamCopy,
      StaticDraw,
      StaticRead,
      StaticCopy,
      DynamicDraw,
      DynamicRead,
      DynamicCopy
    };
    
    /**
     * Class constructor.
     *
     * @param size Number of bytes in data
     * @param data Raw data to be stored in the buffer
     * @param usage Buffer usage hint
     * @param target Buffer bind target
     */
    DVertexBuffer(size_t size, unsigned char *data, UsageHint usage, Target target) {}
    
    /**
     * Class destructor.
     */
    virtual ~DVertexBuffer() {}
    
    /**
     * Binds the vertex buffer.
     */
    virtual void bind() const = 0;
    
    /**
     * Unbinds the vertex buffer.
     */
    virtual void unbind() const = 0;
    
    /**
     * Update the buffer's data.
     */
    virtual void update(size_t size, unsigned char *data, UsageHint usage, Target target) = 0;
};

/**
 * An abstract shader object.
 */
class DShader {
public:
    /**
     * Class constructor.
     *
     * @param srcVertex Vertex shader source
     * @param srcFragment Fragment shader source
     */
    DShader(const char *srcVertex, const char *srcFragment) {};
    
    /**
     * Class destructor.
     */
    virtual ~DShader() {};
    
    /**
     * Activate this shader program.
     */
    virtual void activate() const = 0;
    
    /**
     * Deactivate this shader program.
     */
    virtual void deactivate() const = 0;
    
    /**
     * Binds a vertex attribute pointer to a shader. The shader must be currently
     * bound.
     *
     * @param variable Attribute variable name
     * @param size Number of components per variable
     * @param stride Gap in bytes between consecutive records
     * @param offset Offset into the vertex buffer
     */
    virtual void bindAttributePointer(const char *variable, int size, int stride, int offset) = 0;
    
    /**
     * Binds a vertex attribute location to a shader. The shader must be currently
     * bound.
     *
     * @param index The index of the generic vertex attribute to be bound
     * @param name The name of the vertex shader attribute variable to which index is to be bound.
     */
    virtual void bindAttributeLocation(int index, const char *name) = 0;
    
    /**
     * Set uniform value. The variable is set based on the currently bound shader.
     *
     * @param name Variable name
     * @param size Length of the values array (between 1 and 4)
     * @param values Values to set to the variable
     */
    virtual void setUniform(const char *name, int size, float *values) = 0;
};

// Used for returning font bounding box
typedef std::pair<Vector3f, Vector3f> FontMetrics;

/**
 * An abstract font object.
 */
class DFont {
public:
    /**
     * Class destructor.
     */
    virtual ~DFont() {}
    
    /**
     * Renders some text on the specified location.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     * @param text Text to render
     */
    virtual void render(int x, int y, int z, const std::string &text) = 0;
    
    /**
     * Returns the font's bounding box for the specified text.
     *
     * @param text Text to use
     */
    virtual FontMetrics getBoundingBox(const std::string &text) const = 0;
};

/**
 * An abstract interface for drivers to implement.
 */
class Driver {
public:
    /**
     * Type of primitive to draw.
     */
    enum DrawPrimitive {
      Triangles,
      TriangleStrip,
      Lines
    };
    
    /**
     * Class constructor.
     *
     * @param name Driver name
     */
    Driver(const std::string &name);
    
    /**
     * Class destructor.
     */
    virtual ~Driver() {}
    
    /**
     * Returns driver name.
     */
    std::string getName() const { return m_name; }
    
    /**
     * Initializes the driver context.
     */
    virtual void init() = 0;
    
    /**
     * Starts processing events.
     */
    virtual void processEvents() const = 0;
    
    /**
     * Performs a buffer swap.
     */
    virtual void swap() const = 0;
    
    /**
     * Clears the buffer.
     */
    virtual void clear() const = 0;
    
    /**
     * Setup an event dispatcher.
     *
     * @param dispatcher A valid event dispatcher
     */
    virtual void setEventDispatcher(AbstractEventDispatcher *dispatcher);
    
    /**
     * Returns this driver's debug drawer for bullet dynamics simulation
     * world.
     */
    virtual btIDebugDraw *getDebugBulletDynamicsDrawer() = 0;
    
    /**
     * Enters the 2D mode suitable for drawing GUI elements.
     */
    virtual void enter2DMode() const = 0;
    
    /**
     * Leaves the 2D mode by restoring previous configuration.
     */
    virtual void leave2DMode() const = 0;
    
    /**
     * Sets the state of scissor testing.
     *
     * @param enable True to enable scissor test
     */
    virtual void setScissorTest(bool enable) const = 0;
    
    /**
     * Sets up the scissor region.
     *
     * @param region A vector representing the region
     */
    virtual void setScissorRegion(const Vector4i &region) const = 0;
    
    /**
     * Draws a line between two points.
     *
     * @param c1 Color at start point
     * @param p1 Start point
     * @param c2 Color at end point
     * @param p2 End point
     */
    virtual void drawLine(const Vector4f &c1, const Vector3f &p1, const Vector4f &c2, const Vector3f &p2) const = 0;
    
    /**
     * Draws a rectangle.
     *
     * @param pos Upper left corner position
     * @param dim Dimensions
     * @param c1 Upper left corner color
     * @param c2 Upper right corner color
     * @param c3 Lower right corner color
     * @param c4 Lower left corner color
     */
    virtual void drawRect(const Vector3f &pos, const Vector3f &dim, const Vector4f &c1, const Vector4f &c2,
                          const Vector4f &c3, const Vector4f &c4) const = 0;
    
    /**
     * Draws a filled rectangle.
     *
     * @param pos Upper left corner position
     * @param dim Dimensions
     * @param c1 Upper left corner color
     * @param c2 Upper right corner color
     * @param c3 Lower right corner color
     * @param c4 Lower left corner color
     */
    virtual void fillRect(const Vector3f &pos, const Vector3f &dim, const Vector4f &c1, const Vector4f &c2,
                          const Vector4f &c3, const Vector4f &c4) const = 0;
    
    /**
     * Draws an image.
     *
     * @param pos Upper left corner position
     * @param dim Dimensions
     * @param texture Texture to draw
     */
    virtual void drawImage(const Vector3f &pos, const Vector3f &dim, DTexture *texture) const = 0;
    
    /**
     * Draws elements from the currently bound index buffer.
     *
     * @param count Number of elements to draw
     * @param offset Buffer start offset
     * @param primitive What kind of primitive to draw
     */
    virtual void drawElements(int count, unsigned int offset, DrawPrimitive primitive) const = 0;
    
    /**
     * Apply given model-view transformation.
     *
     * @param transform Transformation matrix
     */
    virtual void applyModelViewTransform(const float *transform) const = 0;
    
    /**
     * Apply given projection transformation.
     *
     * @param transform Transformation matrix
     */
    virtual void applyProjectionTransform(const float *transform) const = 0;
    
    /**
     * Apply given material properties.
     *
     * @param ambient Ambient component
     * @param diffuse Diffuse component
     * @param specular Specular component
     * @param emission Emission
     */
    virtual void applyMaterial(const float *ambient, const float *diffuse, const float *specular,
                               const float *emission) const = 0;
    
    /**
     * Sets the ambient light.
     *
     * @param r Red component
     * @param g Green component
     * @param b Blue component
     */
    virtual void setAmbientLight(float r, float g, float b) const = 0;
    
    /**
     * Sets up the lights.
     *
     * @param lights A list of lights
     * @param limit Number of lights to use
     */
    virtual void setupLights(const LightList &lights, unsigned short limit) = 0;
    
    /**
     * Returns the currently active shader or NULL if there is no such shader.
     */
    virtual DShader *currentShader() = 0;
    
    /**
     * Creates a new shader program.
     *
     * @param srcVertex Vertex shader source
     * @param srcFragment Fragment shader source
     * @return A valid DShader instance
     */
    virtual DShader *createShader(const char *srcVertex, const char *srcFragment) = 0;
    
    /**
     * Creates a new texture.
     *
     * @param format Texture format
     * @return A valid DTexture instance
     */
    virtual DTexture *createTexture(DTexture::Format format) = 0;
    
    /**
     * Creates a new vertex buffer object.
     *
     * @param size Number of bytes in data
     * @param data Raw data to be stored in the buffer
     * @param usage Buffer usage hint
     * @param target Buffer bind target
     * @return A valid DVertexBuffer instance
     */
    virtual DVertexBuffer *createVertexBuffer(size_t size, unsigned char *data, 
                                              DVertexBuffer::UsageHint usage,
                                              DVertexBuffer::Target target) = 0;
    
    /**
     * Creates a new font object.
     *
     * @param path Path to the TrueType font file
     * @param size Font size
     */
    virtual DFont *createFont(const std::string &path, unsigned short size) = 0;
    
    /**
     * Draws the state of the given particle emitter.
     *
     * @param size Size of the vertex and color arrays
     * @param vertices Array of packed vertex coordinates
     * @param colors RGBA components for each vertex
     */
    virtual void drawParticles(int size, float *vertices, float *colors) = 0;
private:
    std::string m_name;
public:
    // Do not modify this directly, use setEventDispatcher instead
    AbstractEventDispatcher *m_dispatcher;
};

/**
 * Abstract sound emitter.
 */
class Player {
public:
    
    enum PlayMode {
        Once,
        Looped
    };
    
    /** 
     * Class constructor.
     */
    Player() {}
    
    /** 
     * Class destructor.
     */
    virtual ~Player() {}
    
    /**
     * Queue a sound to be played on the player.
     * 
     * @param sound Sound object
     */
    virtual void queue(Sound *sound) = 0;
    
    /**
     * Play the queue in the set play mode (Effect PlayMode is the default).
     */
    virtual void play() = 0;
    
    /**
     * Stop playing.
     */
    virtual void stop() = 0;
    
    /**
     * Returns true if the player is playing anything.
     */
    virtual bool isPlaying() = 0;
    
    /**
     * Set the position of the player in space.
     * 
     * @param position Vector of the player's coordinates
     */
    virtual void setPosition(const float *position) = 0;
    
    /**
     * Set the master gain.
     * 
     * @param value Gain value
     */
    virtual void setGain(float value) = 0;
    
    /**
     * Set the mode of playing of queued sounds.
     * 
     * @param mode Playing mode
     */
    virtual void setMode(PlayMode mode) = 0;
};

/**
 * Abstract sound buffer.
 */
class SoundBuffer {
public:
    /**
     * Class constructor.
     *
     * @param filePath Path to the sound file.
     */
    SoundBuffer(const std::string &fileName);
    
    /**
     * Class destructor.
     */
    virtual ~SoundBuffer() {}
    
    /**
     * Do the actual file read.
     */
    virtual void init() = 0;

protected:
    std::string m_fileName;
};

/**
 * Abstract sound listener entity.
 */
class Listener {
public:
    
    /**
     * Class constructor.
     */
    Listener() {}
    
    /**
     * Class destructor.
     */
    virtual ~Listener() {}
    
    /**
     * Set the position of the listener in space.
     * 
     * @param position Vector of the player's coordinates
     */
    virtual void setPosition(const float *position) = 0;
    
    /**
     * Set the listener's orientation.
     * 
     * @param forward Forward vector
     * @param up Up vector
     */
    virtual void setOrientation(const float *forward, const float *up) = 0;
    
    /**
     * Set the master gain.
     * 
     * @param value Gain value
     */
    virtual void setGain(float value) = 0;
};

/**
 * An abstract sound context which is consisted of a
 * listener and several players.
 */
class SoundContext {
public:
    /**
     * Class constructor.
     *
     */
    SoundContext() {}
    
    /**
     * Class destructor.
     */
    virtual ~SoundContext() {}
    
    /**
     * Init context.
     *
     * @param deviceName Device name for the sound context
     */
    virtual void init(const std::string &deviceName) = 0;
};

}

#endif
