//------------------------------------------------------------------------------------------------------------------
#ifndef __PARSER_H__
#define __PARSER_H__
//------------------------------------------------------------------------------------------------------------------
#include "Arduino.h"
#define PARSER_MAX_PARAMS 16
//------------------------------------------------------------------------------------------------------------------
class Parser
{
  public:
  
    bool parse(String line);
    int getArgc(void);
    char** getArgv(void);

  private:
   char buffer[160];
   int argc;
   char* argv[PARSER_MAX_PARAMS];

   void apply_env(void);
};
//------------------------------------------------------------------------------------------------------------------
#endif// __PARSER_H__
//------------------------------------------------------------------------------------------------------------------
