//------------------------------------------------------------------------------------------------------------------
#ifndef __DB200_PROTOCOL_H__
#define __DB200_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
#include "Protocol.h"
//------------------------------------------------------------------------------------------------------------------
class DB200Protocol: public Protocol
{
    public:
        
       DB200Protocol();
       String getName();
       bool fromPulses(int pulses, uint16_t* buffer);
       bool toPulses(uint16_t* buffer, int maxPulses,int* pulses, int frameNo);
       void setData(uint8_t data);
       int getMinimalTime();
       int getMaximalTime();
       int getInterFrameTime();
       int getFrameTime();
       String describe(uint32_t ts);
       int getMinPulses(void);

    private:
        uint8_t code;

        int dataToBytes(void);
};
//------------------------------------------------------------------------------------------------------------------
#endif//__DB200_PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
