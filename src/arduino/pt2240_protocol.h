//------------------------------------------------------------------------------------------------------------------
#ifndef __PT2240_PROTOCOL_H__
#define __PT2240_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
#include "Protocol.h"
//------------------------------------------------------------------------------------------------------------------
class PT2240Protocol: public Protocol
{
    public:
        
       PT2240Protocol();
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
        uint32_t code;

        int dataToBytes(void);
};
//------------------------------------------------------------------------------------------------------------------
#endif//__PT2240_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
