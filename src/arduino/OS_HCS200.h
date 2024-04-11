//------------------------------------------------------------------------------------------------------------------
#ifndef __OS_HCS200_H__
#define __OS_HCS200_H__
//------------------------------------------------------------------------------------------------------------------
#include "settings.h"
#include "Processor.h"
#include "Protocol.h"
#include "hcs200_protocol.h"
//------------------------------------------------------------------------------------------------------------------
#include "CmdHandler.h"
#include "radio_profile.h"
#include "radio_pulses.h"
#include "utils.h"
#if USE_FILE_SYSTEM == 1
#include "LittleFS.h"
#include "files.h"
#endif
//------------------------------------------------------------------------------------------------------------------
#include "env.h"
//------------------------------------------------------------------------------------------------------------------
typedef struct HCS200BruteContext_s
{
    uint32_t enc,serial;
    uint8_t btn;
    uint32_t encmsk;
    uint32_t itstart;
    uint32_t itend;
    uint32_t curr;
    uint8_t bit_spread_strategy;
    int max_time;
    bool cont;
    bool use_rssi;
    int rssi;
    int rssi_wait_ms;

} HCS200BruteContext_t;

HCS200BruteContext_t hcs200_ctx;
//------------------------------------------------------------------------------------------------------------------
extern cmd_handler_t open_sesame_sub_cmd[];
extern cmd_handler_t open_sesame_handler;
//------------------------------------------------------------------------------------------------------------------
CmdStatus_e open_sesame_hcs200(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_hcs200_help(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_hcs200_setcode(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_hcs200_setmask(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_hcs200_setirange(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_hcs200_setirangea(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_hcs200_setcurr(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_hcs200_setcurrf(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_hcs200_setibds(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_hcs200_process(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_hcs200_cont(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_hcs200_smaxt(void* parent,int argc, char* argv[]);
CmdStatus_e open_sesame_hcs200_srssi(void* parent,int argc, char* argv[]);
//------------------------------------------------------------------------------------------------------------------
cmd_handler_t open_sesame_hcs200_sub_cmd[] = {
   { &open_sesame_hcs200_help,       &open_sesame_handler, NULL, "help", "this help", 0, "", "opensesame hcs200 help"},
   { &open_sesame_hcs200_setcode,    &open_sesame_handler, NULL, "scod", "set code enc<32bit hex> serial<28bit hex> bnt<8bit hex>", 3, "xxx", "opensesame hcs200 scod 09800000 EAABBCC 01"},
   { &open_sesame_hcs200_setmask,    &open_sesame_handler, NULL, "smsk", "set iteration mask encmsk<hex>", 1, "x", "opensesame hcs200 smsk 007FFF00"},
   { &open_sesame_hcs200_setirange,  &open_sesame_handler, NULL, "sir",  "set iteration range enc_start<hex> enc_end<hex>", 2, "xx", "opensesame hcs200 sir 00000000 FFFFFFFF"},
   { &open_sesame_hcs200_setirangea, &open_sesame_handler, NULL, "sira", "set auto iteration, set mask first", 0, "", "opensesame hcs200 sira"},
   { &open_sesame_hcs200_setcurr,    &open_sesame_handler, NULL, "sirs",  "set current iteration value, set mask first, curr_enc<hex>", 1, "x", "opensesame hcs200 sirs 0x00AABB00"},
   #if USE_FILE_SYSTEM == 1
   { &open_sesame_hcs200_setcurrf,   &open_sesame_handler, NULL, "sirf",  "set current iteration from file, set mask first, filename<txt>", 1, "t", "opensesame hcs200 sirf os_hcs200.txt"},
   #endif
   { &open_sesame_hcs200_setibds,   &open_sesame_handler, NULL, "sibds", "set iteration bit dispersion strategy: 0 - none, 1 - assymetric, 2 - quad-asssymetric", 1, "i", "opensesame hcs200 sibds 1"},
   { &open_sesame_hcs200_smaxt,     &open_sesame_handler, NULL, "smaxt", "set max opration time in seconds: time<int>", 1, "i", "opensesame hcs200 smaxt 3600 <int>"},
   { &open_sesame_hcs200_srssi,     &open_sesame_handler, NULL, "srssi", "set min rssi for channel monitoring: rssi<signed int> use<int> 0 - 1, wait millis <int>", 3, "isi", "opensesame hcs200 srrsi 1<int> -60 <int> 2000<int>"},
   { &open_sesame_hcs200_process  , &open_sesame_handler, NULL, "brute", "brute iteration of codes, display interval<int>", 1, "i", "opensesame hcs200 brute 10000<int ms>"},   
   { &open_sesame_hcs200_cont     , &open_sesame_handler, NULL, "brutc", "set continuation flag<int> 0-1, ", 1, "i", "opensesame hcs200 brutc 1<int>"},
   { 0,  0, NULL, "",  "", 0, "","" }
};
//------------------------------------------------------------------------------------------------------------------
#endif// __OS_HCS200_H__
//------------------------------------------------------------------------------------------------------------------
