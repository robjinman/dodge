/*
 * Author: Rob Jinman <admin@robjinman.com>
 * Date: 2012
 */

#include <cstring>
#include <string>
#include <cstdio>
#include "Sprite.hpp"


using namespace rapidxml;
using namespace std;


namespace Dodge {


//===========================================
// Sprite::Sprite
//===========================================
Sprite::Sprite(const XmlNode data)
   : Entity(data.nthChild(0)),
     EntityAnimations(this, data.nthChild(1)),
     EntityTransformations(this, data.nthChild(2)) {

      if (data.isNull() || data.name() != "Sprite")
         throw XmlException("Error parsing XML for instance of class Sprite; Expected 'Sprite' tag", __FILE__, __LINE__);
}

//===========================================
// Sprite::Sprite
//===========================================
Sprite::Sprite(const Sprite& copy)
   : Entity(copy), EntityAnimations(copy, this), EntityTransformations(copy, this) {

   deepCopy(copy);
}

//===========================================
// Sprite::Sprite
//===========================================
Sprite::Sprite(const Sprite& copy, long name)
   : Entity(copy, name), EntityAnimations(copy, this), EntityTransformations(copy, this) {

   deepCopy(copy);
}

//===========================================
// Sprite::deepCopy
//===========================================
void Sprite::deepCopy(const Sprite& copy) {}

//===========================================
// Sprite::clone
//===========================================
Sprite* Sprite::clone() const {
   return new Sprite(*this);
}

//===========================================
// Sprite::assignData
//
// All tags and attributes are optional.
//===========================================
void Sprite::assignData(const XmlNode data) {
   if (data.isNull() || data.name() != "Sprite") return;

   XmlNode node = data.firstChild();

   if (!node.isNull() && node.name() == "Entity") {
      Entity::assignData(node);
      node = node.nextSibling();
   }

   if (!node.isNull() && node.name() == "EntityAnimations") {
      EntityAnimations::assignData(node);
      node = node.nextSibling();
   }

   if (!node.isNull() && node.name() == "EntityTransformations") {
      EntityTransformations::assignData(node);
   }
}

#ifdef DEBUG
//===========================================
// Sprite::dbg_print
//===========================================
void Sprite::dbg_print(std::ostream& out, int tab) const {
   for (int i = 0; i < tab; i++) out << "\t";
   out << "Sprite\n";

   Entity::dbg_print(out, tab + 1);
   EntityAnimations::dbg_print(out, tab + 1);
   EntityTransformations::dbg_print(out, tab + 1);
}
#endif

//===========================================
// Sprite::draw
//===========================================
void Sprite::draw() const {
   EntityAnimations::draw();
}

//===========================================
// Sprite::update
//===========================================
void Sprite::update() {
   Entity::update();
   EntityTransformations::update();
   EntityAnimations::update();
}


}
