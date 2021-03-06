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

#ifndef GET_METAINFO_CONTROL_COMMAND_H
#define GET_METAINFO_CONTROL_COMMAND_H

#include "../tia_control_command.h"
#include "../tia_control_command_context.h"
#include "../server/data_server.h"
#include "../tia_meta_info_parse_and_build_functions.h"
#include "../server_impl/control_connection_2.h"


#include <iostream>

#include <boost/current_function.hpp>

namespace tia
{

//-----------------------------------------------------------------------------
class GetMetaInfoControlCommand : public TiAControlCommand
{
public:
    GetMetaInfoControlCommand (TiAControlCommandContext& command_context, ControlConnection2& connection)
        : command_context_ (command_context), connection_ (connection)
    {}

    virtual ~GetMetaInfoControlCommand () {}

    virtual TiAControlMessage execute (TiAControlMessage const& command)
    {

        #ifdef DEBUG
            std::cout << BOOST_CURRENT_FUNCTION << std::endl;
        #endif

        SSConfig default_meta_info = command_context_.getHardwareInterface().getTiAMetaInfo();

        CustomPacketFilter *packet_filter =
                connection_.getPacketFilter().get();

        #ifdef DEBUG
            std::cout << "Default meta info:" <<
                         buildTiAMetaInfoXMLString (default_meta_info) << std::endl;
        #endif


        if(packet_filter != NULL)
        {
            SignalInfo custom_sig_info = packet_filter->getSignalInfoAfterFiltering();

            default_meta_info.signal_info = custom_sig_info;

            #ifdef DEBUG
                std::cout << "Custom meta info:" <<
                             buildTiAMetaInfoXMLString (default_meta_info) << std::endl;
            #endif
        }

        std::string xml = buildTiAMetaInfoXMLString (default_meta_info);
        return TiAControlMessage (command.getVersion(), "MetaInfo", "", xml);
    }

private:
    TiAControlCommandContext& command_context_;
    ControlConnection2& connection_;
};

}

#endif // GET_METAINFO_CONTROL_COMMAND_H
