//------------------------------------------------------------------------------------------------------------------
#include "tesla_um2_protocol.h"
//------------------------------------------------------------------------------------------------------------------
#define TESLA_UM2_TIME_US 400
#define TESLA_UM2_PREAMBLE_PULSES 25
#define TESLA_UM2_POST_PREAMBLE_PULSE_LEN 3
#define TESLA_UM2_CODED_BITS 150
#define TESLA_UM2_FRAME_PULSES (25 + (3 * 70) + 1)
#define TESLA_UM2_GUARD_TIME_US (64 * TESLA_UM2_TIME_US)
static uint8_t tesla_um2_tx[] = {0xAA,0xAA,0xAA,0x8A,0xCB,0x32,0xCC,0xCC,0xCB,0x4D,0x2D,0x4A,0xD3,0x4C,0xAB,0x4B,0x15,0x96,0x65,
    0x99,0x99,0x96,0x9A,0x5A,0x95,0xA6,0x99,0x56,0x96,0x2B,0x2C,0xCB,0x33,0x33,0x2D,0x34,0xB5,0x2B,0x4D,0x32,0xAD,0x28 };
//------------------------------------------------------------------------------------------------------------------
TeslaUM2Protocol::TeslaUM2Protocol()
{
    pulse_divisor = TESLA_UM2_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
String TeslaUM2Protocol::getName(void)
{
    return "tum2";  
}
//------------------------------------------------------------------------------------------------------------------
int TeslaUM2Protocol::getMinimalTime()
{
    return (TESLA_UM2_TIME_US -(TESLA_UM2_TIME_US / 4));
}
//------------------------------------------------------------------------------------------------------------------
int TeslaUM2Protocol::getInterFrameTime()
{
    return TESLA_UM2_TIME_US * 64;
}
//------------------------------------------------------------------------------------------------------------------
int TeslaUM2Protocol::getMaximalTime()
{
   return (42 * 8) * TESLA_UM2_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
int TeslaUM2Protocol::getFrameTime()
{
    return (42 * 8) * TESLA_UM2_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
int TeslaUM2Protocol::getMinPulses(void)
{
    return 42 * 4;
}
//------------------------------------------------------------------------------------------------------------------
bool TeslaUM2Protocol::fromPulses(int pulses, uint16_t* buffer)
{
    return false; // no rx
}
//------------------------------------------------------------------------------------------------------------------
void TeslaUM2Protocol::setData(uint32_t data)
{
    code0 = data;
}
//------------------------------------------------------------------------------------------------------------------
int TeslaUM2Protocol::dataToBytes(void)
{
    bytesClear();
        
    return TESLA_UM2_CODED_BITS;
}
//------------------------------------------------------------------------------------------------------------------
bool TeslaUM2Protocol::toPulses(uint16_t* buffer, int maxPulses,int* pulses, int frameNo)
{
    int pls = 0;

    int len = sizeof(tesla_um2_tx);

    int prev_bit = 0;

    for (int i=0; i< (len * 8); i++)
    {                
         int bit = (tesla_um2_tx[i/8] >> (7-(i&7))) & 1;
         if (prev_bit != bit || i == 0)
         {
             buffer[pls++] = pulseDuration(1);
         }
         else
             buffer[pls-1] += pulseDuration(1);

         prev_bit = bit;
    }   
    
    *pulses = pls;

    return true;
}
//------------------------------------------------------------------------------------------------------------------
String TeslaUM2Protocol::describe(uint32_t ts)
{
      char buffer[128];
     
      sprintf(buffer, "{ \"prot\":\"%s\", \"ts\": %d  }",
          getName().c_str(), ts);
      return buffer;
}
//------------------------------------------------------------------------------------------------------------------
