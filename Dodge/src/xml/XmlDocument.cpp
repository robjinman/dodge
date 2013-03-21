/*
 * Author: Rob Jinman <admin@robjinman.com>
 * Date: 2012
 */

#include <memory>
#include <fstream>
#include <sstream>
#include <xml/xml.hpp>


using namespace std;
using namespace rapidxml;


namespace Dodge {


//===========================================
// XmlDocument::parse
//===========================================
XmlNode XmlDocument::parse(const string& file) {
   ifstream fin(file);
   if (!fin.good())
      throw Exception("Error parsing XML file; Bad file (" + file + ")", __FILE__, __LINE__);

   // Get file length
   fin.seekg (0, ios::end);
   long len = static_cast<long>(fin.tellg());
   fin.seekg (0, ios::beg);

   // Load data and add null byte
   m_data = new char[len + 1];
   m_data[len] = '\0';
   fin.read(m_data, len);

   try {
      m_doc.parse<parse_full ^ parse_comment_nodes>(m_data);
      fin.close();
      m_file = file;

      return XmlNode(m_doc.first_node(), file);
   }
   catch (parse_error& e) {
      fin.close();

      stringstream msg;
      msg << "Error parsing XML file (" << file << "): " << e.what() << " at byte "
         << e.where<char>() - m_data << " '" << string(e.where<char>(), 50) << "'";

      throw XmlException(msg.str(), __FILE__, __LINE__);
   }
}

//===========================================
// XmlDocument::firstNode
//===========================================
XmlNode XmlDocument::firstNode() {
   return isNull() ? XmlNode() : XmlNode(m_doc.first_node(), m_file);
}

//===========================================
// XmlDocument::XmlDocument
//===========================================
XmlDocument::~XmlDocument() {
   delete[] m_data;
}


}
