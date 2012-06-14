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

#ifndef DATA_SERVER_H
#define DATA_SERVER_H

#include "tia-private/newtia/filter/tia_datapacket_custom_filter.h"

namespace tia
{

typedef unsigned ConnectionID;
typedef unsigned short Port;

//-----------------------------------------------------------------------------
/// DataServer
///
/// @brief base class for data servers which broadcast datapackets
class DataServer
{
public:
    virtual ~DataServer () {}

    virtual Port localPort (ConnectionID connection) const = 0;
    virtual std::string getTargetIP() const = 0;

    virtual ConnectionID addConnection (bool udp, CustomPacketFilterPtr packet_filter) = 0;

    virtual bool hasConnection (ConnectionID connection) const = 0;

    virtual bool transmitting (ConnectionID connection) const = 0;

    virtual bool removeConnection (ConnectionID connection) = 0;

    virtual void startTransmission (ConnectionID connection) = 0;

    virtual void stopTransmission (ConnectionID connection) = 0;
};


}

#endif // DATA_SERVER_H
