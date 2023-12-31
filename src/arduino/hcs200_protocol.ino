//------------------------------------------------------------------------------------------------------------------
#include "hcs200_protocol.h"
//------------------------------------------------------------------------------------------------------------------
#define HCS200_PULSLSE_DIVISOR 400
/*66 bits transmitted, LSB first.

|  0-31 | Encrypted Portion
| 32-59 | Serial Number
| 60-63 | Button Status (S3, S0, S1, S2)
|  64   | Battery Low
|  65   | RPT bit, Fixed 1 ??? 
*/
//------------------------------------------------------------------------------------------------------------------
#define HCS_GUARD_TIME_US 15600
#define HCS_PROTO_PULSES 155
#define HCS_PROTO_BITS 66
//------------------------------------------------------------------------------------------------------------------
HCS200Protocol::HCS200Protocol()
{
  pulse_divisor = HCS200_PULSLSE_DIVISOR;
}
//------------------------------------------------------------------------------------------------------------------
String HCS200Protocol::getName(void)
{
    return "hcs200";  
}
//------------------------------------------------------------------------------------------------------------------
int HCS200Protocol::getMinimalTime()
{
    return (HCS200_PULSLSE_DIVISOR -(HCS200_PULSLSE_DIVISOR / 4));
}
//------------------------------------------------------------------------------------------------------------------
int HCS200Protocol::getInterFrameTime()
{
    return HCS_GUARD_TIME_US;
}
//------------------------------------------------------------------------------------------------------------------
int HCS200Protocol::getMaximalTime()
{
   return 128 * HCS200_PULSLSE_DIVISOR;
}
//------------------------------------------------------------------------------------------------------------------
int HCS200Protocol::getFrameTime()
{
    return (3 * 66) * HCS200_PULSLSE_DIVISOR;
}
//------------------------------------------------------------------------------------------------------------------
int HCS200Protocol::getMinPulses(void)
{
    return HCS_PROTO_PULSES;
}
//------------------------------------------------------------------------------------------------------------------
bool HCS200Protocol::fromPulses(int pulses, uint16_t* buffer)
{
        int i;
        int length;
        int preambule = 0;
        int pre_error = 0;

        if (pulses < HCS_PROTO_PULSES)
           return false;

        for (i=0; i< pulses; i++)
        {
          int t = pulseLen(buffer[i]);
          if (t >=0 && t < 2)
          {      
            preambule++;
            if (preambule  == 23)
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

        if (preambule < 23) // should be 23*Te
            return false;
                     
        //should be 10*Te
        if (pulseLen(buffer[i]) >=9 && pulseLen(buffer[i]) <=11)
            i++;
        else {
              return false;
        }        
        length = 0;
        bits = 0;
        bool buffer_fix = false;

        bytesClear();
        
        for (; i< pulses-1; i+=2)
        {
           int p0 = pulseLen(buffer[i]);
           int p1 = pulseLen(buffer[i+1]);
           if (p0==2 && (p1==1 || (p1 >=37)))  /* final pulse, artificially set to frame spacer*/
           {
              length++;
              bits <<= 1;
              bits |= 0;
           }
           else if (p0==1 && (p1==2 || (p1 >=37)))  /* final pulse, artificially set to frame spacer*/
           {
              length++;
              bits <<= 1;
              bits |= 1;
           }
           else        
           {
              //dump_pulses_len(pulses, buffer);
              return false;
           }

           if ((length & 7) == 0)
           {
              // swap nibbles
              bits = reverse8(bits);
              bytesAdd(bits);
              bits = 0;
           }
           
           //next item is last, final zero not in buffer ?
           if (pulses - i == 3) 
           {
              buffer_fix = true;
           }    

           if (length == HCS_PROTO_BITS)
               break;
        }
        
        if (buffer_fix)
        {
        
           int p0 = pulseLen(buffer[i]);
           if (p0==2 /*&& p1==1*/)
           {
              length++;
              bits <<= 1;
              bits |= 0;
           }
           else if (p0==1 /*&& p1==2*/)
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
              // swap nibbles
              bits = reverse8(bits);
              bytesAdd(bits);
              bits = 0;
           }
           
        }
       
        //emulate last bit if not received 66 pulses
        if (length == (HCS_PROTO_BITS -1))
        {
             bits <<= 0;
             length++;
        }
        
        if ((length & 7) != 0)
        {
              bits = reverse8(bits);
              bytesAdd(bits);
        }
        if (length < HCS_PROTO_BITS)
           return false;
        else
           return true; 
}

//------------------------------------------------------------------------------------------------------------------
void HCS200Protocol::setData(uint32_t enc, uint32_t serial, uint8_t btn, bool learn, bool battery_low, bool rpt)
{
   this->enc = enc;
   this->serial = serial;
   this->btn = btn;
   this-> battery_low = battery_low;
   this->learn = learn;
   this->rpt = rpt;
}
//------------------------------------------------------------------------------------------------------------------
int HCS200Protocol::dataToBytes(void)
{
    uint8_t button = btn;
    uint8_t status;
    uint8_t repeat = rpt;
    
    bytesClear();
  
    //encrypted LSB
    bytesAdd(enc & 0xff);
    bytesAdd((enc >> 8) & 0xff);
    bytesAdd((enc >> 16) & 0xff);      
    bytesAdd((enc >> 24) & 0xff);
    //serial 28bits
    bytesAdd((serial & 0xff));
    bytesAdd(((serial >> 8) & 0xff));
    bytesAdd(((serial >> 16) & 0xff));
    //serial-msb + btn
    if (learn)
        button = 0x0f;    
        
    bytesAdd((((serial >> 24) & 0x0f) | ((button & 0x0f) << 4)));
    status = 0;
    if (battery_low)
       status |= 0x01;
    if (repeat)
       status |= 0x02;
    bytesAdd((status & 0xff));

    return HCS_PROTO_BITS;
}
//------------------------------------------------------------------------------------------------------------------
bool HCS200Protocol::toPulses(uint16_t* buffer, int maxPulses,int* pulses, int frameNo)
{
     int length = dataToBytes();
     int pls = 0;

    const int preambule = 23; // 23 * TE

    for (pls=0; pls <preambule; pls++)
        buffer[pls] = pulseDuration(1);

    buffer[pls++] = pulseDuration(10);

    for (int i=0; i<length; i++)
    {
       int bit = (bytes[i/8] >> (i&7)) & 1;
       if (bit)
       {
           buffer[pls++] = pulseDuration(1);
           buffer[pls++] = pulseDuration(2);
       } else
       {
           buffer[pls++] = pulseDuration(2);
           buffer[pls++] = pulseDuration(1);
       }
    }

    buffer[pls - 1] += HCS_GUARD_TIME_US;

    *pulses = pls;    
        
    return true;
}
//------------------------------------------------------------------------------------------------------------------
String HCS200Protocol::describe(uint32_t ts)
{
      char buffer[128];
      int len = bits; // count of bits
      
      enc = ((unsigned)(bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | (bytes[0]));
      serial = (unsigned)(((bytes[7] & 0x0f) << 24) | (bytes[6] << 16) | (bytes[5] << 8) | (bytes[4]));
      btn = ((bytes[7] & 0xf0) >> 4);
      learn = btn == 0x0f;
      battery_low = (bool)((bytes[8] & 0x01) == 0x01);
      rpt = (bool)((bytes[8] & 0x02) == 0x02); 

      sprintf(buffer, "{ \"prot\":\"%s\", \"code\":\"%08x\", \"serial\":\"%07x\", \"btn\": %04x, \"battery_low\": \"%s\", \"rpt\": \"%s\", \"ts\": %d  }",
          getName().c_str(), enc, serial, btn,  battery_low ? "true" : "false", rpt ? "true" : "false", ts);
          
      return buffer;
}
