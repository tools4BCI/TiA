#include "tia-private/newtia/filter_impl/tia_customchannel_filterdecorator.h"

#include "tia/constants.h"

#include <iostream>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/cast.hpp>

namespace tia
{

CustomChannelFilterDecorator::CustomChannelFilterDecorator(CustomPacketFilter &decorated_filter)
    : CustomPacketFilterDecorator(decorated_filter)
{  
    SignalInfo::SignalMap custom_signals = custom_sig_info_.signals();

    modified_signal_info_ = default_sig_info_;

    for(SignalInfo::SignalMap::iterator mod_signal_it = modified_signal_info_.signals().begin();
        mod_signal_it != modified_signal_info_.signals().end(); ++mod_signal_it)
    {

        SignalInfo::SignalMap::iterator custom_signal = custom_signals.find(mod_signal_it->first);

        if(custom_signal == custom_signals.end())
        {
//            std::cout << "Signal " << mod_signal_it->first << ": is not present in custom config!" << std::endl;
            signals_to_exclude_.push_back(constants_.getSignalFlag(mod_signal_it->first));
            modified_signal_info_.signals().erase(mod_signal_it);
        }
        else
        {
            std::vector<Channel> &channels = mod_signal_it->second.channels();

            for(std::vector<Channel>::iterator channel_it = channels.begin();
                channel_it != channels.end();)
            {

                bool chan_is_in_custom_chans = false;

                BOOST_FOREACH(Channel custom_channel, custom_signal->second.channels())
                {
                    if(channel_it->number() == custom_channel.number())
                    {
//                        std::cout << "found channel: " << channel_it->id() << " in custom config" << std::endl;
                        if(channel_it->id() != custom_channel.id())
                        {
                            //custom signal info is invalid
                            has_configured_work_ = is_applicable_ = false;
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
                    channels_to_exclude_[constants_.getSignalFlag(mod_signal_it->first)].push_back(channel_it->number());
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
        }
    }

    is_applicable_ = true;

    has_configured_work_ = (signals_to_exclude_.size() > 0 || channels_to_exclude_.size() > 0);
}

//-----------------------------------------------------------------------------

void CustomChannelFilterDecorator::applyFilter(DataPacket &packet)
{
    decorated_filter_.applyFilter(packet);    

    BOOST_FOREACH(boost::uint32_t signal_flag, signals_to_exclude_)
    {
//        std::cout << " remove signal:" << constants_.getSignalName(signal_flag) << std::endl;
        packet.removeDataBlock(signal_flag);
    }

    BOOST_FOREACH(ChannelNrMap::value_type signal, channels_to_exclude_)
    {

        BOOST_FOREACH(boost::uint32_t channel, signal.second)
        {
//            std::cout << " remove channel: "<< channel <<" signal:"
//            std::cout << constants_.getSignalName(signal.first) << std::endl;
            packet.removeSamples(signal.first,channel - 1);
        }

    }

}

//-----------------------------------------------------------------------------

const SignalInfo &CustomChannelFilterDecorator::getSignalInfoAfterFiltering()
{
    return modified_signal_info_;
}

}
