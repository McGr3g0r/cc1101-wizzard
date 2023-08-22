//------------------------------------------------------------------------------------------------------------------
#include "CmdHandler.h"
#include "settings.h"
#include "utils.h"
//------------------------------------------------------------------------------------------------------------------
void CmdHandler::registerHandler(cmd_handler_t* root, cmd_handler_t* sub_cmd)
{ 
   int idx;
   int maxSize = (sizeof(handlers) / sizeof (cmd_handler_t*));
   bool addingChild = false;



   for (idx = 0; idx < maxSize; idx++)
   {
      if (handlers[idx]->cmd == root->cmd)
          break;
   }
   if (idx >= handlersCnt && handlersCnt < maxSize)
   {
       handlers[handlersCnt++] = root;
       idx = handlersCnt - 1;
   }

   if (sub_cmd != NULL && idx < maxSize)
   {
       if (handlers[idx]->cmd == root->cmd)
       {
         handlers[idx]->sub_cmd = sub_cmd; 
       }
   }
} 
//------------------------------------------------------------------------------------------------------------------
CmdHandler::CmdHandler()
{
  handlersCnt = 0;
  resultMessage= "";
}
//------------------------------------------------------------------------------------------------------------------
void CmdHandler::setResultMessage(String msg)
{
  resultMessage = msg;
}
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t** CmdHandler::getHandlers(void)
{
    return &handlers[0];
}
//------------------------------------------------------------------------------------------------------------------
int CmdHandler::getHandlersCount(void)
{
  return handlersCnt;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e CmdHandler::getLastStatus(void)
{
      return lastCmdStatus;
}
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e CmdHandler::handleSubCommand(cmd_handler_t* subCmd, int argc, char* argv[])
{
   if (argc == 1)
      return subCmd->handler(this, argc, argv);
   else if (argc > 1 && subCmd->sub_cmd != 0)
   {
       int idx = 0;
       cmd_handler_t* sub_cmd = &subCmd->sub_cmd[idx];
       cmd_handler_t* sub_sub_cmd;

       int argcoff = 2;
       
       while (sub_cmd && sub_cmd->handler != NULL)
       {
          if (sub_cmd->cmd == String(argv[1]))
          {

              if (argc >= 3)
              {
                  int idx2 = 0;
                  sub_sub_cmd = &sub_cmd->sub_cmd[idx2];
                  while (sub_sub_cmd && sub_sub_cmd->handler != NULL)
                  {
                        if (sub_sub_cmd->cmd == String(argv[2]))
                        {
                            argcoff = 3;
                            sub_cmd = sub_sub_cmd;
                            break;
                        }
                    
                        idx2++;
                        sub_sub_cmd = &sub_cmd->sub_cmd[idx2];   
                  }
              }
            
              if (argc - argcoff < sub_cmd->min_arg_count)
              {
                  setHint(sub_cmd->hint);
                  return NOT_ENOUGH_PARAMS;   
              }
              if (argc > argcoff)
              for (int currArg = argcoff; currArg < argc; currArg++)
              {
                  if (sub_cmd->validation.length()>= currArg-1 && !validateArgumentType(argv[currArg], sub_cmd->validation[currArg-argcoff]))
                  {
                        setHint(sub_cmd->hint);
                        return WRONG_PARAMS;
                  }
              }
              return sub_cmd->handler(this, argc, argv);
          }
          idx++;
          sub_cmd = &subCmd->sub_cmd[idx];
       }
   }  
   return COMMAND_NOT_FOUND;
}
//------------------------------------------------------------------------------------------------------------------
bool CmdHandler::handleCommand(int argc, char* argv[])
{
     bool result = false;

     lastCmdStatus = COMMAND_NOT_FOUND;
     
     for (int idx = 0; idx < handlersCnt; idx++)
     {       
         if (handlers[idx]->cmd == String(argv[0]))
         {
             lastCmdStatus = handleSubCommand(handlers[idx], argc, argv);
             if (lastCmdStatus != CmdStatus_e::OK)
                 result = false;
             else
                 result = true;
             break;
         }
     }
    return result;  
}
//------------------------------------------------------------------------------------------------------------------
void CmdHandler::showHint(Stream& str)
{
   if (hint.length() > 0)
   {
      str.print("hint: ");
      str.println(hint);
      hint = "";
   }
}
//------------------------------------------------------------------------------------------------------------------
void CmdHandler::print_error(Stream& str)
{
    switch (lastCmdStatus)
    {
      case NOT_ENOUGH_PARAMS:
      {
            str.println("not enough params");
            showHint(str);
            return;
      }
      case WRONG_PARAMS:
      {
            str.println("wrong params");
            showHint(str);
            return;
      }
      case COMMAND_NOT_FOUND:
            str.println("unknown command, use 'help'");
            return;
      case CC_ERROR:
            str.println("CC1101 call error");
            return;      
      default:
        break;
    }
}
//------------------------------------------------------------------------------------------------------------------
void CmdHandler::setHint(String hint)
{
   this->hint = hint;
}
//------------------------------------------------------------------------------------------------------------------
String CmdHandler::getHint(void) 
{
  return hint;
}
//------------------------------------------------------------------------------------------------------------------
bool CmdHandler::validateArgumentType(char *arg, char validationSymbol)
{
    if (validationSymbol == 'i')
       return isNumeric(arg);
    else if (validationSymbol == 'f')
       return isFloat(arg); 
    else if (validationSymbol == 'x')
       return isHex(arg);
    else if (validationSymbol == 's')
       return true; 
    else if (validationSymbol == 'b')
       return isBool(arg);
    else
       return false;
}
//------------------------------------------------------------------------------------------------------------------
String CmdHandler::getResultMessage(void)
{
    return resultMessage;
}
//------------------------------------------------------------------------------------------------------------------
