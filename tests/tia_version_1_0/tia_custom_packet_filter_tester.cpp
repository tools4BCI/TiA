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
#include "tia-private/newtia/filter_impl/tia_downsampling_filterdecorator.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>

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

//    std:: cout << "Signal info after filtering: " << buildTiACustomSignalInfoXMLString(filter_chain->getSignalInfoAfterFiltering())
//                  << std::endl;

//    CHECK(filter_chain->isApplicable());
//    CHECK(filter_chain->hasConfiguredWork());

    filter_chain->applyFilter(packet_);

    CHECK_THROW(packet_.getSingleDataBlock(SIG_EMG), invalid_argument);

    CHECK_EQUAL(3, packet_.getNrOfSignalTypes());

    std::vector<double> tmp = packet_.getSingleDataBlock(SIG_EEG);
    CHECK_ARRAY_EQUAL(target_raw_data_eeg_, tmp, target_raw_data_eeg_.size());

    tmp = packet_.getSingleDataBlock(SIG_EOG);
    CHECK_ARRAY_EQUAL(target_raw_data_eog_, tmp, target_raw_data_eog_.size());

    tmp = packet_.getSingleDataBlock(SIG_BP);
    CHECK_ARRAY_EQUAL(target_raw_data_bp_, tmp, target_raw_data_bp_.size());


    //-----------------------------------------------------------------------------
    //check downsampling filter

    std::string default_str = "<tiaCustomSignalInfo version=\"1.0\">\n"
                                "\t<masterSignal samplingRate=\"500\" blockSize=\"1\"/>\n"
                                "\t<signal type=\"eeg\" samplingRate=\"500\" blockSize=\"1\" numChannels=\"4\">\n"
                                    "\t\t<channel nr=\"1\" label=\"eeg1\"/>\n"
                                    "\t\t<channel nr=\"2\" label=\"eeg2\"/>\n"
                                    "\t\t<channel nr=\"3\" label=\"eeg3\"/>\n"
                                    "\t\t<channel nr=\"4\" label=\"eeg4\"/>\n"
                                "\t</signal>\n"
                                "\t<signal type=\"eog\" samplingRate=\"300\" blockSize=\"2\" numChannels=\"2\">\n"
                                    "\t\t<channel nr=\"1\" label=\"A\"/>\n"
                                    "\t\t<channel nr=\"2\" label=\"B\"/>\n"
                                "\t</signal>\n"
                                "\t<signal type=\"emg\" samplingRate=\"60\" blockSize=\"3\" numChannels=\"2\">\n"
                                    "\t\t<channel nr=\"1\" label=\"C\"/>\n"
                                    "\t\t<channel nr=\"2\" label=\"D\"/>\n"
                                "\t</signal>\n"
                               "</tiaCustomSignalInfo>\n\n";


    SSConfig meta_info = parseTiAMetaInfoFromXMLString(default_str);

    default_sig_info_ = meta_info.signal_info;

    std::string custom_str = "<tiaCustomSignalInfo version=\"1.0\">\n"
                            "\t<masterSignal samplingRate=\"500\" blockSize=\"1\"/>\n"
                            "\t<signal type=\"eeg\" samplingRate=\"250\" blockSize=\"1\" numChannels=\"4\">\n"
                                "\t\t<channel nr=\"1\" label=\"eeg1\"/>\n"
                                "\t\t<channel nr=\"2\" label=\"eeg2\"/>\n"
                                "\t\t<channel nr=\"3\" label=\"eeg3\"/>\n"
                                "\t\t<channel nr=\"4\" label=\"eeg4\"/>\n"
                            "\t</signal>\n"
                            "\t<signal type=\"eog\" samplingRate=\"100\" blockSize=\"2\" numChannels=\"2\">\n"
                                "\t\t<channel nr=\"1\" label=\"A\"/>\n"
                                "\t\t<channel nr=\"2\" label=\"B\"/>\n"
                            "\t</signal>\n"
                            "\t<signal type=\"emg\" samplingRate=\"30\" blockSize=\"3\" numChannels=\"2\">\n"
                                "\t\t<channel nr=\"1\" label=\"C\"/>\n"
                                "\t\t<channel nr=\"2\" label=\"D\"/>\n"
                            "\t</signal>\n"
                           "</tiaCustomSignalInfo>\n\n";
    //-----------------------------------------------------------------------------
    custom_sig_info_= parseTiACustomSignalInfoFromXMLString(custom_str, default_sig_info_);

    CustomPacketFilterPtr filter (new DummyCustomPacketFilter(default_sig_info_, custom_sig_info_));

    filter = CustomPacketFilterPtr(new DownsamplingFilterDecorator(filter));

    DataPacket3Impl ds_1packet, ds_2packet;

    double eeg_1block_arr [] = {1.1, 2.1, 3.1, 4.1};
    std::vector<double> eeg_1block (eeg_1block_arr, eeg_1block_arr + 4);

    double eeg_2block_arr [] = {1.2, 2.2, 3.2, 4.2};
    std::vector<double> eeg_2block (eeg_2block_arr, eeg_2block_arr + 4);

    double eeg_1block_target [] = {1.1, 2.1, 3.1, 4.1};
//    double eeg_2block_target [] = {};

    double eog_1block_arr [] = {1.1, 1.2, 2.1, 2.2};
    std::vector<double> eog_1block (eog_1block_arr, eog_1block_arr + 4);

    double eog_2block_arr [] = {1.3, 1.4, 2.3, 2.4};
    std::vector<double> eog_2block (eog_2block_arr, eog_2block_arr + 4);

    double eog_1block_target [] = {1.1, 2.1};
    double eog_2block_target [] = {1.4, 2.4};

    double emg_1block_arr [] = {1.1, 1.2, 1.3, 2.1, 2.2, 2.3};
    std::vector<double> emg_1block (emg_1block_arr, emg_1block_arr + 6);

    double emg_2block_arr [] = {1.4, 1.5, 1.6, 2.4, 2.5, 2.6};
    std::vector<double> emg_2block (emg_2block_arr, emg_2block_arr + 6);

    double emg_1block_target [] = {1.1, 1.3, 2.1, 2.3};
    double emg_2block_target [] = {1.5, 2.5};

    ds_1packet.insertDataBlock(eeg_1block,SIG_EEG,1);
    ds_1packet.insertDataBlock(eog_1block,SIG_EOG,2);
    ds_1packet.insertDataBlock(emg_1block,SIG_EMG,3);

    filter->applyFilter(ds_1packet);

    tmp = ds_1packet.getSingleDataBlock(SIG_EEG);
    CHECK_ARRAY_EQUAL(eeg_1block_target, tmp, tmp.size());

    tmp = ds_1packet.getSingleDataBlock(SIG_EOG);
    CHECK_ARRAY_EQUAL(eog_1block_target, tmp, tmp.size());

    tmp = ds_1packet.getSingleDataBlock(SIG_EMG);
    CHECK_ARRAY_EQUAL(emg_1block_target, tmp, tmp.size());


    ds_2packet.insertDataBlock(eeg_2block,SIG_EEG,1);
    ds_2packet.insertDataBlock(eog_2block,SIG_EOG,2);
    ds_2packet.insertDataBlock(emg_2block,SIG_EMG,3);

    filter->applyFilter(ds_2packet);

    CHECK_THROW(ds_2packet.getSingleDataBlock(SIG_EEG), std::invalid_argument);

    tmp = ds_2packet.getSingleDataBlock(SIG_EOG);
    CHECK_ARRAY_EQUAL(eog_2block_target, tmp, tmp.size());

    tmp = ds_2packet.getSingleDataBlock(SIG_EMG);
    CHECK_ARRAY_EQUAL(emg_2block_target, tmp, tmp.size());

}

}

