//------------------------------------------------------------------------------------------------------------------
#include "Terminal.h"
//------------------------------------------------------------------------------------------------------------------
Terminal::Terminal(int lineSize)
{
   maxLineLength = lineSize;
   echoEnabled = false;
}
//------------------------------------------------------------------------------------------------------------------
void Terminal::clear(void)
{
  buffer = "";
  avail = false;
}
//------------------------------------------------------------------------------------------------------------------
void Terminal::input(String line)
{
  buffer = line;
  avail = true;
}
//------------------------------------------------------------------------------------------------------------------
void Terminal::input(Stream& term)
{
    if (term.available())
    {
        int data = term.read();
        if (data == '\b' || data == '\177') {  // BS and DEL
            avail = false;
            if (buffer.length()) {
                 if(buffer.length() > 1)
                    buffer = buffer.substring(0, buffer.length()-1);
                 else
                    buffer = "";
                    
                if (echoEnabled) term.write("\b \b");

            }
        }
        else if (data == '\r' || data == '\n' ) {
            if (echoEnabled) term.write("\r\n");    // output CRLF

            if (buffer.length() > 0)
            {
                //term.println(buffer);
                avail = true;
                return;
            }
        }
        else if (buffer.length() < CMD_MAX_BUFFER_LEN - 1) {
            buffer += (char)data;
            if (echoEnabled) term.write(data);
        }  
    }
}
//------------------------------------------------------------------------------------------------------------------    
 void Terminal::setEcho(bool enabled)
 {
    echoEnabled = enabled;
 }
//------------------------------------------------------------------------------------------------------------------
String Terminal::getLine(void)
{
   return buffer;
}
//------------------------------------------------------------------------------------------------------------------
bool Terminal::available()
{
     return avail;
}
//------------------------------------------------------------------------------------------------------------------
