//------------------------------------------------------------------------------------------------------------------
#ifndef __EV1527_PROTOCOL_H__
#define __EV1527_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
#include "Protocol.h"
//------------------------------------------------------------------------------------------------------------------
class EV1527Protocol: public Protocol
{
    public:
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
        uint32_t code;
        int btn;

        int dataToBytes(void);
};
//------------------------------------------------------------------------------------------------------------------
#endif//__EV1527_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
