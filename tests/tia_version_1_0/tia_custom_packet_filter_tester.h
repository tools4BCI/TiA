#ifndef TIA_CUSTOM_PACKET_FILTER_TESTER_H
#define TIA_CUSTOM_PACKET_FILTER_TESTER_H


#include "tia-private/newtia/tia_custom_signal_info_parse_and_build_functions.h"

#include "tia-private/datapacket/data_packet_3_impl.h"

#include "tia/ss_meta_info.h"

using namespace tia;

struct TiACustomFilterTests
{
  TiACustomFilterTests()
  {
      std::string default_str = "<tiaCustomSignalInfo version=\"1.0\">\n"
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


      default_sig_info_ = parseTiACustomSignalInfoFromXMLString(default_str);

      std::string custom_str = "<tiaCustomSignalInfo version=\"1.0\">\n"
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


      custom_sig_info_= parseTiACustomSignalInfoFromXMLString(custom_str);

  }
  ~TiACustomFilterTests() { /* some teardown */ }


  DataPacket3Impl packet_;
  SignalInfo default_sig_info_;
  SignalInfo custom_sig_info_;
};


#endif // TIA_CUSTOM_PACKET_FILTER_TESTER_H
