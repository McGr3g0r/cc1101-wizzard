//------------------------------------------------------------------------------------------------------------------
#ifndef __TESLA_UM2_PROTOCOL_H__
#define __TESLA_UM2_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
#include "Protocol.h"
//------------------------------------------------------------------------------------------------------------------
class TeslaUM2Protocol: public Protocol
{
    public:
        
       TeslaUM2Protocol();
       String getName();
       bool fromPulses(int pulses, uint16_t* buffer);
       bool toPulses(uint16_t* buffer, int maxPulses,int* pulses, int frameNo);
       void setData(uint32_t data);
       int getMinimalTime();
       int getMaximalTime();
       int getInterFrameTime();
       int getFrameTime();
       String describe(uint32_t ts);
       int getMinPulses(void);

    private:
        uint32_t code0;

        int dataToBytes(void);
};
//------------------------------------------------------------------------------------------------------------------
#endif//__TESLA_UM2_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
