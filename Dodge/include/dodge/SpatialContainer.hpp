/*
 * Author: Rob Jinman <admin@robjinman.com>
 * Date: 2012
 */

#ifndef __SPATIAL_CONTAINER_HPP__
#define __SPATIAL_CONTAINER_HPP__


#include <vector>
#ifdef DEBUG
#include <ostream>
#include "renderer/Renderer.hpp"
#endif


namespace Dodge {


class Range;

template <typename T>
class SpatialContainer {
   public:
      virtual bool insert(T item, const Range& boundingBox) = 0;
      virtual bool remove(T item, const Range& boundingBox) = 0;
      virtual void removeAll() = 0;
      virtual int getNumEntries() const = 0;
      virtual void getEntries(const Range& region, std::vector<T>& entries) const = 0;
      virtual const Range& getBoundary() const = 0;

#ifdef DEBUG
      virtual void dbg_draw(const Colour& colour, Renderer::int_t lineWidth, float32_t z) const = 0;
#endif

      virtual ~SpatialContainer() {}
};


}


#endif
