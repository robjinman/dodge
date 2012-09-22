/*
 * Author: Rob Jinman <admin@robjinman.com>
 * Date: 2012
 */

#ifndef __I_RENDERER_HPP__
#define __I_RENDERER_HPP__


#include <cstring>
#include <exception>


namespace Dodge {


// Interface class for cross-platform renderer
template <
   typename T_FLOAT,
   typename T_VERT_ELEM,
   typename T_MAT_ELEM,
   typename T_COL_ELEM,
   typename T_TEXCOORD_ELEM,
   typename T_TEXDATA,
   typename T_TEXID
>
class IRenderer {
   public:
      enum mode_t {
         UNDEFINED,
         TEXTURED_ALPHA,
         NONTEXTURED_ALPHA,
         TEXTURED_NONALPHA,
         NONTEXTURED_NONALPHA
      };

      class Brush {
         public:
            Brush()
               : fillColour({1.f, 1.f, 1.f, 1.f}), lineColour({0.f, 0.f, 0.f, 1.f}), lineWidth(1) {}

            Brush(const T_COL_ELEM fCol[4], const T_COL_ELEM lCol[4], int lineW) {
               memcpy(fillColour, fCol, 4 * sizeof(T_COL_ELEM));
               memcpy(lineColour, lCol, 4 * sizeof(T_COL_ELEM));
               lineWidth = lineW;
            }

            T_COL_ELEM fillColour[4];
            T_COL_ELEM lineColour[4];
            int lineWidth;
      };

      enum primitive_t {
         TRIANGLES,
         LINES,
         QUADS,
         TRIANGLE_STRIP
      };

      //===========================================
      // IRenderer::setP
      //
      // 4x4 projection matrix (in column-major order)
      //===========================================
      virtual void setP(const T_MAT_ELEM* matrix) {
         memcpy(m_projMat, matrix, 16 * sizeof(T_MAT_ELEM));
      }

      //===========================================
      // IRenderer::P
      //===========================================
      virtual T_MAT_ELEM* P(int idx = 0) const {
         if (idx < 0 || idx > 15)
            throw std::runtime_error("Error returning element of P; index out of range");

         return m_projMat + idx;
      }

      //===========================================
      // IRenderer::attachBrush
      //===========================================
      virtual void attachBrush(Brush brush) {
         m_brush = brush;
      }

      //===========================================
      // IRenderer::getBrush
      //===========================================
      virtual Brush& getBrush() const {
         return m_brush;
      }

      // A file may be specified containing implementation-specific options.
      virtual void init(const char* optsFile = 0) = 0;

      virtual void setMode(mode_t mode) = 0;

      // Constructs a texture object from raw image data, and returns its id.
      virtual T_TEXID newTexture(const T_TEXDATA* texture, int width, int height) = 0;

      // Set current model view matrix
      virtual void setMatrix(const T_MAT_ELEM* mat) = 0;

      virtual void setActiveTexture(T_TEXID texId) = 0;

      virtual void setGeometry(const T_VERT_ELEM* verts, primitive_t primitiveType, int count) = 0;

      virtual void setColours(const T_COL_ELEM* colours, int count) = 0;

      virtual void setTextureCoords(const T_TEXCOORD_ELEM* texCoords, int count) = 0;

      virtual void render() = 0;

      virtual void clear() = 0;

   protected:
      static Brush m_brush;
      static T_MAT_ELEM m_projMat[16];
};


// Initialise static members
template <
   typename T_FLOAT, typename T_VERT_ELEM, typename T_MAT_ELEM, typename T_COL_ELEM,
   typename T_TEXCOORD_ELEM, typename T_TEXDATA, typename T_TEXID
>
typename IRenderer<T_FLOAT, T_VERT_ELEM, T_MAT_ELEM, T_COL_ELEM, T_TEXCOORD_ELEM, T_TEXDATA, T_TEXID>::Brush
   IRenderer<T_FLOAT, T_VERT_ELEM, T_MAT_ELEM, T_COL_ELEM, T_TEXCOORD_ELEM, T_TEXDATA, T_TEXID>::m_brush
      = IRenderer<T_FLOAT, T_VERT_ELEM, T_MAT_ELEM, T_COL_ELEM, T_TEXCOORD_ELEM, T_TEXDATA, T_TEXID>::Brush();

template <
   typename T_FLOAT, typename T_VERT_ELEM, typename T_MAT_ELEM, typename T_COL_ELEM,
   typename T_TEXCOORD_ELEM, typename T_TEXDATA, typename T_TEXID
>
T_MAT_ELEM IRenderer<T_FLOAT, T_VERT_ELEM, T_MAT_ELEM, T_COL_ELEM, T_TEXCOORD_ELEM, T_TEXDATA, T_TEXID>::m_projMat[]
   = { 1.f, 0.f, 0.f, 0.f,
       0.f, 1.f, 0.f, 0.f,
       0.f, 0.f, 1.f, 0.f,
       0.f, 0.f, 0.f, 1.f };


}


#endif /*!__I_RENDERER_HPP__*/
