//------------------------------------------------------------------------------------------------------------------
#ifndef __HCS200_PROTOCOL_H__
#define __HCS200_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
#include "Protocol.h"
//------------------------------------------------------------------------------------------------------------------
class HCS200Protocol: public Protocol
{
    public:
        
       HCS200Protocol();
       String getName();
       bool fromPulses(int pulses, uint16_t* buffer);
       bool toPulses(uint16_t* buffer, int maxPulses,int* pulses, int frameNo);
       int getMinimalTime();
       int getMaximalTime();
       int getInterFrameTime();
       int getFrameTime();
       String describe(uint32_t ts);
       int getMinPulses(void);

    private:
        uint32_t enc;
        uint32_t serial;
        uint8_t  btn;
        bool     learn;
        bool     battery_low;
        bool     rpt;

        int dataToBytes(void);
};
//------------------------------------------------------------------------------------------------------------------
#endif//__HCS200_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
