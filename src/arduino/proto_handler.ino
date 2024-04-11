//------------------------------------------------------------------------------------------------------------------
#include "settings.h"
#include "Processor.h"
#include "Protocol.h"
#include "ev1527_protocol.h"
#include "hcs200_protocol.h"
#include "somfy_protocol.h"
#include "db200_protocol.h"
#include "pt2240_protocol.h"
#include "tesla_um2_protocol.h"
#include "retekess_protocol.h"
//------------------------------------------------------------------------------------------------------------------
#include "CmdHandler.h"
#include "radio_profile.h"
#include "radio_pulses.h"
#include "utils.h"
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_main(void* parent,int argc, char* argv[]);
CmdStatus_e proto_list(void* parent,int argc, char* argv[]);
CmdStatus_e proto_somfy(void* parent,int argc, char* argv[]);
CmdStatus_e proto_somfy_help(void* parent,int argc, char* argv[]);
CmdStatus_e proto_somfy_send(void* parent,int argc, char* argv[]);
CmdStatus_e proto_ev1527(void* parent,int argc, char* argv[]);
CmdStatus_e proto_ev1527_help(void* parent,int argc, char* argv[]);
CmdStatus_e proto_ev1527_send(void* parent,int argc, char* argv[]);
CmdStatus_e proto_hcs200(void* parent,int argc, char* argv[]);
CmdStatus_e proto_hcs200_help(void* parent,int argc, char* argv[]);
CmdStatus_e proto_hcs200_send(void* parent,int argc, char* argv[]);

CmdStatus_e proto_db200(void* parent,int argc, char* argv[]);
CmdStatus_e proto_db200_help(void* parent,int argc, char* argv[]);
CmdStatus_e proto_db200_send(void* parent,int argc, char* argv[]);


CmdStatus_e proto_pt2240(void* parent,int argc, char* argv[]);
CmdStatus_e proto_pt2240_help(void* parent,int argc, char* argv[]);
CmdStatus_e proto_pt2240_send(void* parent,int argc, char* argv[]);


CmdStatus_e proto_tesla_um2(void* parent,int argc, char* argv[]);
CmdStatus_e proto_tesla_um2_help(void* parent,int argc, char* argv[]);
CmdStatus_e proto_tesla_um2_send(void* parent,int argc, char* argv[]);


CmdStatus_e proto_retekess(void* parent,int argc, char* argv[]);
CmdStatus_e proto_retekess_help(void* parent,int argc, char* argv[]);
CmdStatus_e proto_retekess_send(void* parent,int argc, char* argv[]);


CmdHandler* getRootCommandHandler(void);
CC1101* getRadio(void);
//------------------------------------------------------------------------------------------------------------------
extern SomfyRTSProtocol somfy;
extern EV1527Protocol ev1527;
extern HCS200Protocol hcs200;
extern DB200Protocol db200;
extern PT2240Protocol pt2240;
extern TeslaUM2Protocol tum2;
extern RetekessProtocol retk;
Protocol* pprots[] = { &ev1527, &hcs200, &somfy, &db200, &pt2240, &tum2, &retk };
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t proto_handler = { &proto_main, NULL, NULL, "protocols", "send data with known protocols, use: 'protocol help'", 0, "", "protocols help" };
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t proto_sub_cmd[] = {
   { &proto_main,      &proto_handler,   NULL, "help",  "this help", 0, "", "protocols help"},
   { &proto_list,      &proto_handler,   NULL, "list",  "list registered protocols", 0, "", "protocols list"},
   { &proto_somfy,     &proto_handler,   NULL, "somfy",  "send somfy rts frame", 0, "", "protocols somfy help"},
   { &proto_ev1527,    &proto_handler,   NULL, "ev1527",  "send ev1527 frame", 0, "", "protocols ev1527 help"},
   { &proto_hcs200,    &proto_handler,   NULL, "hcs200",  "send hcs200 frame", 0, "", "protocols hcs200 help"},
   { &proto_db200,    &proto_handler,   NULL, "db200",  "send db200 frame", 0, "", "protocols db200 help"},
   { &proto_pt2240,    &proto_handler,   NULL, "pt2240",  "send pt2240 frame", 0, "", "protocols pt2240 help"}, 
   { &proto_tesla_um2, &proto_handler,   NULL, "tesla_um2",  "send tesla_um2 frame", 0, "", "protocols tesla_um2 help"},    
   { &proto_retekess , &proto_handler,   NULL, "retekess",  "send retekess frame", 0, "", "protocols retekess help"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t proto_somfy_sub_cmd[] = {
   { &proto_somfy_help,   &proto_handler, NULL, "help", "this help", 0, "", "protocols somfy help"},
   { &proto_somfy_send,   &proto_handler, NULL, "send", "sends somfy rts code: code0<hex> code1<hex> code2<hex> repetitions<int>", 4, "xxxi", "protocols somfy send 098abcdef 12345678 ffea8000 1"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t proto_ev1527_sub_cmd[] = {
   { &proto_ev1527_help,   &proto_handler, NULL, "help", "this help", 0, "", "protocols ev1527 help"},
   { &proto_ev1527_send,   &proto_handler, NULL, "send", "sends ev1527code: code0<hex 20bit> btn<hex 4bit> repetitions<int>", 3, "xxi", "protocols ev1527 send aa330 01 6"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t proto_hcs200_sub_cmd[] = {
   { &proto_hcs200_help,   &proto_handler, NULL, "help", "this help", 0, "", "protocols hcs200 help"},
   { &proto_hcs200_send,   &proto_handler, NULL, "send", "sends hcs200 code: enc<hex 32bit> serial<hex 24bit> btn<hex> repetitions<i>", 4, "xxxi", "protocols hcs200 send 12345678 123456 1 6"},
   { 0,  0, NULL, "",  "", 0, "","" }
};

//------------------------------------------------------------------------------------------------------------------
cmd_handler_t proto_db200_sub_cmd[] = {
   { &proto_db200_help,   &proto_handler, NULL, "help", "this help", 0, "", "protocols db200 help"},
   { &proto_db200_send,   &proto_handler, NULL, "send", "sends db200 code: enc<hex 8bit> repetitions<i>", 2, "xi", "protocols db200 send 59 20"},
   { 0,  0, NULL, "",  "", 0, "","" }
};

//------------------------------------------------------------------------------------------------------------------
cmd_handler_t proto_pt2240_sub_cmd[] = {
   { &proto_pt2240_help,   &proto_handler, NULL, "help", "this help", 0, "", "protocols pt2240 help"},
   { &proto_pt2240_send,   &proto_handler, NULL, "send", "sends pt2240 code: enc<hex 24bit> repetitions<i>", 2, "xi", "protocols pt2240 send aabbcc 20"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t proto_tesla_um2_sub_cmd[] = {
   { &proto_tesla_um2_help,   &proto_handler, NULL, "help", "this help", 0, "", "protocols tesla_um2 help"},
   { &proto_tesla_um2_send,   &proto_handler, NULL, "send", "sends tesla_um2 frame repetitions<int>", 1, "i", "protocols tesla_um2 send 9"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t proto_retekess_sub_cmd[] = {
   { &proto_retekess_help,   &proto_handler, NULL, "help", "this help", 0, "", "protocols retekess help"},
   { &proto_retekess_send,   &proto_handler, NULL, "send", "sends retekess frame id1<hex> id2<hex> <repetitions<int>", 3, "xxi", "protocols retekess send 0fa0 01 50"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
void protocols_cmd_init(void)
{ 
  proto_sub_cmd[2].sub_cmd = proto_somfy_sub_cmd;
  proto_sub_cmd[3].sub_cmd = proto_ev1527_sub_cmd;
  proto_sub_cmd[4].sub_cmd = proto_hcs200_sub_cmd;
  proto_sub_cmd[5].sub_cmd = proto_db200_sub_cmd;
  proto_sub_cmd[6].sub_cmd = proto_pt2240_sub_cmd;
  proto_sub_cmd[7].sub_cmd = proto_tesla_um2_sub_cmd;  
  proto_sub_cmd[8].sub_cmd = proto_retekess_sub_cmd;
  getRootCommandHandler()->registerHandler(&proto_handler, proto_sub_cmd);
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_main(void* parent,int argc, char* argv[])
{
     STDOUT.println(proto_handler.desc);

     int idx = 0 ;
     cmd_handler_t* item = &proto_handler.sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &proto_handler.sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_list(void* parent,int argc, char* argv[])
{
    int maxLen = sizeof(pprots) / sizeof(Protocol*);
    
    STDOUT.println("protocols: ");
    
    for (int idx = 0 ; idx < maxLen; idx++)
    {
        STDOUT.print(pprots[idx]->getName());
        if (idx < maxLen - 1)
            STDOUT.print(",");
              
    }
    STDOUT.print("\n\r");
 
  return OK;
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_somfy(void* parent,int argc, char* argv[])
{
     int idx = 0 ;
     cmd_handler_t* item = &proto_somfy_sub_cmd[idx];

     if (argc < 3)
        return proto_somfy_help(parent, argc, argv);
     
     while (item && item->handler != 0)
     {
            if (String(argv[2]) == item->cmd)
            {
                return item->handler(getRootCommandHandler(), argc, argv);
            }
            idx++;
            item = &proto_somfy_sub_cmd[idx];
      
     }
    
     return COMMAND_NOT_FOUND;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_tesla_um2(void* parent,int argc, char* argv[])
{
     int idx = 0 ;
     cmd_handler_t* item = &proto_tesla_um2_sub_cmd[idx];

     if (argc < 3)
        return proto_tesla_um2_help(parent, argc, argv);
     
     while (item && item->handler != 0)
     {
            if (String(argv[2]) == item->cmd)
            {
                return item->handler(getRootCommandHandler(), argc, argv);
            }
            idx++;
            item = &proto_tesla_um2_sub_cmd[idx];
      
     }
    
     return COMMAND_NOT_FOUND;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_retekess(void* parent,int argc, char* argv[])
{
     int idx = 0 ;
     cmd_handler_t* item = &proto_retekess_sub_cmd[idx];

     if (argc < 3)
        return proto_retekess_help(parent, argc, argv);
     
     while (item && item->handler != 0)
     {
            if (String(argv[2]) == item->cmd)
            {
                return item->handler(getRootCommandHandler(), argc, argv);
            }
            idx++;
            item = &proto_retekess_sub_cmd[idx];
      
     }
    
     return COMMAND_NOT_FOUND;
}
//-----------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_ev1527(void* parent,int argc, char* argv[])
{
     int idx = 0 ;
     cmd_handler_t* item = &proto_ev1527_sub_cmd[idx];

     if (argc < 3)
        return proto_ev1527_help(parent, argc, argv);
     
     while (item && item->handler != 0)
     {
            if (String(argv[2]) == item->cmd)
            {
                return item->handler(getRootCommandHandler(), argc, argv);
            }
            idx++;
            item = &proto_ev1527_sub_cmd[idx];
      
     }
    
     return COMMAND_NOT_FOUND;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_hcs200(void* parent,int argc, char* argv[])
{
     int idx = 0 ;
     cmd_handler_t* item = &proto_hcs200_sub_cmd[idx];

     if (argc < 3)
        return proto_hcs200_help(parent, argc, argv);
     
     while (item && item->handler != 0)
     {
            if (String(argv[2]) == item->cmd)
            {
                return item->handler(getRootCommandHandler(), argc, argv);
            }
            idx++;
            item = &proto_hcs200_sub_cmd[idx];
      
     }
    
     return COMMAND_NOT_FOUND;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_db200(void* parent,int argc, char* argv[])
{
     int idx = 0 ;
     cmd_handler_t* item = &proto_db200_sub_cmd[idx];

     if (argc < 3)
        return proto_db200_help(parent, argc, argv);
     
     while (item && item->handler != 0)
     {
            if (String(argv[2]) == item->cmd)
            {
                return item->handler(getRootCommandHandler(), argc, argv);
            }
            idx++;
            item = &proto_db200_sub_cmd[idx];
      
     }
    
     return COMMAND_NOT_FOUND;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_pt2240(void* parent,int argc, char* argv[])
{
     int idx = 0 ;
     cmd_handler_t* item = &proto_pt2240_sub_cmd[idx];

     if (argc < 3)
        return proto_pt2240_help(parent, argc, argv);
     
     while (item && item->handler != 0)
     {
            if (String(argv[2]) == item->cmd)
            {
                return item->handler(getRootCommandHandler(), argc, argv);
            }
            idx++;
            item = &proto_pt2240_sub_cmd[idx];
      
     }
    
     return COMMAND_NOT_FOUND;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_somfy_help(void* parent,int argc, char* argv[])
{
     STDOUT.println(proto_sub_cmd[2].desc);

     int idx = 0 ;
     cmd_handler_t* item = &proto_somfy_sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &proto_somfy_sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_tesla_um2_help(void* parent,int argc, char* argv[])
{
     STDOUT.println(proto_sub_cmd[7].desc);

     int idx = 0 ;
     cmd_handler_t* item = &proto_tesla_um2_sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &proto_tesla_um2_sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_retekess_help(void* parent,int argc, char* argv[])
{
     STDOUT.println(proto_sub_cmd[8].desc);

     int idx = 0 ;
     cmd_handler_t* item = &proto_retekess_sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &proto_retekess_sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_ev1527_help(void* parent,int argc, char* argv[])
{
     STDOUT.println(proto_sub_cmd[3].desc);

     int idx = 0 ;
     cmd_handler_t* item = &proto_ev1527_sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &proto_ev1527_sub_cmd[idx];
      
     }
    
     return OK;
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_hcs200_help(void* parent,int argc, char* argv[])
{
     STDOUT.println(proto_sub_cmd[4].desc);

     int idx = 0 ;
     cmd_handler_t* item = &proto_hcs200_sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &proto_hcs200_sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_db200_help(void* parent,int argc, char* argv[])
{
     STDOUT.println(proto_sub_cmd[4].desc);

     int idx = 0 ;
     cmd_handler_t* item = &proto_db200_sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &proto_db200_sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_pt2240_help(void* parent,int argc, char* argv[])
{
     STDOUT.println(proto_sub_cmd[5].desc);

     int idx = 0 ;
     cmd_handler_t* item = &proto_pt2240_sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &proto_pt2240_sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_somfy_send(void* parent,int argc, char* argv[])
{
   uint32_t c0 = strtoul(argv[3], 0, 16);
   uint32_t c1 = strtoul(argv[4], 0, 16);      
   uint32_t c2 = strtoul(argv[5], 0, 16);   
   int rep_cnt  = atoi(argv[6]); 

   int pulses;
   int p;
   
   somfy.setCodes(c0, c1, c2);

   if (c0 == 0 || c2 == 0 || c1 ==0)
       return WRONG_PARAMS;
   if (rep_cnt < 0 || rep_cnt > 12)
       return WRONG_PARAMS;
       
   radio_pulses_flush();   
   pulses = 0;
   p = 0;
   for (int rep =0; rep < rep_cnt+1; rep++)
   {  
      somfy.toPulses(&radio_pulses_get_pulses()[pulses], radio_pulses_max_count() - pulses, &p, rep);     
      pulses += p;
   }
   radio_pulses_send(0, pulses);
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_tesla_um2_send(void* parent,int argc, char* argv[])
{  
   int rep_cnt  = atoi(argv[3]); 

   int pulses;
   int p;
 
   if (rep_cnt < 0 || rep_cnt > 40)
       return WRONG_PARAMS;

   tum2.setData(0);
       
   radio_pulses_flush();   
   pulses = 0;
   p = 0;
   for (int rep =0; rep < rep_cnt+1; rep++)
   {  
      tum2.toPulses(&radio_pulses_get_pulses()[pulses], radio_pulses_max_count() - pulses, &p, rep);     
      pulses += p;
   }
   radio_pulses_send(0, pulses);
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_ev1527_send(void* parent,int argc, char* argv[])
{
   uint32_t c0 =  strtoul(argv[3], 0, 16) & 0xfffff;
   uint32_t btn = strtoul(argv[4], 0, 16) & 0xf;  
   int rep_cnt  = atoi(argv[5]);
   int pulses;
   int p;
   
   ev1527.setData(c0, btn);

   if (c0 == 0 || btn == 0)
       return WRONG_PARAMS;


   if (rep_cnt < 0 || rep_cnt > 16)
       return WRONG_PARAMS;

   radio_pulses_flush();  
   pulses = 0;
   p = 0; 
   for (int rep =0; rep < rep_cnt+1; rep++)
   {
       ev1527.toPulses(&radio_pulses_get_pulses()[pulses], radio_pulses_max_count() - pulses, &p, rep);
       pulses += p;
   }
   radio_pulses_send(0, pulses);
   
   return CmdStatus_e::OK;
}

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_hcs200_send(void* parent,int argc, char* argv[])
{
   uint32_t enc    =  strtoul(argv[3], 0, 16);
   uint32_t serial =  strtoul(argv[4], 0, 16) & 0xffffff; //24bits
   uint8_t btn    =  strtoul(argv[5], 0, 16) & 0xf;  
   int rep_cnt  = atoi(argv[6]);
   int pulses;
   int p;
   
   

   if (enc == 0 || serial == 0)
       return WRONG_PARAMS;


   if (rep_cnt < 0 || rep_cnt > 16)
       return WRONG_PARAMS;

   radio_pulses_flush();  
   pulses = 0;
   p = 0; 
   for (int rep =0; rep < rep_cnt+1; rep++)
   {
       hcs200.setData(enc, serial, btn, false, false, rep > 0);
       hcs200.toPulses(&radio_pulses_get_pulses()[pulses], radio_pulses_max_count() - pulses, &p, rep);
       pulses += p;
   }
   radio_pulses_send(0, pulses);
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_db200_send(void* parent,int argc, char* argv[])
{
   uint8_t code =  strtoul(argv[3], 0, 16);  
   int rep_cnt  = atoi(argv[4]);
   int pulses;
   int p;
   
   
   if (code == 0)
       return WRONG_PARAMS;


   if (rep_cnt < 0 || rep_cnt > 100)
       return WRONG_PARAMS;

   radio_pulses_flush();  
   db200.setData(code);
   pulses = 0;
   p = 0; 
   for (int rep =0; rep < rep_cnt+1; rep++)
   {
       db200.toPulses(&radio_pulses_get_pulses()[pulses], radio_pulses_max_count() - pulses, &p, rep);
       pulses += p;
   }
   radio_pulses_send(0, pulses);
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_pt2240_send(void* parent,int argc, char* argv[])
{
   uint8_t code =  strtoul(argv[3], 0, 16);  
   int rep_cnt  = atoi(argv[4]);
   int pulses;
   int p;
   
   
   if (code == 0)
       return WRONG_PARAMS;


   if (rep_cnt < 0 || rep_cnt > 100)
       return WRONG_PARAMS;

   radio_pulses_flush();  
   pt2240.setData(code);
   pulses = 0;
   p = 0; 
   for (int rep =0; rep < rep_cnt+1; rep++)
   {
       pt2240.toPulses(&radio_pulses_get_pulses()[pulses], radio_pulses_max_count() - pulses, &p, rep);
       pulses += p;
   }
   radio_pulses_send(0, pulses);
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e proto_retekess_send(void* parent,int argc, char* argv[])
{
   uint32_t c0 = strtoul(argv[3], 0, 16);
   uint32_t c1 = strtoul(argv[4], 0, 16);
   int rep_cnt  = atoi(argv[5]); 

   int pulses;
   int p;
   
   retk.setData(c0 << 12 | c1);

   if (c0 == 0 || c1 ==0)
       return WRONG_PARAMS;
   if (rep_cnt < 0 || rep_cnt > 50)
       return WRONG_PARAMS;
       
   radio_pulses_flush();   
   pulses = 0;
   p = 0;
   for (int rep =0; rep < rep_cnt; rep++)
   {  
      retk.toPulses(&radio_pulses_get_pulses()[pulses], radio_pulses_max_count() - pulses, &p, rep);     
      pulses += p;
   }
   radio_pulses_send(0, pulses);
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
