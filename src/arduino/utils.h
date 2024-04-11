//------------------------------------------------------------------------------------------------------------------
#ifndef _UTILS__H_
#define _UTILS__H_
//------------------------------------------------------------------------------------------------------------------
#include <stdint.h>
#include <Arduino.h>
//------------------------------------------------------------------------------------------------------------------
bool isNumeric(char* str);
bool isSignedNumeric(char* str);
bool isFloat(char* str);
bool isHex(char* str);
bool isBool(char* str);
bool intInSet(int val, int* intSet, int size);
uint8_t hex_to_u8(char* str, int len);
unsigned char reverse8(unsigned char b);
void print_hex_uint32(Stream& str, uint32_t v);
void print_hex_uint64(Stream& str, uint64_t v);
void apply_value_to_bitmask(uint32_t* b0, uint32_t* b1, uint32_t* b2, uint32_t msk0, uint32_t msk2, uint32_t msk1, uint64_t value);
void apply_value_to_bitmask_32(uint32_t* b0, uint32_t msk0, uint32_t value);
String millis_to_time(uint32_t ms);
int count_bits_set_uint32(uint32_t v);
uint64_t bitscount_to_max_uint64(int cnt);
uint32_t bitscount_to_max_uint32(int cnt);
uint64_t bit_spread_assymetric(uint64_t val ,int bitsCount);
uint64_t bit_spread_quad_assymetric(uint64_t val ,int bitsCount);
void str_trim_end(char* txt);
void path_make_root(char* dst, const char *path);
//------------------------------------------------------------------------------------------------------------------
#endif// _UTILS__H_
//------------------------------------------------------------------------------------------------------------------
