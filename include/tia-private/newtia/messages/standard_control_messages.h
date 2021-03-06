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

#ifndef STANDARD_CONTROL_MESSAGES_H
#define STANDARD_CONTROL_MESSAGES_H

#include "../messages/tia_control_message.h"
#include "../messages/tia_control_message_tags_1_0.h"

#include "../../../tia/ss_meta_info.h"

#include "../../newtia/tia_custom_signal_info_parse_and_build_functions.h"

namespace tia
{

//-----------------------------------------------------------------------------
class OkControlMessage : public TiAControlMessage
{
public:
    OkControlMessage (std::string const& version) : TiAControlMessage (version, "OK", "", "")
    {}
    virtual ~OkControlMessage () {}
};

//-----------------------------------------------------------------------------
class ErrorControlMessage : public TiAControlMessage
{
public:
    ErrorControlMessage (std::string const& version) : TiAControlMessage (version, "Error", "", "")
    {}
    virtual ~ErrorControlMessage () {}
};

//-----------------------------------------------------------------------------
class CustomErrorControlMessage : public TiAControlMessage
{
public:
    CustomErrorControlMessage (std::string const& version,
                               std::string const& text)
      : TiAControlMessage (version, "Error -- " + text, "", "")
    {}
    virtual ~CustomErrorControlMessage () {}
};

//-----------------------------------------------------------------------------
class ValidationErrorControlMessage : public TiAControlMessage
{
public:
    ValidationErrorControlMessage (std::string const& version,
                               std::string const& description)
        : TiAControlMessage (version, "ValidationError","",description)
    {}
    virtual ~ValidationErrorControlMessage () {}
};

//-----------------------------------------------------------------------------
class CheckProtocolVersionTiAControlMessage : public TiAControlMessage
{
public:
    CheckProtocolVersionTiAControlMessage (std::string const& version) : TiAControlMessage (version, TiAControlMessageTags10::CHECK_PROTOCOL_VERSION, "", "")
    {}
    virtual ~CheckProtocolVersionTiAControlMessage () {}
};

//-----------------------------------------------------------------------------
class GetMetaInfoTiAControlMessage : public TiAControlMessage
{
public:
    GetMetaInfoTiAControlMessage (std::string const& version) : TiAControlMessage (version, TiAControlMessageTags10::GET_METAINFO, "", "")
    {}
    virtual ~GetMetaInfoTiAControlMessage () {}
};

//-----------------------------------------------------------------------------
class SetCustomSignalInfo : public TiAControlMessage
{
public:
    SetCustomSignalInfo (std::string const& version, CustomSignalInfoPtr custom_sig_info)
        : TiAControlMessage (version, TiAControlMessageTags10::SET_CUSTOM_SIGNALINFO, "" , buildTiACustomSignalInfoXMLString(custom_sig_info) )
    {}
    virtual ~SetCustomSignalInfo () {}
};

//-----------------------------------------------------------------------------
class GetDataConnectionTiAControlMessage : public TiAControlMessage
{
public:
    GetDataConnectionTiAControlMessage (std::string const& version, bool udp)
        : TiAControlMessage (version, TiAControlMessageTags10::GET_DATA_CONNECTION, udp ? TiAControlMessageTags10::UDP : TiAControlMessageTags10::TCP, "")
    {}
    virtual ~GetDataConnectionTiAControlMessage () {}
};


}

#endif // STANDARD_CONTROL_MESSAGES_H
