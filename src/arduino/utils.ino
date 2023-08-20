//------------------------------------------------------------------------------------------------------------------
#include "utils.h"
//------------------------------------------------------------------------------------------------------------------
bool isAllowedChar(char* charset, char c)
{
   for (int pos = 0; pos < strlen(charset); pos++)
   {
       if (charset[pos] == c)
       {
           return true;
       }
   }
   return false;
}
//------------------------------------------------------------------------------------------------------------------
bool isNumeric(char* str)
{
   char allowedChars[] = "0123456789";
   
   int len = strlen(str);

   if (!len)
       return false;
   
   for (int pos = 0; pos < len; pos++)
   {
       if (!isAllowedChar(allowedChars, str[pos]))
           return false;
   }

   return true;
}
//------------------------------------------------------------------------------------------------------------------
bool isFloat(char* str)
{
   char allowedChars[] = "+-.0123456789";
   
   int len = strlen(str);

   if (!len)
       return false;
   
   for (int pos = 0; pos < len; pos++)
   {
       if (!isAllowedChar(allowedChars, str[pos]))
           return false;
   }

   return true;
}
//------------------------------------------------------------------------------------------------------------------
bool isHex(char* str)
{
   char allowedChars[] = "x0123456789abcdefABCDEF";
   
   int len = strlen(str);

   if (!len)
       return false;
   
   for (int pos = 0; pos < len; pos++)
   {
       if (!isAllowedChar(allowedChars, str[pos]))
           return false;
   }

   return true;
}
//------------------------------------------------------------------------------------------------------------------
bool isBool(char* str)
{
   String s = String(str);
   s.toLowerCase();
   if (s == "true" || s == "false")
       return true;
   else
       return false;
}
//------------------------------------------------------------------------------------------------------------------
bool intInSet(int val, int* intSet, int size)
{
   for (int pos = 0; pos < size; pos++)
   {
        if (intSet[pos] == val)
            return true;
   }
   return false;
}
//------------------------------------------------------------------------------------------------------------------
unsigned char reverse8(unsigned char b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}
//------------------------------------------------------------------------------------------------------------------
