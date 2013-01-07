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


#include "tia-private/newtia/filter_impl/tia_customchannel_filterdecorator.h"

#include "tia/constants.h"

#include <iostream>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/cast.hpp>
#include <boost/current_function.hpp>

namespace tia
{

CustomChannelFilterDecorator::CustomChannelFilterDecorator(CustomPacketFilterPtr decorated_filter)
    : CustomPacketFilterDecorator(decorated_filter)
{  

    #ifdef DEBUG
        std::cout << BOOST_CURRENT_FUNCTION << std::endl;
    #endif


    CustomSignalInfo::CustomSignalMap custom_signals = custom_sig_info_ptr_->signals();

    modified_signal_info_ = default_sig_info_;

    for(SignalInfo::SignalMap::iterator mod_signal_it = modified_signal_info_.signals().begin();
        mod_signal_it != modified_signal_info_.signals().end();)
    {

        CustomSignalInfo::CustomSignalMap::iterator custom_signal = custom_signals.find(mod_signal_it->first);

        if(custom_signal == custom_signals.end())
        {
//            std::cout << "Signal " << mod_signal_it->first << ": is not present in custom config!" << std::endl;
            signals_to_exclude_.push_back(constants_.getSignalFlag(mod_signal_it->first));

            SignalInfo::SignalMap::iterator helper = mod_signal_it;

            ++mod_signal_it;
            modified_signal_info_.signals().erase(helper);

        }
        else
        {
            std::vector<Channel> &channels = mod_signal_it->second.channels();

            for(std::vector<Channel>::iterator channel_it = channels.begin();
                channel_it != channels.end();)
            {
                bool chan_is_in_custom_chans = false;

                BOOST_FOREACH(CustomChannel custom_channel, custom_signal->second.channels())
                {
                    if(channel_it->number() == custom_channel.number())
                    {
//                        std::cout << "found channel: " << channel_it->id() << " in custom config" << std::endl;
                        if(channel_it->id() != custom_channel.id())
                        {
                            //custom signal info is invalid
                            has_configured_work_ = false;
                            std::cerr << "Signal " << mod_signal_it->first << ": Inkonsistent channels!" << std::endl;

                            return;
                        }
                        chan_is_in_custom_chans = true;
                        break;
                    }
                }

                if(chan_is_in_custom_chans)
                    ++channel_it;
                else
                {
                    channels_to_exclude_[constants_.getSignalFlag(mod_signal_it->first)].push_back(channel_it->number() - 1);
                    channel_it = channels.erase(channel_it);
                }
            }

            if(channels_to_exclude_[constants_.getSignalFlag(mod_signal_it->first)].size() == 0)
            {
                //there are all channels used for this signal
                channels_to_exclude_.erase(constants_.getSignalFlag(mod_signal_it->first));
//                std::cout << "Signal " << mod_signal_it->first << ": No channels to filter!" << std::endl;
            }
            else
            {
//                std::cout << "Signal " << mod_signal_it->first << ": Sorting channels!" << std::endl;
                std::sort(channels_to_exclude_[constants_.getSignalFlag(mod_signal_it->first)].begin(),channels_to_exclude_[constants_.getSignalFlag(mod_signal_it->first)].end(), std::greater<boost::uint32_t>());
            }

            ++mod_signal_it;
        }
    }

    has_configured_work_ = (signals_to_exclude_.size() > 0 || channels_to_exclude_.size() > 0);

    //reset channel numbers of modified meta info
    for(SignalInfo::SignalMap::iterator mod_signal_it = modified_signal_info_.signals().begin();
        mod_signal_it != modified_signal_info_.signals().end();++mod_signal_it)
    {
        std::vector<Channel> &channels = mod_signal_it->second.channels();

        boost::uint32_t new_chan_number = 1;

        for(std::vector<Channel>::iterator channel_it = channels.begin();
            channel_it != channels.end();++channel_it)
        {
            channel_it->setNumber(new_chan_number);
            ++new_chan_number;
        }
    }
}

//-----------------------------------------------------------------------------

void CustomChannelFilterDecorator::applyFilter(DataPacket &packet)
{
    decorated_filter_->applyFilter(packet);

    BOOST_FOREACH(boost::uint32_t signal_flag, signals_to_exclude_)
    {
//        std::cout << " remove signal:" << constants_.getSignalName(signal_flag) << std::endl;
        if(packet.hasFlag(signal_flag))
            packet.removeDataBlock(signal_flag);
    }

    BOOST_FOREACH(ChannelNrMap::value_type signal, channels_to_exclude_)
    {

        BOOST_FOREACH(boost::uint32_t channel, signal.second)
        {
//            std::cout << " remove channel: "<< channel <<" signal:";
//            std::cout << constants_.getSignalName(signal.first) << std::endl;
            if(packet.hasFlag(signal.first))
                packet.removeSamples(signal.first,channel);
        }

    }

}

//-----------------------------------------------------------------------------

const SignalInfo &CustomChannelFilterDecorator::getSignalInfoAfterFiltering()
{
    return modified_signal_info_;
}

}
