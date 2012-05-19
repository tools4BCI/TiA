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
#include <stdexcept>

using namespace tia;
using namespace std;

SUITE (TiAVersion10)
{

TEST_FIXTURE(TiACustomFilterTests, TiAChannelFilterTest)
{
    //check custom signal info parser
    string tmp_info_str = buildTiACustomSignalInfoXMLString(*custom_sig_info_);

    SignalInfoPtr tmp_info = parseTiACustomSignalInfoFromXMLString(tmp_info_str, default_sig_info_);

    string new_info_str = buildTiACustomSignalInfoXMLString(*tmp_info);

    CHECK_EQUAL(tmp_info_str,new_info_str);

    //-----------------------------------------------------------------------------
    //check channel filter
    CustomPacketFilterPtr filter_chain (new DummyCustomPacketFilter(default_sig_info_, custom_sig_info_));

    filter_chain = CustomPacketFilterPtr(new CustomChannelFilterDecorator(filter_chain));

    CHECK(filter_chain->isApplicable());
    CHECK(filter_chain->hasConfiguredWork());

    filter_chain->applyFilter(packet_);

    CHECK_THROW(packet_.getSingleDataBlock(SIG_EMG), invalid_argument);

    CHECK_EQUAL(3, packet_.getNrOfSignalTypes());

    std::vector<double> tmp = packet_.getSingleDataBlock(SIG_EEG);
    CHECK_ARRAY_EQUAL(target_raw_data_eeg_, tmp, target_raw_data_eeg_.size());

    tmp = packet_.getSingleDataBlock(SIG_EOG);
    CHECK_ARRAY_EQUAL(target_raw_data_eog_, tmp, target_raw_data_eog_.size());

    tmp = packet_.getSingleDataBlock(SIG_BP);
    CHECK_ARRAY_EQUAL(target_raw_data_bp_, tmp, target_raw_data_bp_.size());
}

}

