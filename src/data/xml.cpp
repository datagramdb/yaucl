//
// Created by giacomo on 25/10/21.
//

#include <yaucl/data/xml.h>

#include <istream>
#include <fstream>



rapidxml::xml_node<> *
yaucl::data::init_XML_parse(const std::string &filename, const std::string &root_label, rapidxml::xml_document<> &doc, std::vector<char>& x) {
    std::ifstream file{filename};
    // Read the xml file into a vector
    {
        std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        buffer.push_back('\0');
        x = buffer;
    }
    // Parse the buffer using the xml file parsing library into doc
    doc.parse<0>(x.data());
    rapidxml::xml_node<> * root_node = doc.first_node(root_label.c_str(), 0, false);
    return root_node;
}