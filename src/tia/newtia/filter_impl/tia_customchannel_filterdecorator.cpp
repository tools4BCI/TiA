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
    //TODO: extract appropriate data for later filtering and
    //      set is_applicalbe and has_configured_work_ flags!       

    SignalInfo::SignalMap default_signals = default_sig_info_.signals();
    SignalInfo::SignalMap custom_signals = custom_sig_info_.signals();

    Constants constants;

    std::cout << "Default channels: " << std::endl;

    std::vector<boost::uint32_t> signals_to_delete;
    std::map<boost::uint32_t, std::vector<boost::uint32_t> > channel_filter_map;

    BOOST_FOREACH(SignalInfo::SignalMap::value_type default_signal, default_signals)
    {
        SignalInfo::SignalMap::iterator custom_signal = custom_signals.find(default_signal.first);

        if(custom_signal == custom_signals.end())
        {
            std::cout << "Signal has no channels: "<< default_signal.first << std::endl;
            signals_to_delete.push_back(constants.getSignalFlag(default_signal.first));
        }
        else
        {
            std::vector<Channel> &default_channels = default_signal.second.channels();

            BOOST_FOREACH(Channel custom_channel, custom_signal->second.channels())
            {
                Channel &default_chan = default_channels[custom_channel.number() - 1];

                if(default_chan.number() != custom_channel.number()
                        || default_chan.id() != custom_channel.id())
                {
                    //custom signal info is invalid
                    has_configured_work_ = is_applicable_ = false;
                    std::cerr << "Signal " << default_signal.first << ": Inkonsistent channels!" << std::endl;
                    return;
                }
                else
                {
                    channel_filter_map[constants.getSignalFlag(default_signal.first)].push_back(custom_channel.number());
                    std::cout << "custom chan: " << custom_channel.number() << ";" << custom_channel.id() << std::endl;
                }
            }

            if(channel_filter_map[constants.getSignalFlag(default_signal.first)].size() == default_channels.size())
            {
                //there are all channels used for this signal
                channel_filter_map.erase(constants.getSignalFlag(default_signal.first));
                std::cout << "Signal " << default_signal.first << ": No channels to filter!" << std::endl;
            }
            else
            {
                std::cout << "Signal " << default_signal.first << ": Sorting channels!" << std::endl;
                std::sort(channel_filter_map[constants.getSignalFlag(default_signal.first)].begin(),channel_filter_map[constants.getSignalFlag(default_signal.first)].end());

            }
        }
    }

    is_applicable_ = true;

    has_configured_work_ = (signals_to_delete.size() > 0 || channel_filter_map.size() > 0);
}

//-----------------------------------------------------------------------------

void CustomChannelFilterDecorator::applyFilter(DataPacket &packet)
{
    decorated_filter_.applyFilter(packet);

    std::cout << " chan filter ";

    //TODO: filter the data in the packet!
}

}
