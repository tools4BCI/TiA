#ifndef TIA_CUSTOM_PACKET_FILTER_TESTER_H
#define TIA_CUSTOM_PACKET_FILTER_TESTER_H


#include "tia-private/newtia/tia_custom_signal_info_parse_and_build_functions.h"
#include "tia-private/newtia/tia_meta_info_parse_and_build_functions.h"

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
                                  "\t<signal type=\"eog\" samplingRate=\"200\" blockSize=\"2\" numChannels=\"5\">\n"
                                      "\t\t<channel nr=\"1\" label=\"A\"/>\n"
                                      "\t\t<channel nr=\"2\" label=\"B\"/>\n"
                                  "\t</signal>\n"
                                  "\t<signal type=\"emg\" samplingRate=\"50\" blockSize=\"4\" numChannels=\"3\">\n"
                                      "\t\t<channel nr=\"1\" label=\"C\"/>\n"
                                      "\t\t<channel nr=\"2\" label=\"D\"/>\n"
                                  "\t</signal>\n"
                                  "\t<signal type=\"bp\" samplingRate=\"50\" blockSize=\"4\" numChannels=\"1\">\n"
                                      "\t\t<channel nr=\"1\" label=\"bp\"/>\n"
                                  "\t</signal>\n"
                                 "</tiaCustomSignalInfo>\n\n";


      SSConfig meta_info = parseTiAMetaInfoFromXMLString(default_str);

      default_sig_info_ = meta_info.signal_info;

      std::string custom_str = "<tiaCustomSignalInfo version=\"1.0\">\n"
                                  "\t<masterSignal samplingRate=\"500\" blockSize=\"1\"/>\n"
                                  "\t<signal type=\"eeg\" samplingRate=\"500\" blockSize=\"1\" numChannels=\"2\">\n"
                                      "\t\t<channel nr=\"1\" label=\"eeg\"/>\n"
                                      "\t\t<channel nr=\"3\" label=\"eeg\"/>\n"
                                  "\t</signal>\n"
                                  "\t<signal type=\"eog\" samplingRate=\"200\" blockSize=\"2\" numChannels=\"3\">\n"
                                      "\t\t<channel nr=\"1\" label=\"A\"/>\n"
                                      "\t\t<channel nr=\"3\" label=\"3\"/>\n"
                                      "\t\t<channel nr=\"5\" label=\"5\"/>\n"
                                  "\t</signal>\n"
                                  "\t<signal type=\"bp\" samplingRate=\"50\" blockSize=\"4\" numChannels=\"1\">\n"
                                      "\t\t<channel nr=\"1\" label=\"bp\"/>\n"
                                  "\t</signal>\n"
                                 "</tiaCustomSignalInfo>\n\n";


      custom_sig_info_= parseTiACustomSignalInfoFromXMLString(custom_str, default_sig_info_);

  }
  ~TiACustomFilterTests() { /* some teardown */ }


  DataPacket3Impl packet_;
  SignalInfo default_sig_info_;
  SignalInfo custom_sig_info_;
};


#endif // TIA_CUSTOM_PACKET_FILTER_TESTER_H
