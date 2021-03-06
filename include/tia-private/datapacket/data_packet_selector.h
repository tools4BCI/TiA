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
* @file data_packet_selector.h
**/

#ifndef DATAPACKETSELECTOR_H
#define DATAPACKETSELECTOR_H

#include <set>
#include <vector>
#include <map>

#include <boost/cstdint.hpp>

#include "tia/data_packet_interface.h"

namespace tia
{

//-----------------------------------------------------------------------------

/**
* @class DataPacketSelector
* @brief Class to create individual data packet with selected channels and signal types.
*/
class DataPacketSelector
{
  public:
    /**
    * @brief Constructor
    */
    DataPacketSelector();

    void setDesiredChannels(  boost::uint32_t signal_type, boost::uint16_t blocksize,
                         std::set<boost::uint16_t> channels);

//    DataPacketImpl getModifiedDataPacket(DataPacketImpl& packet);

  private:

  //-----------------------------------

  private:
    std::map<boost::uint32_t, std::set<boost::uint16_t> >  ch_selection_;

    std::map<boost::uint32_t, std::vector<double> >  sample_map_;

    std::vector< double>  work_vec_;

};

}  //tobiss

#endif // DATAPACKETSELECTOR_H
