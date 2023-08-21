//------------------------------------------------------------------------------------------------------------------
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
class Protocol
{
    public:
       virtual String getName();
       virtual bool fromPulses(int pulses, uint16_t* buffer);
       virtual bool toPulses(uint16_t* buffer, int maxPulses,int* pulses);
       
       virtual int getMinimalTime();
       virtual int getMaximalTime();
       virtual int getInterFrameTime();
       virtual int getFrameTime();
       virtual int getMinPulses(void);

       
       virtual String describe(uint32_t ts);

   protected:
        int pulse_hist[10];
        int pulse_hist_idx[10];
        int pulse_divisor = 0;
        uint8_t bytes[16];
        int bytes_idx;
        int bits;

        int pulseLen(int microseconds);
        int pulses_histogram(int defVal, uint16_t* pulses, int size, int* hist, int* hist_val_idx, int hist_size);
        void dump_pulses(int pulses, uint16_t* buffer);
        void dump_pulses_len(int pulses, uint16_t* buffer);
        void bytesAdd(uint8_t b);
        void bytesClear(void);
};
//------------------------------------------------------------------------------------------------------------------
#endif//__PROTOCOL_H__
//------------------------------------------------------------------------------------------------------------------
