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
CmdStatus_e open_sesame_protocols(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_help(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setcode(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setmask(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_setirange(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_somfy_process(void* parent,int argc, char* argv[]);
CmdHandler* getRootCommandHandler(void);
CC1101* getRadio(void);
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
  
} SomfyBruteContext_t;
//------------------------------------------------------------------------------------------------------------------
SomfyBruteContext_t somfy_ctx;
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t open_sesame_handler = { &open_sesame_main, NULL, NULL, "opensesame", "Open sesame for various radio protocols, use: 'opensesame help'", 0, "", "opensesame help" };
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t open_sesame_sub_cmd[] = {
   { &open_sesame_main,      &open_sesame_handler,   NULL, "help",  "this help", 0, "", "opensesame help"},
   { &open_sesame_protocols, &open_sesame_handler,   NULL, "prot",  "list registered protocols", 0, "", "opensesame prot"},
   { &open_sesame_somfy,     &open_sesame_handler,   NULL, "somfy",  "somfy iterated frames flood", 0, "", "opensesame somfy help"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t open_sesame_somfy_sub_cmd[] = {
   { &open_sesame_somfy_help,      &open_sesame_handler, NULL, "help", "this help", 0, "", "opensesame somfy help"},
   { &open_sesame_somfy_setcode,   &open_sesame_handler, NULL, "scod", "set codes c0<32bit hex> c1<32bit hex> c2<432bit hex>", 3, "xxx", "opensesame somfy scod 09800000 00000000 ffea8000"},
   { &open_sesame_somfy_setmask,   &open_sesame_handler, NULL, "smsk", "set iteration mask c0msk<hex> c1msk<hex> c2msk<hex>", 3, "xxx", "opensesame somfy smsk 007FFFFF FFFFFFFF 00070000"},
   { &open_sesame_somfy_setirange, &open_sesame_handler, NULL, "sir",  "set iteration range start_hi<hex> start_lo<hex> end_hi<hex> end_lo<hex>", 4, "xxxx", "opensesame somfy sir 00000000 00000000 FFFFFFFF FFFFFFFF"},
   { &open_sesame_somfy_process  , &open_sesame_handler, NULL, "brute", "brute iteration of codes, display interval<int>", 1, "i", "opensesame somfy brute 10000<int ms>"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
void opensesame_cmd_init(void)
{ 
  open_sesame_sub_cmd[2].sub_cmd = open_sesame_somfy_sub_cmd;
  getRootCommandHandler()->registerHandler(&open_sesame_handler, open_sesame_sub_cmd); 
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
    
     return OK;
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
CmdStatus_e open_sesame_protocols(void* parent,int argc, char* argv[])
{
    int maxLen = sizeof(osprots) / sizeof(Protocol*);
    
    STDOUT.println("protocols: ");
    
    for (int idx = 0 ; idx < maxLen; idx++)
    {
        STDOUT.print(osprots[idx]->getName());
        if (idx < maxLen - 1)
            STDOUT.print(",");
              
    }
    STDOUT.print("\n\r");
 
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
String open_sesame_somfy_ctx_describe(uint32_t c0, uint32_t c1, uint32_t c2, struct SomfyBruteContext_s& ctx)
{
     char buffer[128];
     sprintf(buffer, "cnt:%08x%08x c0: %08x c1: %08x c2: %08x", 
        (uint32_t)(ctx.curr >> 32) & 0xffffffff, (uint32_t)(ctx.curr) & 0xffffffff,
        c0, c1, c2);

     return String(buffer);
    
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

    somfy_ctx.curr = somfy_ctx.itstart;
    uint32_t s = millis();
    while(somfy_ctx.curr <= somfy_ctx.itend && cont)
    {
       c0 = somfy_ctx.c0;
       c1 = somfy_ctx.c1;
       c2 = somfy_ctx.c2;

       apply_value_to_bitmask(&c0, &c1, &c2, somfy_ctx.c0msk,somfy_ctx.c1msk,somfy_ctx.c2msk, somfy_ctx.curr);

       msg = "brut tx:" + open_sesame_somfy_ctx_describe(c0,c1,c2,somfy_ctx);
       if (millis() -s > disp)
       {

           STDOUT.print(msg); STDOUT.print("\n\r");
           s = millis();
       }

      
       somfy_ctx.curr++;
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
    msg = "brut last tx:" + open_sesame_somfy_ctx_describe(c0,c1,c2,somfy_ctx);
    STDOUT.print(msg); STDOUT.print("\n\r");
    STDOUT.print("brute stop\n\r");
    

    return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
