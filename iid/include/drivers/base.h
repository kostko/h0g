/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_DRIVERS_BASE_H
#define IID_DRIVERS_BASE_H

#include <string>

namespace IID {

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
     * Binds an vertex attribute pointer to a shader. The shader must be currently
     * bound.
     *
     * @param variable Attribute variable name
     * @param size Number of components per variable
     * @param stride Gap in bytes between consecutive records
     * @param offset Offset into the vertex buffer
     */
    virtual void bindAttributePointer(const char *variable, int size, int stride, int offset) = 0;
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
      Lines
    };
    
    /**
     * Class constructor.
     *
     * @param name Driver name
     */
    Driver(const std::string &name);
    
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
    virtual void createLight(int index, const float *position, const float *ambient,
                             const float *diffuse, const float *specular, float attConst,
                             float attLin, float attQuad) const = 0;
    
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
private:
    std::string m_name;
};

}

#endif
