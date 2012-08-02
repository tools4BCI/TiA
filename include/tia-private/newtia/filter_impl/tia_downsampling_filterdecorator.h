#ifndef TIA_DOWNSAMPLING_FILTERDECORATOR_H
#define TIA_DOWNSAMPLING_FILTERDECORATOR_H

#include "../filter/tia_datapacket_custom_filter.h"

#include "downsampling_filterparam.h"

namespace tia
{

class DownsamplingFilterDecorator : public CustomPacketFilterDecorator
{    
    typedef std::map<boost::uint32_t, DownsamplingFilterParam *> SignalFilterMap;
public:
    DownsamplingFilterDecorator(CustomPacketFilterPtr decorated_filter);

    ~DownsamplingFilterDecorator();

    virtual void applyFilter(DataPacket &packet);

    virtual const SignalInfo &getSignalInfoAfterFiltering();

private:
    SignalInfo modified_signal_info_;    

    SignalFilterMap ds_filters_;

};

}

#endif // TIA_DOWNSAMPLING_FILTERDECORATOR_H
