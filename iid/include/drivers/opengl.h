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

namespace IID {

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
protected:
    GLuint compileShader(const char *source, GLenum type) const;
private:
    GLuint m_handle;
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
     * Creates a light.
     *
     * @param index Light index
     * @param position Light position vector
     * @param ambient Ambient component
     * @param diffuse Diffuse component
     * @param specular Specular component
     * @param attConst Constant attenuation
     * @param attLin Linear attenuation
     * @param attQuad Quadratic attenuation
     */
    void createLight(int index, const float *position, const float *ambient,
                     const float *diffuse, const float *specular, float attConst,
                     float attLin, float attQuad) const;
    
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
private:
    // A map of shader programs
    boost::unordered_map<GLuint, OpenGLShader*> m_shaders;
};

}

#endif
