/*
 * Author: Rob Jinman <admin@robjinman.com>
 * Date: 2012
 */

#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__


#include <GLES2/gl2.h>
#include <cml/cml.h>
#include <set>
#include <map>
#include <cstring>
#include <mutex>
#include <thread>
#include <atomic>
#include <boost/variant.hpp>
#include "Colour.hpp"
#include "../RenderBrush.hpp"
#include "../Camera.hpp"
#include "../../WinIO.hpp"
#include "../../definitions.hpp"


namespace Dodge {


class IModel;
class SceneGraph;
class ShaderProgram;

// OpenGL ES 2.0 implementation
class Renderer {
   public:
      typedef GLint int_t;
      typedef GLfloat float_t;
      typedef GLfloat vertexElement_t;
      typedef GLfloat matrixElement_t;
      typedef GLfloat colourElement_t;
      typedef GLfloat texCoordElement_t;
      typedef byte_t textureData_t;
      typedef GLuint textureHandle_t;
      typedef GLuint modelHandle_t;

      enum mode_t {
         UNDEFINED,
         TEXTURED_ALPHA,
         NONTEXTURED_ALPHA
         // ...
      };

      enum primitive_t {
         TRIANGLES,
         LINES,
         QUADS,
         TRIANGLE_STRIP
      };

      void setBgColour(const Colour& col);

      inline void attachCamera(pCamera_t camera);
      inline Camera& getCamera() const;

      void onWindowResize(int_t w, int_t h);
      void newTexture(const textureData_t* texture, int_t width, int_t height, textureHandle_t* handle);

      void bufferModel(IModel* model);
      void freeBufferedModel(IModel* model);

      void stageModel(const IModel* model);
      void unstageModel(const IModel* model);
#ifdef DEBUG
      inline long getFrameRate() const;
#endif
      void start();
      void stop();

      void checkForErrors();

      enum exceptionType_t {
         UNKNOWN_EXCEPTION,
         EXCEPTION,
         RENDERER_EXCEPTION
         // ...
      };

      struct exceptionWrapper_t {
         exceptionType_t type;
         void* data;
      };

   private:
      typedef enum {
         MSG_TEX_HANDLE_REQ,
         MSG_VP_RESIZE_REQ,
         MSG_CONSTRUCT_VBO
         // ...
      } msgType_t;

      struct msgTexHandleReq_t {
         const textureData_t* texData;
         int_t w;
         int_t h;

         textureHandle_t* retVal;
      };

      struct msgVpResizeReq_t {
         int_t w;
         int_t h;
      };

      struct msgConstructVbo_t {
         IModel* model;
      };

      typedef boost::variant<
         msgTexHandleReq_t,
         msgVpResizeReq_t,
         msgConstructVbo_t
         // ...
      > msgData_t;

      struct Message {
         Message(msgType_t type_, msgData_t data_)
            : type(type_), data(data_) {}

         msgType_t type;
         msgData_t data;
      };

      static void renderLoop();
      static void processMessages();

      void init();
      void clear();
      void setMode(mode_t mode);
      void constructShaderProgs();
      GLint primitiveToGLType(primitive_t primitiveType) const;
      void processMessage(const Message& msg);
      textureHandle_t loadTexture(const textureData_t* texture, int_t w, int_t h);
      void constructVBO(IModel* model);
      void queueMsg(Message msg);
#ifdef DEBUG
      void computeFrameRate();
#endif

      static std::map<mode_t, ShaderProgram*> m_shaderProgs;
      static ShaderProgram* m_activeShaderProg;
      static mode_t m_mode;

      static std::atomic<bool> m_init;

      static SceneGraph m_sceneGraph;
      static std::mutex m_sceneGraphMutex;

      static pCamera_t m_camera;
      static std::mutex m_cameraMutex;

      static std::atomic<bool> m_running;
      static std::thread* m_thread;

      static std::vector<Message> m_msgQueue;
      static std::mutex m_msgQueueMutex;

      static std::atomic<bool> m_msgQueueEmpty;

      static Colour m_bgColour;
      static std::mutex m_bgColourMutex;

      static exceptionWrapper_t m_exception;
      static std::atomic<bool> m_errorPending;

#ifdef DEBUG
      static std::atomic<long> m_frameRate;
#endif
};

#ifdef DEBUG
//===========================================
// Renderer::getFrameRate
//===========================================
inline long Renderer::getFrameRate() const {
   return m_frameRate;
}
#endif

//===========================================
// Renderer::attachCamera
//===========================================
inline void Renderer::attachCamera(pCamera_t camera) {
   m_cameraMutex.lock();
   m_camera = camera;
   m_cameraMutex.unlock();
}

//===========================================
// Renderer::getCamera
//===========================================
inline Camera& Renderer::getCamera() const {
   m_cameraMutex.lock();
   Camera& cpy = *m_camera;
   m_cameraMutex.unlock();

   return cpy;
}


}


#endif /*!__RENDERER_HPP__*/
