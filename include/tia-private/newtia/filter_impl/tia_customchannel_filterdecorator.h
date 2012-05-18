#ifndef TIA_CUSTOMCHANNEL_FILTERDECORATOR_H
#define TIA_CUSTOMCHANNEL_FILTERDECORATOR_H

#include "../filter/tia_datapacket_custom_filter.h"


namespace tia
{

class CustomChannelFilterDecorator : public CustomPacketFilterDecorator
{

    typedef std::map<boost::uint32_t, std::vector<boost::uint32_t> > ChannelNrMap;
public:
    CustomChannelFilterDecorator(CustomPacketFilter &decorated_filter);   

    virtual void applyFilter(DataPacket &packet);

    virtual const SignalInfo &getSignalInfoAfterFiltering();

private:
    std::vector<boost::uint32_t> signals_to_exclude_;
    ChannelNrMap channels_to_exclude_;
    SignalInfo modified_signal_info_;
};

}

#endif // TIA_CUSTOMCHANNEL_FILTERDECORATOR_H
