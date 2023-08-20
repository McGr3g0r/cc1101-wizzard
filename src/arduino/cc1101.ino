//------------------------------------------------------------------------------------------------------------------
#include "cc1101.h"
#include "settings.h"
#include "platform.h"

#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <SPI.h>
//------------------------------------------------------------------------------------------------------------------
CC1101::CC1101(void)
{
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::init(void)
{
     default_init();
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::default_init(void)
{
    // initializing library with custom pins selected
     ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, ss);
     ELECHOUSE_cc1101.setGDO(gdo0, gdo2);

    // Main part to tune CC1101 with proper frequency, modulation and encoding    
    ELECHOUSE_cc1101.Init();                // must be set to initialize the cc1101!
    ELECHOUSE_cc1101.setGDO0(gdo0);         // set lib internal gdo pin (gdo0). Gdo2 not use for this example.
    ELECHOUSE_cc1101.setCCMode(1);          // set config for internal transmission mode. value 0 is for RAW recording/replaying
    ELECHOUSE_cc1101.setModulation(2);      // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
    ELECHOUSE_cc1101.setMHZ(433.92);        // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
    ELECHOUSE_cc1101.setDeviation(47.60);   // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
    ELECHOUSE_cc1101.setChannel(0);         // Set the Channelnumber from 0 to 255. Default is cahnnel 0.
    ELECHOUSE_cc1101.setChsp(199.95);       // The channel spacing is multiplied by the channel number CHAN and added to the base frequency in kHz. Value from 25.39 to 405.45. Default is 199.95 kHz.
    ELECHOUSE_cc1101.setRxBW(812.50);       // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
    ELECHOUSE_cc1101.setDRate(9.6);         // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
    ELECHOUSE_cc1101.setPA(10);             // Set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12) Default is max!
    ELECHOUSE_cc1101.setSyncMode(2);        // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
    ELECHOUSE_cc1101.setSyncWord(211, 145); // Set sync word. Must be the same for the transmitter and receiver. Default is 211,145 (Syncword high, Syncword low)
    ELECHOUSE_cc1101.setAdrChk(0);          // Controls address check configuration of received packages. 0 = No address check. 1 = Address check, no broadcast. 2 = Address check and 0 (0x00) broadcast. 3 = Address check and 0 (0x00) and 255 (0xFF) broadcast.
    ELECHOUSE_cc1101.setAddr(0);            // Address used for packet filtration. Optional broadcast addresses are 0 (0x00) and 255 (0xFF).
    ELECHOUSE_cc1101.setWhiteData(0);       // Turn data whitening on / off. 0 = Whitening off. 1 = Whitening on.
    ELECHOUSE_cc1101.setPktFormat(0);       // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
    ELECHOUSE_cc1101.setLengthConfig(1);    // 0 = Fixed packet length mode. 1 = Variable packet length mode. 2 = Infinite packet length mode. 3 = Reserved
    ELECHOUSE_cc1101.setPacketLength(0);    // Indicates the packet length when fixed packet length mode is enabled. If variable packet length mode is used, this value indicates the maximum packet length allowed.
    ELECHOUSE_cc1101.setCrc(0);             // 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX.
    ELECHOUSE_cc1101.setCRC_AF(0);          // Enable automatic flush of RX FIFO when CRC is not OK. This requires that only one packet is in the RXIFIFO and that packet length is limited to the RX FIFO size.
    ELECHOUSE_cc1101.setDcFilterOff(0);     // Disable digital DC blocking filter before demodulator. Only for data rates ≤ 250 kBaud The recommended IF frequency changes when the DC blocking is disabled. 1 = Disable (current optimized). 0 = Enable (better sensitivity).
    ELECHOUSE_cc1101.setManchester(0);      // Enables Manchester encoding/decoding. 0 = Disable. 1 = Enable.
    ELECHOUSE_cc1101.setFEC(0);             // Enable Forward Error Correction (FEC) with interleaving for packet payload (Only supported for fixed packet length mode. 0 = Disable. 1 = Enable.
    ELECHOUSE_cc1101.setPRE(0);             // Sets the minimum number of preamble bytes to be transmitted. Values: 0 : 2, 1 : 3, 2 : 4, 3 : 6, 4 : 8, 5 : 12, 6 : 16, 7 : 24
    ELECHOUSE_cc1101.setPQT(0);             // Preamble quality estimator threshold. The preamble quality estimator increases an internal counter by one each time a bit is received that is different from the previous bit, and decreases the counter by 8 each time a bit is received that is the same as the last bit. A threshold of 4∙PQT for this counter is used to gate sync word detection. When PQT=0 a sync word is always accepted.
    ELECHOUSE_cc1101.setAppendStatus(0);    // When enabled, two status bytes will be appended to the payload of the packet. The status bytes contain RSSI and LQI values, as well as CRC OK.

    lastFreq = 433920;
    mod = 2;
    dev = 47600;
    chan = 0;
    chanSp = 199950;
    ccmode = 1;
    rxbw = 812500;
    drate = 9600;
    pa = 10;
    smode = 2;
    achk = 0;
    addr = 0;
    dataw = 0;
    pktfmt = 0;
    pktlmode = 1;
    pktlen = 0;
    crcen = 0;
    crcaf = 0;
    dcfilteroff = 0;
    manchen = 0;
    fecen = 0;
    prelen = 0;
    pqt = 0;
    asen = 0;
    rxen =0;
    txen = 0;
}
//------------------------------------------------------------------------------------------------------------------
bool CC1101::check(void)
{
  return ELECHOUSE_cc1101.getCC1101();
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setFreq(int khz)
{
    if (khz >= 300000 && khz <= 920000)
    {
        lastFreq = khz;
        ELECHOUSE_cc1101.setMHZ(khz/1000.0); 
    }
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getFreq(void)
{
   return lastFreq;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setModulation(int mod)
{
  ELECHOUSE_cc1101.setModulation(mod);
  this->mod = mod;
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getModulation(void)
{
    return mod;
}
 //------------------------------------------------------------------------------------------------------------------
void CC1101::setDeviation(int dev)
{
   this->dev = dev;
    ELECHOUSE_cc1101.setDeviation(dev / 1000.0);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getDeviation(void)
{
    return dev;
}
 //------------------------------------------------------------------------------------------------------------------
void CC1101::setChanSpaceing(int sp)
{
   this->chanSp = sp;
   ELECHOUSE_cc1101.setChsp(sp / 1000.0);
}
 //------------------------------------------------------------------------------------------------------------------
int CC1101::getChanSpaceing(void)
{
    return chanSp;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setChan(int chan)
{
  this->chan = chan;
  ELECHOUSE_cc1101.setChannel(chan);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getChan(void)
{
  return chan;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setRxBw(int val)
{
   this->rxbw = val;
   ELECHOUSE_cc1101.setRxBW(val / 1000.0);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getRxBw(void)
{
    return rxbw;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setDataRate(int val)
{
   this->drate = val;
   ELECHOUSE_cc1101.setDRate(val / 1000.0);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getDataRate(void)
{
    return drate;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setPA(int val)
{
   this->pa = val;
   ELECHOUSE_cc1101.setPA(val);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getPA(void)
{
    return pa;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setSyncMode(int val)
{
   this->smode = val;
   ELECHOUSE_cc1101.setSyncMode(val);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getSyncMode(void)
{
    return smode;
}
//------------------------------------------------------------------------------------------------------------------ 
void CC1101::setSyncWord(int low, int high)
{
   this->swordHi = high;
   this->swordLo = low;
   ELECHOUSE_cc1101.setSyncWord(high, low); 
}
//------------------------------------------------------------------------------------------------------------------ 
int CC1101::getSyncWordHigh(void)
{
   return swordHi;
}
//------------------------------------------------------------------------------------------------------------------ 
int CC1101::getSyncWordLow(void)
{
   return swordLo;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setAddressCheck(int val)
{
   this->achk = val;
   ELECHOUSE_cc1101.setAdrChk(val);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getAddressCheck(void)
{
    return achk;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setAddress(int val)
{
   this->addr = val;
   ELECHOUSE_cc1101.setAddr(val);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getAddress(void)
{
    return addr;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setDataWhitening(int dw)
{
  this->dataw = dw;
  ELECHOUSE_cc1101.setWhiteData(dw);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getDataWhitening(void)
{
    return dataw;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setPacketFormat(int pktfmt)
{
  this->pktfmt = pktfmt;
  ELECHOUSE_cc1101.setPktFormat(pktfmt);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getPacketFormat(void)
{
    return pktfmt;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setPacketLengthMode(int pktlmode)
{
  this->pktlmode = pktlmode;
  ELECHOUSE_cc1101.setLengthConfig(pktlmode);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getPacketLengthMode(void)
{
    return pktlmode;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setPacketLength(int len)
{
  this->pktlen = len;
  ELECHOUSE_cc1101.setPacketLength(len);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getPacketLength(void)
{
    return pktlen;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setCrcEnable(int crcen)
{
  this->crcen = crcen;
  ELECHOUSE_cc1101.setCrc(crcen);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getCrcEnable(void)
{
    return crcen;
} 
//------------------------------------------------------------------------------------------------------------------
void CC1101::setCrcAf(int af)
{
  this->crcaf = af;
  ELECHOUSE_cc1101.setCRC_AF(af);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getCrcAf(void)
{
    return crcaf;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setDCFilterOff(int off)
{
  this->dcfilteroff = off;
  ELECHOUSE_cc1101.setDcFilterOff(off);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getDCFilterOff(void)
{
  return dcfilteroff;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setManchester(int enable)
{
  this->manchen = enable;
  ELECHOUSE_cc1101.setManchester(enable);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getManchester(void)
{
  return manchen;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setFEC(int enable)
{
  this->fecen = enable;
  ELECHOUSE_cc1101.setFEC(enable);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getFEC(void)
{
  return fecen;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setPreambleLen(int len)
{
  this->prelen = len;
  ELECHOUSE_cc1101.setPRE(len);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getPreambleLen(void)
{
  return prelen;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setPQT(int pqt)
{
  this->pqt = pqt;
  ELECHOUSE_cc1101.setPQT(pqt);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getPQT(void)
{
  return pqt;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setAppendStatus(int enable)
{
  this->asen = enable;
  ELECHOUSE_cc1101.setAppendStatus(asen);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getAppendStatus()
{
  return asen;
}
//------------------------------------------------------------------------------------------------------------------
int  CC1101::getRSSI(void)
{
  return ELECHOUSE_cc1101.getRssi();
}
//------------------------------------------------------------------------------------------------------------------
int  CC1101::getLQI(void)
{
  return ELECHOUSE_cc1101.getLqi();
}
//---------------------------------------------------------------------------------
String CC1101::modulation(int mod)
{
    if (mod == 0)
        return "2fsk";
    else if (mod == 1)
        return "gfsk";      
    else if (mod == 2)
        return "ook"; 
     else if (mod == 3)
        return "4fsk";   
     else if (mod == 4)
        return "msk";  
    else
        return "unknown";
}
//---------------------------------------------------------------------------------
void CC1101::apply(struct radio_profile_s* profile)
{
     setFreq(profile->freq);
     setModulation(profile->mod);
     setRxBw(profile->bwrx);
     setChan(profile->chan);
}
//---------------------------------------------------------------------------------
void CC1101::setRX()
{
  this->rxen = true;
  this->txen = false;
  ELECHOUSE_cc1101.SetRx();
}
//---------------------------------------------------------------------------------
int CC1101::getRX(void)
{
    return rxen;
}
//---------------------------------------------------------------------------------
void CC1101::setTX()
{
  this->txen = true;
  this->rxen = false;
  ELECHOUSE_cc1101.SetTx();
}
//---------------------------------------------------------------------------------
int CC1101::getTX(void)
{
    return txen;
}
//------------------------------------------------------------------------------------------------------------------
void CC1101::setCCMode(int mode)
{
  this->ccmode = mode;
  ELECHOUSE_cc1101.setCCMode(ccmode);
}
//------------------------------------------------------------------------------------------------------------------
int CC1101::getCCMode(void)
{
  return ccmode;
}
//---------------------------------------------------------------------------------
