//----------------------------------------------------------------
#ifndef __RADIO_PROFILE_H__
#define __RADIO_PROFILE_H__
//----------------------------------------------------------------
typedef struct radio_profile_s
{
   String name;
   int freq;
   int bwrx;
   int chan;
   int mod;
} radio_profile_t;
//----------------------------------------------------------------

void radio_profile_init(void);
int  radio_profile_get_count(void);
radio_profile_t* radio_profile_get(int idx);
//----------------------------------------------------------------
#endif// __RADIO_PROFILE_H__
//----------------------------------------------------------------
