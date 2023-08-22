//------------------------------------------------------------------------------------------------------------------
#ifndef __EV1527_PROTOCOL_H__
#define __EV1527_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
#include "Protocol.h"
//------------------------------------------------------------------------------------------------------------------
class EV1527Protocol: public Protocol
{
    public:
        
       EV1527Protocol();
       String getName();
       bool fromPulses(int pulses, uint16_t* buffer);
       bool toPulses(uint16_t* buffer, int maxPulses,int* pulses, int frameNo);
       void setData(uint32_t data, uint8_t btn);
       int getMinimalTime();
       int getMaximalTime();
       int getInterFrameTime();
       int getFrameTime();
       String describe(uint32_t ts);
       int getMinPulses(void);

    private:
        uint32_t code;
        int btn;

        int dataToBytes(void);
};
//------------------------------------------------------------------------------------------------------------------
#endif//__EV1527_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
