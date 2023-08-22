//------------------------------------------------------------------------------------------------------------------
#include "radio_pulses.h"
#include "radio_buffer.h"
#include "settings.h"
#include "cc1101.h"
//------------------------------------------------------------------------------------------------------------------
CC1101* getRadio(void);
//------------------------------------------------------------------------------------------------------------------
int radio_pulses_cnt;
uint16_t* radio_pulses_buffer;
int radio_pulses_buffer_len;
//------------------------------------------------------------------------------------------------------------------
void radio_pulses_init(void)
{
  radio_pulses_buffer = (uint16_t*)radio_buffer_get(0);
  radio_pulses_buffer_len = radio_buffer_size(0) / 2; /*buffer is uint8_t, pulses are uint16_t*/
  radio_pulses_flush();
}
//------------------------------------------------------------------------------------------------------------------
void radio_pulses_flush(void)
{
   radio_pulses_cnt = 0;
   memset(radio_pulses_buffer, 0, sizeof(radio_pulses_buffer)); 
}
//------------------------------------------------------------------------------------------------------------------
int radio_pulses_get_count(void)
{
  return radio_pulses_cnt;
}
//------------------------------------------------------------------------------------------------------------------
uint16_t* radio_pulses_get_pulses(void)
{
   return &radio_pulses_buffer[0];
}
//------------------------------------------------------------------------------------------------------------------
int radio_pulses_grab(Stream& input, int startOffset, int minPulseUs, int maxSpaceUs)
{
    CC1101* radio = getRadio();

    int idx;
    uint32_t s2;
    int tdiff;    
    
    radio->setCCMode(0); 
    radio->setPacketFormat(3);
    radio->setRX();
            
    pinMode(gdo0, INPUT);
    
    uint32_t s = micros();
    while ( digitalRead(gdo0) == LOW && micros() -s < maxSpaceUs); 
    if ( micros() -s < maxSpaceUs)
    {
       idx = startOffset;
       uint8_t curr_val = 1;

       s = micros();
       while ((idx < radio_pulses_buffer_len) && (micros() -s < maxSpaceUs))
       {
            while((digitalRead(gdo0) == curr_val) && (micros() -s < maxSpaceUs));
            s2 = micros();
            tdiff = s2 -s;
            if (tdiff < maxSpaceUs)
            {  
               if (tdiff >= minPulseUs) {
                   radio_pulses_buffer[idx++] = tdiff;
               }
               s = micros();
               curr_val++;
               curr_val &= 0x01;
            }     
       }

       radio_pulses_cnt = idx;
        // setting normal pkt format again
       radio->setCCMode(1); 
       radio->setPacketFormat(0);
       radio->setRX();
    };

    return idx - startOffset;
}
//------------------------------------------------------------------------------------------------------------------
int radio_pulses_send(int startOffset, int pulses)
{
    int total_us = 0; 
    uint32_t s;
    if (pulses > 0 && startOffset >=0 && startOffset < radio_pulses_buffer_len)
    { 
        CC1101* radio = getRadio();        
        radio->setCCMode(0); 
        radio->setPacketFormat(3);
        radio->setTX();

        pinMode(gdo0, OUTPUT);
        if (radio_pulses_buffer[startOffset] == 0xffff) /*hack*/
        {
            digitalWrite(gdo0, LOW);
            delayMicroseconds(1000);
        }
        
        digitalWrite(RXLED, LOW);   // set the RX LED ON
        uint8_t curr_val = 1;
        for (int idx = startOffset; idx < startOffset + pulses; idx++) {
           if (idx >= radio_pulses_buffer_len)
               break;
           
            digitalWrite(gdo0, curr_val);
            s = micros();
            while (micros() -s < radio_pulses_buffer[idx]);
            //delayMicroseconds(radio_pulses_buffer[idx]);
            curr_val++;
            curr_val &= 0x01;              
        }
            
        digitalWrite(gdo0, 0);
        digitalWrite(RXLED, HIGH);   // set the RX LED OFF

        // setting normal pkt format again
        radio->setCCMode(1); 
        radio->setPacketFormat(0);
        radio->setRX();

        for (int idx = startOffset; idx < startOffset + pulses; idx++) {
           if (idx >= radio_pulses_buffer_len)
               break;
               
            total_us += radio_pulses_buffer[idx];
        }
        radio_pulses_cnt = startOffset + pulses;
    }

    return total_us;
}
//------------------------------------------------------------------------------------------------------------------
int radio_pulses_add(uint16_t pulse)
{
    if ( radio_pulses_cnt + 1< radio_pulses_buffer_len)
    {
        radio_pulses_buffer[radio_pulses_cnt] = pulse;
        radio_pulses_cnt++;
    }

    return radio_pulses_cnt;  
}
//------------------------------------------------------------------------------------------------------------------
int radio_pulses_set(int offset, uint16_t pulse)
{
    if (offset < radio_pulses_buffer_len)
    {
        radio_pulses_buffer[offset] = pulse;
        if (offset >= radio_pulses_cnt)
        radio_pulses_cnt = offset + 1;
    }

    return radio_pulses_cnt;
}
//------------------------------------------------------------------------------------------------------------------
int  radio_pulses_max_count(void)
{
   return radio_pulses_buffer_len - radio_pulses_cnt;
}
//------------------------------------------------------------------------------------------------------------------
