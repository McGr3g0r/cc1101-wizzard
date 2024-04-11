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
#define PREAMBLE_PULSE_LEN 4
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
        {
           //STDOUT.print("dbg#1");
           return false;
        }
        
        for (i=0; i< pulses; i++)
        {
          int t = pulseLen(buffer[i]);
          if (t >=PREAMBLE_PULSE_LEN && t <= PREAMBLE_PULSE_LEN+1)
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
        {
            //STDOUT.print("dbg#2");STDOUT.print("pr:");STDOUT.println(preambule);
            return false;
        }
                     
        //should be SOFT_SYNC_PULSES --> 4800us
        if (buffer[i] >= SOFT_SYNC_US-(HALF_TIME_CLOCK/2) && buffer[i] <= SOFT_SYNC_US+(HALF_TIME_CLOCK/2)+(HALF_TIME_CLOCK/10)) {
            i++;
        }        
        else {
              /*STDOUT.println("dbg#3");
              STDOUT.print(" p:");STDOUT.print(pulses);
              STDOUT.print(" i:");STDOUT.print(i);
              STDOUT.print(" l:");STDOUT.println(buffer[i]);*/
              return false;
        }
            
        length = 0;
        int bits = 0;
        
        bytesClear();
        //STDOUT.print("dbg#31");
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
             //STDOUT.print("dbg#4");
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

        if (length == SOMFY_CODED_BITS - 1)
        {
              length++;
              bits <<= 1;
              bits |= 1;
                      
              if ((length & 7) != 0)
              {
                   bytesAdd(bits);
              }
        }
        if (length < SOMFY_CODED_BITS)
        {
          //STDOUT.print("dbg#5");
          //STDOUT.print("l:");STDOUT.println(length);
           return false;
        }
        else
        {
          //STDOUT.print("dbg#6");
          //STDOUT.print("l:");STDOUT.println(length);
           return true;
        }
}
//------------------------------------------------------------------------------------------------------------------
int SomfyRTSProtocol::dataToBytes(void)
{
     bytesClear();
     
     bytesAdd(reverse8((code0 >> 24) & 0xff));
     bytesAdd(reverse8((code0 >> 16) & 0xff));
     bytesAdd(reverse8((code0 >> 8) & 0xff));
     bytesAdd(reverse8(code0 & 0xff));
      
     bytesAdd(reverse8((code1 >> 24) & 0xff));
     bytesAdd(reverse8((code1 >> 16) & 0xff));
     bytesAdd(reverse8((code1 >> 8) & 0xff));
     bytesAdd(reverse8(code1 & 0xff));
      
     bytesAdd(reverse8((code2 >> 24) & 0xff));
     bytesAdd(reverse8((code2 >> 16) & 0xff));
     bytesAdd(reverse8((code2 >> 8)  & 0x80));
     
    return SOMFY_CODED_BITS;
}
//------------------------------------------------------------------------------------------------------------------
bool SomfyRTSProtocol::toPulses(uint16_t* buffer, int maxPulses,int* pulses,int frameNo)
{
    int pls = 0;

    int length = dataToBytes();

    int preamble_pulses = frameNo == 0 ? 24 : 12;
  
    if (maxPulses < (2 * SOMFY_FRAME_PULSES))
       return false;
             
    for (; pls < preamble_pulses; pls++)
    {
        if (!pls)
            buffer[pls] = pulseDuration(PREAMBLE_PULSE_LEN+1);
        else
            buffer[pls] = pulseDuration(PREAMBLE_PULSE_LEN);
    }

    buffer[pls++] = SOFT_SYNC_US;

    for (int i=0; i< length; i++)
    {
         int bit = (bytes[i/8] >> (i&7)) & 1;
         if (bit)
         {
             buffer[pls++] = pulseDuration(1);
             buffer[pls++] = pulseDuration(1);
         } else
         {
             buffer[pls++] = pulseDuration(2);
         }
    }
          
    //last pulse not LOW -add frame spacer ???
    if ((pls % 2) == 1)
    {
       //last pulse must be zero - add the time guard for frames separation
       buffer[pls++] = SOMFY_GUARD_TIME_US; //frame repeat pause        
    } else {
       buffer[pls - 1] += SOMFY_GUARD_TIME_US;
    }       

    *pulses = pls;
    
  return true;
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
//------------------------------------------------------------------------------------------------------------------
void SomfyRTSProtocol::setCodes(uint32_t code0, uint32_t code1, uint32_t code2)
{
    this->code0 = code0;
    this->code1 = code1;
    this->code2 = code2;
}
//------------------------------------------------------------------------------------------------------------------
