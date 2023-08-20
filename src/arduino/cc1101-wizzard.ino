//------------------------------------------------------------------------------------------------------------------
#include "cc1101.h"
#include "platform.h"
#include "Parser.h"
#include "Terminal.h"
#include "Processor.h"
#include "CmdHandler.h"
#include "radio_profile.h"
#include "radio_pulses.h"
//------------------------------------------------------------------------------------------------------------------
#define CMD_MAX_BUFFER_LEN 80
//------------------------------------------------------------------------------------------------------------------
CC1101 cc;
Parser parser;
Terminal term(CMD_MAX_BUFFER_LEN);
CmdHandler handler;
Processor proc;
//------------------------------------------------------------------------------------------------------------------
void radio_cmd_init(void);
void help_cmd_init(void);
void monitor_cmd_init(void);
//------------------------------------------------------------------------------------------------------------------
CmdHandler* getRootCommandHandler(void)
{
  return &handler;
}
//------------------------------------------------------------------------------------------------------------------
void commands_init(void)
{
  help_cmd_init();
  radio_cmd_init();
  monitor_cmd_init();
}
//------------------------------------------------------------------------------------------------------------------
CC1101* getRadio() 
{
  return &cc;
}
//------------------------------------------------------------------------------------------------------------------
Processor* getProc()
{
  return &proc;
}
//------------------------------------------------------------------------------------------------------------------
void setup() {
     Serial.begin(115200);

     while (!Serial);
     delay(1000);
     
     Serial.println(F("CC1101 Wizard tool connected, use 'help' for list of commands...\n\r"));
     Serial.println(F("(C) Grzegorz Rajtar grzegorz.rajtar@gmail.com 2023\n\r"));
     Serial.print(F("Plarform:"));Serial.println(platform_get_name());
     Serial.println();

     platform_init();

     radio_profile_init();

     radio_pulses_init();

     commands_init();

     cc.init();

     if (cc.check()) {
        Serial.println(F("CC1101 [OK].\n\r"));
     } else {
        Serial.println(F("CC1101 [connection error]!\n\r"));
     };
     
     term.setEcho(true);
     term.clear();
     
}
//------------------------------------------------------------------------------------------------------------------
void loop() {

  term.input(Serial);

  if (term.available())
  {
      bool cmdOk = false;
      if (parser.parse(term.getLine()))
      {
          cmdOk = handler.handleCommand(parser.getArgc(), parser.getArgv());
          term.clear();
      }
      if (!cmdOk)
      {
           handler.print_error(Serial);
      } 
      else 
      {
        if (handler.getResultMessage().length()> 0)
        {
            Serial.println(handler.getResultMessage());
            handler.setResultMessage("");
        }
        else
            Serial.println("OK");
      }
  }
  proc.process();
}
//------------------------------------------------------------------------------------------------------------------
