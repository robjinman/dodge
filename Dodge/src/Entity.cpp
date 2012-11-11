/*
 * Author: Rob Jinman <admin@robjinman.com>
 * Date: 2012
 */

#include <sstream>
#include <cstdio>
#include <Entity.hpp>
#include <EEntityMoved.hpp>
#include <Exception.hpp>
#include <StringId.hpp>
#include <AssetManager.hpp>
#include <globals.hpp>


using namespace rapidxml;
using namespace std;


namespace Dodge {


EventManager Entity::m_eventManager = EventManager();
int Entity::m_count = 0;


#ifdef DEBUG
//===========================================
// Entity::dbg_print
//===========================================
void Entity::dbg_print(std::ostream& out, int tab) const {
   for (int i = 0; i < tab; i++) out << "\t";
   out << "Entity\n";

   for (int i = 0; i < tab + 1; i++) out << "\t";
   out << "name: " << getInternedString(m_name) << "\n";

   for (int i = 0; i < tab + 1; i++) out << "\t";
   out << "type: " << getInternedString(m_type) << "\n";

   for (int i = 0; i < tab + 1; i++) out << "\t";
   out << "scale: (" << m_scale.x << ", " << m_scale.y << ")\n";

   for (int i = 0; i < tab + 1; i++) out << "\t";
   out << "translation: (" << m_transl.x << ", " << m_transl.y << ")\n";

   for (int i = 0; i < tab + 1; i++) out << "\t";
   out << "z: " << m_z << "\n";

   for (int i = 0; i < tab + 1; i++) out << "\t";
   out << "rotation: " << m_rot << "\n";

   for (int i = 0; i < tab + 1; i++) out << "\t";
   out << "shape (src):\n";
   if (m_shape) m_shape->dbg_print(out, tab + 1);

   for (int i = 0; i < tab + 1; i++) out << "\t";
   out << "boundary:\n";
   m_boundary.dbg_print(out, tab + 1);

   for (int i = 0; i < tab + 1; i++) out << "\t";
   out << "parent: " << (m_parent ? getInternedString(m_parent->getName()) : "NULL") << "\n";

   for (int i = 0; i < tab + 1; i++) out << "\t";
   out << "children: ";
   if (m_children.size() > 0) {
      out << "\n";
      for (set<pEntity_t>::const_iterator c = m_children.begin(); c != m_children.end(); ++c) {
         for (int i = 0; i < tab + 2; i++) out << "\t";
         out << getInternedString((*c)->getName()) << "\n";
      }
   }
   else
      out << "(none)\n";
}
#endif

//===========================================
// Entity::Entity
//===========================================
Entity::Entity(const XmlNode data)
   : m_silent(false) {

   if (data.isNull() || data.name() != "Entity")
      throw XmlException("Error parsing XML for instance of class Entity; Expected 'Entity' tag", __FILE__, __LINE__);

   AssetManager assetManager;

   XmlAttribute attr = data.firstAttribute();
   if (attr.isNull() || attr.name() != "type")
      throw XmlException("Error parsing XML for instance of class Entity; Expected 'type' attribute", __FILE__, __LINE__);

   m_type = internString(attr.value());
   attr = attr.nextAttribute();

   if (attr.isNull() || attr.name() != "name")
      throw XmlException("Error parsing XML for instance of class Entity; Expected 'name' attribute", __FILE__, __LINE__);

   m_name = internString(attr.value());
   attr = attr.nextAttribute();

   if (attr.isNull() || attr.name() != "x")
      throw XmlException("Error parsing XML for instance of class Entity; Expected 'x' attribute", __FILE__, __LINE__);

   sscanf(attr.value().data(), "%f", &m_transl.x);
   m_transl.x *= gGetPixelSize().x;

   attr = attr.nextAttribute();

   if (attr.isNull() || attr.name() != "y")
      throw XmlException("Error parsing XML for instance of class Entity; Expected 'y' attribute", __FILE__, __LINE__);

   sscanf(attr.value().data(), "%f", &m_transl.y);
   m_transl.y *= gGetPixelSize().y;

   attr = attr.nextAttribute();

   if (attr.isNull() || attr.name() != "z")
      throw XmlException("Error parsing XML for instance of class Entity; Expected 'z' attribute", __FILE__, __LINE__);

   sscanf(attr.value().data(), "%d", &m_z);
   attr = attr.nextAttribute();

   if (attr.isNull() || attr.name() != "rot")
      throw XmlException("Error parsing XML for instance of class Entity; Expected 'rot' attribute", __FILE__, __LINE__);

   sscanf(attr.value().data(), "%f", &m_rot);

   XmlNode node = data.firstChild();
   if (node.isNull() || node.name() != "scale")
      throw XmlException("Error parsing XML for instance of class Entity; Expected 'scale' tag", __FILE__, __LINE__);

   m_scale = Vec2f(node.firstChild());
   node = node.nextSibling();

   if (node.isNull() || node.name() != "shape")
      throw XmlException("Error parsing XML for instance of class Entity; Expected 'shape' tag", __FILE__, __LINE__);

//   m_shape = unique_ptr<Primitive>(primitiveFactory.create(node.firstChild()));
   node = node.nextSibling();

   if (node.isNull() || node.name() != "children")
      throw XmlException("Error parsing XML for instance of class Entity; Expected 'children' tag", __FILE__, __LINE__);

   XmlNode node_ = node.firstChild();

   while (!node_.isNull() && node_.name() == "child") {
      XmlAttribute attr = node_.firstAttribute();

      if (!attr.isNull() && attr.name() == "ptr") {
         long id = 0;
         sscanf(attr.value().data(), "%ld", &id);

         pEntity_t child = boost::dynamic_pointer_cast<Entity>(assetManager.getAssetPointer(id));

         if (!child)
            throw XmlException("Error parsing XML for instance of class Entity; Bad entity asset id", __FILE__, __LINE__);

         addChild(child);
      }

      node_ = node_.nextSibling();
   }

   ++m_count;
}

//===========================================
// Entity::Entity
//===========================================
Entity::Entity(long name, long type)
   : m_name(name), m_type(type), m_silent(false), m_scale(1.f, 1.f), m_transl(0.f, 0.f),
     m_z(1), m_rot(0.f), m_parent(NULL) {

   ++m_count;
}

//===========================================
// Entity::Entity
//===========================================
Entity::Entity(long type)
   : m_type(type), m_silent(false), m_scale(1.f, 1.f), m_transl(0.f, 0.f), m_z(1),
     m_rot(0.f), m_parent(NULL) {

   m_name = generateName();

   ++m_count;
}

//===========================================
// Entity::Entity
//
// Construct deep copy.
//===========================================
Entity::Entity(const Entity& copy) : m_parent(NULL) {
   deepCopy(copy);
   m_name = generateName();

   ++m_count;
}

//===========================================
// Entity::Entity
//
// Construct deep copy.
//===========================================
Entity::Entity(const Entity& copy, long name) : m_parent(NULL) {
   deepCopy(copy);
   m_name = name;

   ++m_count;
}

//===========================================
// Entity::deepCopy
//
// Copy's 'family' is not inherited
//===========================================
void Entity::deepCopy(const Entity& copy) {
   m_name = copy.m_name;
   m_type = copy.m_type;
   m_silent = copy.m_silent;
   m_scale = copy.m_scale;
   m_transl = copy.m_transl;
   m_z = copy.m_z;
   m_rot = copy.m_rot;

   m_shape = copy.m_shape ? unique_ptr<Primitive>(copy.m_shape->clone()) : unique_ptr<Primitive>();
   m_boundary = copy.m_boundary;
}

//===========================================
// Entity::generateName
//===========================================
long Entity::generateName() {
   stringstream str;
   str << "entity" << m_count;
   return internString(str.str());
}

//===========================================
// Entity::assignData
//
// 'data' contains partial or complete description of the object. This
// function extracts whatever data is available, but does not raise an
// error if there is missing data, as this is permitted.
//===========================================
void Entity::assignData(const XmlNode data) {
   if (data.isNull() || data.name() != "Entity") return;

   AssetManager assetManager;

   XmlAttribute attr = data.firstAttribute();
   if (!attr.isNull() && attr.name() == "type") {
      m_type = internString(attr.value());
      attr = attr.nextAttribute();
   }
   if (!attr.isNull() && attr.name() == "name") {
      m_name = internString(attr.value());
      attr = attr.nextAttribute();
   }
   if (!attr.isNull() && attr.name() == "x") {
      sscanf(attr.value().data(), "%f", &m_transl.x);
      m_transl.x *= gGetPixelSize().x;
      attr = attr.nextAttribute();
   }
   if (!attr.isNull() && attr.name() == "y") {
      sscanf(attr.value().data(), "%f", &m_transl.y);
      m_transl.y *= gGetPixelSize().y;
      attr = attr.nextAttribute();
   }
   if (!attr.isNull() && attr.name() == "z") {
      sscanf(attr.value().data(), "%d", &m_z);
      attr = attr.nextAttribute();
   }
   if (!attr.isNull() && attr.name() == "rot") {
      sscanf(attr.value().data(), "%f", &m_rot);
   }
   XmlNode node = data.firstChild();
   if (!node.isNull() && node.name() == "scale") {
      m_scale = Vec2f(node.firstChild());
      node = node.nextSibling();
   }
   if (!node.isNull() && node.name() == "shape") {
//      m_shape = unique_ptr<Primitive>(primitiveFactory.create(node.firstChild()));
      node = node.nextSibling();
   }
   if (!node.isNull() && node.name() == "children") {
      XmlNode node_ = node.firstChild();

      while (!node_.isNull() && node_.name() == "child") {
         XmlAttribute attr = node_.firstAttribute();

         if (!attr.isNull() && attr.name() == "ptr") {
            long id = 0;
            sscanf(attr.value().data(), "%ld", &id);

            pEntity_t child = boost::dynamic_pointer_cast<Entity>(assetManager.getAssetPointer(id));

            if (!child)
               throw XmlException("Error parsing XML for instance of class Entity; Bad entity asset id", __FILE__, __LINE__);

            addChild(child);
         }

         node_ = node_.nextSibling();
      }
   }
}

//===========================================
// Entity::setParent
//===========================================
void Entity::setParent(Entity* parent) {
   if (m_parent) m_parent->removeChild(shared_from_this());
   m_parent = parent;
   m_parent->m_children.insert(shared_from_this());

   Range bounds = m_boundary;
   recomputeBoundary();

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));

   // Children must dispatch EEntityMoved events too
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->parentMovedHandler();
}

//===========================================
// Entity::getTranslation_abs
//===========================================
Vec2f Entity::getTranslation_abs() const {
   if (m_parent) {
      float32_t x = m_transl.x * cos(DEG_TO_RAD(m_parent->getRotation_abs()))
         - m_transl.y * sin(DEG_TO_RAD(m_parent->getRotation_abs()));

      float32_t y = m_transl.x * sin(DEG_TO_RAD(m_parent->getRotation_abs()))
         + m_transl.y * cos(DEG_TO_RAD(m_parent->getRotation_abs()));

      Vec2f t = m_parent->getTranslation_abs();

      return t + Vec2f(x, y);
   }
   else {
      return getTranslation();
   }
}

//===========================================
// Entity::recomputeBoundary
//===========================================
void Entity::recomputeBoundary() {
   Vec2f min = m_shape ? m_shape->getMinimum() : Vec2f(0.f, 0.f);
   Vec2f max = m_shape ? m_shape->getMaximum() : Vec2f(0.f, 0.f);

   m_boundary.setPosition(getTranslation_abs() + min);
   m_boundary.setSize(max - min);
}

//===========================================
// Entity::setTranslation
//===========================================
void Entity::setTranslation(float32_t x, float32_t y) {
   Range bounds = m_boundary;

   Vec2f diff = Vec2f(x, y) - m_transl;
   m_transl = Vec2f(x, y);
   m_boundary.setPosition(m_boundary.getPosition() + diff);

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));

   // Children must dispatch EEntityMoved events too
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->parentMovedHandler();
}

//===========================================
// Entity::setTranslation
//===========================================
void Entity::setTranslation(const Vec2f& t) {
   Range bounds = m_boundary;

   Vec2f diff = t - m_transl;
   m_transl = t;
   m_boundary.setPosition(m_boundary.getPosition() + diff);

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));

   // Children must dispatch EEntityMoved events too
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->parentMovedHandler();
}

//===========================================
// Entity::setTranslation_x
//===========================================
void Entity::setTranslation_x(float32_t x) {
   Range bounds = m_boundary;

   float32_t dx = x - m_transl.x;
   m_transl.x = x;
   m_boundary.setPosition(m_boundary.getPosition() + Vec2f(dx, 0.f));

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));

   // Children must dispatch EEntityMoved events too
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->parentMovedHandler();
}

//===========================================
// Entity::setTranslation_y
//===========================================
void Entity::setTranslation_y(float32_t y) {
   Range bounds = m_boundary;

   float32_t dy = y - m_transl.y;
   m_transl.y = y;
   m_boundary.setPosition(m_boundary.getPosition() + Vec2f(0.f, dy));

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));

   // Children must dispatch EEntityMoved events too
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->parentMovedHandler();
}

//===========================================
// Entity::translate
//===========================================
void Entity::translate(float32_t x, float32_t y) {
   Range bounds = m_boundary;

   m_transl = m_transl + Vec2f(x, y);
   m_boundary.setPosition(m_boundary.getPosition() + Vec2f(x, y));

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));

   // Children must dispatch EEntityMoved events too
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->parentMovedHandler();
}

//===========================================
// Entity::translate_x
//===========================================
void Entity::translate_x(float32_t x) {
   Range bounds = m_boundary;

   m_transl.x += x;
   m_boundary.setPosition(m_boundary.getPosition() + Vec2f(x, 0.f));

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));

   // Children must dispatch EEntityMoved events too
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->parentMovedHandler();
}

//===========================================
// Entity::translate_y
//===========================================
void Entity::translate_y(float32_t y) {
   Range bounds = m_boundary;

   m_transl.y += y;
   m_boundary.setPosition(m_boundary.getPosition() + Vec2f(0.f, y));

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));

   // Children must dispatch EEntityMoved events too
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->parentMovedHandler();
}

//===========================================
// Entity::rotate
//
// Pivot is in model space
//===========================================
void Entity::rotate(float32_t deg, const Vec2f& pivot) {
   // - Find model's origin (bottom-left corner) in parent's model space (o)
   //    - This is just m_transl (despite any prior rotations)
   // - Find pivot in parent's model space (p)
   //    - p = (pivot rotated by m_rot) + o
   // - Compute value of o rotated about p by deg degrees
   // - Set m_transl to o
   // - Add deg to m_rot

   // Pivot in parent's model space (or world space if this is root)
   float32_t px = pivot.x * cos(DEG_TO_RAD(m_rot)) - pivot.y * sin(DEG_TO_RAD(m_rot));
   float32_t py = pivot.x * sin(DEG_TO_RAD(m_rot)) + pivot.y * cos(DEG_TO_RAD(m_rot));
   Vec2f p(px + m_transl.x, py + m_transl.y);

   // Rotate bottom-left corner around pivot
   Vec2f o = m_transl - p;
   o.x = o.x * cos(DEG_TO_RAD(deg)) - o.y * sin(DEG_TO_RAD(deg));
   o.y = o.x * sin(DEG_TO_RAD(deg)) + o.y * cos(DEG_TO_RAD(deg));

   m_transl.x = p.x + o.x;
   m_transl.y = p.y + o.y;

   m_rot += deg;

   if (m_shape) m_shape->rotate(deg);
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->m_shape->rotate(deg);

   Range bounds = m_boundary;
   recomputeBoundary();

   // Children must dispatch EEntityMoved events too
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->parentMovedHandler();

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));
}

//===========================================
// Entity::rotate
//===========================================
void Entity::rotate(float32_t deg) {
   Range bounds = m_boundary;

   if (m_shape) m_shape->rotate(deg);
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->m_shape->rotate(deg);

   m_rot += deg;
   recomputeBoundary();

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));

   // Children must dispatch EEntityMoved events too
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->parentMovedHandler();
}

//===========================================
// Entity::setShape
//===========================================
void Entity::setShape(std::unique_ptr<Primitive> shape) {
   Range bounds = m_boundary;

   m_shape = std::move(shape);
   if (m_shape) m_shape->rotate(m_rot);
   if (m_shape) m_shape->scale(m_scale);
   recomputeBoundary();

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));
}

//===========================================
// Entity::setScale
//===========================================
void Entity::setScale(float32_t x, float32_t y) {
   Range bounds = m_boundary;

   if (m_shape) m_shape->scale(Vec2f(x / m_scale.x, y / m_scale.y));
   m_scale.x = x;
   m_scale.y = y;
   recomputeBoundary();

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));
}

//===========================================
// Entity::setScale
//===========================================
void Entity::setScale(float32_t s) {
   Range bounds = m_boundary;

   if (m_shape) m_shape->scale(Vec2f(s / m_scale.x, s / m_scale.y));
   m_scale.x = s;
   m_scale.y = s;
   recomputeBoundary();

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));
}

//===========================================
// Entity::scale
//===========================================
void Entity::scale(float32_t x, float32_t y) {
   Range bounds = m_boundary;

   if (m_shape) m_shape->scale(Vec2f(x, y));
   m_scale.x *= x;
   m_scale.y *= y;
   recomputeBoundary();

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));
}

//===========================================
// Entity::scale
//===========================================
void Entity::scale(float32_t s) {
   Range bounds = m_boundary;

   if (m_shape) m_shape->scale(Vec2f(s, s));
   m_scale.x *= s;
   m_scale.y *= s;
   recomputeBoundary();

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));
}

//===========================================
// Entity::parentMovedHandler
//===========================================
void Entity::parentMovedHandler() {
   Range bounds = m_boundary;
   recomputeBoundary();

   if (!m_silent)
      m_eventManager.queueEvent(new EEntityMoved(shared_from_this(), bounds, m_boundary));
}

//===========================================
// Entity::~Entity
//===========================================
Entity::~Entity() {}


}
