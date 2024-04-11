//------------------------------------------------------------------------------------------------------------------
#include "db200_protocol.h"
//------------------------------------------------------------------------------------------------------------------
#define DB200_TIME_US 333
#define DB200_PROTO_BITS 4
#define DB200_PULSES_PER_BIT 2
#define DB200_GUARD_TIME_US 12000
//------------------------------------------------------------------------------------------------------------------
DB200Protocol::DB200Protocol()
{
    pulse_divisor = DB200_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
String DB200Protocol::getName(void)
{
    return "db200";  
}
//------------------------------------------------------------------------------------------------------------------
int DB200Protocol::getMinimalTime()
{
    return (DB200_TIME_US -(DB200_TIME_US / 4));
}
//------------------------------------------------------------------------------------------------------------------
int DB200Protocol::getInterFrameTime()
{
    return 12280;
}
//------------------------------------------------------------------------------------------------------------------
int DB200Protocol::getMaximalTime()
{
   return 35 * DB200_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
int DB200Protocol::getFrameTime()
{
    return (24 + 12 + 3) * DB200_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
int DB200Protocol::getMinPulses(void)
{
    return 16+9;
}
//------------------------------------------------------------------------------------------------------------------
bool DB200Protocol::fromPulses(int pulses, uint16_t* buffer)
{
    int i;
    int length;
    int preambule = 0;
        
    int hist[4];
    int hist_idx[4];
        
    if (pulses < (16 + (DB200_PROTO_BITS + DB200_PULSES_PER_BIT) + 2))
           return false; 

    pulse_divisor = pulses_histogram(DB200_TIME_US, buffer, pulses, pulse_hist, pulse_hist_idx, sizeof(pulse_hist_idx) / sizeof(int));

    //STDOUT.println("#dbg1");
        
    for (i=0; i<pulses-1; i++)
    {
        int t1 = pulseLen(buffer[i]);
        int t2 = pulseLen(buffer[i+1]);
      
        if (t1 == 1 && t2 == 2)
        {
           preambule++;
           i++;
           if (preambule == 8)
           {
               i++;
               break;
           }
        }
         else
             preambule = 0;
    }

    if (preambule != 8) {
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
       
       //STDOUT.print(" i:");STDOUT.print(i);STDOUT.print(" p0:");STDOUT.print(p0);STDOUT.print(" p1:");STDOUT.println(p1);
       
       if (p0==1 && (p1==1 || (p1 >=1 && length == DB200_PROTO_BITS - 1))) /* final pulse, artificially set to frame spacer*/
       {
          length++;
          bits <<= 1;
          bits |= 1;
       }
       else if (p0==2 && (p1==1 || (p1 >=2 && length == DB200_PROTO_BITS - 1)))  /* final pulse, artificially set to frame spacer*/
       {
          length++;
          bits <<= 1;
          bits |= 0;
       }
       else        
       {
        /*
        STDOUT.println("#dbg1122");
        STDOUT.print(" length:");STDOUT.print(length);
        STDOUT.print(" i:");STDOUT.print(i);
        STDOUT.print(" p0:");STDOUT.print(p0);
        STDOUT.print(" p1:");STDOUT.println(p1);*/
        
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
      
       if (length == DB200_PROTO_BITS)
           break; 
    }
        
    if (buffer_fix)
    {
    
       int p0 = pulseLen(buffer[i]);
       if (p0==1 /*&& p1==1*/)
       {
          length++;
          bits <<= 1;
          bits |= 1;
       }
       else if (p0==2 /*&& p1==1*/)
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
           
    if (length < DB200_PROTO_BITS)
       return false;
    else
       return true;
}
//------------------------------------------------------------------------------------------------------------------
void DB200Protocol::setData(uint8_t data)
{
    code = data;
}
//------------------------------------------------------------------------------------------------------------------
int DB200Protocol::dataToBytes(void)
{

    bytesClear();

    //code 7bit  bits
    bytesAdd(/*reverse8*/(code  & 0x0f));

    //24bits in protocol
    return DB200_PROTO_BITS;
}
//------------------------------------------------------------------------------------------------------------------
bool DB200Protocol::toPulses(uint16_t* buffer, int maxPulses,int* pulses, int frameNo)
{
    int pls = 0;

    int length = dataToBytes();
  
    if (maxPulses < (16 + (4 * DB200_PULSES_PER_BIT) + 3))
       return false;

    for (int i = 0; i < 8; i++)
    {
      buffer[pls++] = pulseDuration(1);
      buffer[pls++] = pulseDuration(2);
    }
      
    for (int i=0; i<length; i++)
    {
         int bit = (bytes[i/8] >> (i&7)) & 1;
         if (bit)
         {
            buffer[pls++] = pulseDuration(1);
            buffer[pls++] = pulseDuration(1);
         } else
         {
            buffer[pls++] = pulseDuration(2);//!!! was 3
            buffer[pls++] = pulseDuration(1);
         }
    }

    if ((pls & 0x01) == 0x00)
        buffer[pls-1] +=pulseDuration(1);
    else
        buffer[pls++] = pulseDuration(1);
        
    buffer[pls++] = pulseDuration(1);
    buffer[pls++] = pulseDuration(1);

    buffer[pls - 1] += DB200_GUARD_TIME_US;

    *pulses = pls;

  
    return true;
}
//------------------------------------------------------------------------------------------------------------------
String DB200Protocol::describe(uint32_t ts)
{
      char buffer[64];
      int len = bits; // count of bits
      
      code = (uint32_t)(bytes[0]);

      sprintf(buffer, "{ \"prot\":\"%s\", \"code\":\"%01x\", \"ts\": %d  }",
          getName().c_str(), code, ts);
      return buffer;
}
//------------------------------------------------------------------------------------------------------------------
