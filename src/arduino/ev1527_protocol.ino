//------------------------------------------------------------------------------------------------------------------
#include "ev1527_protocol.h"
//------------------------------------------------------------------------------------------------------------------
#define EV1527_TIME_US 250
//------------------------------------------------------------------------------------------------------------------
String EV1527Protocol::getName(void)
{
    return "ev1527";  
}
//------------------------------------------------------------------------------------------------------------------
int EV1527Protocol::getMinimalTime()
{
    return (EV1527_TIME_US -(EV1527_TIME_US / 4));
}
//------------------------------------------------------------------------------------------------------------------
int EV1527Protocol::getInterFrameTime()
{
    return 2000;
}
//------------------------------------------------------------------------------------------------------------------
int EV1527Protocol::getMaximalTime()
{
   return 32 * EV1527_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
int EV1527Protocol::getFrameTime()
{
    return (32 + 4 * 32 + 4) * EV1527_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
int EV1527Protocol::getMinPulses(void)
{
    return 50;
}
//------------------------------------------------------------------------------------------------------------------
bool EV1527Protocol::fromPulses(int pulses, uint16_t* buffer)
{
    pulse_divisor = EV1527_TIME_US;  

    int i;
    int length;
    bool preambule = false;
        
    int hist[4];
    int hist_idx[4];
        
    if (pulses < (2 + (2* 24)))
           return false; 

    pulse_divisor = pulses_histogram(EV1527_TIME_US, buffer, pulses, pulse_hist, pulse_hist_idx, sizeof(pulse_hist_idx) / sizeof(int));
        
    for (i=0; i<pulses-1; i++)
    {
        int t1 = pulseLen(buffer[i]);
        int t2 = pulseLen(buffer[i+1]);
      
        if (t1 == 1 && (t2 >=30 && t2 <= 34))
        {
           preambule = true;
           i += 2;
           break;
        }
    }

    if (!preambule) {
        return false;   
    }
           
    length = 0;
    bits = 0;
    bool buffer_fix = false;
    bytes_idx = 0;
    
    for (; i<pulses-1; i+=2)
    {
       int p0 = pulseLen(buffer[i]);
       int p1 = pulseLen(buffer[i+1]);
       if (p0==3 && (p1==1 || (p1 >=1 && i+2 >= pulses - 1))) /* final pulse, artificially set to frame spacer*/
       {
          length++;
          bits <<= 1;
          bits |= 1;
       }
       else if (p0==1 && (p1==3 || (p1 >=3 && i+2 >=pulses - 1)))  /* final pulse, artificially set to frame spacer*/
       {
          length++;
          bits <<= 1;
          bits |= 0;
       }
       else        
       {
          return false;
       }

       if ((length & 7) == 0)
       {
          bytes[bytes_idx++] = bits;
          bits = 0;
       }
       
       //next item is last, final zero not in buffer ?
       if (pulses - i == 3) 
       {
          buffer_fix = true;
       }    
      
       if (length == 24)
           break; 
    }
        
    if (buffer_fix)
    {
    
       int p0 = pulseLen(buffer[i]);
       if (p0==3 /*&& p1==1*/)
       {
          length++;
          bits <<= 1;
          bits |= 1;
       }
       else if (p0==1 /*&& p1==3*/)
       {
          length++;
          bits <<= 1;
          bits |= 0;
       }
       else        
       {
          return false;
       }

       if ((length & 7) == 0)
       {
          bytes[bytes_idx++] = bits;
          bits = 0;
       }
    }

    
    if ((length & 7) != 0)
    {
          bytes[bytes_idx++] = bits;
    }
           
    if (length < 24)
    {
       return false;
    }
    else
       return true;
}
//------------------------------------------------------------------------------------------------------------------
bool EV1527Protocol::toPulses(uint16_t* buffer, int maxPulses,int* pulses)
{
    return false;
}
//------------------------------------------------------------------------------------------------------------------
String EV1527Protocol::describe(uint32_t ts)
{
      char buffer[64];
      int len = bits; // count of bits
      
      code = (uint32_t)((bytes[0] << 16) | (bytes[1] << 8) | bytes[2]);
      btn = (bytes[2] & 0x0f);

      sprintf(buffer, "{ \"prot\":\"%s\", \"code\":\"%05x\", \"btn\": %d, \"ts\": %d  }",
          getName().c_str(), code, btn, ts);
      return buffer;
}
//------------------------------------------------------------------------------------------------------------------
