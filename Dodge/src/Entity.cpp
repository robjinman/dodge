/*
 * Author: Rob Jinman <admin@robjinman.com>
 * Date: 2012
 */

#include <sstream>
#include <cstdio>
#include <Entity.hpp>
#include <Exception.hpp>
#include <StringId.hpp>
#include <AssetManager.hpp>
#include <globals.hpp>
#include <ShapeFactory.hpp>


using namespace std;


namespace Dodge {


EventManager Entity::m_eventManager = EventManager();
int Entity::m_count = 0;


#ifdef DEBUG
//===========================================
// Entity::dbg_print
//===========================================
void Entity::dbg_print(std::ostream& out, int tab) const {
   for (int i = 0; i < tab; ++i) out << "\t";
   out << "Entity\n";

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "name: " << getInternedString(m_name) << "\n";

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "type: " << getInternedString(m_type) << "\n";

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "scale: (" << m_scale.x << ", " << m_scale.y << ")\n";

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "translation: (" << m_transl.x << ", " << m_transl.y << ")\n";

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "z: " << m_z << "\n";

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "rotation: " << m_rot << "\n";

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "shape (src):\n";
   if (m_shape) m_shape->dbg_print(out, tab + 1);

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "boundary:\n";
   m_boundary.dbg_print(out, tab + 1);

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "parent: " << (m_parent ? getInternedString(m_parent->getName()) : "NULL") << "\n";

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "children: ";
   if (m_children.size() > 0) {
      out << "\n";
      for (set<pEntity_t>::const_iterator c = m_children.begin(); c != m_children.end(); ++c) {
         for (int i = 0; i < tab + 2; ++i) out << "\t";
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
   : Asset(internString("Entity")),
     m_silent(false),
     m_parent(NULL) {

   AssetManager assetManager;
   ShapeFactory shapeFactory;

   try {
      setSilent(true);

      XML_NODE_CHECK(data, Entity);

      XmlAttribute attr = data.firstAttribute();
      XML_ATTR_CHECK(attr, type);
      m_type = internString(attr.getString());

      attr = attr.nextAttribute();
      XML_ATTR_CHECK(attr, name);
      m_name = internString(attr.getString());

      attr = attr.nextAttribute();
      XML_ATTR_CHECK(attr, x);
      m_transl.x = attr.getFloat();

      attr = attr.nextAttribute();
      XML_ATTR_CHECK(attr, y);
      m_transl.y = attr.getFloat();

      attr = attr.nextAttribute();
      XML_ATTR_CHECK(attr, z);
      m_z = attr.getFloat();

      // So that no Z values are 'exactly' equal
      m_z += 0.1f * static_cast<float32_t>(rand()) / static_cast<float32_t>(RAND_MAX);

      attr = attr.nextAttribute();
      XML_ATTR_CHECK(attr, rot);
      float32_t rot = attr.getFloat();

      XmlNode node = data.firstChild();
      if (!node.isNull() && node.name() == "shape") {
         m_shape = unique_ptr<Shape>(shapeFactory.create(node.firstChild()));
         node = node.nextSibling();
      }

      m_rot = 0;
      setRotation(rot);

      XML_NODE_CHECK(node, scale);
      m_scale = Vec2f(1.f, 1.f);
      setScale(Vec2f(node.firstChild()));

      node = node.nextSibling();
      XML_NODE_CHECK(node, fillColour);
      m_fillColour = Colour(node.firstChild());

      node = node.nextSibling();
      XML_NODE_CHECK(node, lineColour);
      m_lineColour = Colour(node.firstChild());

      node = node.nextSibling();
      XML_NODE_CHECK(node, lineWidth);
      m_lineWidth = node.getInt();

      node = node.nextSibling();
      XML_NODE_CHECK(node, children);
      XmlNode node_ = node.firstChild();
      while (!node_.isNull() && node_.name() == "child") {
         XmlAttribute attr = node_.firstAttribute();

         if (!attr.isNull() && attr.name() == "ptr") {
            long id = attr.getLong();

            pEntity_t child = boost::dynamic_pointer_cast<Entity>(assetManager.getAssetPointer(id));

            if (!child)
               throw XmlException("Bad entity asset id", __FILE__, __LINE__);

            addChild(child);
         }

         node_ = node_.nextSibling();
      }

      setSilent(false);

      ++m_count;
   }
   catch (XmlException& e) {
      e.prepend("Error parsing XML for instance of class Entity; ");
      throw;
   }

   recomputeBoundary();
}

//===========================================
// Entity::Entity
//===========================================
Entity::Entity(long name, long type)
   : Asset(internString("Entity")),
     m_name(name),
     m_type(type),
     m_silent(false),
     m_scale(1.f, 1.f),
     m_transl(0.f, 0.f),
     m_z(1),
     m_rot(0.f),
     m_lineWidth(0),
     m_parent(NULL) {

   // So that no Z values are 'exactly' equal
   m_z += 0.1f * static_cast<float32_t>(rand()) / static_cast<float32_t>(RAND_MAX);

   ++m_count;
}

//===========================================
// Entity::Entity
//===========================================
Entity::Entity(long type)
   : Asset(internString("Entity")),
     m_type(type),
     m_silent(false),
     m_scale(1.f, 1.f),
     m_transl(0.f, 0.f),
     m_z(1),
     m_rot(0.f),
     m_lineWidth(0),
     m_parent(NULL) {

   // So that no Z values are 'exactly' equal
   m_z += 0.1f * static_cast<float32_t>(rand()) / static_cast<float32_t>(RAND_MAX);

   m_name = generateName();

   ++m_count;
}

//===========================================
// Entity::Entity
//
// Construct deep copy.
//===========================================
Entity::Entity(const Entity& copy)
   : Asset(internString("Entity")),
     m_silent(false),
     m_parent(NULL) {

   deepCopy(copy);
   m_name = generateName();

   ++m_count;
}

//===========================================
// Entity::Entity
//
// Construct deep copy.
//===========================================
Entity::Entity(const Entity& copy, long name)
   : Asset(internString("Entity")),
     m_silent(false),
     m_parent(NULL) {

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
   m_scale = copy.m_scale;
   m_fillColour = copy.m_fillColour;
   m_lineColour = copy.m_lineColour;
   m_lineWidth = copy.m_lineWidth;
   m_transl = copy.m_transl;
   m_z = copy.m_z;
   m_rot = copy.m_rot;

   // So that no Z values are 'exactly' equal
   m_z += 0.1f * static_cast<float32_t>(rand()) / static_cast<float32_t>(RAND_MAX);

   if (copy.m_shape)
      m_shape = unique_ptr<Shape>(dynamic_cast<Shape*>(copy.m_shape->clone()));

   m_boundary = copy.m_boundary;

   if (copy.m_auxData)
      m_auxData = unique_ptr<IAuxData>(copy.m_auxData->clone());
}

//===========================================
// Entity::getSize
//===========================================
size_t Entity::getSize() const {
   return sizeof(Entity)
      + (m_shape ? m_shape->getSize() : 0)
      + m_children.size() * sizeof(pEntity_t)
      + (m_auxData ? m_auxData->getSize() : 0);
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
//===========================================
void Entity::assignData(const XmlNode data) {
   if (data.isNull() || data.name() != "Entity") return;

   AssetManager assetManager;
   ShapeFactory shapeFactory;

   try {
      bool silent = isSilent();
      setSilent(true);

      XmlAttribute attr = data.firstAttribute();
      if (!attr.isNull() && attr.name() == "type") {
         m_type = internString(attr.getString());
         attr = attr.nextAttribute();
      }

      if (!attr.isNull() && attr.name() == "name") {
         m_name = internString(attr.getString());
         attr = attr.nextAttribute();
      }

      Vec2f transl = m_transl;

      if (!attr.isNull() && attr.name() == "x") {
         transl.x = attr.getFloat();
         attr = attr.nextAttribute();
      }

      if (!attr.isNull() && attr.name() == "y") {
         transl.y = attr.getFloat();
         attr = attr.nextAttribute();
      }

      setTranslation(transl);

      if (!attr.isNull() && attr.name() == "z") {
         m_z = attr.getFloat();

         // So that no Z values are 'exactly' equal
         m_z += 0.1f * static_cast<float32_t>(rand()) / static_cast<float32_t>(RAND_MAX);

         attr = attr.nextAttribute();
      }

      XmlNode node = data.firstChild();
      if (!node.isNull() && node.name() == "shape") {
         m_shape = unique_ptr<Shape>(shapeFactory.create(node.firstChild()));
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "scale") {
         setScale(Vec2f(node.firstChild()));
         node = node.nextSibling();
      }

      if (!attr.isNull() && attr.name() == "rot") {
         setRotation(attr.getFloat());
      }

      if (!node.isNull() && node.name() == "fillColour") {
         m_fillColour = Colour(node.firstChild());
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "lineColour") {
         m_lineColour = Colour(node.firstChild());
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "lineWidth") {
         m_lineWidth = node.getInt();
         node = node.nextSibling();
      }

      if (!node.isNull() && node.name() == "children") {
         XmlNode node_ = node.firstChild();

         while (!node_.isNull() && node_.name() == "child") {
            XmlAttribute attr = node_.firstAttribute();

            if (!attr.isNull() && attr.name() == "ptr") {
               long id = attr.getLong();

               pEntity_t child = boost::dynamic_pointer_cast<Entity>(assetManager.getAssetPointer(id));

               if (!child)
                  throw XmlException("Bad entity asset id", __FILE__, __LINE__);

               addChild(child);
            }

            node_ = node_.nextSibling();
         }
      }

      setSilent(silent);
   }
   catch (XmlException& e) {
      e.prepend("Error parsing XML for instance of class Entity; ");
      throw;
   }

   recomputeBoundary();
}

//===========================================
// Entity::draw
//===========================================
void Entity::draw() const {
   if (m_shape) {
      Vec2f pos = getTranslation_abs();
      m_shape->setRenderTransform(pos.x, pos.y, m_z);

      m_shape->draw();
   }
}

//===========================================
// Entity::setParent
//===========================================
void Entity::setParent(Entity* parent) {
   Entity* oldParent = m_parent;

   if (m_parent) m_parent->removeChild(shared_from_this());
   m_parent = parent;
   m_parent->m_children.insert(shared_from_this());

   rotateShapes_r(m_parent->getRotation_abs());

   onParentTransformation(oldParent ? oldParent->getRotation_abs() : 0.f,
      oldParent ? oldParent->getTranslation_abs() : Vec2f(0.f, 0.f));

   Range bounds = m_boundary;
   recomputeBoundary();

   if (!m_silent) {
      EEvent* event1 = new EEntityBoundingBox(shared_from_this(), bounds, m_boundary);

      onEvent(event1);

      m_eventManager.queueEvent(event1);
   }
}

//===========================================
// Entity::addChild
//===========================================
void Entity::addChild(pEntity_t child) {
   child->setParent(this);
}

//===========================================
// Entity::removeChild
//===========================================
void Entity::removeChild(pEntity_t child) {
   assert(child->m_parent == this);

   child->rotateShapes_r(-getRotation_abs());

   child->m_parent = NULL;

   child->onParentTransformation(getRotation_abs(), getTranslation_abs());

   m_children.erase(child);
}

//===========================================
// Entity::setZ
//===========================================
void Entity::setZ(float32_t z) {
   m_z = z;

   // So that no Z values are 'exactly' equal
   m_z += 0.1f * static_cast<float32_t>(rand()) / static_cast<float32_t>(RAND_MAX);
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
   Vec2f max = m_shape ? m_shape->getMaximum() : Vec2f(0.001f, 0.001f);

   m_boundary.setPosition(getTranslation_abs() + min);
   m_boundary.setSize(max - min);
}

//===========================================
// Entity::translate
//===========================================
void Entity::translate(float32_t x, float32_t y) {
   Range bounds = m_boundary;

   float32_t oldRot = getRotation_abs();
   Vec2f oldTransl = getTranslation_abs();

   m_transl = m_transl + Vec2f(x, y);
   recomputeBoundary();

   if (!m_silent) {
      Vec2f t = getTranslation_abs();

      EEvent* event1 = new EEntityBoundingBox(shared_from_this(), bounds, m_boundary);
      EEvent* event2 = new EEntityTranslation(shared_from_this(), m_transl - Vec2f(x, y), oldTransl, m_transl, t);

      onEvent(event1);
      onEvent(event2);

      m_eventManager.queueEvent(event1);
      m_eventManager.queueEvent(event2);
   }

   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->onParentTransformation(oldRot, oldTransl);
}

//===========================================
// Entity::setTranslation_abs
//===========================================
void Entity::setTranslation_abs(float32_t x, float32_t y) {
   if (m_parent) {
      Vec2f r = Vec2f(x, y) - m_parent->getTranslation_abs();
      Vec2f s;

      s.x = r.x * cos(DEG_TO_RAD(-m_parent->getRotation_abs()))
         - r.y * sin(DEG_TO_RAD(-m_parent->getRotation_abs()));

      s.y = r.x * sin(DEG_TO_RAD(-m_parent->getRotation_abs()))
         + r.y * cos(DEG_TO_RAD(-m_parent->getRotation_abs()));

      setTranslation(s);
   }
   else {
      setTranslation(x, y);
   }
}

//===========================================
// Entity::setRotation_abs
//
// Pivot is in model space
//===========================================
void Entity::setRotation_abs(float32_t deg) {
   rotate(deg - getRotation_abs());
}

//===========================================
// Entity::rotate
//
// Pivot is in model space
//===========================================
void Entity::rotate(float32_t deg, const Vec2f& pivot) {
   float32_t oldRot = m_rot;
   float32_t oldRot_abs = getRotation_abs();
   Vec2f oldTransl = m_transl;
   Vec2f oldTransl_abs = getTranslation_abs();

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

   rotateShapes_r(deg);

   Range bounds = m_boundary;
   recomputeBoundary();

   Vec2f ds = m_transl - oldTransl;

   if (!m_silent) {
      EEvent* event1 = new EEntityBoundingBox(shared_from_this(), bounds, m_boundary);
      EEvent* event2 = new EEntityRotation(shared_from_this(), oldRot, oldRot_abs, m_rot, getRotation_abs());
      EEvent* event3 = new EEntityTranslation(shared_from_this(), oldTransl, oldTransl_abs, m_transl, oldTransl_abs + ds);

      onEvent(event1);
      onEvent(event2);
      onEvent(event3);

      m_eventManager.queueEvent(event1);
      m_eventManager.queueEvent(event2);
      m_eventManager.queueEvent(event3);
   }

   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->onParentTransformation(oldRot_abs, oldTransl_abs);
}

//===========================================
// Entity::rotateShapes_r
//===========================================
void Entity::rotateShapes_r(float32_t deg) {
   if (m_shape) m_shape->rotate(deg);
   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->rotateShapes_r(deg);
}

//===========================================
// Entity::onParentTransformation
//===========================================
void Entity::onParentTransformation(float32_t a, const Vec2f& s) {
   Range bounds = m_boundary;
   recomputeBoundary();

   float32_t x = m_transl.x * cos(DEG_TO_RAD(a))
      - m_transl.y * sin(DEG_TO_RAD(a));

   float32_t y = m_transl.x * sin(DEG_TO_RAD(a))
      + m_transl.y * cos(DEG_TO_RAD(a));

   Vec2f oldTransl(x, y);
   Vec2f oldTransl_abs = s + oldTransl;

   float32_t oldRot_abs = a + m_rot;

   if (!m_silent) {
      float32_t rot_abs = getRotation_abs();
      Vec2f transl_abs = getTranslation_abs();

      EEvent* event1 = new EEntityBoundingBox(shared_from_this(), bounds, m_boundary);
      EEvent* event2 = new EEntityRotation(shared_from_this(), m_rot, oldRot_abs, m_rot, rot_abs);
      EEvent* event3 = new EEntityTranslation(shared_from_this(), m_transl, oldTransl_abs, m_transl, transl_abs);

      onEvent(event1);
      onEvent(event2);
      onEvent(event3);

      m_eventManager.queueEvent(event1);
      m_eventManager.queueEvent(event2);
      m_eventManager.queueEvent(event3);
   }

   for (set<pEntity_t>::iterator i = m_children.begin(); i != m_children.end(); ++i)
      (*i)->onParentTransformation(oldRot_abs, oldTransl_abs);
}

//===========================================
// Entity::setShape
//===========================================
void Entity::setShape(std::unique_ptr<Shape> shape) {
   Range bounds = m_boundary;
   Shape* oldShape = m_shape ? dynamic_cast<Shape*>(m_shape->clone()) : NULL;
   float32_t oldRot_abs = getRotation_abs();

   m_shape = std::move(shape);

   if (m_shape) {
      m_shape->rotate(m_rot);
      m_shape->scale(m_scale);

      m_shape->setLineWidth(m_lineWidth);
      m_shape->setFillColour(m_fillColour);
      m_shape->setLineColour(m_lineColour);
   }

   recomputeBoundary();

   if (!m_silent) {
      EEvent* event1 = new EEntityBoundingBox(shared_from_this(), bounds, m_boundary);
      EEvent* event2 = new EEntityShape(shared_from_this(), pShape_t(oldShape), oldRot_abs, pShape_t(dynamic_cast<Shape*>(m_shape->clone())), getRotation_abs());

      onEvent(event1);
      onEvent(event2);

      m_eventManager.queueEvent(event1);
      m_eventManager.queueEvent(event2);
   }
}

//===========================================
// Entity::scale
//===========================================
void Entity::scale(float32_t x, float32_t y) {
   Range bounds = m_boundary;
   pShape_t oldShape = m_shape ? pShape_t(dynamic_cast<Shape*>(m_shape->clone())) : pShape_t();
   float32_t oldRot_abs = getRotation_abs();

   if (m_shape) m_shape->scale(Vec2f(x, y));
   m_scale.x *= x;
   m_scale.y *= y;
   recomputeBoundary();

   if (!m_silent) {
      EEvent* event1 = new EEntityBoundingBox(shared_from_this(), bounds, m_boundary);

      EEvent* event2 = new EEntityShape(shared_from_this(), oldShape, oldRot_abs,
         pShape_t(m_shape ? dynamic_cast<Shape*>(m_shape->clone()) : NULL), getRotation_abs());

      onEvent(event1);
      onEvent(event2);

      m_eventManager.queueEvent(event1);
      m_eventManager.queueEvent(event2);
   }
}

//===========================================
// Entity::setSilent
//===========================================
void Entity::setSilent(bool b) {
   m_silent = b;
}

//===========================================
// Entity::clone
//===========================================
Asset* Entity::clone() const {
   return new Entity(*this);
}

//===========================================
// Entity::setFillColour
//===========================================
void Entity::setFillColour(const Colour& colour) {
   m_fillColour = colour;
   if (m_shape) m_shape->setFillColour(colour);
}

//===========================================
// Entity::setLineColour
//===========================================
void Entity::setLineColour(const Colour& colour) {
   m_lineColour = colour;
   if (m_shape) m_shape->setLineColour(colour);
}

//===========================================
// Entity::setLineWidth
//===========================================
void Entity::setLineWidth(Renderer::int_t lineWidth) {
   m_lineWidth = lineWidth;
   if (m_shape) m_shape->setLineWidth(lineWidth);
}

//===========================================
// Entity::getFillColour
//===========================================
const Colour& Entity::getFillColour() const {
   return m_fillColour;
}

//===========================================
// Entity::getLineColour
//===========================================
const Colour& Entity::getLineColour() const {
   return m_lineColour;
}

//===========================================
// Entity::getLineWidth
//===========================================
Renderer::int_t Entity::getLineWidth() const {
   return m_lineWidth;
}

//===========================================
// Entity::~Entity
//===========================================
Entity::~Entity() {}


}
