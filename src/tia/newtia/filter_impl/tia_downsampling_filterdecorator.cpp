#include "tia-private/newtia/filter_impl/tia_downsampling_filterdecorator.h"

#include <boost/foreach.hpp>
#include <boost/cast.hpp>
#include <string>
#include <sstream>

#include "tia-private/newtia/tia_exceptions.h"
#include "tia/constants.h"

namespace tia
{

DownsamplingFilterDecorator::DownsamplingFilterDecorator(CustomPacketFilterPtr decorated_filter)
    : CustomPacketFilterDecorator(decorated_filter)
{
    SignalInfo::SignalMap custom_signals = custom_sig_info_ptr_->signals();

    modified_signal_info_ = default_sig_info_;


    for(SignalInfo::SignalMap::iterator mod_signal_it = modified_signal_info_.signals().begin();
        mod_signal_it != modified_signal_info_.signals().end();)
    {

        SignalInfo::SignalMap::iterator custom_signal = custom_signals.find(mod_signal_it->first);

        if(custom_signal == custom_signals.end())
        {
            std::cout << "Signal " << mod_signal_it->first << " is not present in custom config!" << std::endl;
            ++mod_signal_it;
        }
        else
        {
            double signal_fs = mod_signal_it->second.samplingRate();
            double rational_ds_factor = signal_fs / ((double)custom_signal->second.samplingRate());

            if((rational_ds_factor - ((boost::uint16_t)rational_ds_factor)) > 0.001)
            {
                std::stringstream ess;
                ess << "Downsampling factor "<< rational_ds_factor << " of signal type "
                    << mod_signal_it->first << " is not an exact integer greater than 1.";
                throw TiAException(ess.str());
            }
            else if(rational_ds_factor > 1)
            {
                std::cout << "Adding signal " << mod_signal_it->first << " with ds factor" << rational_ds_factor << "!" << std::endl;

                signal_ds_factor_map_[constants_.getSignalFlag(mod_signal_it->first)] = (boost::uint16_t)rational_ds_factor;

                std::pair<boost::uint16_t, IIRButterLpf<double> > new_filter
                        (constants_.getSignalFlag(mod_signal_it->first),
                         IIRButterLpf<double>(signal_fs, signal_fs / 2.0 / rational_ds_factor, 5.0, 4, false));

                lp_filters_.insert(new_filter);

                mod_signal_it->second.setSamplingRate(custom_signal->second.samplingRate());
            }

            ++mod_signal_it;
        }
    }

    is_applicable_ = true;

    has_configured_work_ = signal_ds_factor_map_.size() > 0;
}

//-----------------------------------------------------------------------------

void DownsamplingFilterDecorator::applyFilter(DataPacket &packet)
{
    decorated_filter_->applyFilter(packet);    

    BOOST_FOREACH(SignalFilterMap::value_type signal_filter, lp_filters_)
    {
        std::cout << "Filtering signal type " << constants_.getSignalName(signal_filter.first);

        if(packet.hasFlag(signal_filter.first))
        {
            std::vector<double> signal_samples = packet.getSingleDataBlock(signal_filter.first);
            boost::uint16_t nr_of_channels = packet.getNrOfChannels(signal_filter.first);

            boost::uint16_t block_size = packet.getNrSamplesPerChannel(signal_filter.first);

            std::cout << " with " << nr_of_channels << " channels and blocksize "<< block_size <<"." << std::endl;

            std::vector<double> filtered_samples (signal_samples.size());

            signal_filter.second.filter(signal_samples, filtered_samples);

            packet.removeDataBlock(signal_filter.first);

            packet.insertDataBlock(filtered_samples,signal_filter.first,block_size);

        }
    }

//    BOOST_FOREACH(boost::uint32_t signal_flag, signals_to_exclude_)
//    {
////        std::cout << " remove signal:" << constants_.getSignalName(signal_flag) << std::endl;
//        if(packet.hasFlag(signal_flag))
//            packet.removeDataBlock(signal_flag);
//    }
}

//-----------------------------------------------------------------------------

const SignalInfo &DownsamplingFilterDecorator::getSignalInfoAfterFiltering()
{
    return modified_signal_info_;
}

}
