/*
 * Author: Rob Jinman <admin@robjinman.com>
 * Date: 2012
 */

#ifndef __UI_BUTTON_HPP__
#define __UI_BUTTON_HPP__


#include <boost/shared_ptr.hpp>
#include "../utils/Functor.hpp"
#include "../Sprite.hpp"
#include "EntityUi.hpp"
#include "../rapidxml/rapidxml.hpp"
#ifdef DEBUG
#include <ostream>
#endif


namespace Dodge {


class UiButton : public Sprite, public EntityUi {
   public:
      UiButton(long type, pTexture_t texture)
         : Sprite(type, texture),
           EntityUi(this),
           m_state(BTN_IDLE),
           m_mouseOver(false),
           m_btn1Pressed(false),
           m_onClick(&UiButton::void_entityPtr),
           m_onRelease(&UiButton::void_entityPtr) {}

      UiButton(long name, long type, pTexture_t texture)
         : Sprite(name, type, texture),
           EntityUi(this),
           m_state(BTN_IDLE),
           m_mouseOver(false),
           m_btn1Pressed(false),
           m_onClick(&UiButton::void_entityPtr),
           m_onRelease(&UiButton::void_entityPtr) {}

      UiButton(const UiButton& copy)
         : Sprite(copy),
           EntityUi(copy, this),
           m_state(BTN_IDLE),
           m_mouseOver(false),
           m_btn1Pressed(false),
           m_onClick(&UiButton::void_entityPtr),
           m_onRelease(&UiButton::void_entityPtr) {}

      UiButton(const UiButton& copy, long name)
         : Sprite(copy, name),
           EntityUi(copy, this),
           m_state(BTN_IDLE),
           m_mouseOver(false),
           m_btn1Pressed(false),
           m_onClick(&UiButton::void_entityPtr),
           m_onRelease(&UiButton::void_entityPtr) {}

      // TODO
      virtual UiButton* clone() const { return NULL; }

#ifdef DEBUG
      virtual void dbg_print(std::ostream& out) const;
#endif
      virtual void assignData(const rapidxml::xml_node<>* data);

      virtual void update();

      virtual void onBtn1Press(float32_t x, float32_t y);
      virtual void onBtn1Release(float32_t x, float32_t y);
      virtual void onHoverOn(float32_t x, float32_t y);
      virtual void onHoverOff(float32_t x, float32_t y);

      inline void setOnPressHandler(Functor<void, TYPELIST_1(pEntity_t)> func);
      inline void setOnReleaseHandler(Functor<void, TYPELIST_1(pEntity_t)> func);

      // Do nothing function
      static void void_entityPtr(pEntity_t) {}

   private:
      typedef enum {
         BTN_IDLE,
         BTN_PRESS,
         BTN_RELEASE,
         BTN_HOVER_ON,
         BTN_HOVER_OFF
      } state_t;

      state_t m_state;
      bool m_mouseOver;
      bool m_btn1Pressed;

      Functor<void, TYPELIST_1(pEntity_t)> m_onClick;
      Functor<void, TYPELIST_1(pEntity_t)> m_onRelease;
};

typedef boost::shared_ptr<UiButton> pUiButton_t;

//===========================================
// UiButton::setOnPressHandler
//===========================================
inline void UiButton::setOnPressHandler(Functor<void, TYPELIST_1(pEntity_t)> func) {
   m_onClick = func;
}

//===========================================
// UiButton::setOnReleaseHandler
//===========================================
inline void UiButton::setOnReleaseHandler(Functor<void, TYPELIST_1(pEntity_t)> func) {
   m_onRelease = func;
}


}


#endif