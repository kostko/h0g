/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "drivers/opengl.h"
#include "events/base.h"
#include "exceptions.h"

#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <iostream>

namespace IID {

// Global OpenGL driver instance
static OpenGLDriver *gOpenGLDriver = 0;

OpenGLTexture::OpenGLTexture(Format format)
  : DTexture(format)
{
  glGenTextures(1, &m_handle);
  
  // Check for errors
  if (glGetError() != GL_NO_ERROR)
    throw Exception("OpenGL: Texture generation has failed!");
}

OpenGLTexture::~OpenGLTexture()
{
  glDeleteTextures(1, &m_handle);
}

void OpenGLTexture::setParameter(Parameter param, Value value)
{
  glTexParameterf(formatToOpenGL(), paramToOpenGL(param), valueToOpenGL(value));
}

void OpenGLTexture::setParameter(Parameter param, float value)
{
  glTexParameterf(formatToOpenGL(), paramToOpenGL(param), value);
}

void OpenGLTexture::bind(int textureUnit)
{
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(formatToOpenGL(), m_handle);
  
  // Check for errors
  if (glGetError() != GL_NO_ERROR)
    throw Exception("OpenGL: Texture bind has failed!");
}

void OpenGLTexture::unbind(int textureUnit)
{
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(formatToOpenGL(), 0);
}

void OpenGLTexture::buildMipMaps2D(int components, int width, int height, PixelFormat format, unsigned char *data)
{
  GLenum _format;
  switch (format) {
    case RGB: _format = GL_RGB; break;
    case RGBA: _format = GL_RGBA; break;
  }
  
  gluBuild2DMipmaps(formatToOpenGL(), components, width, height, _format, GL_UNSIGNED_BYTE, (void*) data);
}

GLenum OpenGLTexture::formatToOpenGL() const
{
  switch (m_format) {
    case Texture1D: return GL_TEXTURE_1D;
    case Texture2D: return GL_TEXTURE_2D;
    case Texture3D: return GL_TEXTURE_3D;
  }
}

GLenum OpenGLTexture::paramToOpenGL(Parameter param) const
{
  switch (param) {
    case WrapS: return GL_TEXTURE_WRAP_S;
    case WrapT: return GL_TEXTURE_WRAP_T;
    case MagFilter: return GL_TEXTURE_MAG_FILTER;
    case MinFilter: return GL_TEXTURE_MIN_FILTER;
  }
}

GLenum OpenGLTexture::valueToOpenGL(Value val) const
{
  switch (val) {
    case Repeat: return GL_REPEAT;
    case Linear: return GL_LINEAR;
    case LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
  }
}

OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size, unsigned char *data, UsageHint usage, Target target)
  : DVertexBuffer(size, data, usage, target),
    m_target(targetToOpenGL(target))
{
  glGenBuffers(1, &m_handle);
  glBindBuffer(m_target, m_handle);
  glBufferData(m_target, size, data, usageToOpenGL(usage));
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
  glDeleteBuffers(1, &m_handle);
}

GLenum OpenGLVertexBuffer::usageToOpenGL(UsageHint usage) const
{
  switch (usage) {
    case StreamDraw: return GL_STREAM_DRAW;
    case StreamRead: return GL_STREAM_READ;
    case StreamCopy: return GL_STREAM_COPY;
    case StaticDraw: return GL_STATIC_DRAW;
    case StaticRead: return GL_STATIC_READ;
    case StaticCopy: return GL_STATIC_COPY;
    case DynamicDraw: return GL_DYNAMIC_DRAW;
    case DynamicRead: return GL_DYNAMIC_READ;
    case DynamicCopy: return GL_DYNAMIC_COPY;
  }
}

GLenum OpenGLVertexBuffer::targetToOpenGL(Target target) const
{
  switch (target) {
    case VertexArray: return GL_ARRAY_BUFFER;
    case ElementArray: return GL_ELEMENT_ARRAY_BUFFER;
  }
}

void OpenGLVertexBuffer::bind() const
{
  glBindBuffer(m_target, m_handle);
}

void OpenGLVertexBuffer::unbind() const
{
  glBindBuffer(m_target, 0);
}

OpenGLShader::OpenGLShader(const char *srcVertex, const char *srcFragment)
  : DShader(srcVertex, srcFragment)
{
  GLuint vshader = 0;
  GLuint fshader = 0;
  
  // Create a new shader program handle
  m_handle = glCreateProgram();
  
  if (strlen(srcVertex)) {
    vshader = compileShader(srcVertex, GL_VERTEX_SHADER);
    glAttachShader(m_handle, vshader);
  }
  
  if (strlen(srcFragment)) {
    fshader = compileShader(srcFragment, GL_FRAGMENT_SHADER);
    glAttachShader(m_handle, fshader);
  }
  
  glLinkProgram(m_handle);
  
  if (vshader)
    glDeleteShader(vshader);
  if (fshader)
    glDeleteShader(fshader);
}

OpenGLShader::~OpenGLShader()
{
}

GLuint OpenGLShader::compileShader(const char *source, GLenum type) const
{
  GLuint shader = glCreateShader(type);
  GLint len = strlen(source);
  glShaderSource(shader, 1, &source, &len);
  glCompileShader(shader);
  
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (!status) {
    // TODO handle compilation log
    glDeleteShader(shader);
    throw Exception("OpenGL: Shader compilation has failed!");
  }
  
  return shader;
}

void OpenGLShader::bindAttributePointer(const char *variable, int size, int stride, int offset)
{
  GLint attribId = glGetAttribLocation(m_handle, variable);
  if (attribId != -1) {
    glVertexAttribPointer(attribId, size, GL_FLOAT, 0, stride, (GLvoid*) offset);
    glEnableVertexAttribArray(attribId);
  }
}

void OpenGLShader::activate() const
{
  glUseProgram(m_handle);
}

void OpenGLShader::deactivate() const
{
  glUseProgram(0);
}

OpenGLDriver::OpenGLDriver()
  : Driver("OpenGL")
{
  gOpenGLDriver = this;
}

void OpenGLDriver::init()
{
  int argc = 0;
  glutInit(&argc, 0);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(1024, 768);
  glutCreateWindow("Infinite Improbability Drive");
  
  glClearColor(0, 0, 0, 0);
  glClearDepth(1);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
  glViewport(0, 0, 1024, 768);
}

static void __glutKeyboardCallback(unsigned char key, int, int)
{
  gOpenGLDriver->m_dispatcher->keyboardEvent(false, (int) key);
}

static void __glutSpecialCallback(int key, int, int)
{
  AbstractEventDispatcher::Key rkey;
  switch (key) {
    case GLUT_KEY_UP: rkey = AbstractEventDispatcher::Up; break;
    case GLUT_KEY_DOWN: rkey = AbstractEventDispatcher::Down; break;
    case GLUT_KEY_LEFT: rkey = AbstractEventDispatcher::Left; break;
    case GLUT_KEY_RIGHT: rkey = AbstractEventDispatcher::Right; break;
    default: return;
  }
  
  gOpenGLDriver->m_dispatcher->keyboardEvent(true, rkey);
}

void OpenGLDriver::processEvents() const
{
  // Setup event handlers if a dispatcher has been specified
  if (m_dispatcher) {
    glutKeyboardFunc(__glutKeyboardCallback);
    glutSpecialFunc(__glutSpecialCallback);
  }
  
  // Enter the main loop
  glutMainLoop();
}

void OpenGLDriver::swap() const
{
  glutSwapBuffers();
}

void OpenGLDriver::clear() const
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLDriver::drawElements(int count, unsigned int offset, DrawPrimitive primitive) const
{
  switch (primitive) {
    case Triangles: glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (GLvoid*) offset); break;
    case Lines: glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, (GLvoid*) offset); break;
  }
}

void OpenGLDriver::applyModelViewTransform(const float *transform) const
{
  glLoadMatrixf(transform);
}

void OpenGLDriver::applyProjectionTransform(const float *transform) const
{
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(transform);
  glMatrixMode(GL_MODELVIEW);
}

void OpenGLDriver::applyMaterial(const float *ambient, const float *diffuse, const float *specular, const float *emission) const
{
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT, GL_EMISSION, emission);
}

void OpenGLDriver::createLight(int index, const float *position, const float *ambient,
                               const float *diffuse, const float *specular, float attConst,
                               float attLin, float attQuad) const
{
  glLightfv(GL_LIGHT0 + index, GL_POSITION, position);
  glLightfv(GL_LIGHT0 + index, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0 + index, GL_SPECULAR, specular);
  glLightf(GL_LIGHT0 + index, GL_CONSTANT_ATTENUATION, attConst);
  glLightf(GL_LIGHT0 + index, GL_LINEAR_ATTENUATION, attLin);
  glLightf(GL_LIGHT0 + index, GL_QUADRATIC_ATTENUATION, attQuad);
}

DShader *OpenGLDriver::currentShader()
{
  GLint programId;
  glGetIntegerv(GL_CURRENT_PROGRAM, &programId);
  if (!programId)
    return 0;
  
  return m_shaders[programId];
}

DShader *OpenGLDriver::createShader(const char *srcVertex, const char *srcFragment)
{
  OpenGLShader *shader = new OpenGLShader(srcVertex, srcFragment);
  m_shaders[shader->m_handle] = shader;
  return shader;
}

DTexture *OpenGLDriver::createTexture(DTexture::Format format)
{
  return new OpenGLTexture(format);
}

DVertexBuffer *OpenGLDriver::createVertexBuffer(size_t size, unsigned char *data, 
                                                DVertexBuffer::UsageHint usage,
                                                DVertexBuffer::Target target)
{
  return new OpenGLVertexBuffer(size, data, usage, target);
}

}
