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


#include "tia-private/newtia/commands/set_custom_signalinfo_control_command.h"
#include "tia-private/newtia/messages/tia_control_message_tags_1_0.h"
#include "tia-private/newtia/messages/standard_control_messages.h"

#include "tia-private/newtia/tia_custom_signal_info_parse_and_build_functions.h"

#include "tia-private/newtia/filter_impl/tia_customchannel_filterdecorator.h"
#include "tia-private/newtia/filter_impl/tia_downsampling_filterdecorator.h"

#include <iostream>
#include <stdexcept>

using std::string;

namespace tia
{

TiAControlMessage SetCustomSignalInfoControlCommand::execute(const TiAControlMessage &command)
{
    string version = command.getVersion();

    string content;

    if(command.getCommand() != TiAControlMessageTags10::SET_CUSTOM_SIGNALINFO)
        throw std::invalid_argument("Not able to handle this type of controlmessage!");

    CustomPacketFilter *packet_filter =
            connection_.getPacketFilter().get();

    if(packet_filter != NULL)
        return CustomErrorControlMessage(version,"Details:This connection has already a custom config that can not be altered!");

    content = command.getContent();

//    std::cout << "execute setCustomSignalInfo: " << std::endl << content << std::endl;

    const SSConfig &meta_info = command_context_.getHardwareInterface().getTiAMetaInfoAsConstRef();


    try
    {
        const SignalInfo &default_sig_info = meta_info.signal_info;

        const CustomSignalInfoPtr &custom_sig_info_ptr = parseTiACustomSignalInfoFromXMLString(content,default_sig_info);


        CustomPacketFilter *dummy_filter_ptr = new DummyCustomPacketFilter(default_sig_info,custom_sig_info_ptr);

        CustomPacketFilterPtr filter_chain_ptr =
                CustomPacketFilterPtr(dummy_filter_ptr);

        CustomPacketFilterPtr channel_filter_ptr =
                 CustomPacketFilterPtr(new CustomChannelFilterDecorator(filter_chain_ptr));

        if(channel_filter_ptr->hasConfiguredWork())
            filter_chain_ptr = channel_filter_ptr;
        else
            std::cout << "SetCustomSignalInfoControlCommand::NO channel filtering necessary.. " << std::endl;

        CustomPacketFilterPtr downsampling_filter_ptr =
                CustomPacketFilterPtr(new DownsamplingFilterDecorator(filter_chain_ptr));

        if(downsampling_filter_ptr->hasConfiguredWork())
            filter_chain_ptr = downsampling_filter_ptr;
        else
            std::cout << "SetCustomSignalInfoControlCommand::NO downsampling necessary.. " << std::endl;


        if(filter_chain_ptr.get() == dummy_filter_ptr)
            std::cout << "SetCustomSignalInfoControlCommand::NO customization necessary.." << std::endl;
        else
            connection_.setPacketFilter(filter_chain_ptr);

        return OkControlMessage(version);


    }catch(std::exception &ex)
    {
        return ValidationErrorControlMessage(version, ex.what());
    }
}

}
