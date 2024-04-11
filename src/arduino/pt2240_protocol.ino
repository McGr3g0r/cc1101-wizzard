//------------------------------------------------------------------------------------------------------------------
#include "pt2240_protocol.h"
//------------------------------------------------------------------------------------------------------------------
#define PT2240_TIME_US 270
#define PT2240_PROTO_BITS 24
#define PT2240_PULSES_PER_BIT 2
#define PT2240_GUARD_TIME_US (7 * 270)
//------------------------------------------------------------------------------------------------------------------
PT2240Protocol::PT2240Protocol()
{
    pulse_divisor = PT2240_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
String PT2240Protocol::getName(void)
{
    return "pt2240";  
}
//------------------------------------------------------------------------------------------------------------------
int PT2240Protocol::getMinimalTime()
{
    return (PT2240_TIME_US -(PT2240_TIME_US / 4));
}
//------------------------------------------------------------------------------------------------------------------
int PT2240Protocol::getInterFrameTime()
{
    return 270 * 8;
}
//------------------------------------------------------------------------------------------------------------------
int PT2240Protocol::getMaximalTime()
{
   return 25 * PT2240_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
int PT2240Protocol::getFrameTime()
{
    return (24*4 + 8) * EV1527_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
int PT2240Protocol::getMinPulses(void)
{
    return 48;
}
//------------------------------------------------------------------------------------------------------------------
bool PT2240Protocol::fromPulses(int pulses, uint16_t* buffer)
{
    int i;
    int length;
    bool preambule = false;
        
    int hist[4];
    int hist_idx[4];
        
    if (pulses < (2 + (2* 24)))
           return false; 

    pulse_divisor = pulses_histogram(PT2240_TIME_US, buffer, pulses, pulse_hist, pulse_hist_idx, sizeof(pulse_hist_idx) / sizeof(int));
        
    for (i=0; i<pulses-1; i++)
    {
        int t1 = pulseLen(buffer[i]);
        int t2 = pulseLen(buffer[i+1]);
      
        if (t1 == 1 && (t2 >=7 && t2 <= 8))
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

    bytesClear();
    
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
          bytesAdd(bits);
          bits = 0;
       }
       
       //next item is last, final zero not in buffer ?
       if (pulses - i == 3) 
       {
          buffer_fix = true;
       }    
      
       if (length == PT2240_PROTO_BITS)
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
          bytesAdd(bits);
          bits = 0;
       }
    }
    
    if ((length & 7) != 0)
    {
        bytesAdd(bits);
    }
           
    if (length < PT2240_PROTO_BITS)
       return false;
    else
       return true;
}
//------------------------------------------------------------------------------------------------------------------
void PT2240Protocol::setData(uint32_t data)
{
    code = data;
}
//------------------------------------------------------------------------------------------------------------------
int PT2240Protocol::dataToBytes(void)
{

    bytesClear();

    //code 24bit bits
    bytesAdd(reverse8((code >> 24) & 0xff));
    bytesAdd(reverse8((code >> 16) & 0xff));
    bytesAdd(reverse8(code & 0xff));

    //24bits in protocol
    return PT2240_PROTO_BITS;
}
//------------------------------------------------------------------------------------------------------------------
bool PT2240Protocol::toPulses(uint16_t* buffer, int maxPulses,int* pulses, int frameNo)
{
    int pls = 0;

    int length = dataToBytes();
  
    if (maxPulses < ((24 + 1) * PT2240_PULSES_PER_BIT))
       return false;
       
    for (int i=0; i<length; i++)
    {
         int bit = (bytes[i/8] >> (i&7)) & 1;
         if (bit)
         {
            buffer[pls++] = pulseDuration(3);
            buffer[pls++] = pulseDuration(1);
         } else
         {
            buffer[pls++] = pulseDuration(1);
            buffer[pls++] = pulseDuration(3);
         }
    }

    buffer[pls++] = pulseDuration(1);
    buffer[pls++] = pulseDuration(7);

    *pulses = pls;

    return true;
}
//------------------------------------------------------------------------------------------------------------------
String PT2240Protocol::describe(uint32_t ts)
{
      char buffer[64];
      int len = bits; // count of bits
      
      code = (uint32_t)((bytes[0] << 16) | (bytes[1] << 8) | bytes[2]);

      sprintf(buffer, "{ \"prot\":\"%s\", \"code\":\"%06x\", \"ts\": %d  }",
          getName().c_str(), code, ts);
      return buffer;
}
//------------------------------------------------------------------------------------------------------------------
