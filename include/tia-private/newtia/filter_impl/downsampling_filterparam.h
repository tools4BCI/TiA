#ifndef DOWNSAMPLING_FILTERPARAM_H
#define DOWNSAMPLING_FILTERPARAM_H

#include "extern/include/filterTools/BasicFilter.h"
#include "extern/include/filterTools/iirbutterlpf.h"
#include "extern/include/filterTools/iirchebylpf.h"


#include <boost/cstdint.hpp>

namespace tia
{

class DownsamplingFilterParam
{
public:
    DownsamplingFilterParam(boost::int16_t sampling_rate, boost::uint16_t ds_factor,
                            boost::uint16_t sample_counter, boost::uint16_t num_channels) :
        sampling_rate_(sampling_rate), ds_factor_(ds_factor), sample_counter_(sample_counter)
    {

        double cut_off_frequz = 0.8 * ((double)sampling_rate) / 2.0 / ((double)ds_factor);

        for(boost::uint16_t chan_idx = 0; chan_idx < num_channels; ++chan_idx)
        {
            BasicFilter<double> *chan_lpf = new IIRButterLpf<double>(sampling_rate, cut_off_frequz, 5, 8 );

//            BasicFilter<double> *chan_lpf = new IIRChebyLpf<double>(false,sampling_rate, cut_off_frequz ,0.5,8,false);

            lpf_.push_back(chan_lpf);
        }
    }

    ~DownsamplingFilterParam()
    {
        while(!lpf_.empty())
        {
            delete lpf_.back();
            lpf_.pop_back();
        }
    }

//    inline BasicFilter<double> *getLpFilter()
//    {
//        return lpf_;
//    }

    void filter(const std::vector<double>& in, std::vector<double>& out, boost::uint16_t block_size)
    {
        for(boost::uint16_t chan_idx = 0; chan_idx < lpf_.size(); ++chan_idx)
        {
            for(boost::int16_t sample_idx = 0; sample_idx < block_size ; ++sample_idx)
            {
                out[chan_idx * block_size + sample_idx] = lpf_[chan_idx]->clock(in[chan_idx * block_size + sample_idx]);
            }
        }
    }

    inline boost::uint16_t getDsFactor()
    {
        return ds_factor_;
    }

    inline void setDsFactor(boost::uint16_t ds_factor)
    {
        ds_factor_ = ds_factor;
    }

    inline boost::uint16_t getSampleCounter()
    {
        return sample_counter_;
    }

    inline void setSampleCounter(boost::uint16_t sample_counter)
    {
        sample_counter_ = sample_counter;
    }

private:   
    std::vector<BasicFilter<double> *> lpf_;
    boost::uint16_t sampling_rate_;
    boost::uint16_t ds_factor_;
    boost::uint16_t sample_counter_;    
};

}

#endif // DOWNSAMPLING_FILTERPARAM_H
