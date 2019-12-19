/*
* Copyright (c) 2018 Reisyukaku, naehrwert
*
* This program is free software; you can redistribute it and/or modify it
* under the terms and conditions of the GNU General Public License,
* version 2, as published by the Free Software Foundation.
*
* This program is distributed in the hope it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <string.h>
#include <stddef.h>
#include "hwinit.h"
#include "fs.h"
#include "package.h"
#include "error.h"
#include "bootloader.h"
#include "patches.h"
#include "secmon.h"
#include "sept.h"
#include "hwinit/di.h"

//Boot status
#define BOOT_STATE_ADDR (vu32 *)0x40002EF8
#define SECMON_STATE_ADDR (vu32 *)0x40002EFC
#define BOOT_STATE_ADDR7X (vu32 *)0x400000F8
#define SECMON_STATE_ADDR7X (vu32 *)(0x400000F8 + 4)

#define BOOT_PKG2_LOADED 2
#define BOOT_DONE 3

#define BOOT_PKG2_LOADED_4X 3
#define BOOT_DONE_4X 4

#define PAYLOAD_ADDR 0xCFF00000

void firmware();
