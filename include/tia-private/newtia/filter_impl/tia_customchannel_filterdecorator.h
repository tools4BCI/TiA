#ifndef TIA_CUSTOMCHANNEL_FILTERDECORATOR_H
#define TIA_CUSTOMCHANNEL_FILTERDECORATOR_H

#include "../filter/tia_datapacket_custom_filter.h"

namespace tia
{

class CustomChannelFilterDecorator : public CustomPacketFilterDecorator
{
public:
    CustomChannelFilterDecorator(CustomPacketFilter &decorated_filter);

    virtual void applyFilter(DataPacket &packet);

private:
    std::vector<boost::uint32_t> signals_to_exclude_;
    std::map<boost::uint32_t, std::vector<boost::uint32_t> > channels_to_include_;
};

}

#endif // TIA_CUSTOMCHANNEL_FILTERDECORATOR_H
