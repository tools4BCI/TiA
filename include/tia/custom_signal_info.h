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

/**
* @file ss_meta_info.h
* @brief This file includes classes used to store meta information used in TiA.
* @todo Rename file to TiA_meta_information
**/

#ifndef CUSTOM_SIGNAL_INFO_H
#define CUSTOM_SIGNAL_INFO_H

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

// STL
#include <string>
#include <map>
#include <vector>

#include "ss_meta_info.h"

namespace tia
{

//-----------------------------------------------------------------------------

/**
 * @class CustomChannel
 *
 */
class CustomChannel : public Channel
{
    //TODO: For future add channel specific customization attributes here!
};

//-----------------------------------------------------------------------------

/**
 * @class CustomSignal
 *
 * @brief This class is written to store information for a specific signal type.
 *
 * This class is designed to hold information which is consistent
 * within a single signal type like sampling rate or block size.
 *
 * @todo Find a way to be consistent with signal types and signal type names within TiA.
 */
class CustomSignal : public Signal
{
  public:


      boost::uint16_t getDSFactor() const { return downsampling_factor_; }

      void setDSFactor(boost::uint16_t downsampling_factor) { downsampling_factor_ = downsampling_factor; }

      /**
       * @brief Get a vector holding specific information for every channel of this signal type.
       */
      const std::vector<CustomChannel>& custom_channels() const
      {
          return static_cast<const std::vector<CustomChannel> >( channels());
      }

      std::vector<CustomChannel>& custom_channels()
      {
          return static_cast<std::vector<CustomChannel> >( channels());
      }

  private:
    boost::uint16_t       downsampling_factor_;
};

//-----------------------------------------------------------------------------

/**
 * @class CustomSignalInfo
 *
 * @brief This class stores information for all transmitted signals.
 *
 * This class stores information for all signal transmitted via TiA.
 * It also stores information concerning the master device, as this
 * device is responsible for the rate, data packets are delivered with.
 */
class CustomSignalInfo
{
  public:
    typedef std::map<std::string, CustomSignal> CustomSignalMap;

  public:

#ifdef signals
    #define SIGNAL_INFO_HELPER signals
    #undef signals
#endif

    const CustomSignalMap& custom_signals() const
    {
        return static_cast<const CustomSignalMap >( signals());
    }

    CustomSignalMap& custom_signals()
    {
        return static_cast<CustomSignalMap >( signals());
    }

#ifdef SIGNAL_INFO_HELPER
    #define signals SIGNAL_INFO_HELPER
    #undef SIGNAL_INFO_HELPER
#endif
};

typedef boost::shared_ptr<CustomSignalInfo> CustomSignalInfoPtr;

} // Namespace tobiss

//-----------------------------------------------------------------------------

#endif // CUSTOM_SIGNAL_INFO_H
