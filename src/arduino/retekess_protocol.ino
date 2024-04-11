//------------------------------------------------------------------------------------------------------------------
#include "retekess_protocol.h"
//------------------------------------------------------------------------------------------------------------------
#define RETEKESS_TIME_US 325
#define RETEKESS_SYMBOL_TIME_US 1300
#define RETEKESS_PROTO_BITS 25
#define RETEKESS_PULSES_PER_BIT 2
#define RETEKESS_GUARD_TIME_US 7500
//------------------------------------------------------------------------------------------------------------------
RetekessProtocol::RetekessProtocol()
{
    pulse_divisor = RETEKESS_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
String RetekessProtocol::getName(void)
{
    return "retekess";  
}
//------------------------------------------------------------------------------------------------------------------
int RetekessProtocol::getMinimalTime()
{
    return (RETEKESS_TIME_US -(RETEKESS_TIME_US / 4));
}
//------------------------------------------------------------------------------------------------------------------
int RetekessProtocol::getInterFrameTime()
{
    return RETEKESS_GUARD_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
int RetekessProtocol::getMaximalTime()
{
   return RETEKESS_PROTO_BITS * RETEKESS_SYMBOL_TIME_US * 2;
}
//------------------------------------------------------------------------------------------------------------------
int RetekessProtocol::getFrameTime()
{
    return (RETEKESS_PROTO_BITS * RETEKESS_SYMBOL_TIME_US) + RETEKESS_GUARD_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
int RetekessProtocol::getMinPulses(void)
{
    return RETEKESS_PROTO_BITS * RETEKESS_PULSES_PER_BIT;
}
//------------------------------------------------------------------------------------------------------------------
bool RetekessProtocol::fromPulses(int pulses, uint16_t* buffer)
{
    int i;
    int length;
    bool preambule = false;
        
    int hist[4];
    int hist_idx[4];
        
    if (pulses < (RETEKESS_PROTO_BITS * RETEKESS_PULSES_PER_BIT))
    {
           return false; 
    }

    pulse_divisor = pulses_histogram(RETEKESS_TIME_US, buffer, pulses, pulse_hist, pulse_hist_idx, sizeof(pulse_hist_idx) / sizeof(int));
        
    for (i=0; i<pulses-2; i++)
    {
        int t1 = pulseLen(buffer[i]);
        int t2 = pulseLen(buffer[i+1]);
      
        if ((t1 == 1 && t2 == 3) || (t1 == 3 && t2 == 1))
        {
           preambule = true;
           break;
        }
    }

    if (!preambule) {
        //STDOUT.println("#dbg2");
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
       
       if (p0==1 && (p1==3 || (p1 >=3 && i+2 >= (RETEKESS_PROTO_BITS * RETEKESS_PULSES_PER_BIT) - 1))) /* final pulse, artificially set to frame spacer*/
       {
          length++;
          bits <<= 1;
          bits |= 0;
       }
       else if (p0==3 && (p1==1 || (p1 >=1 && i+2 >= (RETEKESS_PROTO_BITS * RETEKESS_PULSES_PER_BIT) - 1)))  /* final pulse, artificially set to frame spacer*/
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
       
       //next item is last, final zero not in buffer ?
       if (pulses - i == 3) 
       {
          buffer_fix = true;
       }    
      
       if (length == RETEKESS_PROTO_BITS)
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
           
    if (length < RETEKESS_PROTO_BITS)
       return false;
    else
       return true;
}
//------------------------------------------------------------------------------------------------------------------
void RetekessProtocol::setData(uint32_t data)
{
    code = data;
}
//------------------------------------------------------------------------------------------------------------------
int RetekessProtocol::dataToBytes(void)
{

    bytesClear();

    //code 24bit bits
    bytesAdd((code >> 17) & 0xff);
    bytesAdd((code >> 9) & 0xff);
    bytesAdd((code >> 1) & 0xff);
    bytesAdd((code & 0x01) << 7);

    //25 bits in protocol
    return RETEKESS_PROTO_BITS;
}
//------------------------------------------------------------------------------------------------------------------
bool RetekessProtocol::toPulses(uint16_t* buffer, int maxPulses,int* pulses, int frameNo)
{
    int pls = 0;

    int length = dataToBytes();
  
    if (maxPulses < 2 * (RETEKESS_PROTO_BITS * RETEKESS_PULSES_PER_BIT))
       return false;
       
    for (int i=0; i<length; i++)
    {
         int bit = (bytes[i/8] >> (7 - (i&7))) & 1;
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


    
    buffer[pls - 1] += RETEKESS_GUARD_TIME_US;

    *pulses = pls;

    return true;
}
//------------------------------------------------------------------------------------------------------------------
String RetekessProtocol::describe(uint32_t ts)
{
      char buffer[64];
      int len = bits; // count of bits
      
      code = (uint32_t)((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3]));
      //13,12 bits
      //0 1111 1110 0000 0000 0001 1111
      int c1 = bytes[0] << 5 | ((bytes[1] >> 3) & 0x1F);
      int c2 = (bytes[1] & 0x07) << 9 | bytes[2] << 1 | (bytes[3] & 0x1);

      sprintf(buffer, "{ \"prot\":\"%s\", \"id1\":\"%04x\", \"id2\":\"%03x\", \"ts\": %d  }",
          getName().c_str(), c1, c2, ts);
      return buffer;
}
//------------------------------------------------------------------------------------------------------------------
