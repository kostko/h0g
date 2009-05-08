/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "drivers/opengl.h"
#include "events/base.h"
#include "exceptions.h"

// OpenGL
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Bullet dynamics
#include <bullet/LinearMath/btIDebugDraw.h>

#include <iostream>

namespace IID {

// Global OpenGL driver instance
static OpenGLDriver *gOpenGLDriver = 0;

/**
 * An OpenGL debug drawer for Bullet dynamics physics simulation.
 */
class OpenGLDebugDrawer : public btIDebugDraw {
public:
    /**
     * Class constructor.
     */
    OpenGLDebugDrawer()
      : m_debugMode(DBG_DrawWireframe)
    {
    }
    
    /**
     * Sets debug mode setting.
     */
    void setDebugMode(int mode)
    {
      m_debugMode = mode;
    }
    
    /**
     * Returns current debug mode setting.
     */
    int getDebugMode() const
    {
      return m_debugMode;
    }
    
    /**
     * Draws a simple line with gradient color.
     *
     * @param from Start point
     * @param to End point
     * @param fromColor Start color
     * @param toColor End color
     */
    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &fromColor, const btVector3 &toColor)
    {
      glBegin(GL_LINES);
      glColor3f(fromColor.getX(), fromColor.getY(), fromColor.getZ());
      glVertex3d(from.getX(), from.getY(), from.getZ());
      glColor3f(toColor.getX(), toColor.getY(), toColor.getZ());
      glVertex3d(to.getX(), to.getY(), to.getZ());
      glEnd();
    }
    
    /**
     * Draws a simple line.
     *
     * @param from Start point
     * @param to End point
     * @param color Color
     */
    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
      glBegin(GL_LINES);
      glColor4f(color.getX(), color.getY(), color.getZ(), 1.f);
      glVertex3d(from.getX(), from.getY(), from.getZ());
      glVertex3d(to.getX(), to.getY(), to.getZ());
      glEnd();
    }
    
    /**
     * Draws a simple sphere.
     *
     * @param p Center point
     * @param radius Sphere radius
     * @param color Color
     */
    void drawSphere(const btVector3 &p, btScalar radius, const btVector3 &color)
    {
      glColor4f(color.getX(), color.getY(), color.getZ(), btScalar(1.0f));
      glPushMatrix();
      glTranslatef(p.getX(), p.getY(), p.getZ());
      glutSolidSphere(radius, 10, 10);
      glPopMatrix();
    }
    
    /**
     * Draws a simple box.
     *
     * @param boxMin Minimum point
     * @param boxMax Maximum point
     * @param color Color
     * @param alpha Alpha value
     */
    void drawBox(const btVector3 &boxMin, const btVector3 &boxMax, const btVector3 &color, btScalar alpha)
    {
      btVector3 halfExtent = (boxMax - boxMin) * btScalar(0.5f);
      btVector3 center = (boxMax + boxMin) * btScalar(0.5f);
      //glEnable(GL_BLEND);     // Turn blending On
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glColor4f(color.getX(), color.getY(), color.getZ(), alpha);
      glPushMatrix();
      glTranslatef(center.getX(), center.getY(), center.getZ());
      glScaled(2 * halfExtent[0], 2 * halfExtent[1], 2 * halfExtent[2]);
      glutSolidCube(1.0);
      glPopMatrix();
      //glDisable(GL_BLEND);
    }
    
    /**
     * Draws a simple triangle.
     *
     * @param a First point
     * @param b Second point
     * @param c Third point
     * @param color Color
     * @param alpha Alpha value
     */
    void drawTriangle(const btVector3 &a, const btVector3 &b, const btVector3 &c, const btVector3 &color, btScalar alpha)
    {
      const btVector3 n = cross(b-a, c-a).normalized();
      
      glBegin(GL_TRIANGLES);
      glColor4f(color.getX(), color.getY(), color.getZ(), alpha);
      glNormal3d(n.getX(), n.getY(), n.getZ());
      glVertex3d(a.getX(), a.getY(), a.getZ());
      glVertex3d(b.getX(), b.getY(), b.getZ());
      glVertex3d(c.getX(), c.getY(), c.getZ());
      glEnd();
    }
    
    /**
     * Reports an error to the console.
     */
    void reportErrorWarning(const char *warningString)
    {
      // FIXME should use the logger interface
      std::cout << warningString;
    }
    
    /**
     * Draws a contact point.
     */
    void drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
    {
      btVector3 to = pointOnB + normalOnB * distance;
      const btVector3 &from = pointOnB;
      glColor4f(color.getX(), color.getY(), color.getZ(), 1.f);
      //glColor4f(0,0,0,1.f);

      glBegin(GL_LINES);
      glVertex3d(from.getX(), from.getY(), from.getZ());
      glVertex3d(to.getX(), to.getY(), to.getZ());
      glEnd();
    }
    
    /**
     * Draws some text.
     */
    void draw3dText(const btVector3 &location, const char *textString)
    {
      // Text drawing is not implemented
    }
private:
    int m_debugMode;
};

OpenGLTexture::OpenGLTexture(Format format)
  : DTexture(format),
    m_oglFormat(formatToOpenGL())
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
  glTexParameterf(m_oglFormat, paramToOpenGL(param), valueToOpenGL(value));
}

void OpenGLTexture::setParameter(Parameter param, float value)
{
  glTexParameterf(m_oglFormat, paramToOpenGL(param), value);
}

void OpenGLTexture::bind(int textureUnit)
{
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(m_oglFormat, m_handle);
  
  // Check for errors
  if (glGetError() != GL_NO_ERROR)
    throw Exception("OpenGL: Texture bind has failed!");
}

void OpenGLTexture::unbind(int textureUnit)
{
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(m_oglFormat, 0);
}

void OpenGLTexture::buildMipMaps2D(int components, int width, int height, PixelFormat format, unsigned char *data)
{
  GLenum _format;
  switch (format) {
    case RGB: _format = GL_RGB; break;
    case RGBA: _format = GL_RGBA; break;
  }
  
  gluBuild2DMipmaps(m_oglFormat, components, width, height, _format, GL_UNSIGNED_BYTE, (void*) data);
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
  : Driver("OpenGL"),
    m_debugDrawer(0)
{
  gOpenGLDriver = this;
}

OpenGLDriver::~OpenGLDriver()
{
  delete m_debugDrawer;
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

btIDebugDraw *OpenGLDriver::getDebugBulletDynamicsDrawer()
{
  if (!m_debugDrawer) {
    // Create a new OpenGL debug drawer
    m_debugDrawer = new OpenGLDebugDrawer();
  }
  
  return m_debugDrawer;
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
