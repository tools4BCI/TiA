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

#ifndef TIA_META_INFO_PARSE_AND_BUILD_FUNCTIONS_H
#define TIA_META_INFO_PARSE_AND_BUILD_FUNCTIONS_H

#include "tia/ssconfig.h"

#include <set>

namespace tia
{

namespace XML_TAGS
{
    std::string const TIA_META_INFO = "tiaMetaInfo";
    std::string const TIA_META_INFO_VERSION = "version";
    std::string const TIA_META_INFO_CURRENT_VERSION = "1.0";

    std::string const SUBJECT = "subject";
    std::string const SUBJECT_ID = "id";
    std::string const SUBJECT_FIRSTNAME = "firstName";
    std::string const SUBJECT_SURNAME = "surname";

    std::string const MASTER_SIGNAL = "masterSignal";
    std::string const SIGNAL = "signal";
    std::string const SIGNAL_TYPE = "type";
    std::string const SIGNAL_SAMPLINGRATE = "samplingRate";
    std::string const SIGNAL_NUMCHANNELS = "numChannels";
    std::string const SIGNAL_BLOCKSIZE = "blockSize";    
    std::string const SIGNAL_REQUIRED_ATTRIBUTES_ARRAY[] = {SIGNAL_TYPE, SIGNAL_SAMPLINGRATE, SIGNAL_BLOCKSIZE, SIGNAL_NUMCHANNELS};
    std::set<std::string> const SIGNAL_REQUIRED_ATTRIBUTES (SIGNAL_REQUIRED_ATTRIBUTES_ARRAY, SIGNAL_REQUIRED_ATTRIBUTES_ARRAY + 4);

    std::string const CHANNEL = "channel";
    std::string const CHANNEL_NR = "nr";
    std::string const CHANNEL_LABEL = "label";
    std::string const CHANNEL_REQUIRED_ATTRIBUTES_ARRAY[] = {CHANNEL_NR, CHANNEL_LABEL};
    std::set<std::string> const CHANNEL_REQUIRED_ATTRIBUTES (CHANNEL_REQUIRED_ATTRIBUTES_ARRAY, CHANNEL_REQUIRED_ATTRIBUTES_ARRAY + 2);
}

//-----------------------------------------------------------------------------
SSConfig parseTiAMetaInfoFromXMLString (std::string const& tia_meta_info_xml_string);

//-----------------------------------------------------------------------------
std::string buildTiAMetaInfoXMLString (SSConfig const& tia_meta_info);

}

#endif // TIA_META_INFO_PARSE_AND_BUILD_FUNCTIONS_H
