#pragma once

#include "hwinit/types.h"
//Boot status
#define BOOT_STATE_ADDR (vu32 *)0x40002EF8
#define SECMON_STATE_ADDR (vu32 *)0x40002EFC

#define BOOT_PKG2_LOADED 2
#define BOOT_DONE 3

#define BOOT_PKG2_LOADED_4X 3
#define BOOT_DONE_4X 4

//Instructions
#define NOP 0xD503201F

void firmware();