//------------------------------------------------------------------------------------------------------------------
#include "settings.h"
//------------------------------------------------------------------------------------------------------------------
#include "CmdHandler.h"
#include "utils.h"
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e env_main(void* parent,int argc, char* argv[]);
CmdHandler* getRootCommandHandler(void);
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t env_handler = { &env_main, NULL, NULL, "env", "enviroment variables utils, use: 'env help'", 0, "", "env help" };
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t env_sub_cmd[] = {
   { &env_main,    &env_handler,   NULL, "help",  "this help", 0, "", "env help"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
void env_cmd_init(void)
{ 
  getRootCommandHandler()->registerHandler(&env_handler, env_sub_cmd); 
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e env_main(void* parent,int argc, char* argv[])
{
     STDOUT.println(env_handler.desc);

     int idx = 0 ;
     cmd_handler_t* item = &env_handler.sub_cmd[idx];
     while (item && item->handler != 0)
     {
            STDOUT.print("- ");
            STDOUT.print(item->cmd);
            STDOUT.print( " : ");
            STDOUT.println( item->desc);

            idx++;
            item = &env_handler.sub_cmd[idx];
      
     }
    
     return OK;
}
//------------------------------------------------------------------------------------------------------------------
