#include "tia-private/newtia/filter_impl/tia_downsampling_filterdecorator.h"

#include <boost/foreach.hpp>
#include <boost/cast.hpp>
#include <string>
#include <sstream>
#include <utility>
#include <string.h>

#include "tia-private/newtia/tia_exceptions.h"
#include "tia/constants.h"

#include "extern/include/filterTools/iirbutterlpf.h"

namespace tia
{

DownsamplingFilterDecorator::DownsamplingFilterDecorator(CustomPacketFilterPtr decorated_filter)
    : CustomPacketFilterDecorator(decorated_filter)
{
    CustomSignalInfo::CustomSignalMap custom_signals = custom_sig_info_ptr_->signals();

    modified_signal_info_ = default_sig_info_;


    for(SignalInfo::SignalMap::iterator mod_signal_it = modified_signal_info_.signals().begin();
        mod_signal_it != modified_signal_info_.signals().end();)
    {

        CustomSignalInfo::CustomSignalMap::iterator custom_signal = custom_signals.find(mod_signal_it->first);

        if(custom_signal == custom_signals.end())
        {
            std::cout << "Signal " << mod_signal_it->first << " is not present in custom config!" << std::endl;
            ++mod_signal_it;
        }
        else
        {
            double signal_fs = mod_signal_it->second.samplingRate();
//            double rational_ds_factor = signal_fs / ((double)custom_signal->second.samplingRate());

            double rational_ds_factor = custom_signal->second.DSFactor();

            if((rational_ds_factor - ((boost::uint16_t)rational_ds_factor)) > 0.001)
            {
                std::stringstream ess;
                ess << "Downsampling factor "<< rational_ds_factor << " of signal type "
                    << mod_signal_it->first << " is not an exact integer greater than 1.";
                throw TiAException(ess.str());
            }
            else if(rational_ds_factor > 1)
            {
//                std::cout << "Adding signal " << mod_signal_it->first << " with ds factor" << rational_ds_factor << "!" << std::endl;


                //TODO: handle out of memory!
                DownsamplingFilterParam *ds_param = new DownsamplingFilterParam ( signal_fs ,
                                                  (boost::uint16_t)rational_ds_factor, 0,
                                                  mod_signal_it->second.channels().size());



//                std::pair<boost::uint32_t, DownsamplingFilterParam > new_filter
//                        (constants_.getSignalFlag(mod_signal_it->first),
//                         ds_param);

//                ds_filters_.insert(new_filter);

                ds_filters_[constants_.getSignalFlag(mod_signal_it->first)] = ds_param;

                mod_signal_it->second.setSamplingRate(custom_signal->second.samplingRate() / (boost::uint16_t)rational_ds_factor);
            }

            ++mod_signal_it;
        }
    }

    is_applicable_ = true;

    has_configured_work_ = ds_filters_.size() > 0;
}

//-----------------------------------------------------------------------------

DownsamplingFilterDecorator::~DownsamplingFilterDecorator()
{
    BOOST_FOREACH(SignalFilterMap::value_type signal_filter, ds_filters_)
    {
        delete signal_filter.second;
    }
}

//-----------------------------------------------------------------------------

void DownsamplingFilterDecorator::applyFilter(DataPacket &packet)
{
    decorated_filter_->applyFilter(packet);    

    BOOST_FOREACH(SignalFilterMap::value_type signal_filter, ds_filters_)
    {
//        std::cout << "Filtering signal type " << constants_.getSignalName(signal_filter.first);

        if(packet.hasFlag(signal_filter.first))
        {
            std::vector<double> signal_samples = packet.getSingleDataBlock(signal_filter.first);
            boost::uint16_t nr_of_channels = packet.getNrOfChannels(signal_filter.first);

            boost::uint16_t bs_old = packet.getNrSamplesPerChannel(signal_filter.first);

//            std::cout << " with " << nr_of_channels << " channels and blocksize "<< bs_old <<"." << std::endl;

//            std::vector<double> filtered_samples (signal_samples.size());
            //TODO: delete after testing and uncomment line before
            std::vector<double> filtered_samples (signal_samples.begin(), signal_samples.end());

            DownsamplingFilterParam *filter_params = signal_filter.second;

//            BasicFilter<double> *lpf = filter_params->getLpFilter();

            //TODO: uncomment after testing!
            filter_params->filter(signal_samples,filtered_samples,bs_old);

//            lpf->filter(signal_samples, filtered_samples);

            boost::uint16_t sc = filter_params->getSampleCounter();
            boost::uint16_t ds = filter_params->getDsFactor();
            boost::uint16_t chan_nr = 0;

            //position of first downsampled channel within this datablock
            boost::uint16_t sample_pos = (sc == 0 ? 0 : ds - sc);

            //calculate new block size after downsampling this data block
            //therefore start with last taken sample (= modeled with sc) and add
            //the current block size - 1 (-1 is because we want the position
            //of the last sample in the data block in relation to the last sample accepted (which is our origin)
            //dividing this sum by the ds factor already computes the new block size
            boost::uint16_t bs_new = (boost::uint16_t)((((double) bs_old - 1.0) + (double)sc) / (double) ds);
            //if sc is zero the last sample accepted is the first one of this block
            //thus it has to be added and we get the new block size
            bs_new += (sc == 0 ? 1 : 0);

//            std::cout << "bs_new: " << bs_new << ", sc: " << sc << ", ds: " << ds << ", channels: " << nr_of_channels << std::endl;

            //update sample counter for next packet
            filter_params->setSampleCounter((sc + bs_old) % ds);

            if(bs_new)
            {

                double buffer [bs_new * nr_of_channels];

                //now start with the first downsampled sample
                //and take every ds. sample
                for(; sample_pos < bs_old; sample_pos += ds)
                {
                    //take every ds. sample of each channel of this signal type
                    for(chan_nr = 0; chan_nr < nr_of_channels; ++chan_nr)
                        buffer[sample_pos / ds + chan_nr * bs_new] = filtered_samples[sample_pos + chan_nr * bs_old];
                }

                //for faster performance extract the vectors underlying array
                double *filtered_samples_ptr = &(filtered_samples[0]);

//                std::cout << "new values: ";

//                for (int var = 0; var < bs_new * nr_of_channels; ++var) {
//                    std::cout << ", " << buffer[var];
//                }
//                std::cout << std::endl;

                //finally copy the buffer back to the vector and
                //adjust the vectors size
                memcpy(filtered_samples_ptr, buffer,sizeof(double) * bs_new * nr_of_channels);
                filtered_samples.resize(bs_new * nr_of_channels);

//                std::cout << "new values in vector: ";

//                for (int var = 0; var < filtered_samples.size(); ++var) {
//                    std::cout << ", " << filtered_samples[var];
//                }
//                std::cout << std::endl;

                packet.removeDataBlock(signal_filter.first);

                packet.insertDataBlock(filtered_samples,signal_filter.first,bs_new);
            }
            else
            {
                packet.removeDataBlock(signal_filter.first);
            }

        }
    }
}

//-----------------------------------------------------------------------------

const SignalInfo &DownsamplingFilterDecorator::getSignalInfoAfterFiltering()
{
    return modified_signal_info_;
}

}
