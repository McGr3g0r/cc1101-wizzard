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
bool isSignedNumeric(char* str)
{
  if (str[0] == '-')
     return isNumeric(&str[1]);
  else
     return isNumeric(str);
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
void str_trim_end(char* txt)
{
  for (int idx = strlen(txt) -1 ; idx >= 0; idx--)
  {
    if (txt[idx] == '\n' || txt[idx] == '\r' || txt[idx] == ' ')
        txt[idx] = 0;
  }
  
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
void print_hex_uint32(Stream& str, uint32_t v)
{
   char buf[16+1];
   sprintf(buf, "%08x", v);
   str.print(buf);
}
//------------------------------------------------------------------------------------------------------------------
void print_hex_uint64(Stream& str, uint64_t v)
{
   char buf[32+1];
   sprintf(buf, "%08x%08x", (uint32_t)(v >> 32), (uint32_t)(v & 0xffffffff));
   str.print(buf);
}
//------------------------------------------------------------------------------------------------------------------
void uint32_write_buffer(uint8_t* dst, int offset, uint32_t val)
{
    dst[offset++] = val & 0xff;
    dst[offset++] = (val >> 8)  & 0xff; 
    dst[offset++] = (val >> 16) & 0xff;
    dst[offset++] = (val >> 24) & 0xff; 
    
}

//------------------------------------------------------------------------------------------------------------------
uint32_t uint32_read_buffer(uint8_t* dst, int offset)
{
    uint32_t res = 0;

    res |= dst[offset++];
    res |= (dst[offset++] << 8) & 0xFF00;
    res |= (dst[offset++] << 16) & 0xFF0000;
    res |= (dst[offset++] << 24) & 0xFF000000;

    return res;
}
//------------------------------------------------------------------------------------------------------------------
void apply_value_to_bitmask(uint32_t* b0, uint32_t* b1, uint32_t* b2, uint32_t msk0, uint32_t msk1, uint32_t msk2, uint64_t value)
{
   uint8_t msk[12];
   uint8_t b[12];

   uint32_write_buffer(&msk[0], 0, msk2);
   uint32_write_buffer(&msk[4], 0, msk1);
   uint32_write_buffer(&msk[8], 0, msk0);
   
   uint32_write_buffer(&b[0], 0, *b2);
   uint32_write_buffer(&b[4], 0, *b1);
   uint32_write_buffer(&b[8], 0, *b0);
         
   for (int pos = 0; pos < 64; pos++)
   {
      uint8_t bitVal = (value >> pos) & 0x01;
      int mskOneCnt = 0;
      for (int bitMskPos = 0; bitMskPos < 96; bitMskPos++)
      {
          int bytePos = bitMskPos >> 3;
          int bitPos =  bitMskPos & 0x07;
          uint8_t mskBit =  (msk[bytePos] >> bitPos) & 0x01;
          if (mskBit)
              mskOneCnt++;

          if (mskOneCnt == pos && mskBit)
          {
              b[bytePos] &= ~(0x01   << bitPos);
              b[bytePos] |=  (bitVal << bitPos);
          }
      }
      
   }

    *b2 = uint32_read_buffer(&b[0], 0);
    *b1 = uint32_read_buffer(&b[4], 0);
    *b0 = uint32_read_buffer(&b[8], 0);
}
//------------------------------------------------------------------------------------------------------------------
String millis_to_time(uint32_t ms)
{
    uint32_t rest = ms;
    char buffer[64];
    
    int days = rest / (24 * 3600 * 1000);
    rest -= days *  (24 * 3600 * 1000);

    int hours = rest / (3600 * 1000);
    rest -= hours *  (3600 * 1000);

    int mins = rest / (60 * 1000);
    rest -= mins *  (60 * 1000);

    int secs = rest / (1000);
    rest -= secs *  (1000);
    sprintf(buffer, "%02d.%02d:%02d:%02d.%03d", days, hours, mins, secs, rest);

    return String(buffer);
    
}
//------------------------------------------------------------------------------------------------------------------
int count_bits_set_uint32(uint32_t v)
{
   int res = 0;
   for (int i = 0; i <32 ; i++)
       if ((v >> i) & 0x01)
           res++; 

   return res;
}
//------------------------------------------------------------------------------------------------------------------
uint64_t bitscount_to_max_uint64(int cnt)
{
    uint64_t res = 0;
    for (int pos = 0; pos < 64; pos++)
    {
       if (cnt)
          res |= (1 << pos);
       cnt--;
       if (cnt == 0)
           break;
    }
    return res;
}  

//------------------------------------------------------------------------------------------------------------------
uint64_t bit_spread_assymetric(uint64_t val ,int bitsCount)
{
   uint64_t v = 0;

   for (int pos = 0; pos < bitsCount; pos++)
   {
      uint8_t bit = (val >> pos) & 0x01;
      if ((pos & 0x01) == 0)
         v |= (bit << (pos >> 1));
      else
         v |= (bit << (bitsCount - (1 +  (pos >> 1))));   
   }
   return v;
}

//------------------------------------------------------------------------------------------------------------------
uint64_t bit_spread_quad_assymetric(uint64_t val ,int bitsCount)
{
   uint64_t v = 0;

   for (int pos = 0; pos < bitsCount; pos+=4)
   {
      uint8_t bit1 = (val >> pos) & 0x01;
      uint8_t bit2 = (val >> (pos+1)) & 0x01;
      uint8_t bit3 = (val >> (pos+2)) & 0x01;
      uint8_t bit4 = (val >> (pos+3)) & 0x01;
      
      v |= bit1 << (pos >> 2);
      v |= bit2 << (bitsCount/2 - ((pos+1) >> 2));
      v |= bit3 << (bitsCount/2 + ((pos+2) >> 2));     
      v |= bit4 << (bitsCount - ((pos+3) >> 2));         
   }
   return v;
}
//------------------------------------------------------------------------------------------------------------------
void path_make_root(char* dst, const char *path)
{
     if (path[0] != '/')
         sprintf(dst, "/%s", path);
     else
         sprintf(dst, "%s", path);
} 
//------------------------------------------------------------------------------------------------------------------
