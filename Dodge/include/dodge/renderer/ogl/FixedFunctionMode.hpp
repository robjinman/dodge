/*
 * Author: Rob Jinman <admin@robjinman.com>
 * Date: 2012
 */

#ifndef __FIXED_FUNCTION_MODE_HPP__
#define __FIXED_FUNCTION_MODE_HPP__


#include "RenderMode.hpp"


namespace Dodge {


class FixedFunctionMode : public RenderMode {
   public:
      FixedFunctionMode();

      virtual bool hasPending() const { return false; }
      virtual void setActive();
      virtual void sendData(const IModel* model, const cml::matrix44f_c& projMat);
      virtual void flush() {}

      virtual ~FixedFunctionMode() {}

   private:
      bool isSupported(const IModel* model) const;
};


}


#endif