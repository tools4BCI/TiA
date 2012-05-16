#include "tia_customchannel_filterdecorator.h"

#include <iostream>

namespace tia
{

CustomChannelFilterDecorator::CustomChannelFilterDecorator(CustomPacketFilter &decorated_filter)
    : CustomPacketFilterDecorator(decorated_filter)
{
    //TODO: extract appropriate data for later filtering and
    //      set is_applicalbe and has_configured_work_ flags!
}

//-----------------------------------------------------------------------------

void CustomChannelFilterDecorator::applyFilter(DataPacket &packet)
{
    decorated_filter_.applyFilter(packet);

    std::cout << " chan filter ";

    //TODO: filter the data in the packet!
}

}
