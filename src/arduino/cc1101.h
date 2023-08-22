//------------------------------------------------------------------------------------------------------------------
#ifndef __CC1101_H__
#define __CC1101_H__
//------------------------------------------------------------------------------------------------------------------
#include "radio_profile.h"
//------------------------------------------------------------------------------------------------------------------
class CC1101 {
    public:
        CC1101(void);
        void init(void);
        bool check(void);
        void setFreq(int khz);
        int getFreq(void);
        void setModulation(int mod);
        int getModulation(void);
        void setDeviation(int dev);
        int getDeviation(void);
        void setChan(int chan);
        int getChan(void);

        void setChanSpaceing(int space);
        int getChanSpaceing(void);

        void setRxBw(int val);
        int getRxBw(void);

        void setDataRate(int val);
        int getDataRate(void);

        void setPA(int pa);
        int getPA(void);

        void setSyncMode(int val);
        int getSyncMode(void);

        void setSyncWord(int low, int high);
        int getSyncWordHigh(void);
        int getSyncWordLow(void);
        void setAddressCheck(int chk);
        int getAddressCheck(void);

        void setAddress(int addr);
        int getAddress(void);

        
        void setDataWhitening(int wdata);
        int getDataWhitening(void);

        void setPacketFormat(int pktfmt);
        int getPacketFormat(void);

        void setPacketLengthMode(int mode);
        int getPacketLengthMode(void);
        void setPacketLength(int len);
        int getPacketLength(void);  

        void setCrcEnable(int enable);
        int getCrcEnable(void);  

        void setCrcAf(int af);
        int getCrcAf(void); 

        void setDCFilterOff(int off);
        int getDCFilterOff(void);

        void setManchester(int enable);
        int getManchester(void);

        void setFEC(int enable);
        int getFEC(void);

        void setPQT(int pqt);
        int getPQT(void);

        void setPreambleLen(int len);
        int getPreambleLen(void);

        void setAppendStatus(int enable);
        int getAppendStatus(void);

        int getRSSI(void);
        int getLQI(void);

        void setRX();
        int getRX(void);
        void setTX();
        int getTX(void);

        static String modulation(int mod);

        void apply(struct radio_profile_s* profile);

        void setCCMode(int mode);
        int getCCMode(void);

        void sendData(uint8_t* data, int len);
        
    private:
        void default_init(void);
        int lastFreq;
        int mod;
        int chan;
        int chanSp;
        int dev;
        int rxbw;
        int drate;
        int ccmode;
        int pa;
        int smode;
        int swordHi;
        int swordLo;
        int achk;
        int addr;
        int dataw;
        int pktfmt;
        int pktlmode;
        int pktlen;
        int crcen;
        int crcaf;
        int dcfilteroff;
        int manchen;
        int fecen;
        int prelen;
        int pqt;
        int asen;
        int rxen;
        int txen;
};
//------------------------------------------------------------------------------------------------------------------
#endif// __CC1101_H__
//------------------------------------------------------------------------------------------------------------------
