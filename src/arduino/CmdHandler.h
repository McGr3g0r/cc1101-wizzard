//------------------------------------------------------------------------------------------------------------------
#ifndef __CMD_HANDLER_H__
#define __CMD_HANDLER_H__
//------------------------------------------------------------------------------------------------------------------
#include "CmdStatus.h"
//------------------------------------------------------------------------------------------------------------------
typedef CmdStatus_e(*CmdHandlerFunc)(void* parent,int argc, char* argv[]);
//------------------------------------------------------------------------------------------------------------------
typedef struct cmd_handler_s
{
  CmdHandlerFunc handler;
  struct cmd_handler_s* parent;
  struct cmd_handler_s* sub_cmd;
  String cmd;
  String desc;
  int    min_arg_count;
  String validation;
  String hint;
} cmd_handler_t;
//------------------------------------------------------------------------------------------------------------------
#define CMD_MAX_HANDLERS 16
//------------------------------------------------------------------------------------------------------------------
class CmdHandler
{
   public:
      CmdHandler();

      bool handleCommand(int argc, char* argv[]);
      
      void registerHandler(cmd_handler_t* root, cmd_handler_t* sub_cmd);
      
      CmdStatus_e getLastStatus(void);

      cmd_handler_t** getHandlers(void);
      int getHandlersCount(void);

      void print_error(Stream& str);

      void setHint(String hint);
      String getHint(void);

      void setResultMessage(String msg);
      String getResultMessage(void);

   private:
      CmdStatus_e lastCmdStatus;
      
      cmd_handler_t* handlers[CMD_MAX_HANDLERS];
      
      int handlersCnt;

      String hint;

      String resultMessage;

      void showHint(Stream& str);

     CmdStatus_e handleSubCommand(cmd_handler_t* subCmd, int argc, char* argv[]);
     bool validateArgumentType(char *arg, char validationSymbol);
      
};
//------------------------------------------------------------------------------------------------------------------
#endif// __CMD_HANDLER_H__
//------------------------------------------------------------------------------------------------------------------
