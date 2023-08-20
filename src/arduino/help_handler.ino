//------------------------------------------------------------------------------------------------------------------
#include "CmdHandler.h"
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e help_main(void* parent,int argc, char* argv[]);
CmdHandler* getRootCommandHandler(void);
cmd_handler_t help_handler = { &help_main, NULL, NULL, "help", "generic help, use: 'help'", 0, "", ""};
//------------------------------------------------------------------------------------------------------------------
void help_cmd_init(void)
{ 
  getRootCommandHandler()->registerHandler(&help_handler, NULL); 
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e help_main(void* parent,int argc, char* argv[])
{
   if (argc >= 1 && String(argv[0]) == "help")
   {
       CmdHandler* handler = (CmdHandler*) parent;
       STDOUT.println("help:");
       for (int idx = 0; idx < handler->getHandlersCount(); idx++)
       {    if (handler->getHandlers()[idx]->cmd != "help")
            {
              STDOUT.print(" ");
              STDOUT.print( handler->getHandlers()[idx]->cmd);
              STDOUT.print( " : ");
              STDOUT.println( handler->getHandlers()[idx]->desc);
           }
       }
       return CmdStatus_e::OK;
   }
   else
       return CmdStatus_e::COMMAND_NOT_FOUND;
}
//------------------------------------------------------------------------------------------------------------------
