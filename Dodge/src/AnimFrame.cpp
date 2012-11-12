#include <cstdlib>
#include <cstring>
#include <Exception.hpp>
#include <AnimFrame.hpp>
#include <PrimitiveFactory.hpp>


using namespace std;


namespace Dodge {


//===========================================
// AnimFrame::AnimFrame
//===========================================
AnimFrame::AnimFrame(const XmlNode data) {
   string msg("Error parsing XML for instance of class AnimFrame");

   XML_NODE_CHECK(msg, data, AnimFrame);

   XmlNode node = data.firstChild();

   XML_NODE_CHECK(msg, node, pos);

   pos = Vec2i(node.firstChild());
   node = node.nextSibling();

   XML_NODE_CHECK(msg, node, dim);

   dim = Vec2i(node.firstChild());
   node = node.nextSibling();

   XML_NODE_CHECK(msg, node, col);

   col = Colour(node.firstChild());
   node = node.nextSibling();

   if (!node.isNull() && node.name() == "shape") {     // TODO: PrimitiveDelta
      PrimitiveFactory primitiveFactory;
      shape = unique_ptr<Primitive>(primitiveFactory.create(node.firstChild()));
   }
}

//===========================================
// AnimFrame::AnimFrame
//===========================================
AnimFrame::AnimFrame(Vec2i pos_, Vec2i dim_, std::unique_ptr<Primitive> shape_, const Colour& col_)
   : pos(pos_), dim(dim_), col(col_) {

   shape = std::move(shape_);
}

//===========================================
// AnimFrame::AnimFrame
//===========================================
AnimFrame::AnimFrame(Vec2i pos_, Vec2i dim_, const Colour& col_)
   : pos(pos_), dim(dim_), col(col_) {}

//===========================================
// AnimFrame::AnimFrame
//===========================================
AnimFrame::AnimFrame(const AnimFrame& copy) {
   pos = copy.pos;
   dim = copy.dim;
   shape = copy.shape ? unique_ptr<Primitive>(copy.shape->clone()) : unique_ptr<Primitive>();
   col = copy.col;
}

#ifdef DEBUG
//===========================================
// AnimFrame::AnimFrame
//===========================================
void AnimFrame::dbg_print(std::ostream& out, int tab) const {
   for (int i = 0; i < tab; i++) out << "\t";
   out << "AnimFrame\n";

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "pos: (" << pos.x << ", " << pos.y << ")\n";

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "dim: (" << dim.x << ", " << dim.y << ")\n";

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "col: (" << col.r << ", " << col.g << ", " << col.b << ", " << col.a << ")\n";

   for (int i = 0; i < tab + 1; ++i) out << "\t";
   out << "shape:\n";
   if (shape) shape->dbg_print(out, tab + 1);
}
#endif

//===========================================
// AnimFrame::operator=
//===========================================
AnimFrame& AnimFrame::operator=(const AnimFrame& rhs) {
   pos = rhs.pos;
   dim = rhs.dim;
   shape = unique_ptr<Primitive>(rhs.shape->clone());
   col = rhs.col;

   return *this;
}


}
