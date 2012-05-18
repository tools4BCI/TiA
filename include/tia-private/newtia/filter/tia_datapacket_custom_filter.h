#ifndef TIA_DATAPACKET_CUSTOM_FILTER_H
#define TIA_DATAPACKET_CUSTOM_FILTER_H

#include "tia/data_packet_interface.h"
#include "tia/ss_meta_info.h"
#include "tia/constants.h"

#include <iostream>

namespace tia
{

//-----------------------------------------------------------------------------
class CustomPacketFilter
{
    friend class CustomPacketFilterDecorator;
public:
    CustomPacketFilter(const SignalInfo &default_info, const SignalInfo &custom_info)
        : default_sig_info_ (default_info), custom_sig_info_ (custom_info)
    { }

    virtual void applyFilter(DataPacket &packet) = 0;

    virtual const SignalInfo &getSignalInfoAfterFiltering() = 0;

    virtual bool isApplicable() = 0;

    virtual bool hasConfiguredWork() = 0;

protected:
    const SignalInfo &default_sig_info_;
    const SignalInfo &custom_sig_info_;
    Constants constants_;
};

//-----------------------------------------------------------------------------

class DummyCustomPacketFilter : public CustomPacketFilter
{
public:
    DummyCustomPacketFilter(const SignalInfo &default_info, const SignalInfo &custom_info)
        : CustomPacketFilter(default_info, custom_info)
    { }


    virtual void applyFilter(DataPacket &)
    { }

    virtual const SignalInfo &getSignalInfoAfterFiltering()
    {
        return default_sig_info_;
    }

    virtual bool isApplicable()
    {
        return true;
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
    CustomPacketFilterDecorator(CustomPacketFilter &decorated_filter)
        : CustomPacketFilter(decorated_filter.getSignalInfoAfterFiltering(), decorated_filter.custom_sig_info_),
          decorated_filter_ (decorated_filter), is_applicable_(false), has_configured_work_ (false)
    { }

    virtual void applyFilter(DataPacket &packet) = 0;

    virtual const SignalInfo &getSignalInfoAfterFiltering() = 0;

    virtual bool isApplicable()
    {
        return is_applicable_;
    }

    virtual bool hasConfiguredWork()
    {
        return has_configured_work_;
    }


protected:
    CustomPacketFilter &decorated_filter_;
    bool is_applicable_;
    bool has_configured_work_;
};



}


#endif // TIA_DATAPACKET_CUSTOM_FILTER_H
