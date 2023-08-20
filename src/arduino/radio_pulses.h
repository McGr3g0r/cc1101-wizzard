//------------------------------------------------------------------------------------------------------------------
#ifndef __RADIO_PULSES_H__
#define __RADIO_PULSES_H__
//------------------------------------------------------------------------------------------------------------------
void radio_pulses_init(void);
void radio_pulses_flush(void);
int  radio_pulses_get_count(void);
int  radio_pulses_max_count(void);
uint16_t*  radio_pulses_get_pulses(void);
int radio_pulses_grab(Stream& input, int startOffset, int minPulseUs, int maxSpaceUs);
int radio_pulses_send(int startOffset, int pulses);
//------------------------------------------------------------------------------------------------------------------
#endif//__RADIO_PULSES_H__
//------------------------------------------------------------------------------------------------------------------
