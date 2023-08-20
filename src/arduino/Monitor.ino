//------------------------------------------------------------------------------------------------------------------
#include "settings.h"
#include "Processor.h"
#include "Protocol.h"
#include "ev1527_protocol.h"
#include "hcs200_protocol.h"
//------------------------------------------------------------------------------------------------------------------
#include "CmdHandler.h"
#include "radio_profile.h"
#include "radio_pulses.h"
#include "utils.h"
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e monitor_main(void* parent,int argc, char* argv[]);
CmdStatus_e monitor_start(void* parent,int argc, char* argv[]);
CmdStatus_e monitor_protocols(void* parent,int argc, char* argv[]);
CmdHandler* getRootCommandHandler(void);
CC1101* getRadio(void);
//------------------------------------------------------------------------------------------------------------------
EV1527Protocol ev1527;
HCS200Protocol hcs200;
Protocol* protocols[] = { &ev1527, &hcs200 };
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t monitor_handler = { &monitor_main, NULL, NULL, "monitor", "Monitoring protocol utility, use: 'monitor help'", 0, "", "monitor help" };
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t monitor_sub_cmd[] = {
   { &monitor_main,    &monitor_handler,   NULL, "help",  "this help", 0, "", "somfy help"},
   { &monitor_start, &monitor_handler,   NULL, "start",  "starts monitoring, stops with ctrl-c", 0, "", "monitor start"},
   { &monitor_protocols, &monitor_handler,   NULL, "prot",  "list registered protocols", 0, "", "monitor prot"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
void monitor_cmd_init(void)
{ 
  getRootCommandHandler()->registerHandler(&monitor_handler, monitor_sub_cmd); 
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e monitor_main(void* parent,int argc, char* argv[])
{
     STDOUT.println(monitor_handler.desc);

     int idx = 0 ;
     cmd_handler_t* item = &monitor_handler.sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &monitor_handler.sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
int monitor_get_minimal_time(void)
{
    int res = 0;
    for (int idx = 0 ; idx < sizeof(protocols) / sizeof(Protocol*); idx++)
    {
         int v = protocols[idx]->getMinimalTime();
         if (idx == 0 || res > v)
            res = v;          
    }

    return res;
}
//------------------------------------------------------------------------------------------------------------------
int monitor_get_maximal_time(void)
{
    int res = 0;
    for (int idx = 0 ; idx < sizeof(protocols) / sizeof(Protocol*); idx++)
    {
         int v = protocols[idx]->getMinimalTime();
         if (idx == 0 || res < v)
            res = v;          
    }

    return res;
}

//------------------------------------------------------------------------------------------------------------------
int monitor_protocols_get_count(void)
{
   return sizeof(protocols) / sizeof(Protocol*);
}
void monitor_process(int minPulseTime, int maxObsTime)
{
     if (maxObsTime < 2000000)
         maxObsTime = 2000000;
         
     radio_pulses_flush();
     radio_pulses_grab(STDIN, 0, minPulseTime, maxObsTime);

     for (int idx = 0 ; idx < sizeof(protocols) / sizeof(Protocol*); idx++)
     {
         int pulses = radio_pulses_get_count();
         if (pulses >= protocols[idx]->getMinPulses())
         {
             if (protocols[idx]->fromPulses(pulses, radio_pulses_get_pulses()))
             {
                 String s = protocols[idx]->describe(millis());
                 STDOUT.println(s);
             }
         }
     }
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e monitor_start(void* parent,int argc, char* argv[])
{
    bool cont = true;

    int minTime    = monitor_get_minimal_time();
    int maxObsTime = monitor_get_maximal_time();

    if (monitor_protocols_get_count() == 0)
    {
       STDOUT.println("no protocols...");
       return OK;
    }

    STDOUT.print("monitor freq: ");
    STDOUT.print(getRadio()->getFreq());
    STDOUT.println(" Khz, press ctrl-c...");
    while(cont)
    { 
      if (STDIN.available() > 0)
      {
         byte c = STDIN.read();
         if (c == 0x03 /*ctrl-c*/)
         {
             cont = false;
             continue;
         }
      }
      monitor_process(minTime, maxObsTime);
      
    }
    STDOUT.println("monitor done...");
    return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e monitor_protocols(void* parent,int argc, char* argv[])
{
    int maxLen = sizeof(protocols) / sizeof(Protocol*);
    
    STDOUT.println("protocols: ");
    
    for (int idx = 0 ; idx < maxLen; idx++)
    {
        STDOUT.print(protocols[idx]->getName());
        if (idx < maxLen - 1)
            STDOUT.print(",");
              
    }
    STDOUT.print("\n\r");
 
  return OK;
}
//------------------------------------------------------------------------------------------------------------------
