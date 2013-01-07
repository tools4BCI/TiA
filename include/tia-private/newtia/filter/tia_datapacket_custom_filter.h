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

#ifndef TIA_DATAPACKET_CUSTOM_FILTER_H
#define TIA_DATAPACKET_CUSTOM_FILTER_H

#include "tia/data_packet_interface.h"
#include "tia/ss_meta_info.h"
#include "tia/custom_signal_info.h"
#include "tia/constants.h"

#include <iostream>

namespace tia
{

//-----------------------------------------------------------------------------
class CustomPacketFilter
{
    friend class CustomPacketFilterDecorator;
public:
    CustomPacketFilter(const SignalInfo &default_info, const CustomSignalInfoPtr custom_info)
        : default_sig_info_ (default_info), custom_sig_info_ptr_ (custom_info)
    { }

    virtual void applyFilter(DataPacket &packet) = 0;

    virtual const SignalInfo &getSignalInfoAfterFiltering() = 0;

    virtual bool hasConfiguredWork() = 0;

protected:
    const SignalInfo &default_sig_info_;
    const CustomSignalInfoPtr custom_sig_info_ptr_;
    Constants constants_;
};

typedef boost::shared_ptr<CustomPacketFilter> CustomPacketFilterPtr;

//-----------------------------------------------------------------------------

class DummyCustomPacketFilter : public CustomPacketFilter
{
public:
    DummyCustomPacketFilter(const SignalInfo &default_info, const CustomSignalInfoPtr custom_info_ptr)
        : CustomPacketFilter(default_info, custom_info_ptr)
    { }


    virtual void applyFilter(DataPacket &)
    { }

    virtual const SignalInfo &getSignalInfoAfterFiltering()
    {
        return default_sig_info_;
    }

    virtual bool hasConfiguredWork()
    {
        return false;
    }

};

//-----------------------------------------------------------------------------

class CustomPacketFilterDecorator : public CustomPacketFilter
{
public:
    CustomPacketFilterDecorator(CustomPacketFilterPtr decorated_filter)
        : CustomPacketFilter(decorated_filter->getSignalInfoAfterFiltering(), decorated_filter->custom_sig_info_ptr_),
          decorated_filter_ (decorated_filter), has_configured_work_ (false)
    { }

    virtual void applyFilter(DataPacket &packet) = 0;

    virtual const SignalInfo &getSignalInfoAfterFiltering() = 0;


    virtual bool hasConfiguredWork()
    {
        return has_configured_work_;
    }


protected:
    CustomPacketFilterPtr decorated_filter_;
    bool has_configured_work_;
};



}


#endif // TIA_DATAPACKET_CUSTOM_FILTER_H
