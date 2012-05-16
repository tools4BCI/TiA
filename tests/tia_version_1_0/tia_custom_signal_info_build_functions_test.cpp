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

#include "../UnitTest++/UnitTest++.h"

#include "tia-private/newtia/tia_custom_signal_info_parse_and_build_functions.h"

#include "tia/ss_meta_info.h"

#include <iostream>

using namespace tia;
using namespace std;

SUITE (TiAVersion10)
{

TEST(TiACustomSignalBuildFunctionsTest)
{
    std::string original_str = "<tiaCustomSignalInfo version=\"1.0\">\n"
                                "\t<masterSignal samplingRate=\"500\" blockSize=\"1\"/>\n"
                                "\t<signal type=\"eeg\" samplingRate=\"500\" blockSize=\"1\" numChannels=\"3\">\n"
                                    "\t\t<channel nr=\"1\" label=\"eeg\"/>\n"
                                    "\t\t<channel nr=\"2\" label=\"eeg\"/>\n"
                                    "\t\t<channel nr=\"3\" label=\"eeg\"/>\n"
                                "\t</signal>\n"
                                "\t<signal type=\"eog\" samplingRate=\"200\" blockSize=\"2\" numChannels=\"2\">\n"
                                    "\t\t<channel nr=\"1\" label=\"A\"/>\n"
                                    "\t\t<channel nr=\"2\" label=\"B\"/>\n"
                                "\t</signal>\n"
                               "</tiaCustomSignalInfo>\n\n";


    SignalInfo sig_info = parseTiACustomSignalInfoFromXMLString(original_str);

    std::string parsed_str = buildTiACustomSignalInfoXMLString(sig_info);

    CHECK_EQUAL(original_str,parsed_str);
}

}
