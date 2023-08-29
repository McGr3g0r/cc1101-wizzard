//------------------------------------------------------------------------------------------------------------------
#include "Script.h"
 //------------------------------------------------------------------------------------------------------------------
Script::Script(void)
{
  
}
 //------------------------------------------------------------------------------------------------------------------
 bool Script::load(const char* file)
 {
     scr = LittleFS.open(file, "r");

     if (scr)
        return false;
     else
        return true;
 }
//------------------------------------------------------------------------------------------------------------------
 int Script::available(void)
 {
    if (scr)
       return scr.available();
    else
       return 0;
 }
  //------------------------------------------------------------------------------------------------------------------
 String trim_end(String str)
 {
     String result = str;


     if (result[result.length() - 1] == '\n')
         result = result.substring(0, result.length() - 1);
     if (result[result.length() - 1] == '\r')
         result = result.substring(0, result.length() - 1);
     return result;
  
 }
 //------------------------------------------------------------------------------------------------------------------
 String Script::getLine(void)
 {
    if (scr)
    {
       String l = readLine();

       l = trim_end(l);
       STDOUT.print("scr>");
       STDOUT.println(l);
       return l;
    }
    else
       return "";
 }
 //------------------------------------------------------------------------------------------------------------------
 String Script::readLine(void)
 {
    String line = scr.readStringUntil('\n');
    if (line.startsWith("include "))
    {
      String fname = line.substring(8, line.length());
      scr.close();
      scr = LittleFS.open("/" + fname, "r");
      if (scr)
      {
        line = scr.readStringUntil('\n');
        return line;   
      }
      else 
        return "";      
      
    } else
    {
      return line;
    }
 }
 //------------------------------------------------------------------------------------------------------------------
