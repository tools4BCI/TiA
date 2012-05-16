/*
    This file is part of the TOBI SignalServer test routine.

    The TOBI SignalServer test routine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The TOBI SignalServer test routine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the TOBI SignalServer test routine. If not, see <http://www.gnu.org/licenses/>.

    Copyright 2012 Reinmar Kobler
    Contact: reinmar.kobler@student.tugraz.at
*/

#include "tia_custom_packet_filter_tester.h"

#include "../UnitTest++/UnitTest++.h"

#include "tia-private/newtia/filter/tia_datapacket_custom_filter.h"
#include "tia-private/newtia/filter_impl/tia_customchannel_filterdecorator.h"

#include <iostream>

using namespace tia;
using namespace std;

SUITE (TiAVersion10)
{

TEST_FIXTURE(TiACustomFilterTests, TiAChannelFilterTest)
{

    DummyCustomPacketFilter dummy_filter (default_sig_info_, custom_sig_info_);

    CustomChannelFilterDecorator chan_filter (dummy_filter);


    chan_filter.applyFilter(packet_);
}


}

