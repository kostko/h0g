/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_DRIVERS_OPENGL_H
#define IID_DRIVERS_OPENGL_H

#include "drivers/base.h"
#include "globals.h"

#include <GL/gl.h>
#include <FTGL/ftgl.h>

class btIDebugDraw;

namespace IID {

class ParticleEmitter;
    
/**
 * OpenGL texture handle.
 */
class OpenGLTexture : public DTexture {
public:
    /**
     * Class constructor.
     *
     * @param format Texture format
     */
    OpenGLTexture(Format format);
    
    /**
     * Class destructor.
     */
    ~OpenGLTexture();
    
    /**
     * Set texture parameter.
     *
     * @param param Parameter type
     * @param value Parameter value
     */
    void setParameter(Parameter param, Value value);
    
    /**
     * Set texture parameter.
     *
     * @param param Parameter type
     * @param value Parameter value
     */
    void setParameter(Parameter param, float value);
    
    /**
     * Bind texture.
     *
     * @param textureUnit Texture unit to bind to
     */
    void bind(int textureUnit);
    
    /**
     * Unbind texture.
     *
     * @param textureUnit Texture unit to unbind from
     */
    void unbind(int textureUnit);
    
    /**
     * Builds 2D mipmaps for the specified image and uploads it to the GPU.
     *
     * @param components Number of color components
     * @param width Image width
     * @param height Image height
     * @param data Image data
     */
    void buildMipMaps2D(int components, int width, int height, PixelFormat format, unsigned char *data);
protected:
    GLenum formatToOpenGL() const;
    GLenum paramToOpenGL(Parameter param) const;
    GLenum valueToOpenGL(Value val) const;
private:
    // OpenGL texture handle
    GLuint m_handle;
    
    // Texture format
    GLenum m_oglFormat;
};

/**
 * OpenGL vertex buffer object.
 */
class OpenGLVertexBuffer : public DVertexBuffer {
public:
    /**
     * Class constructor.
     *
     * @param size Number of bytes in data
     * @param data Raw data to be stored in the buffer
     * @param usage Buffer usage hint
     * @param target Buffer bind target
     */
    OpenGLVertexBuffer(size_t size, unsigned char *data, UsageHint usage, Target target);
    
    /**
     * Class destructor.
     */
    ~OpenGLVertexBuffer();
    
    /**
     * Binds the vertex buffer.
     */
    void bind() const;
    
    /**
     * Unbinds the vertex buffer.
     */
    void unbind() const;
    
    /**
     * Update the buffer's data.
     */
    void update(size_t size, unsigned char *data, UsageHint usage, Target target);
protected:
    GLenum usageToOpenGL(UsageHint usage) const;
    GLenum targetToOpenGL(Target target) const;
private:
    GLuint m_handle;
    GLenum m_target;
};

/**
 * An OpenGL shader object.
 */
class OpenGLShader : public DShader {
friend class OpenGLDriver;
public:
    /**
     * Class constructor.
     *
     * @param srcVertex Vertex shader source
     * @param srcFragment Fragment shader source
     */
    OpenGLShader(const char *srcVertex, const char *srcFragment);
    
    /**
     * Class destructor.
     */
    ~OpenGLShader();
    
    /**
     * Activate this shader program.
     */
    void activate() const;
    
    /**
     * Deactivate this shader program.
     */
    void deactivate() const;
    
    /**
     * Binds an vertex attribute pointer to a shader. The shader must be currently
     * bound.
     *
     * @param variable Attribute variable name
     * @param size Number of components per variable
     * @param stride Gap in bytes between consecutive records
     * @param offset Offset into the vertex buffer
     */
    void bindAttributePointer(const char *variable, int size, int stride, int offset);
    
    /**
     * Binds a vertex attribute location to a shader. The shader must be currently
     * bound.
     *
     * @param index The index of the generic vertex attribute to be bound
     * @param name The name of the vertex shader attribute variable to which index is to be bound.
     */
    void bindAttributeLocation(int index, const char *name);
    
    /**
     * Set uniform value. The variable is set based on the currently bound shader.
     *
     * @param name Variable name
     * @param size Length of the values array (between 1 and 4)
     * @param values Values to set to the variable
     */
    void setUniform(const char *name, int size, float *values);
protected:
    GLuint compileShader(const char *source, GLenum type) const;
private:
    GLuint m_handle;
};

/**
 * An OpenGL/FTGL font object.
 */
class OpenGLFont : public DFont {
public:
    /**
     * Class constructor.
     *
     * @param font FTGL font object
     */
    OpenGLFont(FTTextureFont *font);
    
    /**
     * Class destructor.
     */
    ~OpenGLFont();
    
    /**
     * Renders some text on the specified location.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     */
    void render(int x, int y, int z, const std::string &text);
    
    /**
     * Returns the font's bounding box for the specified text.
     *
     * @param text Text to use
     */
    FontMetrics getBoundingBox(const std::string &text) const;
private:
    FTTextureFont *m_font;
};

/**
 * Driver for OpenGL.
 */
class OpenGLDriver : public Driver {
public:
    /**
     * Class constructor.
     */
    OpenGLDriver();
    
    /**
     * Class destructor.
     */
    ~OpenGLDriver();
    
    /**
     * Initializes the driver context.
     */
    void init();
    
    /**
     * Starts processing events.
     */
    void processEvents() const;
    
    /**
     * Performs a buffer swap.
     */
    void swap() const;
    
    /**
     * Clears the buffer.
     */
    void clear() const;
    
    /**
     * Returns this driver's debug drawer for bullet dynamics simulation
     * world.
     */
    btIDebugDraw *getDebugBulletDynamicsDrawer();
    
    /**
     * Enters the 2D mode suitable for drawing GUI elements.
     */
    void enter2DMode() const;
    
    /**
     * Leaves the 2D mode by restoring previous configuration.
     */
    void leave2DMode() const;
    
    /**
     * Sets the state of scissor testing.
     *
     * @param enable True to enable scissor test
     */
    void setScissorTest(bool enable) const;
    
    /**
     * Sets up the scissor region.
     *
     * @param region A vector representing the region
     */
    void setScissorRegion(const Vector4i &region) const;
    
    /**
     * Draws a line between two points.
     *
     * @param c1 Color at start point
     * @param p1 Start point
     * @param c2 Color at end point
     * @param p2 End point
     */
    void drawLine(const Vector4f &c1, const Vector3f &p1, const Vector4f &c2, const Vector3f &p2) const;
    
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
    void drawRect(const Vector3f &pos, const Vector3f &dim, const Vector4f &c1, const Vector4f &c2,
                  const Vector4f &c3, const Vector4f &c4) const;
    
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
    void fillRect(const Vector3f &pos, const Vector3f &dim, const Vector4f &c1, const Vector4f &c2,
                  const Vector4f &c3, const Vector4f &c4) const;
    
    /**
     * Draws an image.
     *
     * @param pos Upper left corner position
     * @param dim Dimensions
     * @param texture Texture to draw
     */
    void drawImage(const Vector3f &pos, const Vector3f &dim, DTexture *texture) const;
    
    /**
     * Draws elements from the currently bound index buffer.
     *
     * @param count Number of elements to draw
     * @param offset Buffer start offset
     * @param primitive What kind of primitive to draw
     */
    void drawElements(int count, unsigned int offset, DrawPrimitive primitive) const;
    
    /**
     * Apply given model-view transformation.
     *
     * @param transform Transformation matrix
     */
    void applyModelViewTransform(const float *transform) const;
    
    /**
     * Apply given projection transformation.
     *
     * @param transform Transformation matrix
     */
    void applyProjectionTransform(const float *transform) const;
    
    /**
     * Apply given material properties.
     *
     * @param ambient Ambient component
     * @param diffuse Diffuse component
     * @param specular Specular component
     * @param emission Emission
     */
    void applyMaterial(const float *ambient, const float *diffuse, const float *specular,
                       const float *emission) const;
    
    /**
     * Sets the ambient light.
     *
     * @param r Red component
     * @param g Green component
     * @param b Blue component
     */
    void setAmbientLight(float r, float g, float b) const;
    
    /**
     * Sets up the lights.
     *
     * @param lights A list of lights
     * @param limit Number of lights to use
     */
    void setupLights(const LightList &lights, unsigned short limit);
    
    /**
     * Returns the currently active shader or NULL if there is no such shader.
     */
    DShader *currentShader();
    
    /**
     * Creates a new shader program.
     *
     * @param srcVertex Vertex shader source
     * @param srcFragment Fragment shader source
     * @return A valid DShader instance
     */
    DShader *createShader(const char *srcVertex, const char *srcFragment);
    
    /**
     * Creates a new texture.
     *
     * @param format Texture format
     * @return A valid DTexture instance
     */
    DTexture *createTexture(DTexture::Format format);
    
    /**
     * Creates a new vertex buffer object.
     *
     * @param size Number of bytes in data
     * @param data Raw data to be stored in the buffer
     * @param usage Buffer usage hint
     * @param target Buffer bind target
     * @return A valid DVertexBuffer instance
     */
    DVertexBuffer *createVertexBuffer(size_t size, unsigned char *data, 
                                      DVertexBuffer::UsageHint usage,
                                      DVertexBuffer::Target target);
    
    /**
     * Creates a new font object.
     *
     * @param path Path to the TrueType font file
     * @param size Font size
     */
    DFont *createFont(const std::string &path, unsigned short size);
    
    /**
     * Draws the state of the given particle emitter.
     *
     * @param size Size of the vertex and color arrays
     * @param vertices Array of packed vertex coordinates
     * @param colors RGBA components for each vertex
     */
    void drawParticles(int size, float *vertices, float *colors);
protected:
    /**
     * A helper method for setting up OpenGL lights.
     */
    void setupGLLight(unsigned short index, Light *light);
    
    /**
     * A helper method for setting up OpenGL light position and direction.
     */
    void setupGLLightPositionDirection(GLenum index, Light *light);
private:
    // A map of shader programs
    boost::unordered_map<GLuint, OpenGLShader*> m_shaders;
    
    // Debug drawer for Bullet dynamics
    btIDebugDraw *m_debugDrawer;
    
    // Lighting slots
    Light *m_lights[8];
    unsigned short m_currentLights;
};

}

#endif
