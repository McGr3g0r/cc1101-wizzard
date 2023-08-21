//------------------------------------------------------------------------------------------------------------------
#include "somfy_protocol.h"
//------------------------------------------------------------------------------------------------------------------
/*
Somfy RTS Keytis

Preamble 12 times  2560us HIGH then 2560us LOW
4810us HIGH - soft sync
166 times half-clock(640us) manchester encoded bits

Frame repeat

Preamble 6 times  2560us HIGH then 2560us LOW
4810us HIGH - soft sync
162 times half-clock(640us) manchester encoded bits

*/
#define HALF_TIME_CLOCK 640
#define SOMFY_GUARD_TIME_US 1200
#define PREAMBLE_PULSES 4
#define SOFT_SYNC_PULSES 7.5
#define SOFT_SYNC_US 4800
#define SOMFY_CODED_BITS 81
#define SOMFY_FRAME_PULSES (25+81)
//------------------------------------------------------------------------------------------------------------------
SomfyRTSProtocol::SomfyRTSProtocol()
{
  pulse_divisor = HALF_TIME_CLOCK;
}
//------------------------------------------------------------------------------------------------------------------
String SomfyRTSProtocol::getName(void)
{
    return "Somfy RTS Keytis";  
}
//------------------------------------------------------------------------------------------------------------------
int SomfyRTSProtocol::getMinimalTime()
{
    return (HALF_TIME_CLOCK -(HALF_TIME_CLOCK / 2));
}
//------------------------------------------------------------------------------------------------------------------
int SomfyRTSProtocol::getInterFrameTime()
{
    return SOMFY_GUARD_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
int SomfyRTSProtocol::getMaximalTime()
{
   return 2 * HALF_TIME_CLOCK;
}
//------------------------------------------------------------------------------------------------------------------
int SomfyRTSProtocol::getFrameTime()
{
    return (25 + 2 * 81) * HALF_TIME_CLOCK;
}
//------------------------------------------------------------------------------------------------------------------
int SomfyRTSProtocol::getMinPulses(void)
{
    return SOMFY_FRAME_PULSES;
}

//------------------------------------------------------------------------------------------------------------------
bool SomfyRTSProtocol::fromPulses(int pulses, uint16_t* buffer)
{
        int i;
        int length;
        int preambule = 0;
        int pre_error = 0;
        
        //12 pulses Preamble, 1 pulse soft sync, 166 halfbits       
      
        if (pulses < SOMFY_FRAME_PULSES)
           return false;
        
        for (i=0; i< pulses; i++)
        {
          int t = pulseLen(buffer[i]);
          if (t >=PREAMBLE_PULSES && t <= PREAMBLE_PULSES+1)
          {      
            preambule++;
            if (preambule  == 24 || (preambule == 12 && i > 30 /* repeated frame*/))
            {
              i++;
                break;
            }
          }
          else
          {
            preambule = 0;
          }
        }
        

        if (preambule < 12 && preambule < 24) // should be 24 or 12 for repeated frame
            return false;
                     
        //should be SOFT_SYNC_PULSES --> 4800us
        if (buffer[i] >= SOFT_SYNC_US-(HALF_TIME_CLOCK/2) && buffer[i] <= SOFT_SYNC_US+(HALF_TIME_CLOCK/2)) {
            i++;
        }
        else {
              return false;
        }
            
        length = 0;
        int bits = 0;
        
        bytesClear();
        
        for (; i< pulses - 1; i+=2)
        {
           //Manchester encoding 
           int p0 = pulseLen(buffer[i]);
           int p1 = pulseLen(buffer[i+1]);
           
           if (p0==1 && p1 ==1) //NRZ double transition means HIGH
           {
              length++;
              bits <<= 1;
              bits |= 1;
           }
           else if (p0==2) //NRZ no transition means LOW
           {
              length++;
              bits <<= 1;
              bits |= 0;
              i--;
           }
           else if (p0==1 && p1 ==3) //NRZ double transition plus frame gap ??
           {
              length++;
              bits <<= 1;
              bits |= 1;
           }
           else        
           {
              return false;
           }

           if ((length & 7) == 0)
           {
              bytesAdd(bits);
              bits = 0;
           }  
           
           
           if (length >= SOMFY_CODED_BITS)
               break;
        }
        
        
        if ((length & 7) != 0)
        {
             bytesAdd(bits);
        }
        
        if (length < SOMFY_CODED_BITS)
           return false;
        else
           return true;
}
//------------------------------------------------------------------------------------------------------------------
bool SomfyRTSProtocol::toPulses(uint16_t* buffer, int maxPulses,int* pulses)
{
  return false;
}
//------------------------------------------------------------------------------------------------------------------
String SomfyRTSProtocol::describe(uint32_t ts)
{
      char buffer[128];
      int len = bits; // count of bits
      
      code0 = (unsigned)((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3]));
      code1 = (unsigned)((bytes[4] << 24) | (bytes[5] << 16) | (bytes[6] << 8) | (bytes[7]));
      code2 = (unsigned)((bytes[8] << 24) | (bytes[9] << 16) | (bytes[10] << (8+7)));

      sprintf(buffer, "{ \"prot\":\"%s\", \"code0\":\"%08x\", \"code1\":\"%08x\", \"code2\":\"%08x\", \"ts\": %d  }",
          getName().c_str(), code0, code1, code2, ts);
          
      return buffer;
}
