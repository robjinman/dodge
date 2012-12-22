#include <set>
#include <dodge/ETransFinished.hpp>
#include <dodge/StringId.hpp>
#include "Player.hpp"


using namespace std;
using namespace Dodge;


//===========================================
// Player::Player
//===========================================
Player::Player(const XmlNode data)
   : Entity(data.firstChild().firstChild()),
     Item(data.firstChild()),
     PhysicalSprite<Box2dPhysics>(data.nthChild(1)),
     m_mode(DIG_MODE),
     m_modeLocked(0) {

#ifdef DEBUG
   dbg_flags = 0;
#endif

   try {
      XML_NODE_CHECK(data, Player);

      XmlNode node = data.nthChild(2);
      XML_NODE_CHECK(node, speed);
      m_speed = node.getFloat();

      node = node.nextSibling();
      XML_NODE_CHECK(node, acceleration);
      m_acc = node.getFloat();

      node = node.nextSibling();
      XML_NODE_CHECK(node, gridSize);
      m_gridSize = Vec2f(node.firstChild());

      node = node.nextSibling();
      XML_NODE_CHECK(node, footSensor);
      m_footSensor = Quad(node.firstChild());

      node = node.nextSibling();
      XML_NODE_CHECK(node, headSensor);
      m_headSensor = Quad(node.firstChild());

      node = node.nextSibling();
      XML_NODE_CHECK(node, leftSensor);
      m_leftSensor = Quad(node.firstChild());

      node = node.nextSibling();
      XML_NODE_CHECK(node, rightSensor);
      m_rightSensor = Quad(node.firstChild());

      node = node.nextSibling();
      XML_NODE_CHECK(node, midSensor);
      m_midSensor = Quad(node.firstChild());

      init();
   }
   catch (XmlException& e) {
      e.prepend("Error parsing XML for instance of class Player; ");
      throw;
   }
}

//===========================================
// Player::Player
//===========================================
Player::Player(const Player& copy, long name)
   : Entity(copy, name),
     Item(copy, name),
     PhysicalSprite<Box2dPhysics>(copy, name),
     m_mode(DIG_MODE),
     m_modeLocked(0) {

   m_gridSize = copy.m_gridSize;
   m_footSensor = copy.m_footSensor;
   m_headSensor = copy.m_headSensor;
   m_leftSensor = copy.m_leftSensor;
   m_rightSensor = copy.m_rightSensor;
   m_midSensor = copy.m_midSensor;

   init();
}

//===========================================
// Player::init
//===========================================
void Player::init() {
#ifdef DEBUG
   dbg_flags = 0;

   m_footSensor.setFillColour(Colour(1.f, 0.f, 0.f, 0.4f));
   m_footSensor.setLineWidth(0);

   m_headSensor.setFillColour(Colour(1.f, 0.f, 0.f, 0.4f));
   m_headSensor.setLineWidth(0);

   m_leftSensor.setFillColour(Colour(1.f, 0.f, 0.f, 0.4f));
   m_leftSensor.setLineWidth(0);

   m_rightSensor.setFillColour(Colour(1.f, 0.f, 0.f, 0.4f));
   m_rightSensor.setLineWidth(0);

   m_midSensor.setFillColour(Colour(1.f, 0.f, 0.f, 0.4f));
   m_midSensor.setLineWidth(0);
#endif
}

//===========================================
// Player::clone
//===========================================
Player* Player::clone() const {
   return new Player(*this);
}

//===========================================
// Player::snapToGridH
//===========================================
void Player::snapToGridH(float32_t offset) {
   static long snapToGridHStr = internString("snapToGridH");

   float32_t x = getTranslation_abs().x;
   float32_t targetx = floor((x / m_gridSize.x) + 0.5) * m_gridSize.x + offset;

   vector<TransFrame> frames;
   frames.push_back(TransFrame(Vec2f(targetx - x, 0.f), 0.f, Vec2f(1.f, 1.f)));

   pTransformation_t t(new Transformation(snapToGridHStr, 16.f, frames));
   t->setSmooth(8);

   addTransformation(t);
   playTransformation(snapToGridHStr);
}

//===========================================
// Player::snapToGridV
//===========================================
void Player::snapToGridV(float32_t offset) {
   static long snapToGridVStr = internString("snapToGridV");

   float32_t y = getTranslation_abs().y;
   float32_t targety = floor((y / m_gridSize.y) + 0.5) * m_gridSize.y + offset;

   vector<TransFrame> frames;
   frames.push_back(TransFrame(Vec2f(0.f, targety - y), 0.f, Vec2f(1.f, 1.f)));

   pTransformation_t t(new Transformation(snapToGridVStr, 16.f, frames));
   t->setSmooth(8);

   addTransformation(t);
   playTransformation(snapToGridVStr);
}

//===========================================
// Player::addToWorld
//===========================================
void Player::addToWorld() {
   PhysicalSprite<Box2dPhysics>::addToWorld();
}

//===========================================
// Player::removeFromWorld
//===========================================
void Player::removeFromWorld() {
   PhysicalSprite<Box2dPhysics>::removeFromWorld();
}

//===========================================
// Player::update
//===========================================
void Player::update() {
   static long gravityRegionStr = internString("gravityRegion");

   PhysicalSprite<Box2dPhysics>::update();

   if (m_modeLockTimer.getTime() > m_modeLocked) {
      bool b = false;
      vector<pEntity_t> vec;
      m_worldSpace.getEntities(Range(getBoundary()), vec);
      for (uint_t i = 0; i < vec.size(); ++i) {
         if (vec[i]->getTypeName() == gravityRegionStr) {
            if (Math::contains(vec[i]->getShape(), vec[i]->getTranslation_abs(), m_midSensor, getTranslation_abs())) {
               b = true;
               break;
            }
         }
      }

      if (b) {
         if (m_mode == DIG_MODE) {
            makeDynamic();
         }

         m_mode = PLATFORM_MODE;
      }
      else {
         if (m_mode == PLATFORM_MODE) {
            makeStatic();
         }

         m_mode = DIG_MODE;
      }
   }
}

//===========================================
// Player::draw
//===========================================
void Player::draw() const {
   PhysicalSprite<Box2dPhysics>::draw();

#ifdef DEBUG
   Vec2f pos = getTranslation_abs();

   if (dbg_flags & DBG_DRAW_SENSORS) {
      m_footSensor.setRenderTransform(pos.x, pos.y, 9);
      m_headSensor.setRenderTransform(pos.x, pos.y, 9);
      m_leftSensor.setRenderTransform(pos.x, pos.y, 9);
      m_rightSensor.setRenderTransform(pos.x, pos.y, 9);
      m_midSensor.setRenderTransform(pos.x, pos.y, 9);

      m_footSensor.draw();
      m_headSensor.draw();
      m_leftSensor.draw();
      m_rightSensor.draw();
      m_midSensor.draw();
   }

   if (dbg_flags & DBG_DRAW_SHAPE) {
      const Shape& shape = getShape();

      shape.setRenderTransform(pos.x, pos.y, 9);
      shape.setFillColour(Colour(1.f, 0.f, 0.f, 0.4f));
      shape.setLineWidth(0);
      shape.draw();
   }
#endif
}

//===========================================
// Player::grounded
//===========================================
bool Player::grounded() const {
   vector<pEntity_t> vec;
   m_worldSpace.getEntities(getBoundary(), vec);

   for (uint_t i = 0; i < vec.size(); ++i) {
      pItem_t item = boost::dynamic_pointer_cast<Item>(vec[i]);
      assert(item);

      if (item.get() == this) continue;
      if (!item->isSolid()) continue;
      if (!item->hasShape()) continue;

      if (Math::overlap(m_footSensor, getTranslation_abs(), item->getShape(), item->getTranslation_abs()))
         return true;
   }

   return false;
}

//===========================================
// Player::jump
//===========================================
void Player::jump() {
   static long jumpStr = internString("jump");

   if (grounded() && m_jumpTimer.getTime() > 0.2) {
      stopAnimation();
      playAnimation(jumpStr);
      applyLinearImpulse(Vec2f(0.0, 0.2), Vec2f(0.0, 0.0));
      m_jumpTimer.reset();
   }
}

//===========================================
// Player::move
//
// 0 = left, 1 = up, 2 = right, 3 = down
//===========================================
bool Player::move(int dir) {
   static long gravityRegionStr = internString("gravityRegion");
   static long moveLeftStr = internString("moveLeft");
   static long hitFromRightStr = internString("hitFromRight");
   static long moveRightStr = internString("moveRight");
   static long hitFromLeftStr = internString("hitFromLeft");
   static long moveUpStr = internString("moveUp");
   static long hitFromBottomStr = internString("hitFromBottom");
   static long moveDownStr = internString("moveDown");
   static long hitFromTopStr = internString("hitFromTop");

   if (m_mode == DIG_MODE) {
      Shape* sensor = NULL;
      dir_t facing = LEFT;
      long plyrAnim = 0, eventType = 0;
      switch (dir) {
         case 0:           // Left
            sensor = &m_leftSensor;
            facing = LEFT;
            plyrAnim = moveLeftStr;
            eventType = hitFromRightStr;
            break;
         case 1:           // Up
            sensor = &m_headSensor;
            facing = UP;
            plyrAnim = moveUpStr;
            eventType = hitFromBottomStr;
            break;
         case 2:           // Right
            sensor = &m_rightSensor;
            facing = RIGHT;
            plyrAnim = moveRightStr;
            eventType = hitFromLeftStr;
            break;
         case 3:           // Down
            sensor = &m_footSensor;
            facing = DOWN;
            plyrAnim = moveDownStr;
            eventType = hitFromTopStr;
            break;
      }

      playAnimation(plyrAnim);

      if (numActiveTransformations() == 0) {
         playTransformation(plyrAnim);

         Vec2f min = sensor->getMinimum();
         Vec2f pos = getTranslation_abs() + min;
         Range range(pos, sensor->getMaximum() - min);

         vector<pEntity_t> vec;
         m_worldSpace.getEntities(range, vec);
         for (uint_t i = 0; i < vec.size(); ++i) {
            if (vec[i].get() == this) continue;
            if (!vec[i]->hasShape()) continue;

            if (Math::overlap(*sensor, getTranslation_abs(), vec[i]->getShape(), vec[i]->getTranslation_abs())) {
               EEvent* event = new EEvent(eventType);
               vec[i]->onEvent(event);
            }
         }

         m_facing = facing;

         return true;
      }
   }
   else if (m_mode == PLATFORM_MODE) {
      switch (dir) {
         case 0: {
            bool b = false;
            vector<pEntity_t> vec;
            m_worldSpace.getEntities(Range(getBoundary()), vec);
            for (uint_t i = 0; i < vec.size(); ++i) {
               if (vec[i]->getTypeName() == gravityRegionStr) {
                  if (!Math::contains(vec[i]->getShape(), vec[i]->getTranslation_abs(), m_leftSensor, getTranslation_abs())) {
                     b = true;
                     break;
                  }
               }
            }

            if (b) {
               m_mode = DIG_MODE;
               makeStatic();

               snapToGridV();
               snapToGridH();

               m_modeLocked = 0.2;
               m_modeLockTimer.reset();

               move(0);
            }
            else {
               playAnimation(moveLeftStr);
               if (getLinearVelocity().x > -m_speed)
                  applyForce(Vec2f(-m_acc, 0.0), Vec2f(0.0, 0.0));
            }
         }
         break;
         case 1: {
            bool b = false;
            vector<pEntity_t> vec;
            m_worldSpace.getEntities(Range(getBoundary()), vec);
            for (uint_t i = 0; i < vec.size(); ++i) {
               if (vec[i]->getTypeName() == gravityRegionStr) {
                  if (!Math::contains(vec[i]->getShape(), vec[i]->getTranslation_abs(), m_headSensor, getTranslation_abs())) {
                     b = true;
                     break;
                  }
               }
            }

            if (b) {
               m_mode = DIG_MODE;
               makeStatic();

               snapToGridV();
               snapToGridH();

               m_modeLocked = 0.2;
               m_modeLockTimer.reset();

               move(0);
            }
            else {
               jump();
            }
         }
         break;
         case 2: {
            bool b = false;
            vector<pEntity_t> vec;
            m_worldSpace.getEntities(Range(getBoundary()), vec);
            for (uint_t i = 0; i < vec.size(); ++i) {
               if (vec[i]->getTypeName() == gravityRegionStr) {
                  if (!Math::contains(vec[i]->getShape(), vec[i]->getTranslation_abs(), m_rightSensor, getTranslation_abs())) {
                     b = true;
                     break;
                  }
               }
            }

            if (b) {
               m_mode = DIG_MODE;
               makeStatic();

               snapToGridV();
               snapToGridH();

               m_modeLocked = 0.2;
               m_modeLockTimer.reset();

               move(2);
            }
            else {
               playAnimation(moveRightStr);
               if (getLinearVelocity().x < m_speed)
                  applyForce(Vec2f(m_acc, 0.0), Vec2f(0.0, 0.0));
            }
         }
         break;
         case 3: {
            bool b = false;
            vector<pEntity_t> vec;
            m_worldSpace.getEntities(Range(getBoundary()), vec);
            for (uint_t i = 0; i < vec.size(); ++i) {
               if (vec[i]->getTypeName() == gravityRegionStr) {
                  if (!Math::contains(vec[i]->getShape(), vec[i]->getTranslation_abs(), m_footSensor, getTranslation_abs())) {
                     b = true;
                     break;
                  }
               }
            }

            if (b) {
               m_mode = DIG_MODE;
               makeStatic();

               snapToGridV();
               snapToGridH();

               m_modeLocked = 0.2;
               m_modeLockTimer.reset();

               move(3);
            }
         }
         break;
      }
      return true;
   }

   return false;
}

//===========================================
// Player::assignData
//===========================================
void Player::assignData(const XmlNode data) {
   try {
      XML_NODE_CHECK(data, Player)

      XmlNode node = data.firstChild();
      if (!node.isNull() && node.name() == "Item") {
         Item::assignData(node);
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "PhysicalSprite") {
         PhysicalSprite<Box2dPhysics>::assignData(node);
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "speed") {
         m_speed = node.getFloat();
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "acceleration") {
         m_acc = node.getFloat();
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "gridSize") {
         m_gridSize = Vec2f(node.firstChild());
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "footSensor") {
         m_footSensor = Quad(node.firstChild());
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "headSensor") {
         m_headSensor = Quad(node.firstChild());
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "leftSensor") {
         m_leftSensor = Quad(node.firstChild());
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "rightSensor") {
         m_rightSensor = Quad(node.firstChild());
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "midSensor") {
         m_midSensor = Quad(node.firstChild());
      }
   }
   catch (XmlException& e) {
      e.prepend("Error parsing XML for instance of class Player; ");
      throw;
   }
}
