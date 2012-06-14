#ifndef TIA_CUSTOM_PACKET_FILTER_TESTER_H
#define TIA_CUSTOM_PACKET_FILTER_TESTER_H


#include "tia-private/newtia/tia_custom_signal_info_parse_and_build_functions.h"
#include "tia-private/newtia/tia_meta_info_parse_and_build_functions.h"

#include "tia-private/datapacket/data_packet_3_impl.h"

#include "tia/ss_meta_info.h"

#include "tia/defines.h"

using namespace tia;

struct TiACustomFilterTests
{
  TiACustomFilterTests()
  {
      std::string default_str = "<tiaCustomSignalInfo version=\"1.0\">\n"
                                  "\t<masterSignal samplingRate=\"500\" blockSize=\"1\"/>\n"
                                  "\t<signal type=\"eeg\" samplingRate=\"500\" blockSize=\"1\" numChannels=\"4\">\n"
                                      "\t\t<channel nr=\"0\" label=\"eeg1\"/>\n"
                                      "\t\t<channel nr=\"1\" label=\"eeg2\"/>\n"
                                      "\t\t<channel nr=\"2\" label=\"eeg3\"/>\n"
                                      "\t\t<channel nr=\"3\" label=\"eeg4\"/>\n"
                                  "\t</signal>\n"
                                  "\t<signal type=\"eog\" samplingRate=\"200\" blockSize=\"2\" numChannels=\"5\">\n"
                                      "\t\t<channel nr=\"0\" label=\"A\"/>\n"
                                      "\t\t<channel nr=\"1\" label=\"B\"/>\n"
                                  "\t</signal>\n"
                                  "\t<signal type=\"emg\" samplingRate=\"50\" blockSize=\"4\" numChannels=\"3\">\n"
                                      "\t\t<channel nr=\"0\" label=\"C\"/>\n"
                                      "\t\t<channel nr=\"1\" label=\"D\"/>\n"
                                  "\t</signal>\n"
                                  "\t<signal type=\"bp\" samplingRate=\"50\" blockSize=\"4\" numChannels=\"1\">\n"
                                      "\t\t<channel nr=\"0\" label=\"bp\"/>\n"
                                  "\t</signal>\n"
                                 "</tiaCustomSignalInfo>\n\n";


      SSConfig meta_info = parseTiAMetaInfoFromXMLString(default_str);

      default_sig_info_ = meta_info.signal_info;

      std::string custom_str = "<tiaCustomSignalInfo version=\"1.0\">\n"
                                  "\t<masterSignal samplingRate=\"500\" blockSize=\"1\"/>\n"
                                  "\t<signal type=\"eeg\" samplingRate=\"500\" blockSize=\"1\" numChannels=\"2\">\n"
                                      "\t\t<channel nr=\"0\" label=\"eeg1\"/>\n"
                                      "\t\t<channel nr=\"2\" label=\"eeg3\"/>\n"
                                  "\t</signal>\n"
                                  "\t<signal type=\"eog\" samplingRate=\"200\" blockSize=\"2\" numChannels=\"3\">\n"
                                      "\t\t<channel nr=\"0\" label=\"A\"/>\n"
                                      "\t\t<channel nr=\"2\" label=\"2\"/>\n"
                                      "\t\t<channel nr=\"4\" label=\"4\"/>\n"
                                  "\t</signal>\n"
                                  "\t<signal type=\"bp\" samplingRate=\"50\" blockSize=\"4\" numChannels=\"1\">\n"
                                      "\t\t<channel nr=\"0\" label=\"bp\"/>\n"
                                  "\t</signal>\n"
                                 "</tiaCustomSignalInfo>\n\n";


      //-----------------------------------------------------------------------------
      custom_sig_info_= parseTiACustomSignalInfoFromXMLString(custom_str, default_sig_info_);

      packet_.reset();

      unsigned raw_data_size = 100;

      for (unsigned counter = 0; counter < raw_data_size; ++counter)
      {
          raw_data_.push_back((double)rand()/(double)RAND_MAX);
      }

      unsigned block_sizes [] = { 1, 2, 4, 4};
      unsigned num_channels [] = { 4, 5, 3, 1};

      //-----------------------------------------------------------------------------

      std::vector<double> eeg_data;

      eeg_data.assign(raw_data_.begin(), raw_data_.begin() + block_sizes[0] * num_channels[0]);

      packet_.insertDataBlock(eeg_data,SIG_EEG,block_sizes[0]);

      for (unsigned counter = 0; counter < eeg_data.size(); ++counter)
      {
          if(counter % 4 == 0 || counter % 4 == 2)
            target_raw_data_eeg_.push_back(eeg_data[counter]);
      }

      //-----------------------------------------------------------------------------

      std::vector<double> eog_data;

      eog_data.assign(raw_data_.begin(), raw_data_.begin() + block_sizes[1] * num_channels[1]);

      packet_.insertDataBlock(eog_data,SIG_EOG,block_sizes[1]);

      for (unsigned counter = 0; counter < eog_data.size(); ++counter)
      {
          if(counter % 10 == 0 || counter % 10 == 1
                  || counter % 10 == 4 || counter % 10 == 5
                  || counter % 10 == 8 || counter % 10 == 9)
            target_raw_data_eog_.push_back(eog_data[counter]);
      }

      //-----------------------------------------------------------------------------

      packet_.insertDataBlock(raw_data_,SIG_EMG,block_sizes[2]);

      target_raw_data_bp_.assign(raw_data_.begin(), raw_data_.begin() + block_sizes[3] * num_channels[4]);

      packet_.insertDataBlock(target_raw_data_bp_,SIG_BP,block_sizes[3]);

  }
  ~TiACustomFilterTests() { /* some teardown */ }


  DataPacket3Impl packet_;
  SignalInfo default_sig_info_;
  SignalInfoPtr custom_sig_info_;
  std::vector<double> target_raw_data_eeg_, target_raw_data_eog_, target_raw_data_bp_;
  std::vector<double> raw_data_;
};


#endif // TIA_CUSTOM_PACKET_FILTER_TESTER_H
