//------------------------------------------------------------------------------------------------------------------
#ifndef __RETEKESS_PROTOCOL_H__
#define __RETEKESS_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
#include "Protocol.h"
//------------------------------------------------------------------------------------------------------------------
class RetekessProtocol: public Protocol
{
    public:
        
       RetekessProtocol();
       String getName();
       bool fromPulses(int pulses, uint16_t* buffer);
       bool toPulses(uint16_t* buffer, int maxPulses,int* pulses, int frameNo);
       void setData(uint32_t code);
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
#endif//__RETEKESS_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
