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


#include "tia-private/newtia/filter_impl/tia_downsampling_filterdecorator.h"

#include <iostream>

#include <boost/current_function.hpp>
#include <boost/foreach.hpp>
#include <boost/cast.hpp>
#include <string>
#include <sstream>
#include <utility>
#include <string.h>

#include "tia-private/newtia/tia_exceptions.h"
#include "tia/constants.h"

#include "tia-private/newtia/tia_meta_info_parse_and_build_functions.h"

namespace tia
{

DownsamplingFilterDecorator::DownsamplingFilterDecorator(CustomPacketFilterPtr decorated_filter)
  : CustomPacketFilterDecorator(decorated_filter)
{

  #ifdef DEBUG
      std::cout << BOOST_CURRENT_FUNCTION << std::endl;
  #endif

  CustomSignalInfo::CustomSignalMap custom_signals = custom_sig_info_ptr_->signals();

  boost::uint16_t master_sr = 0, master_bs = 1;

  modified_signal_info_ = default_sig_info_;    

  for(SignalInfo::SignalMap::iterator mod_signal_it = modified_signal_info_.signals().begin();
      mod_signal_it != modified_signal_info_.signals().end();)
  {      

    CustomSignalInfo::CustomSignalMap::iterator custom_signal = custom_signals.find(mod_signal_it->first);

    if(custom_signal != custom_signals.end())
    {
      double signal_fs = mod_signal_it->second.samplingRate();      

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
        //  std::cout << "Adding signal " << mod_signal_it->first << " with ds factor" << rational_ds_factor << "!" << std::endl;

		#pragma warning(disable: 4244)

        DownsamplingFilterParam *ds_param = new DownsamplingFilterParam ( signal_fs , ((boost::uint16_t)rational_ds_factor), 0,
                                                                          mod_signal_it->second.channels().size());

		#pragma warning(default: 4244)
        // test out of memory
        if(ds_param == NULL)
        {
            throw runtime_error("Ran out of memory during creating downsamlpingfilterdecorator.");
        }

        ds_filters_[constants_.getSignalFlag(mod_signal_it->first)] = ds_param;

        mod_signal_it->second.setSamplingRate(custom_signal->second.samplingRate() / (boost::uint16_t)rational_ds_factor);
      }      
    }
    #ifdef DEBUG
    else
        std::cout << "Signal " << mod_signal_it->first << " is not present in custom config!" << std::endl;
    #endif

    // determine a new master sampling rate for the modified signal info on the fly
    if(mod_signal_it->second.samplingRate() / mod_signal_it->second.blockSize() > master_sr / master_bs)
    {
      master_sr = mod_signal_it->second.samplingRate();
      master_bs = mod_signal_it->second.blockSize();
    }

    ++mod_signal_it;

  }

  modified_signal_info_.setMasterSamplingRate(master_sr);
  modified_signal_info_.setMasterBlockSize(master_bs);

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

      std::vector<double> filtered_samples (signal_samples.size());

      DownsamplingFilterParam *filter_params = signal_filter.second;

      filter_params->filter(signal_samples,filtered_samples,bs_old);      

      boost::uint16_t sc = filter_params->getSampleCounter();
      boost::uint16_t ds = filter_params->getDsFactor();
      boost::uint16_t chan_nr = 0;

      //calculate new block size after downsampling this data block
      //therefore start with last taken sample (= modeled with sc) and add
      //the current block size - 1 (-1 is because we want the position
      //of the last sample in the data block in relation to the last sample accepted (which is our origin)
      //dividing this sum by the ds factor already computes the new block size
      boost::uint16_t bs_new = (boost::uint16_t)((((double) bs_old - 1.0) + (double)sc) / (double) ds);
      //if sc is zero the last sample accepted is the first one of this block
      //thus it has to be added and we get the new block size
      bs_new += (sc == 0 ? 1 : 0);

//          std::cout << "bs_new: " << bs_new << ", sc: " << sc << ", ds: " << ds << ", channels: " << nr_of_channels << std::endl;

      //update sample counter for next packet
      filter_params->setSampleCounter((sc + bs_old) % ds);

      if(bs_new)
      {

        //take every ds. sample of each channel of this signal type
        for(chan_nr = 0; chan_nr < nr_of_channels; ++chan_nr)
        {
            //position of first downsampled channel within this datablock
            boost::uint16_t sample_pos = (sc == 0 ? 0 : ds - sc);

            //now start with the first downsampled sample
            //and take every ds. sample
            for(; sample_pos < bs_old; sample_pos += ds)
                filtered_samples[sample_pos / ds + chan_nr * bs_new] = filtered_samples[sample_pos + chan_nr * bs_old];
        }

        //adjust the vectors size
        filtered_samples.resize(bs_new * nr_of_channels);


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
