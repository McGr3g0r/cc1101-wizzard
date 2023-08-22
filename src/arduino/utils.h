//------------------------------------------------------------------------------------------------------------------
#ifndef _UTILS__H_
#define _UTILS__H_
//------------------------------------------------------------------------------------------------------------------

bool isNumeric(char* str);
bool isFloat(char* str);
bool isHex(char* str);
bool isBool(char* str);
bool intInSet(int val, int* intSet, int size);
unsigned char reverse8(unsigned char b);
void print_hex_uint32(Stream& str, uint32_t v);
void print_hex_uint64(Stream& str, uint64_t v);
void apply_value_to_bitmask(uint32_t* b0, uint32_t* b1, uint32_t* b2, uint32_t msk0, uint32_t msk2, uint32_t msk1, uint64_t value);
//------------------------------------------------------------------------------------------------------------------
#endif// _UTILS__H_
//------------------------------------------------------------------------------------------------------------------
