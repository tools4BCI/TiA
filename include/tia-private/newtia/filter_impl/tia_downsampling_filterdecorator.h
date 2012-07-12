#ifndef TIA_DOWNSAMPLING_FILTERDECORATOR_H
#define TIA_DOWNSAMPLING_FILTERDECORATOR_H

#include "../filter/tia_datapacket_custom_filter.h"

#include "extern/include/filterTools/iirbutterlpf.h"

namespace tia
{

class DownsamplingFilterDecorator : public CustomPacketFilterDecorator
{
    typedef std::map<boost::uint32_t, boost::uint16_t > SignalDsFactorMap;
    typedef std::map<boost::uint32_t, IIRButterLpf<double> > SignalFilterMap;
public:
    DownsamplingFilterDecorator(CustomPacketFilterPtr decorated_filter);

    virtual void applyFilter(DataPacket &packet);

    virtual const SignalInfo &getSignalInfoAfterFiltering();

private:
    SignalInfo modified_signal_info_;
    SignalDsFactorMap signal_ds_factor_map_;

    SignalFilterMap lp_filters_;

};

}

#endif // TIA_DOWNSAMPLING_FILTERDECORATOR_H
