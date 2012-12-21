/*
 * Author: Rob Jinman <admin@robjinman.com>
 * Date: 2012
 */

#ifndef __GLOBALS_HPP__
#define __GLOBALS_HPP__


#include "definitions.hpp"
#include "StackAllocator.hpp"
#include "math/Vec2f.hpp"


namespace Dodge {


struct projectSettings_t {
   projectSettings_t()
      : targetFrameRate(120),
        globalStackSize(102400) {}

   float32_t targetFrameRate;
   size_t globalStackSize;
};


extern void gInitialise(const projectSettings_t& settings = projectSettings_t());

extern float32_t gGetTargetFrameRate();
extern StackAllocator& gGetMemStack();
extern Vec2f gGetPixelSize();


};


#endif
