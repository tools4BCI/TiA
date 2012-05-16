#include "tia-private/newtia/tia_parse_and_build_helper_functions.h"

#include "tia-private/newtia/tia_exceptions.h"

#include <sstream>

using std::string;
using std::map;
using std::set;

using namespace std;


namespace tia
{
//-----------------------------------------------------------------------------
void addAttribute (rapidxml::xml_document<>* doc, rapidxml::xml_node<>* node, std::string const& attribute_name, std::string const& attribute_value)
{
    if (attribute_value.size())
    {
        char *attribute_name_rx = doc->allocate_string (attribute_name.c_str ());
        char *attribute_value_rx = doc->allocate_string (attribute_value.c_str());

        rapidxml::xml_attribute<>* attribute = doc->allocate_attribute (attribute_name_rx, attribute_value_rx, attribute_name.size (), attribute_value.size());
        node->append_attribute (attribute);
    }
}

//-----------------------------------------------------------------------------
void addAttribute (rapidxml::xml_document<>* doc, rapidxml::xml_node<>* node, std::string const& attribute_name, unsigned attribute_value)
{
    std::ostringstream oss;
    oss << std::dec << attribute_value;
    addAttribute (doc, node, attribute_name, oss.str ());
}

//-----------------------------------------------------------------------------
map<string, string> getAttributes (rapidxml::xml_node<>* node)
{
    set<string> no_required_attributes;
    return getAttributes (node, no_required_attributes);
}

//-----------------------------------------------------------------------------
map<std::string, std::string> getAttributes (rapidxml::xml_node<>* node, set<string> required_attributes)
{
    std::map<string, string> attributes;
    if (!node)
        return attributes;
    rapidxml::xml_attribute<>* attribute = node->first_attribute ();
    while (attribute)
    {
        string key (attribute->name (), attribute->name_size ());
        required_attributes.erase (key);
        string value (attribute->value (), attribute->value_size ());
        attributes[key] = value;
        attribute = attribute->next_attribute ();
    }

    if (required_attributes.size ())
    {
        string missing_attributes;
        for (set<string>::const_iterator iter = required_attributes.begin (); iter != required_attributes.end (); )
        {
            missing_attributes += *iter;
            if (++iter != required_attributes.end ())
                missing_attributes += ", ";
        }
        throw TiAException (string ("Parse TiAMetaInfo: Required attributes missing: ") + missing_attributes);
    }
    return attributes;
}

}
