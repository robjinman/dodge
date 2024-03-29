/*
 * Author: Rob Jinman <admin@robjinman.com>
 * Date: 2012
 */

#ifndef __VEC2I_HPP__
#define __VEC2I_HPP__


#include <math.h>
#include "../definitions.hpp"
#include "../xml/xml.hpp"


namespace Dodge {


class Vec2f;

class Vec2i {
   public:
      int x, y;

      Vec2i() : x(0), y(0) {}
      Vec2i(const XmlNode data);
      Vec2i(int x_, int y_) : x(x_), y(y_) {}
      Vec2i(const Vec2i& v) : x(v.x), y(v.y) {}
      Vec2i(const Vec2f& v);

      Vec2i operator+(const Vec2i& v) const { return Vec2i(x + v.x, y + v.y); }
      Vec2i operator-(const Vec2i& v) const { return Vec2i(x - v.x, y - v.y); }
      Vec2i operator*(int s) const { return Vec2i(x * s, y * s); }
      Vec2i operator/(int s) const { return Vec2i(x / s, y / s); }

      bool operator==(const Vec2i& v) const { return x == v.x && y == v.y; }
      bool operator!=(const Vec2i& v) const { return !(*this == v); }

      Vec2i& operator=(const Vec2i& rhs) { x = rhs.x; y = rhs.y; return *this; }

      void rotate(const Vec2i& p, float32_t a);
};


}


#endif
