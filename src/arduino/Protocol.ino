//------------------------------------------------------------------------------------------------------------------
#include "Protocol.h"
//------------------------------------------------------------------------------------------------------------------
int Protocol::pulseLen(int microseconds)
{
    return (microseconds+(pulse_divisor/2))/pulse_divisor;
}
//------------------------------------------------------------------------------------------------------------------
int Protocol::pulses_histogram(int defVal, uint16_t* pulses, int size, int* hist, int* hist_val_idx, int hist_size)
{
    int res = defVal;
    memset(hist, 0, hist_size);
    memset(hist_val_idx, 0, hist_size);
    
    for (int idx = 0; idx < size; idx++)
    {
        bool mark = false;
        for (int val_idx = 0; val_idx < hist_size; val_idx++)
        {
            if (!mark && (hist_val_idx[val_idx] == pulses[idx] || hist_val_idx[val_idx] == 0))
            {
               hist_val_idx[val_idx] = pulses[idx];
               hist[val_idx]++;
               mark = true;
            }
        } 
    }

    int high_idx = -1;
    for (int val_idx = 0; val_idx < hist_size; val_idx++)
    {
      if ( hist_val_idx[val_idx] >= 160 && hist_val_idx[val_idx] < 500)
      {
           if (high_idx == -1 || hist[val_idx] > hist[high_idx])
              high_idx = val_idx;
           
      }
    }
    
    if (high_idx != -1)
        res = hist_val_idx[high_idx];

    return res;
}
//------------------------------------------------------------------------------------------------------------------
void Protocol::dump_pulses(int pulses, uint16_t* buffer)
{
    STDOUT.println("pulses:");
    for (int idx = 0; idx < pulses; idx++)
    {
        STDOUT.print(buffer[idx]);
        if (idx > 0 && idx < pulses - 1)
            STDOUT.print(",");
        if (idx > 0 && ((idx % 32) == 0))
            STDOUT.print("\n\r");
            
    }
}
//------------------------------------------------------------------------------------------------------------------
void Protocol::dump_pulses_len(int pulses, uint16_t* buffer)
{
    STDOUT.println("pulses:");
    for (int idx = 0; idx < pulses; idx++)
    {
        STDOUT.print(pulseLen(buffer[idx]));
        if (idx > 0 && idx < pulses - 1)
            STDOUT.print(",");
        if (idx > 0 && ((idx % 32) == 0))
            STDOUT.print("\n\r");
            
    }
}
//------------------------------------------------------------------------------------------------------------------
