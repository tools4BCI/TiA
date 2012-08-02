#include "../UnitTest++/UnitTest++.h"

#include "tia-private/newtia/tia_custom_signal_info_parse_and_build_functions.h"
#include "tia-private/newtia/tia_meta_info_parse_and_build_functions.h"


#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace tia;
using namespace std;

SUITE (TiAVersion10)
{

TEST(TiACustomParseFunctionsTest)
{
    std::string default_str = "<tiaMetaInfo version=\"1.0\">\n"
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
                               "</tiaMetaInfo>\n\n";

    std::string target = "<tiaCustomSignalInfo version=\"1.0\">\n"
                                "\t<signal type=\"eeg\" samplingRate=\"500\" blockSize=\"1\" downsamplingFactor=\"1\" numChannels=\"4\">\n"
                                    "\t\t<channel nr=\"1\" label=\"eeg1\"/>\n"
                                    "\t\t<channel nr=\"2\" label=\"eeg2\"/>\n"
                                    "\t\t<channel nr=\"3\" label=\"eeg3\"/>\n"
                                    "\t\t<channel nr=\"4\" label=\"eeg4\"/>\n"
                                "\t</signal>\n"
                                "\t<signal type=\"emg\" samplingRate=\"60\" blockSize=\"3\" downsamplingFactor=\"1\" numChannels=\"2\">\n"
                                    "\t\t<channel nr=\"1\" label=\"C\"/>\n"
                                    "\t\t<channel nr=\"2\" label=\"D\"/>\n"
                                "\t</signal>\n"
                                "\t<signal type=\"eog\" samplingRate=\"300\" blockSize=\"2\" downsamplingFactor=\"1\" numChannels=\"2\">\n"
                                    "\t\t<channel nr=\"1\" label=\"A\"/>\n"
                                    "\t\t<channel nr=\"2\" label=\"B\"/>\n"
                                "\t</signal>\n"
                               "</tiaCustomSignalInfo>\n\n";

    SSConfig meta_info = parseTiAMetaInfoFromXMLString(default_str);

    //-----------------------------------------------------------------------------
    CustomSignalInfoPtr custom_sig_info = parseCustomSignalInfoFromTiAMetaInfoXMLString(default_str);

    std::string to_test =  buildTiACustomSignalInfoXMLString(custom_sig_info);

    CHECK_EQUAL(target, to_test);

}

}
