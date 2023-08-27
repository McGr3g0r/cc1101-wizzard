//------------------------------------------------------------------------------------------------------------------
#include "CmdHandler.h"
#include "radio_profile.h"
#include "radio_pulses.h"
#include "utils.h"
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_main(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setfreq(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setmod(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setdev(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setchan(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setchansp(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setrxbw(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setdrate(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setpa(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setsyncmode(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setsyncword(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setaddrchk(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setaddress(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setwdata(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setpktfmt(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setpktlmode(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setpktlen(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setcrc(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setcrcaf(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setdcfilteroff(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setmanch(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setfec(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setpre(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setpqt(void* parent,int argc, char* argv[]);
CmdStatus_e radio_setas(void* parent,int argc, char* argv[]);
CmdStatus_e radio_getrssi(void* parent,int argc, char* argv[]);
CmdStatus_e radio_profiles_list(void* parent,int argc, char* argv[]);
CmdStatus_e radio_profiles_apply(void* parent,int argc, char* argv[]);
CmdStatus_e radio_scan(void* parent,int argc, char* argv[]);
CmdStatus_e radio_pulses_count(void* parent,int argc, char* argv[]);
CmdStatus_e radio_pulses_flush(void* parent,int argc, char* argv[]);
CmdStatus_e radio_pulses_grab(void* parent,int argc, char* argv[]);
CmdStatus_e radio_pulses_show(void* parent,int argc, char* argv[]);
CmdStatus_e radio_pulses_play(void* parent,int argc, char* argv[]);
CmdStatus_e radio_pulses_set(void* parent,int argc, char* argv[]);
CmdStatus_e radio_pulses_add(void* parent,int argc, char* argv[]);
CmdStatus_e radio_jammer(void* parent,int argc, char* argv[]);
//------------------------------------------------------------------------------------------------------------------
CmdHandler* getRootCommandHandler(void);
CC1101* getRadio(void);
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t radio_handler        = { &radio_main, NULL, NULL, "radio", "CC1101 radio commands, use: 'radio help'", 0, "", "radio help" };
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t radio_sub_cmd[] = {
   { &radio_main,    &radio_handler,   NULL, "help",  "this help", 0, "", "radio help"},
   { &radio_setfreq, &radio_handler,   NULL, "sfreq", "set frequency, use Khz, for example 433920'", 1, "i", "radio sfreq 433920"},
   { &radio_setmod,  &radio_handler,   NULL, "smod",  "set modulation, 0 to 4 or 2fsk, gfsk, ook, 4fsk, msk", 1, "f", "radio smod ook"},
   { &radio_setdev,  &radio_handler,   NULL, "sdev",  "set freqency deviation in Hz. Value from 158 to 380850. Default is 47600 Hz.",1, "i","radio sdev 47600" },
   { &radio_setchan, &radio_handler,   NULL, "schan", "set the channel number from 0 to 255. Default channel is 0",1, "i","radio schan 1" },
   { &radio_setchansp, &radio_handler, NULL, "schsp", "set channel spacing <hz>, it is multiplied by the channel number and added to the base frequency in Hz. Value from 25390 to 405450. Default is 199950 Hz.",1, "f","radio schsp 199950<Hz>" },
   { &radio_setrxbw,  &radio_handler,  NULL, "srxbw", "set the receive bandwidth in Hz. Value from 58030 to 812500. Default is 812500 Hz",1, "i","radio srxbw 815200<Hz>" },
   { &radio_setdrate, &radio_handler,  NULL, "sdrate","set the data rate <Baud>. Value from 20 to 1621830. Default is 99970 Baud",1, "i","radio sdrate 99970<Baud>" },
   { &radio_setpa, &radio_handler,       NULL, "spa",    "set rf transmission power. Values depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12) Default is 12", 1, "i","radio spa 12" },
   { &radio_setsyncmode, &radio_handler, NULL, "ssm", String("set sync mode 0 - no preamble/sync,\n\r") +
        String("\t  1 - 16bit sync word, 2-16/16bits sync word\n\r") +
        String("\t  3 - 30/32bits sync word, 4-No preamble/sync, carrier-sense above threshold\n\r") +
        String("\t  5 - 15/16bits + carrier-sense above threshold\n\r") +
        String("\t  6 - 16/16bits + carrier-sense above threshold\n\r") +
        String("\t  7 - 30/32 + carrier-sense above threshold."), 1, "i","radio ssm 0" },
   { &radio_setsyncword, &radio_handler, NULL, "ssw", "set sync word <high> <low>. Must be the same for the transmitter and receiver. Default is 211,145", 2, "ii","radio ssw 211<high> 145<low>" },
   { &radio_setaddrchk, &radio_handler,  NULL, "sac", "set address check <chk>: 0 = No address check. 1 = Address check, no broadcast. 2 = Address check and 0 (0x00) broadcast. 3 = Address check and 0 (0x00) and 255 (0xFF) broadcast.", 1, "i","radio sac 3" },
   { &radio_setaddress, &radio_handler,  NULL, "sad", "set address <hex address>, address used for packet filtration. Optional broadcast addresses are 0x00 0xFF" , 1, "x","radio sad 0x67" },
   { &radio_setwdata,  &radio_handler,  NULL, "swd", "set white data <whitening>, Turn data whitening on / off. 0 = Whitening off. 1 = Whitening on." , 1, "i","radio swd 0" },
   { &radio_setpktfmt, &radio_handler,  NULL, "spf", String("set packet format, 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.") +
          String("\t  2 = Random TX mode; sends random data using PN9 generator.") +
          String("\t  3 = Asynchronous serial mode"), 1, "i","radio spf 0" },
   { &radio_setpktlmode,  &radio_handler,  NULL, "splm", "set packet length mode, 0 = Fixed packet length mode. 1 = Variable packet length mode. 2 = Infinite packet length mode. 3 = Reserved." , 1, "i","radio splm 0" },
   { &radio_setpktlen,    &radio_handler,  NULL, "spl", "set packet length <len>, set packet length when fixed packed lenght mode is set. If variable packet length mode is used, this value indicates the maximum packet length allowed." , 1, "i","radio spl 20<int>" },

   { &radio_setcrc,    &radio_handler,  NULL, "sc", "switches on/of CRC calculation and check. 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX." , 1, "i","radio sc 0<int>" },
   { &radio_setcrcaf,  &radio_handler,  NULL, "sca", "Enable automatic flush of RX FIFO when CRC is not OK. This requires that only one packet is in the RXIFIFO and that packet length is limited to the RX FIFO size." , 1, "i","radio sca 0<int>" },
   { &radio_setdcfilteroff,  &radio_handler,  NULL, "sdfo", " Disable digital DC blocking filter before demodulator. Only for data rates ≤ 250 kBaud The recommended IF frequency changes when the DC blocking is disabled. 1 = Disable (current optimized). 0 = Enable (better sensitivity)." , 1, "i","radio sdfo 0<int>" },
   { &radio_setmanch,  &radio_handler,  NULL, "sma", " Enables Manchester encoding/decoding. 0 = Disable. 1 = Enable." , 1, "i","radio sma 0<int>" },
   { &radio_setfec  ,  &radio_handler,  NULL, "sfe", " Enables Forward Error Correction (FEC) with interleaving for packet payload (Only supported for fixed packet length mode. 0 = Disable. 1 = Enable." , 1, "i","radio sfe 0<int>" },
   { &radio_setpre  ,  &radio_handler,  NULL, "spr", " Sets the minimum number of preamble bytes to be transmitted. Values: 0 : 2, 1 : 3, 2 : 4, 3 : 6, 4 : 8, 5 : 12, 6 : 16, 7 : 24." , 1, "i","radio spr 0<int>" },
   { &radio_setpqt  ,  &radio_handler,  NULL, "spq", " Preamble quality estimator threshold." , 1, "i","radio spq 0<int>" },
   { &radio_setas  ,   &radio_handler,  NULL, "sas", " Set append status. When enabled, two status bytes will be appended to the payload of the packet. The status bytes contain RSSI and LQI values, as well as CRC OK." , 1, "i","radio sas 1<int>" },
   { &radio_getrssi,   &radio_handler,  NULL, "rssi", "Display quality information about last received frames over RF." , 0, "","radio rssi" },
   { &radio_scan   ,   &radio_handler,  NULL, "scan", "Scan frequency range <start> <stop> Khz for the highest signal." , 2, "ii","radio scan 433900<int> 434000<int>" },
   { &radio_profiles_list,   &radio_handler,  NULL, "prol", "list predefined radio profiels" , 0, "","radio prol" },
   { &radio_profiles_apply, &radio_handler,  NULL, "proa", "apply profile by idx or name" , 1, "t","radio 1<int> or radio profile_name" },
   { &radio_pulses_count,   &radio_handler,  NULL, "pulc", "get pulses count in buffer" , 0, "","radio pulc" },
   { &radio_pulses_flush,   &radio_handler,  NULL, "pulf", "flush puses buffer" , 0, "","radio pulf" },
   { &radio_pulses_show ,   &radio_handler,  NULL, "puls", "show puses buffer" , 0, "","radio puls" },
   { &radio_pulses_grab,    &radio_handler,  NULL, "pulg", "grab pulses <offset><min_time_us><max_space_time_us>" , 3, "iii","radio pulg 0<int buffer offset> 200<int microseconds> 1500<int microseconds>" },  
   { &radio_pulses_play,    &radio_handler,  NULL, "pulp", "play pulses <offset><pulses>" , 2, "ii","radio pulp 0<int> 2000<int>" },
   { &radio_pulses_add,     &radio_handler,  NULL, "pula", "add pulses <pulses> .." , 1, "i","radio pula 0<int> 20<int> ..." },
   { &radio_pulses_addoff,  &radio_handler,  NULL, "puao", "add pulses <offset> <pulses> ..." , 2, "ii","radio puao 0<int> 20<int> ..." },
   { &radio_jammer,         &radio_handler,  NULL, "jam",  "jam current channel <time millis> 0 - until ctrl-c" , 1, "i","radio jam 0<int>" },

      
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
void radio_cmd_init(void)
{ 
  getRootCommandHandler()->registerHandler(&radio_handler, radio_sub_cmd); 
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_main(void* parent,int argc, char* argv[])
{
     STDOUT.println(radio_handler.desc);

     int idx = 0 ;
     cmd_handler_t* item = &radio_handler.sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &radio_handler.sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setfreq(void* parent,int argc, char* argv[])
{
     int freq = atoi(argv[2]);
     getRadio()->setFreq(freq);
     if (getRadio()->getFreq() == freq)
         return CmdStatus_e::OK;
     else
         return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setmod(void* parent,int argc, char* argv[])
{
     int mod = 0;
     if (String(argv[2]) == "2fsk")
         mod = 0;
     else if (String(argv[2]) == "gfsk")
         mod = 1;        
     else if (String(argv[2]) == "ook")
         mod = 2;         
     else if (String(argv[2]) == "4fsk")
         mod = 3;          
     else if (String(argv[2]) == "msk")
         mod = 4;             
     else if (isNumeric(argv[2]))
         mod = atoi(argv[2]);
     else
         return WRONG_PARAMS;
         
     if (mod < 0 || mod > 4)
         mod = 0;

     getRadio()->setModulation(mod);
     if (getRadio()->getModulation() == mod)
         return CmdStatus_e::OK;
     else
         return CmdStatus_e::CC_ERROR;
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setdev(void* parent,int argc, char* argv[])
{
   int dev;
  
   dev = atoi(argv[2]);
   if (dev < 1580 || dev > 80850)
       return WRONG_PARAMS;       
   getRadio()->setDeviation(dev);
   if (getRadio()->getDeviation() == dev)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
   return CmdStatus_e::COMMAND_NOT_FOUND;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setchan(void* parent,int argc, char* argv[])
{
     int chan = atoi(argv[2]);
     getRadio()->setChan(chan);
     if (getRadio()->getChan() == chan)
         return CmdStatus_e::OK;
     else
         return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setchansp(void* parent,int argc, char* argv[])
{
   int chsp;
  
   chsp = atoi(argv[2]);
   if (chsp < 25390 || chsp > 405450)
       return WRONG_PARAMS;       
   getRadio()->setChanSpaceing(chsp);
   if (getRadio()->getChanSpaceing() == chsp)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setrxbw(void* parent,int argc, char* argv[])
{
   int rxbw;
  
   rxbw = atoi(argv[2]);
   if (rxbw < 58030 || rxbw > 812500)
       return WRONG_PARAMS;       
   getRadio()->setRxBw(rxbw);
   if (getRadio()->getRxBw() == rxbw)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setdrate(void* parent,int argc, char* argv[])
{
   int drate;
  
   drate = atoi(argv[2]);
   if (drate < 20 || drate > 1621830)
       return WRONG_PARAMS;       
   getRadio()->setDataRate(drate);
   if (getRadio()->getDataRate() == drate)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setpa(void* parent,int argc, char* argv[])
{
   int pa;
   int allowed_pa[] = {-30,-20,-15,-10,-6,0,5,7,10,11,12 };
  
   pa = atoi(argv[2]);
   if (!intInSet(pa, allowed_pa, sizeof(allowed_pa)/sizeof(int)))
       return WRONG_PARAMS;       
   getRadio()->setPA(pa);
   if (getRadio()->getPA() == pa)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setsyncmode(void* parent,int argc, char* argv[])
{
   int smode;
  
   smode = atoi(argv[2]);
   if (smode < 0 || smode >7)
       return WRONG_PARAMS;       
   getRadio()->setSyncMode(smode);
   if (getRadio()->getSyncMode() == smode)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setsyncword(void* parent,int argc, char* argv[])
{
   int low, high;
  
   high = atoi(argv[2]);
   low = atoi(argv[3]);
   
   if (high < 0 || high >255)
       return WRONG_PARAMS;       
    if (low < 0 || low >255)
       return WRONG_PARAMS;  
   getRadio()->setSyncWord(high, low);
   
   if (getRadio()->getSyncWordLow() == low && getRadio()->getSyncWordHigh() == high)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setaddrchk(void* parent,int argc, char* argv[])
{
   int achk;
  
   achk = atoi(argv[2]);
   if (achk < 0 || achk > 3)
       return WRONG_PARAMS;       
   getRadio()->setAddressCheck(achk);
   if (getRadio()->getAddressCheck() == achk)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setaddress(void* parent,int argc, char* argv[])
{
   int addr;
  
   addr = strtol(argv[2], 0, 16);
   if (addr < 0 || addr > 255)
       return WRONG_PARAMS;       
   getRadio()->setAddress(addr);
   if (getRadio()->getAddress() == addr)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setwdata(void* parent,int argc, char* argv[])
{
   int wdata;
  
   wdata = atoi(argv[2]);
   if (wdata < 0 || wdata > 1)
       return WRONG_PARAMS;       
   getRadio()->setDataWhitening(wdata);
   if (getRadio()->getDataWhitening() == wdata)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setpktfmt(void* parent,int argc, char* argv[])
{
   int pktfmt;
  
   pktfmt = atoi(argv[2]);
   if (pktfmt < 0 || pktfmt > 3)
       return WRONG_PARAMS;       
   getRadio()->setPacketFormat(pktfmt);
   if (getRadio()->getPacketFormat() == pktfmt)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setpktlmode(void* parent,int argc, char* argv[])
{
   int pktlmode;
  
   pktlmode = atoi(argv[2]);
   if (pktlmode < 0 || pktlmode > 3)
       return WRONG_PARAMS;       
   getRadio()->setPacketLengthMode(pktlmode);
   if (getRadio()->getPacketLengthMode() == pktlmode)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setpktlen(void* parent,int argc, char* argv[])
{
   int len;
  
   len = atoi(argv[2]);
   if (len < 0 || len > 64)
       return WRONG_PARAMS;       
   getRadio()->setPacketLength(len);
   if (getRadio()->getPacketLength() == len)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setcrc(void* parent,int argc, char* argv[])
{
   int crcen;
  
   crcen = atoi(argv[2]);
   if (crcen < 0 || crcen > 1)
       return WRONG_PARAMS;       
   getRadio()->setCrcEnable(crcen);
   if (getRadio()->getCrcEnable() == crcen)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setcrcaf(void* parent,int argc, char* argv[])
{
   int crcaf;
  
   crcaf = atoi(argv[2]);
   if (crcaf < 0 || crcaf > 1)
       return WRONG_PARAMS;       
   getRadio()->setCrcAf(crcaf);
   if (getRadio()->getCrcAf() == crcaf)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setdcfilteroff(void* parent,int argc, char* argv[])
{
   int dcfilteroff;
  
   dcfilteroff = atoi(argv[2]);
   if (dcfilteroff < 0 || dcfilteroff > 1)
       return WRONG_PARAMS;       
   getRadio()->setDCFilterOff(dcfilteroff);
   if (getRadio()->getDCFilterOff() == dcfilteroff)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setmanch(void* parent,int argc, char* argv[])
{
   int manch;
  
   manch = atoi(argv[2]);
   if (manch < 0 || manch > 1)
       return WRONG_PARAMS;       
   getRadio()->setManchester(manch);
   if (getRadio()->getManchester() == manch)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setfec(void* parent,int argc, char* argv[])
{
   int fec;
  
   fec = atoi(argv[2]);
   if (fec < 0 || fec > 1)
       return WRONG_PARAMS;       
   getRadio()->setFEC(fec);
   if (getRadio()->getFEC() == fec)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setpre(void* parent,int argc, char* argv[])
{
   int pre;
  
   pre = atoi(argv[2]);
   if (pre < 0 || pre > 7)
       return WRONG_PARAMS;       
   getRadio()->setPreambleLen(pre);
   if (getRadio()->getPreambleLen() == pre)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setpqt(void* parent,int argc, char* argv[])
{
   int pqt;
  
   pqt = atoi(argv[2]);
   if (pqt < 0 || pqt > 7)
       return WRONG_PARAMS;       
   getRadio()->setPQT(pqt);
   if (getRadio()->getPQT() == pqt)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_setas(void* parent,int argc, char* argv[])
{
   int appendStatus;
  
   appendStatus = atoi(argv[2]);
   if (appendStatus < 0 || appendStatus > 1)
       return WRONG_PARAMS;       
   getRadio()->setAppendStatus(appendStatus);
   if (getRadio()->getAppendStatus() == appendStatus)
       return CmdStatus_e::OK;
   else
       return CmdStatus_e::CC_ERROR;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_getrssi(void* parent,int argc, char* argv[])
{      
   char buf[64];
   int r = getRadio()->getRSSI();
   int l = getRadio()->getLQI();
   sprintf(buf, "rssi: %d %d", r, l);
   getRootCommandHandler()->setResultMessage(buf);
   return CmdStatus_e::OK;
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_scan(void* parent,int argc, char* argv[])
{      
   char buf[64];

   int fstart = atoi(argv[2]);
   int fstop =  atoi(argv[3]);
   int mrssi;
   int mfreq;
   int rssi;
   int freq = fstart;
   long cfreq = fstart;

   getRadio()->setRxBw(58);
   getRadio()->setRX();

   mrssi=-100;   

   STDOUT.println("scanning...");
   
   while (!STDIN.available())        
   {
      getRadio()->setFreq(freq);
      rssi = getRadio()->getRSSI();
      if (rssi>-75)
      {
          if (rssi > mrssi)
          {
                mrssi = rssi;  
                mfreq = freq;
          };
      };

      freq+=10;

      if (freq > fstop)
      {
           freq = fstart;

           if (mrssi>-75)
            {
              long fr = mfreq;
              if (fr == cfreq)
                  {
                    STDOUT.print(F("\r\nSignal found at  "));
                    STDOUT.print(F("Freq: "));
                    STDOUT.print(mfreq);
                    STDOUT.print(F(" Rssi: "));
                    STDOUT.println(mrssi);
                    mrssi=-100;
                    cfreq = 0;
                    mfreq = 0;
                  }
              else
                  {
                    cfreq = mfreq;
                    freq  = mfreq - 10;
                    mfreq =  0;
                    mrssi = -100;
                  };
            };
            
      }; // end of IF freq>stop frequency 
      
  };  // End of While 
   return CmdStatus_e::OK;
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_profiles_list(void* parent,int argc, char* argv[])
{      
   
    STDOUT.println("radio profiles:");
    for (int idx = 0; idx < radio_profile_get_count(); idx++)
    {
       struct radio_profile_s* profile = radio_profile_get(idx);
       STDOUT.print("p:");STDOUT.print(profile->name);STDOUT.print(" f:");;STDOUT.print(profile->freq);STDOUT.print(" rxbw:");;STDOUT.print(profile->bwrx);STDOUT.print(" modulation:");;STDOUT.println(CC1101::modulation(profile->mod));
    }

   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_profiles_apply(void* parent,int argc, char* argv[])
{
   char* pid = argv[2];
    for (int idx = 0; idx < radio_profile_get_count(); idx++)
    {
       struct radio_profile_s* profile = radio_profile_get(idx);
       if (profile->name == String(pid))
       {
           getRadio()->apply(profile);
           return CmdStatus_e::OK;
       }
    }
    if (isNumeric(pid))
    {
        int idx = atoi(pid);
        struct radio_profile_s* profile = radio_profile_get(idx);
        if (profile != 0)
        {
            getRadio()->apply(profile);
            return CmdStatus_e::OK;  
        }
    }
 
    return CmdStatus_e::WRONG_PARAMS;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_pulses_count(void* parent,int argc, char* argv[])
{
  
   int cnt = radio_pulses_get_count();
   STDOUT.print("pulses cnt:");STDOUT.println(cnt);
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_pulses_flush(void* parent,int argc, char* argv[])
{
   radio_pulses_flush();
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_pulses_show(void* parent,int argc, char* argv[])
{
    uint16_t* pulses = radio_pulses_get_pulses();
    STDOUT.println("pulses:");
    for (int idx = 0; idx < radio_pulses_get_count(); idx++)
    {
        STDOUT.print(pulses[idx]);
        if (idx < radio_pulses_get_count() - 1)
            STDOUT.print(",");
        if (idx > 0 && ((idx % 16) == 0))
            STDOUT.print("\n\r");
    }
    STDOUT.print("\n\r");
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_pulses_grab(void* parent,int argc, char* argv[])
{
   int offs;
   int mint;
   int maxt;
  
   offs = atoi(argv[2]);
   if (offs < 0 || offs > radio_pulses_max_count())
       return WRONG_PARAMS;    
   mint = atol(argv[3]);
   maxt = atol(argv[4]);
   
   if (mint < 0 || mint > maxt)
      return WRONG_PARAMS;
      
   if (maxt < 0 || maxt < mint)
      return WRONG_PARAMS;
   STDIN.flush();
   STDOUT.println(F("recording pulses..."));
   int  cnt = radio_pulses_grab(STDIN, offs, mint, maxt);
   STDOUT.print("pulses received:");STDOUT.println(cnt);
   
       
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_pulses_play(void* parent,int argc, char* argv[])
{
   int offs;
   int total;
  
   offs = atoi(argv[2]);
   
   if (offs < 0 || offs >= radio_pulses_max_count())
       return CmdStatus_e::WRONG_PARAMS;    
   
   total = atoi(argv[3]);
   
   if (total <= 0 || (total + offs > radio_pulses_max_count()))
      return CmdStatus_e::WRONG_PARAMS;
      
   STDIN.flush();
   int periodUs = radio_pulses_send(offs, total);
   STDOUT.print("pulses sent:");STDOUT.print(periodUs);STDOUT.print(" uS.\n\r");
   
       
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_pulses_addoff(void* parent,int argc, char* argv[])
{
   int offs;
  
   offs = atoi(argv[2]);
   
   if (offs < 0 || (offs + argc -3) >= radio_pulses_max_count())
       return CmdStatus_e::WRONG_PARAMS;    

   if (argc < 3)
       return CmdStatus_e::WRONG_PARAMS;          
   
   for (int pos = 3; pos < argc; pos++)
   {
       if (isNumeric(argv[pos]))
       {
           uint16_t pulse = strtoul(argv[pos], 0, 10);
          if (pulse < 1)
              return CmdStatus_e::WRONG_PARAMS;
          else
          {
            radio_pulses_set(offs++, pulse);    
          }
       }
       else
           return CmdStatus_e::WRONG_PARAMS;
   }
       
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_pulses_add(void* parent,int argc, char* argv[])
{
   if (argc < 2)
       return CmdStatus_e::WRONG_PARAMS;          
   
   for (int pos = 2; pos < argc; pos++)
   {
       if (isNumeric(argv[pos]))
       {
          uint16_t pulse = strtoul(argv[pos], 0, 10);
          if (pulse < 1)
              return CmdStatus_e::WRONG_PARAMS;
          else
          {
            radio_pulses_add(pulse);    
          }
       }
       else
           return CmdStatus_e::WRONG_PARAMS;
   }
       
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e radio_jammer(void* parent,int argc, char* argv[])
{
   int timems;
   boolean done = false;
  
   timems = atoi(argv[2]);
   
   if (timems < 0)
       return CmdStatus_e::WRONG_PARAMS;    

    STDOUT.print("jamming ...");
    if (timems)
    {
      STDOUT.print(" for ");
      STDOUT.print(timems);
      STDOUT.print(" ms");
      STDOUT.print("\n\r");
    }
    else
        STDOUT.println(" press ctrl-c/f/d to terminate...");

   uint32_t s = millis();
   while (!done)
   {
        randomSeed(analogRead(0));
        for (int i = 0; i< CC_PACKET_BUFFER_LEN; i++)
        { 
            radio_buffer_get(0)[i] = (byte)random(255);
        };                
        digitalWrite(RXLED, ~digitalRead(RXLED));
        getRadio()->sendData(radio_buffer_get(0), CC_PACKET_BUFFER_LEN - 4);
        if (timems)
            done = millis() -s >= timems;
        else {
            if (STDIN.available() > 0)
            {
               byte c = STDIN.read();
               
               if (c == CTRL_C || c == CTRL_F || c == CTRL_D)
               {
                   done = true;
                   continue;
               }
            }          
        }        
   }
   digitalWrite(RXLED, LOW);

       
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
