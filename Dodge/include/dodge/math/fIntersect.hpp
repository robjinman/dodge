/*
 * Author: Rob Jinman <admin@robjinman.com>
 * Date: 2012
 */

#ifndef __F_INTERSECT_HPP__
#define __F_INTERSECT_HPP__


#include "Primitive.hpp"
#include "Vec2f.hpp"


namespace Dodge {
namespace Math {


bool intersect(const Primitive& obj1, const Vec2f& pos1, const Primitive& obj2, const Vec2f& pos2);


}
}


#endif
