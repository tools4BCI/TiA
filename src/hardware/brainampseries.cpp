/*
    This file is part of the TOBI signal server.

    The TOBI signal server is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The TOBI signal server is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2010 Christian Breitwieser
    Contact: c.breitwieser@tugraz.at
*/

/**
* @file brainampseries.cpp
**/

#include <iostream>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <math.h>

#include "hardware/brainampseries.h"

namespace tobiss
{

using std::string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;

using std::cout;
using std::cerr;
using std::endl;

using boost::lexical_cast;
using boost::bad_lexical_cast;
using boost::algorithm::to_lower_copy;

const HWThreadBuilderTemplateRegistratorWithoutIOService<BrainAmpSeries> BrainAmpSeries::factory_registrator_ ("brainamp");

const float BrainAmpSeries::scaling_values_[4] = {.1f, .5f, 10.f, 152.6f };
const double BRAINAMP_SAMPLING_RATE = 5000;
const unsigned int MAX_NR_OF_AMPS = 4;
const unsigned int CHANNELS_PER_AMP = 32;

#define DEVICE_USB    "\\\\.\\BrainAmpUSB1"   // USB device


const string BrainAmpSeries::hw_low_imp_("use_low_impedance");
const string BrainAmpSeries::hw_trigger_value("trigger_hold_value");
const string BrainAmpSeries::hw_250lp_("lowpass_250Hz");
const string BrainAmpSeries::hw_dc_coupl_("dc_coupling");
const string BrainAmpSeries::hw_resolution_("resolution");
const string BrainAmpSeries::hw_calibration_("calibration_mode");
const string BrainAmpSeries::hw_calib_on_("on");
const string BrainAmpSeries::hw_calib_sig_("signal");
const string BrainAmpSeries::hw_calib_freq_("freq");
const string BrainAmpSeries::hw_value_("value");

//-----------------------------------------------------------------------------

BrainAmpSeries::BrainAmpSeries(ticpp::Iterator<ticpp::Element> hw)
  : acqu_type_(DataAcqu), dev_handle_(INVALID_HANDLE_VALUE), driver_version_(0),
    ds_factor_(0)
{
  setType("Brainamp Series");

  resolution_names_.insert(make_pair("100nv",  0)); // nV is in lower case because of to_lower_copy
  resolution_names_.insert(make_pair("500nv",  1));
  resolution_names_.insert(make_pair("10muv",  2));
  resolution_names_.insert(make_pair("152muv", 3));

  calibration_signals_.insert(make_pair("ramp",     0));
  calibration_signals_.insert(make_pair("triangle", 1));
  calibration_signals_.insert(make_pair("square",   2));
  calibration_signals_.insert(make_pair("sine",     3));

  ZeroMemory(&brainamp_settings_, sizeof(brainamp_settings_));

  brainamp_settings_.nChannels = 0;
  brainamp_settings_.nPoints = 0;
  brainamp_settings_.nLowImpedance = 0;
  brainamp_settings_.nHoldValue = 0x0;

  for (unsigned int n = 0; n < 256; n++)
  {
    brainamp_settings_.nChannelList[n] = 0;
    brainamp_settings_.n250Hertz[n] = 0;
    brainamp_settings_.nResolution[n] = 0;
    brainamp_settings_.nDCCoupling[n] = 0;
  }


  brainamp_calibration_settings_.nWaveForm  = 3;         // sine wave
  brainamp_calibration_settings_.nFrequency = 10000;     // 10 Hz

  setHardware(hw);

  brainamp_settings_.nChannels = channel_info_.size();

  checkHighestChannelNr( GetNrOfSimilarConnectedAmps() );

  if(ds_factor_)
  {
    initDownsamplingFilters();
    brainamp_settings_.nPoints = ds_factor_ * blocks_;

    raw_buffer_.resize( (nr_ch_ +1) * ds_factor_ * blocks_, 0);
    downsampling_buffer_.resize( nr_ch_ * ds_factor_ * blocks_, 0);
  }
  else
  {
    brainamp_settings_.nPoints = blocks_;
    raw_buffer_.resize((nr_ch_ +1) * blocks_, 0);
    downsampling_buffer_.resize((nr_ch_ +1) * blocks_, 0);
  }

  samples_.resize(nr_ch_ * blocks_, 0);
  data_.init(blocks_, nr_ch_, channel_types_);

  if(!OpenUSBDevice())
    throw(std::runtime_error("Error opening USB device!") );


  if(acqu_type_ == DataAcqu)
  {
    DWORD bytes_returned = 0;
    if (!DeviceIoControl(dev_handle_, IOCTL_BA_SETUP, &brainamp_settings_, sizeof(brainamp_settings_),
                       NULL, 0, &bytes_returned, NULL))
     throw(std::runtime_error("Setup failed, error code: " +  lexical_cast<string>(GetLastError() ) ) );
  }

  unsigned int nModule = driver_version_ % 10000;
  unsigned int nMinor = (driver_version_ % 1000000) / 10000;
  unsigned int nMajor = driver_version_ / 1000000;

  cout << endl;
  cout << " * Brainamp sucessfully initialized -- Mode: " << acqu_type_ << endl;
  cout << "    driver version: " << nMajor << "." << nMinor << "." << nModule << endl;
  cout << "    fs: " << fs_ << "Hz, nr of channels: " << nr_ch_ << ", blocksize: " << blocks_ << endl;
  cout << endl;

#ifdef DEBUG
  for (int n = 0; n < nr_ch_; n++)
  {
    cout << "ch: " << n << ";  nr:" <<  (int)brainamp_settings_.nChannelList[n] << ";  ";
    cout << "res: " <<  (int)brainamp_settings_.nResolution[n] << endl;
  }
#endif
}

//-----------------------------------------------------------------------------

BrainAmpSeries::~BrainAmpSeries()
{
  if(running_)
    stop();

  if (dev_handle_ != INVALID_HANDLE_VALUE)
    CloseHandle(dev_handle_);
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::run()
{
  uint16_t pullup = 0;
	DWORD bytes_returned = 0;
	if (!DeviceIoControl(dev_handle_, IOCTL_BA_DIGITALINPUT_PULL_UP, &pullup, sizeof(pullup),
                       NULL, 0, &bytes_returned, NULL))
    throw(std::runtime_error("Can't set pull up/down resistors, error code: "+  lexical_cast<string>(GetLastError() ) ) );

  if (!DeviceIoControl(dev_handle_, IOCTL_BA_START, &acqu_type_, sizeof(acqu_type_), NULL, 0,
                       &bytes_returned, NULL))
  {
    int error_code = 0;
    if(!DeviceIoControl(dev_handle_, IOCTL_BA_ERROR_STATE, NULL, 0, &error_code,
                        sizeof(error_code), &bytes_returned, NULL))
      throw(std::runtime_error("Run failed -- Could not retrieve error state!"  ));

      if(error_code != 0)
        throw(std::runtime_error("Run failed -- "  + getErrorMsg(error_code) ));

      throw(std::runtime_error("Run failed (check battery, power switch and connectors) -- Error code: "  +  lexical_cast<string>(GetLastError() ) ));
  }
  running_ = 1;

  cout << " * Brainamp sucessfully started" << endl;
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::stop()
{
  running_ = 0;
  DWORD bytes_returned = 0;
  if(!DeviceIoControl(dev_handle_, IOCTL_BA_STOP, NULL, 0, NULL, 0, &bytes_returned, NULL))
    throw(std::runtime_error("Stop failed, error code: "+  lexical_cast<string>(GetLastError() ) ) );

  cout << " * Brainamp sucessfully stopped" << endl;
}

//-----------------------------------------------------------------------------

SampleBlock<double> BrainAmpSeries::getSyncData()
{
  if(!running_)
    return(data_);

  if(!readData())
  {
    for(uint32_t n = 0; n < samples_.size(); n++)
      samples_[n] = 0;

    data_.setSamples(samples_);
  }
  else
    if(ds_factor_)
      doDownSamplingAndFillSampleVector();
    else
      fillSampleVector();

  data_.setSamples(samples_);
  return(data_);
}

//-----------------------------------------------------------------------------

SampleBlock<double> BrainAmpSeries::getAsyncData()
{
  throw(std::runtime_error("BrainAmp -- Async mode not supported yet!"));
  return(data_);
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setHardware(ticpp::Iterator<ticpp::Element>const &hw)
{
  checkMandatoryHardwareTags(hw);
  ticpp::Iterator<ticpp::Element> ds(hw->FirstChildElement(hw_devset_, true));

  setDeviceSettings(ds);

  ticpp::Iterator<ticpp::Element> cs(hw->FirstChildElement(hw_chset_, false));
  if (cs != cs.end())
  {
    for(ticpp::Iterator<ticpp::Element> it(cs); ++it != it.end(); )
      if(it->Value() == hw_chset_)
      {
        string ex_str(type_ + " -- ");
        ex_str += "Multiple channel_settings found!";
        throw(std::invalid_argument(ex_str));
      }
      setChannelSettings(cs);
  }

  setBrainAmpChannelList();
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setDeviceSettings(ticpp::Iterator<ticpp::Element>const &father)
{
  //  fs_ = BRAINAMP_SAMPLING_RATE;

  ticpp::Iterator<ticpp::Element> elem(father->FirstChildElement(hw_fs_,true));
  if(elem != elem.end())
    setBrainAmpSamplingRate(elem);

  elem = father->FirstChildElement(hw_channels_,false);
  if(elem != elem.end())
    setDeviceChannels(elem);

  elem = father->FirstChildElement(hw_blocksize_,false);
  if(elem != elem.end())
    setBlocks(elem);


  elem = father->FirstChildElement(hw_low_imp_,false);
  if(elem != elem.end())
    setDeviceLowImp(elem);

  elem = father->FirstChildElement(hw_trigger_value,false);
  if(elem != elem.end())
    setDeviceTriggerValue(elem);

  elem = father->FirstChildElement(hw_250lp_,false);
  if(elem != elem.end())
    setDeviceLowpass250(elem);

  elem = father->FirstChildElement(hw_dc_coupl_,false);
  if(elem != elem.end())
    setDeviceDCCoupling(elem);

  elem = father->FirstChildElement(hw_resolution_,false);
  if(elem != elem.end())
    setDeviceResolution(elem);

  elem = father->FirstChildElement(hw_calibration_,false);
  if(elem != elem.end())
    setDeviceCalibrationMode(elem);

}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setChannelSettings(ticpp::Iterator<ticpp::Element>const &father)
{
  ticpp::Iterator<ticpp::Element> elem(father->FirstChildElement(hw_chset_sel_,false));
  if (elem != elem.end())
    setChannelSelection(elem);

  setBrainAmpChannelList();

  elem = father->FirstChildElement(hw_250lp_,false);
  if(elem != elem.end())
    setChannelLowpass250(elem);

  elem = father->FirstChildElement(hw_dc_coupl_,false);
  if(elem != elem.end())
    setChannelDCDecoupling(elem);

  elem = father->FirstChildElement(hw_resolution_,false);
  if(elem != elem.end())
    setChannelResolution(elem);
}

//-----------------------------------------------------------------------------

std::string BrainAmpSeries::getErrorMsg(int error_code)
{
	uint32_t code = error_code & 0xffff;

	switch (code)
	{
		case 0:
			break;
		case 1:
			return(string("Connection between Brainamp and USB 2 Adapter / PCI is broken.\n"
				"Please check connectors, switch and battery power. After the\n"
				"connection is established and if you wish to continue the\n"
				"recording, please restart the program."));
		case 2:
			return(string(
				"The voltage in the amplifier is too low!\n"
				"Check the batteries!"));
		case 3:
			return(string(
				"Could not establish communication with the amplifier.\n"
				"Check the connectors and the battery power!"));
		case 4:
			return(string("Out of synch, Barker words missing!"));
		case 5:
			return(string(
				"Connection between USB 2 Adapter and Computer is broken.\n"
				"Monitoring or recording was interrupted. Please check\n"
				"the USB connectors."));
		default:
			return(string("Unknown Amplifier Error\n"));
	}
	return "";
}

//-----------------------------------------------------------------------------

bool BrainAmpSeries::OpenUSBDevice()
{
  DWORD dwFlags = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH;

  dev_handle_ = CreateFile(DEVICE_USB, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                           OPEN_EXISTING, dwFlags, NULL);

  if (dev_handle_ == INVALID_HANDLE_VALUE)
    return(false);

  DWORD dwBytesReturned;
  DeviceIoControl(dev_handle_, IOCTL_BA_DRIVERVERSION, NULL, 0, &driver_version_, sizeof(driver_version_),
    &dwBytesReturned, NULL);

  return(true);
}

//-----------------------------------------------------------------------------

boost::uint32_t BrainAmpSeries::GetNrOfSimilarConnectedAmps()
{
  vector<AmpType> amplifiers(MAX_NR_OF_AMPS, None);

	USHORT amps[MAX_NR_OF_AMPS];
	DWORD dwBytesReturned;

	DeviceIoControl(dev_handle_, IOCTL_BA_AMPLIFIER_TYPE, NULL, 0, amps, sizeof(amps),
			&dwBytesReturned, NULL);

	for(int i = 0; i < MAX_NR_OF_AMPS; i++)
		amplifiers[i] = static_cast<AmpType>(amps[i]);



  if(amplifiers[0] == None)
    return(0);

  int nAmps = 1;
	for(int i = 1; i < MAX_NR_OF_AMPS; i++)
		if(amplifiers[0] == amplifiers[i])
			nAmps++;

	return nAmps;
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::checkHighestChannelNr(boost::uint32_t nr_amps)
{
	int highest_ch = 0;
	for(int i = 0; i < brainamp_settings_.nChannels; i++)
		highest_ch = max(brainamp_settings_.nChannelList[i], highest_ch);

	unsigned int required_amps = (highest_ch + 1) / CHANNELS_PER_AMP;

  if (nr_amps < required_amps)
	{
    string ex_str("Not enough amplifiers connected -- required: ");
    ex_str += required_amps;
    ex_str += ";  connected: ";
    ex_str += nr_amps;
		throw(std::invalid_argument(ex_str));
	}

}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setBrainAmpChannelList()
{
  #ifdef DEBUG
    cout << "BrainAmpSeries: setBrainAmpChannels" << endl;
  #endif

  map<uint16_t, pair<string, uint32_t> >::iterator it(channel_info_.begin());
  map<uint16_t, pair<string, uint32_t> >::iterator stop(channel_info_.end());
  unsigned int n = 0;

  for(  ; it != stop; it++)
  {
    brainamp_settings_.nChannelList[n] = boost::numeric_cast<CHAR>( it->first) -1;
    n++;
  }
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setBrainAmpSamplingRate(ticpp::Iterator<ticpp::Element>const &elem)
{
  setSamplingRate(elem);

  if(BRAINAMP_SAMPLING_RATE < fs_)
    throw(std::invalid_argument("Invalid sampling rate -- max. fs: "
                                + boost::lexical_cast<string>(BRAINAMP_SAMPLING_RATE)) );

  if( fmod(BRAINAMP_SAMPLING_RATE, fs_) != 0.0)
    throw(std::invalid_argument("Invalid sampling rate -- max. fs (5kHz) must be divisible by desired fs without a rest!"));

  if(BRAINAMP_SAMPLING_RATE == fs_)
    ds_factor_ = 0;
  else
  {
    ds_factor_ = boost::numeric_cast<unsigned int>(BRAINAMP_SAMPLING_RATE / fs_);
    cout << "    Using downsampling by factor: " << ds_factor_;
    cout << " ( ... " << BRAINAMP_SAMPLING_RATE/ds_factor_ << " Hz sampling rate)" << endl;
  }
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setDeviceLowImp(ticpp::Iterator<ticpp::Element>const &elem)
{
  if(equalsYesOrNo(elem->GetText(true)))
    brainamp_settings_.nLowImpedance = 10;
  else
    brainamp_settings_.nLowImpedance = 0;
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setDeviceTriggerValue(ticpp::Iterator<ticpp::Element>const &elem)
{
  uint16_t value = 0;
  try{
    value = lexical_cast<uint16_t>( elem->GetText(true) );
  }
  catch(bad_lexical_cast &)
  {
    string ex_str(type_ + " -- ");
    ex_str += "Tag <"+ hw_trigger_value + "> : Value is not a unsigned short (16bit)!";
    throw(std::invalid_argument(ex_str));
  }

  brainamp_settings_.nHoldValue = value;
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setDeviceLowpass250(ticpp::Iterator<ticpp::Element>const &elem)
{
  bool lp250 = equalsOnOrOff(elem->GetText(true));
  for (unsigned int n = 0; n < nr_ch_; n++)
    brainamp_settings_.n250Hertz[n] = lp250;
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setDeviceDCCoupling(ticpp::Iterator<ticpp::Element>const &elem)
{
  bool dc = equalsOnOrOff(elem->GetText(true));
  for (unsigned int n = 0; n < nr_ch_; n++)
    brainamp_settings_.nDCCoupling[n] = dc;
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setDeviceResolution(ticpp::Iterator<ticpp::Element>const &elem)
{
  string str(elem->GetText(true));
  map<string, unsigned int>::iterator it;
  it = resolution_names_.find(to_lower_copy(str));
  if(it == resolution_names_.end())
  {
    cout << "  * Possible resolutions: " << endl;
    cout << "     ";
    for(it = resolution_names_.begin(); it != resolution_names_.end(); it++ )
      cout << it->first << ", ";
    cout  << endl;

    string e = "Resolution \"" + str + "\" not found -- please also check spelling!";
    throw(std::invalid_argument(e));
  }

  for (unsigned int n = 0; n < nr_ch_; n++)
    brainamp_settings_.nResolution[n] = resolution_names_[to_lower_copy(str)];
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setDeviceCalibrationMode(ticpp::Iterator<ticpp::Element>const &elem)
{
  if(!elem.Get()->HasAttribute(hw_calib_on_))
  {
    string ex_str(type_ + " -- ");
    ex_str += "Tag <"+hw_calibration_+"> given, "+hw_calib_on_+" attribute not given!";
    throw(std::invalid_argument(ex_str));
  }
  if(!elem.Get()->HasAttribute(hw_calib_sig_))
  {
    string ex_str(type_ + " -- ");
    ex_str += "Tag <"+hw_calibration_+"> given, "+hw_calib_sig_+" attribute not given!";
    throw(std::invalid_argument(ex_str));
  }
  if(!elem.Get()->HasAttribute(hw_calib_freq_))
  {
    string ex_str(type_ + " -- ");
    ex_str += "Tag <"+hw_calibration_+"> given, "+hw_calib_freq_+" attribute not given!";
    throw(std::invalid_argument(ex_str));
  }




  string str( elem.Get()->GetAttribute(hw_calib_sig_)  );
  map<string, unsigned int>::iterator it;
  it = calibration_signals_.find(to_lower_copy(str));
  if(it == calibration_signals_.end())
  {
    cout << "  * Possible calibration signal types: " << endl;
    cout << "     ";
    for(it = calibration_signals_.begin(); it != calibration_signals_.end(); it++ )
      cout << it->first << ", ";
    cout  << endl;

    string e = "Calibration signal \"" + str + "\" not found -- please also check spelling!";
    throw(std::invalid_argument(e));
  }

  brainamp_calibration_settings_.nWaveForm = calibration_signals_[str];

  double freq = 0;

  try{
    freq = lexical_cast<double>( elem.Get()->GetAttribute(hw_calib_freq_) );
  }
  catch(bad_lexical_cast &)
  {
    string ex_str(type_ + " -- ");
    ex_str += "Tag <"+ hw_calib_freq_ + "> : Value is not a floating point number!";
    throw(std::invalid_argument(ex_str));
  }

  brainamp_calibration_settings_.nFrequency =
      boost::numeric_cast<uint32_t>(floor(freq * 1000));  // value has to be in millihertz

  if(equalsYesOrNo( elem.Get()->GetAttribute(hw_calib_on_) ) )
  {
    acqu_type_ = TestSignal;
    cout << "  -- Brainamp: Using calibration signal -- ";
    cout << elem.Get()->GetAttribute(hw_calib_sig_) << ";  ";
    cout << elem.Get()->GetAttribute(hw_calib_freq_) << " Hz" << endl;

    DWORD bytes_returned = 0;
    if (!DeviceIoControl(dev_handle_, IOCTL_BA_CALIBRATION_SETTINGS, &brainamp_calibration_settings_,
                         sizeof(brainamp_calibration_settings_), NULL, 0, &bytes_returned, NULL))
      throw(std::runtime_error("Calibration failed, error code: "+  lexical_cast<string>(GetLastError() ) ) );
  }

}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setChannelLowpass250(ticpp::Iterator<ticpp::Element>const &father)
{
  ticpp::Iterator<ticpp::Element> elem = father->FirstChildElement(hw_chset_ch_,false);

  cout << " * Brainamp -- 250 Hz lowpass filter activated on channels:" << endl;

  for(  ; elem != elem.end(); elem++)
    if(elem->Value() == hw_chset_ch_)
    {
      if(!elem.Get()->HasAttribute(hw_ch_nr_))
      {
        string ex_str(type_ + " -- ");
        ex_str += "Tag <"+hw_250lp_+"> given, but channel number ("+hw_ch_nr_+") not given!";
        throw(std::invalid_argument(ex_str));
      }
      if(!elem.Get()->HasAttribute(hw_value_))
      {
        string ex_str(type_ + " -- ");
        ex_str += "Tag <"+hw_250lp_+"> given, but attribute ("+hw_value_+") (on/off) not given!";
        throw(std::invalid_argument(ex_str));
      }

      uint16_t ch = 0;
      try{
        ch = lexical_cast<uint16_t>( elem.Get()->GetAttribute(hw_ch_nr_) );
      }
      catch(bad_lexical_cast &)
      {
        string ex_str(type_ + " -- ");
        ex_str += "Tag <"+ hw_250lp_ + "> : Channel is not a number!";
        throw(std::invalid_argument(ex_str));
      }
      if( channel_info_.find(ch) ==  channel_info_.end() )
      {
        string ex_str(type_ + " -- ");
        ex_str += "Tag <"+ hw_250lp_ + "> - Channel "+ lexical_cast<string>(ch) +" not set for recording!";
        throw(std::invalid_argument(ex_str));
      }

      bool value = equalsOnOrOff( elem.Get()->GetAttribute(hw_value_) );

      brainamp_settings_.n250Hertz[ch-1] = value;
      cout << "  ... ch " << ch << ": " << value << "; ";
    }
    else
      throw(std::invalid_argument("BrainAmpSeries::setChannelLowpass250 -- Tag not equal to \""+hw_chset_ch_+"\"!"));
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setChannelDCDecoupling(ticpp::Iterator<ticpp::Element>const &father)
{
  ticpp::Iterator<ticpp::Element> elem = father->FirstChildElement(hw_chset_ch_,false);

  cout << " * Brainamp -- DC coupling activated on channels:" << endl;

  for(  ; elem != elem.end(); elem++)
    if(elem->Value() == hw_chset_ch_)
    {
      if(!elem.Get()->HasAttribute(hw_ch_nr_))
      {
        string ex_str(type_ + " -- ");
        ex_str += "Tag <"+hw_dc_coupl_+"> given, but channel number ("+hw_ch_nr_+") not given!";
        throw(std::invalid_argument(ex_str));
      }
      if(!elem.Get()->HasAttribute(hw_value_))
      {
        string ex_str(type_ + " -- ");
        ex_str += "Tag <"+hw_dc_coupl_+"> given, but attribute ("+hw_value_+") (on/off) not given!";
        throw(std::invalid_argument(ex_str));
      }

      uint16_t ch = 0;
      try{
        ch = lexical_cast<uint16_t>( elem.Get()->GetAttribute(hw_ch_nr_) );
      }
      catch(bad_lexical_cast &)
      {
        string ex_str(type_ + " -- ");
        ex_str += "Tag <"+ hw_dc_coupl_ + "> : Channel is not a number!";
        throw(std::invalid_argument(ex_str));
      }
      if( channel_info_.find(ch) ==  channel_info_.end() )
      {
        string ex_str(type_ + " -- ");
        ex_str += "Tag <"+ hw_dc_coupl_ + "> - Channel "+ lexical_cast<string>(ch) +" not set for recording!";
        throw(std::invalid_argument(ex_str));
      }

      bool value = equalsOnOrOff( elem.Get()->GetAttribute(hw_value_) );

      brainamp_settings_.nDCCoupling[ch-1] = value;
      cout << "  ... ch " << ch << ": " << value << "; ";
    }
    else
      throw(std::invalid_argument("BrainAmpSeries::setChannelDCDecoupling -- Tag not equal to \""+hw_chset_ch_+"\"!"));
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::setChannelResolution(ticpp::Iterator<ticpp::Element>const &father)
{
  ticpp::Iterator<ticpp::Element> elem = father->FirstChildElement(hw_chset_ch_,false);

  cout << " * Brainamp -- Individual resolution set on channels:" << endl;

  for(  ; elem != elem.end(); elem++)
    if(elem->Value() == hw_chset_ch_)
    {
      if(!elem.Get()->HasAttribute(hw_ch_nr_))
      {
        string ex_str(type_ + " -- ");
        ex_str += "Tag <"+hw_resolution_+"> given, but channel number ("+hw_ch_nr_+") not given!";
        throw(std::invalid_argument(ex_str));
      }
      if(!elem.Get()->HasAttribute(hw_value_))
      {
        string ex_str(type_ + " -- ");
        ex_str += "Tag <"+hw_resolution_+"> given, but attribute ("+hw_value_+") not given!";
        throw(std::invalid_argument(ex_str));
      }

      uint16_t ch = 0;
      try{
        ch = lexical_cast<uint16_t>( elem.Get()->GetAttribute(hw_ch_nr_) );
      }
      catch(bad_lexical_cast &)
      {
        string ex_str(type_ + " -- ");
        ex_str += "Tag <"+ hw_resolution_ + "> : Channel is not a number!";
        throw(std::invalid_argument(ex_str));
      }
      if( channel_info_.find(ch) ==  channel_info_.end() )
      {
        string ex_str(type_ + " -- ");
        ex_str += "Tag <"+ hw_resolution_ + "> - Channel "+ lexical_cast<string>(ch) +" not set for recording!";
        throw(std::invalid_argument(ex_str));
      }

      string str( elem.Get()->GetAttribute(hw_value_) );
      map<string, unsigned int>::iterator it;
      it = resolution_names_.find(to_lower_copy(str));
      if(it == resolution_names_.end())
      {
        cout << "  * Possible resolutions: " << endl;
        cout << "     ";
        for(it = resolution_names_.begin(); it != resolution_names_.end(); it++ )
          cout << it->first << ", ";
        cout  << endl;

        string e = "Resolution \"" + str + "\" not found -- please also check spelling!";
        throw(std::invalid_argument(e));
      }

      brainamp_settings_.nResolution[ch-1] = resolution_names_[to_lower_copy(str)];
      cout << "  ... ch " << ch << ": " << str << "; ";
    }
    else
      throw(std::invalid_argument("BrainAmpSeries::setChannelResolution -- Tag not equal to \""+hw_chset_ch_+"\"!"));
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::initDownsamplingFilters()
{
  for (unsigned int n = 0; n < nr_ch_; n++)
    lp_filters_.push_back( IIRButterLpf<double>( BRAINAMP_SAMPLING_RATE, fs_/2.0, 5, 4, 0) );
}

//-----------------------------------------------------------------------------

bool BrainAmpSeries::readData()
{
  #ifdef DEBUG
    cout << "BrainAmpSeries::readData()" << endl;
  #endif

  int error_code = 0;
  DWORD bytes_returned = 0;
  int bytes_to_receive = raw_buffer_.size() * sizeof(short);

  do
  {
    if(!DeviceIoControl(dev_handle_, IOCTL_BA_ERROR_STATE, NULL, 0, &error_code,
                        sizeof(error_code), &bytes_returned, NULL))
      throw(std::runtime_error("Acquisition Error -- Could not retrieve error state!"  ));

    if(error_code != 0)
      throw(std::runtime_error("Acquisition Error -- "  + getErrorMsg(error_code) ));

    if(!ReadFile(dev_handle_, &raw_buffer_[0], bytes_to_receive, &bytes_returned, NULL))
    {
      cerr << "Acquisition Error (filling samples with '0'): " <<  GetLastError() << endl;
      return(false);
    }

    if(!bytes_returned)
      Sleep(1);
  }
  while(!bytes_returned && running_);

  return(true);
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::fillSampleVector()
{
  unsigned int nr_markers = 0;
  for(unsigned int i = 0; i < blocks_; i++)
  {
    for(unsigned int j = 0; j < nr_ch_; j++)
      samples_[i + j*blocks_] =
          scaling_values_[ brainamp_settings_.nResolution[j] ] *  raw_buffer_[ i*nr_ch_ + j +nr_markers ];

    nr_markers++;
  }
  // MISSING:  Marker logic!!
}

//-----------------------------------------------------------------------------

void BrainAmpSeries::doDownSamplingAndFillSampleVector()
{
  #ifdef DEBUG
    cout << "BrainAmpSeries::doDownSamplingAndFillSampleVector()" << endl;
  #endif

  unsigned int nr_markers = 0;
  for(unsigned int i = 0; i < blocks_*ds_factor_; i++)
  {
    for(unsigned int j = 0; j < nr_ch_; j++)
      downsampling_buffer_[i + j*blocks_*ds_factor_] =
         lp_filters_[j].clock( scaling_values_[ brainamp_settings_.nResolution[j] ]
                               *  raw_buffer_[ i*nr_ch_ + j +nr_markers ] );

    nr_markers++;
  }

  //cout << "Buffer filled -- Size: " << downsampling_buffer_.size();
  //cout << ";  Factor: " << ds_factor_<< std::flush << endl;

  for(unsigned int n = ds_factor_, sample = 0;
      n <= downsampling_buffer_.size();
      n+=ds_factor_, sample++ )
  {
    //cout << "n: " << n << ";  Sample: " << sample << std::flush << endl;
    samples_[sample] = downsampling_buffer_[n-1];
  }
}

//-----------------------------------------------------------------------------

}