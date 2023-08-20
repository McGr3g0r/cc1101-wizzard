//------------------------------------------------------------------------------------------------------------------
#include "radio_pulses.h"
#include "settings.h"
#include "cc1101.h"
//------------------------------------------------------------------------------------------------------------------
CC1101* getRadio(void);
//------------------------------------------------------------------------------------------------------------------
int radio_pulses_cnt;
uint16_t radio_pulses_buffer[RADIO_PULSES_BUFFER_LEN];
//------------------------------------------------------------------------------------------------------------------
void radio_pulses_init(void)
{
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
       while ((idx < RADIO_PULSES_BUFFER_LEN) && (micros() -s < maxSpaceUs))
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
    if (pulses > 0 && startOffset >=0 && startOffset < RADIO_PULSES_BUFFER_LEN)
    { 
        CC1101* radio = getRadio();
    
        radio->setCCMode(0); 
        radio->setPacketFormat(3);
        radio->setTX();
            
        pinMode(gdo0, OUTPUT);
    
        digitalWrite(RXLED, LOW);   // set the RX LED ON
        uint8_t curr_val = 1;
        for (int idx = startOffset; idx < startOffset + pulses; idx++) {
           if (idx >= RADIO_PULSES_BUFFER_LEN)
               break;

            
            uint32_t s = micros();
            digitalWrite(gdo0, curr_val);
            
            delayMicroseconds(radio_pulses_buffer[idx]);
            curr_val++;
            curr_val &= 0x01;

               
        }
            
        digitalWrite(gdo0, 0);
        digitalWrite(RXLED, HIGH);   // set the RX LED OFF

        for (int idx = startOffset; idx < startOffset + pulses; idx++) {
           if (idx >= RADIO_PULSES_BUFFER_LEN)
               break;
               
            total_us += radio_pulses_buffer[idx];
        }
        // setting normal pkt format again
        radio->setCCMode(1); 
        radio->setPacketFormat(0);
        radio->setRX();
    }

    return total_us;
}
//------------------------------------------------------------------------------------------------------------------
int  radio_pulses_max_count(void)
{
   return RADIO_PULSES_BUFFER_LEN - radio_pulses_cnt;
}
//------------------------------------------------------------------------------------------------------------------
