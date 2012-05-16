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
};

}

#endif // TIA_CUSTOMCHANNEL_FILTERDECORATOR_H
