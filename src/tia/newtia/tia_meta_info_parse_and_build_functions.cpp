/*
    This file is part of the TOBI Interface A (TiA) library.

    Commercial Usage
    Licensees holding valid Graz University of Technology Commercial
    licenses may use this file in accordance with the Graz University
    of Technology Commercial License Agreement provided with the
    Software or, alternatively, in accordance with the terms contained in
    a written agreement between you and Graz University of Technology.

    --------------------------------------------------

    GNU Lesser General Public License Usage
    Alternatively, this file may be used under the terms of the GNU Lesser
    General Public License version 3.0 as published by the Free Software
    Foundation and appearing in the file lgpl.txt included in the
    packaging of this file.  Please review the following information to
    ensure the GNU General Public License version 3.0 requirements will be
    met: http://www.gnu.org/copyleft/lgpl.html.

    In case of GNU Lesser General Public License Usage ,the TiA library
    is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the TiA library. If not, see <http://www.gnu.org/licenses/>.

    Copyright 2010 Graz University of Technology
    Contact: TiA@tobi-project.org
*/

#include "tia-private/newtia/tia_meta_info_parse_and_build_functions.h"
#include "tia-private/newtia/tia_parse_and_build_helper_functions.h"

#include "tia-private/newtia/tia_exceptions.h"

#include "tia-private/newtia/string_utils.h"
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>

#include <map>
#include <set>

using std::string;
using std::map;
using std::set;

namespace tia
{

//-----------------------------------------------------------------------------
SSConfig parseTiAMetaInfoFromXMLString (std::string const& tia_meta_info_xml_string)
{
    SSConfig tia_meta_info;
    rapidxml::xml_document<> xml_doc;
    try
    {
        xml_doc.parse<rapidxml::parse_non_destructive | rapidxml::parse_validate_closing_tags> ((char*)tia_meta_info_xml_string.c_str ());
    }
    catch (rapidxml::parse_error &error)
    {
        throw TiAException (string (error.what()));
    }

    rapidxml::xml_node<>* tia_metainfo_node = xml_doc.first_node ();
    if (tia_metainfo_node->next_sibling ())
        throw TiAException ("Parsing TiAMetaInfo String: Too many first level nodes.");

    // parse subject
    rapidxml::xml_node<>* subject_node = tia_metainfo_node->first_node (XML_TAGS::SUBJECT.c_str());
    if (subject_node)
    {
        std::map<string, string> attributes = getAttributes (subject_node);
        if (attributes.count (XML_TAGS::SUBJECT_ID))
            tia_meta_info.subject_info.setId (attributes.at(XML_TAGS::SUBJECT_ID));
        if (attributes.count (XML_TAGS::SUBJECT_FIRSTNAME))
            tia_meta_info.subject_info.setFirstName (attributes.at(XML_TAGS::SUBJECT_FIRSTNAME));
        if (attributes.count (XML_TAGS::SUBJECT_SURNAME))
            tia_meta_info.subject_info.setSurname (attributes.at(XML_TAGS::SUBJECT_SURNAME));
    }

    // parse master signal info
    rapidxml::xml_node<>* master_signal_node = 0;
    master_signal_node = tia_metainfo_node->first_node (XML_TAGS::MASTER_SIGNAL.c_str());
    if (master_signal_node)
    {
        std::map<string, string> attributes = getAttributes (master_signal_node);
        if (attributes.count (XML_TAGS::SIGNAL_SAMPLINGRATE))
            tia_meta_info.signal_info.setMasterSamplingRate (toUnsigned (attributes.at(XML_TAGS::SIGNAL_SAMPLINGRATE)));
        if (attributes.count (XML_TAGS::SIGNAL_BLOCKSIZE))
            tia_meta_info.signal_info.setMasterBlockSize (toUnsigned (attributes.at(XML_TAGS::SIGNAL_BLOCKSIZE)));
    }    

    // parse signals
    rapidxml::xml_node<>* signal_node = 0;
    signal_node = tia_metainfo_node->first_node (XML_TAGS::SIGNAL.c_str());
    SignalInfo::SignalMap& signal_map = tia_meta_info.signal_info.signals ();
    while (signal_node)
    {
        Signal signal;
        map<string, string> signal_attributes = getAttributes (signal_node, XML_TAGS::SIGNAL_REQUIRED_ATTRIBUTES);
        signal.setType (signal_attributes[XML_TAGS::SIGNAL_TYPE]);
        signal.setSamplingRate (toUnsigned (signal_attributes[XML_TAGS::SIGNAL_SAMPLINGRATE]));
        signal.setBlockSize (toUnsigned (signal_attributes[XML_TAGS::SIGNAL_BLOCKSIZE]));

        unsigned const num_channels = toUnsigned (signal_attributes[XML_TAGS::SIGNAL_NUMCHANNELS]);

        // parse channels
        std::vector<Channel>& channel_vector = signal.channels();
        for (unsigned channel_nr = 1; channel_nr <= num_channels; ++channel_nr)
        {
            Channel channel;
            channel.setId (toString (channel_nr));
            channel.setNumber(channel_nr);
            channel_vector.push_back (channel);
        }

        rapidxml::xml_node<>* channel_node = signal_node->first_node (XML_TAGS::CHANNEL.c_str());

        while (channel_node)
        {
            map<string, string> channel_attributes = getAttributes (channel_node, XML_TAGS::CHANNEL_REQUIRED_ATTRIBUTES);
            unsigned channel_nr = toUnsigned (channel_attributes[XML_TAGS::CHANNEL_NR]);

            if (channel_nr > num_channels)
                throw TiAException ("Parse TiAMetaInfo: nr-attribute of channel exceeds numChannels attribute of signal!");

            channel_vector[channel_nr - 1].setNumber (channel_nr);
            channel_vector[channel_nr - 1].setId (channel_attributes[XML_TAGS::CHANNEL_LABEL]);

            channel_node = channel_node->next_sibling (XML_TAGS::CHANNEL.c_str ());
        }

        signal_map[signal_attributes[XML_TAGS::SIGNAL_TYPE]] = signal;
        signal_node = signal_node->next_sibling (XML_TAGS::SIGNAL.c_str ());
    }

    return tia_meta_info;
}

//-----------------------------------------------------------------------------
std::string buildTiAMetaInfoXMLString (SSConfig const& tia_meta_info)
{
    rapidxml::xml_document<> xml_doc;

    char *tia_metainfo_node_name = xml_doc.allocate_string (XML_TAGS::TIA_META_INFO.c_str ());
    rapidxml::xml_node<>* tia_metainfo_node = xml_doc.allocate_node (rapidxml::node_element, tia_metainfo_node_name);

    addAttribute (&xml_doc, tia_metainfo_node,
                  XML_TAGS::TIA_META_INFO_VERSION, XML_TAGS::TIA_META_INFO_CURRENT_VERSION);

    // subject node
    char *subject_node_name = xml_doc.allocate_string (XML_TAGS::SUBJECT.c_str ());
    rapidxml::xml_node<>* subject_node = xml_doc.allocate_node (rapidxml::node_element, subject_node_name);
    addAttribute (&xml_doc, subject_node, XML_TAGS::SUBJECT_ID, tia_meta_info.subject_info.id());
    addAttribute (&xml_doc, subject_node, XML_TAGS::SUBJECT_FIRSTNAME, tia_meta_info.subject_info.firstName());
    addAttribute (&xml_doc, subject_node, XML_TAGS::SUBJECT_SURNAME, tia_meta_info.subject_info.surname());
    // TODO: add further attributes
    tia_metainfo_node->append_node (subject_node);


    // master signal data
    char *master_signal_node_name = xml_doc.allocate_string (XML_TAGS::MASTER_SIGNAL.c_str ());
    rapidxml::xml_node<>* master_signal_node = xml_doc.allocate_node (rapidxml::node_element, master_signal_node_name);
    addAttribute (&xml_doc, master_signal_node, XML_TAGS::SIGNAL_SAMPLINGRATE, tia_meta_info.signal_info.masterSamplingRate());
    addAttribute (&xml_doc, master_signal_node, XML_TAGS::SIGNAL_BLOCKSIZE, tia_meta_info.signal_info.masterBlockSize());
    tia_metainfo_node->append_node (master_signal_node);


    // signals
    for (SignalInfo::SignalMap::const_iterator signal_iter = tia_meta_info.signal_info.signals().begin ();
         signal_iter != tia_meta_info.signal_info.signals ().end (); ++signal_iter)
    {
        char *signal_node_name = xml_doc.allocate_string (XML_TAGS::SIGNAL.c_str ());
        rapidxml::xml_node<>* signal_node = xml_doc.allocate_node (rapidxml::node_element, signal_node_name);
        addAttribute (&xml_doc, signal_node, XML_TAGS::SIGNAL_TYPE, signal_iter->second.type ());
        addAttribute (&xml_doc, signal_node, XML_TAGS::SIGNAL_SAMPLINGRATE, signal_iter->second.samplingRate());
        addAttribute (&xml_doc, signal_node, XML_TAGS::SIGNAL_BLOCKSIZE, signal_iter->second.blockSize());
        addAttribute (&xml_doc, signal_node, XML_TAGS::SIGNAL_NUMCHANNELS, signal_iter->second.channels().size ());

        for (unsigned channel_nr = 0; channel_nr < signal_iter->second.channels().size (); channel_nr++)
        {
            char *channel_node_name = xml_doc.allocate_string (XML_TAGS::CHANNEL.c_str ());
            rapidxml::xml_node<>* channel_node = xml_doc.allocate_node (rapidxml::node_element, channel_node_name);
            addAttribute (&xml_doc, channel_node, XML_TAGS::CHANNEL_NR, signal_iter->second.channels ()[channel_nr].number());
            addAttribute (&xml_doc, channel_node, XML_TAGS::CHANNEL_LABEL, signal_iter->second.channels ()[channel_nr].id());
            signal_node->append_node (channel_node);
        }

        tia_metainfo_node->append_node (signal_node);
    }


    xml_doc.append_node (tia_metainfo_node);

    string xml_string;
    rapidxml::print (std::back_inserter (xml_string), xml_doc, 0);
    return xml_string;
}

}
