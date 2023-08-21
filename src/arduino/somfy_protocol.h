//------------------------------------------------------------------------------------------------------------------
#ifndef __SOMFY_PROTOCOL_H__
#define __SOMFY_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
#include "Protocol.h"
//------------------------------------------------------------------------------------------------------------------
class SomfyRTSProtocol: public Protocol
{
    public:
       SomfyRTSProtocol();
       String getName();
       bool fromPulses(int pulses, uint16_t* buffer);
       bool toPulses(uint16_t* buffer, int maxPulses,int* pulses);
       int getMinimalTime();
       int getMaximalTime();
       int getInterFrameTime();
       int getFrameTime();
       String describe(uint32_t ts);
       int getMinPulses(void);

    private:
        uint32_t code0;
        uint32_t code1;
        uint32_t code2;        
};
//------------------------------------------------------------------------------------------------------------------
#endif//__SOMFY_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
