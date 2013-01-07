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


#ifndef DOWNSAMPLING_FILTERPARAM_H
#define DOWNSAMPLING_FILTERPARAM_H

#include "filterTools/BasicFilter.h"
#include "filterTools/iirbutterlpf.h"
#include "filterTools/iirchebylpf.h"


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
