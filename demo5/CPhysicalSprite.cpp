#include "CPhysicalSprite.hpp"


using namespace Dodge;


//===========================================
// CPhysicalSprite::CPhysicalSprite
//===========================================
CPhysicalSprite::CPhysicalSprite(const XmlNode data)
   : Entity(data.firstChild().firstChild()),
     Item(data.firstChild()),
     PhysicalSprite<Box2dPhysics>(data.nthChild(1)) {

   try {
      XML_NODE_CHECK(data, CPhysicalSprite);
   }
   catch (XmlException& e) {
      e.prepend("Error parsing XML for instance of class CPhysicalSprite; ");
   }
}

//===========================================
// CPhysicalSprite::clone
//===========================================
CPhysicalSprite* CPhysicalSprite::clone() const {
   return new CPhysicalSprite(*this);
}

//===========================================
// CPhysicalSprite::addToWorld
//===========================================
void CPhysicalSprite::addToWorld() {
   PhysicalSprite<Box2dPhysics>::addToWorld();
   Item::addToWorld();
}

//===========================================
// CPhysicalSprite::removeFromWorld
//===========================================
void CPhysicalSprite::removeFromWorld() {
   PhysicalSprite<Box2dPhysics>::removeFromWorld();
   Item::removeFromWorld();
}

//===========================================
// CPhysicalSprite::update
//===========================================
void CPhysicalSprite::update() {
   PhysicalSprite<Box2dPhysics>::update();
   Item::update();
}

//===========================================
// CPhysicalSprite::render
//===========================================
void CPhysicalSprite::render() const {
   PhysicalSprite<Box2dPhysics>::render();
}

//===========================================
// CPhysicalSprite::unrender
//===========================================
void CPhysicalSprite::unrender() const {
   PhysicalSprite<Box2dPhysics>::unrender();
}

//===========================================
// CPhysicalSprite::assignData
//===========================================
void CPhysicalSprite::assignData(const XmlNode data) {
   try {
      XML_NODE_CHECK(data, CPhysicalSprite)

      XmlNode node = data.firstChild();
      if (!node.isNull() && node.name() == "Item") {
         Item::assignData(node);
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "PhysicalSprite") {
         PhysicalSprite<Box2dPhysics>::assignData(node);
      }
   }
   catch (XmlException& e) {
      e.prepend("Error parsing XML for instance of class CPhysicalSprite; ");
      throw;
   }
}
