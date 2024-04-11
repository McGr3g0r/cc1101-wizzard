//------------------------------------------------------------------------------------------------------------------
#include "radio_raw.h"
#include "radio_buffer.h"
#include "settings.h"
#include "utils.h"
#include "cc1101.h"
//------------------------------------------------------------------------------------------------------------------
CC1101* getRadio(void);
//------------------------------------------------------------------------------------------------------------------
int radio_bytes_cnt;
uint8_t* radio_buffer;
int radio_buffer_len;
//------------------------------------------------------------------------------------------------------------------
void radio_raw_init(void)
{
  radio_buffer = (uint8_t*)radio_buffer_get(0);
  radio_buffer_len = radio_buffer_size(0);
  radio_bytes_cnt = 0;
}
//------------------------------------------------------------------------------------------------------------------
int radio_raw_add(uint8_t data)
{
    if (radio_bytes_cnt + 1< radio_buffer_len)
    {
        radio_buffer[radio_bytes_cnt] = data;
        radio_bytes_cnt++;
    }

    return radio_bytes_cnt;  
}
//------------------------------------------------------------------------------------------------------------------
int radio_raw_set(int offset, uint8_t data)
{
    if (offset < radio_buffer_len)
    {
        radio_buffer[offset] = data;
        if (offset >= radio_bytes_cnt)
           radio_bytes_cnt = offset + 1;
    }

    return radio_bytes_cnt;
}
//------------------------------------------------------------------------------------------------------------------
void radio_raw_add_hex(char* hex)
{
   int len = strlen(hex);
   for (int idx = 0; idx < len; idx++)
   {
       if (idx < len -1)
       {
           radio_raw_add(hex_to_u8(&hex[idx], 2));
           idx++;
       } else
       {
         radio_raw_add(hex_to_u8(&hex[idx], 1));
       }
   }
}
//------------------------------------------------------------------------------------------------------------------
void radio_raw_send(int startOffset, int dataLen)
{
    int total_us = 0; 
    uint32_t s;
    if (dataLen > 0 && startOffset >=0 && startOffset < radio_buffer_len)
    { 
        CC1101* radio = getRadio();        
        radio->setCCMode(1); 
        radio->setPacketFormat(0);
 
        digitalWrite(RXLED, LOW);   // set the RX LED OFF

        radio->sendData(&radio_buffer[startOffset], dataLen);
        
        
        digitalWrite(RXLED, HIGH);   // set the RX LED OFF


        radio->setRX();

    }
}

//------------------------------------------------------------------------------------------------------------------
int radio_raw_get_data_len(void)
{
    return radio_bytes_cnt;
}
//------------------------------------------------------------------------------------------------------------------
