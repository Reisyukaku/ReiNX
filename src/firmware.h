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

#include "hwinit/types.h"

//Boot status
#define BOOT_STATE_ADDR (vu32 *)0x40002EF8
#define SECMON_STATE_ADDR (vu32 *)0x40002EFC

#define BOOT_PKG2_LOADED 2
#define BOOT_DONE 3

#define BOOT_PKG2_LOADED_4X 3
#define BOOT_DONE_4X 4

#define PAYLOAD_ADDR 0xCFF00000

// TODO: Maybe find these with memsearch
static const pk11_offs _pk11_offs[] = {
    { KB_FIRMWARE_VERSION_100, 0x1900, 0x3FE0, { 2, 1, 0 }, 0x40014020, 0x8000D000, 1 }, //1.0.0
    { KB_FIRMWARE_VERSION_200, 0x1900, 0x3FE0, { 0, 1, 2 }, 0x4002D000, 0x8000D000, 1 }, //2.0.0 - 2.3.0
    { KB_FIRMWARE_VERSION_300, 0x1A00, 0x3FE0, { 0, 1, 2 }, 0x4002D000, 0x8000D000, 1 }, //3.0.0
    { KB_FIRMWARE_VERSION_301, 0x1A00, 0x3FE0, { 0, 1, 2 }, 0x4002D000, 0x8000D000, 1 }, //3.0.1 - 3.0.2
    { KB_FIRMWARE_VERSION_400, 0x1800, 0x3FE0, { 1, 2, 0 }, 0x4002B000, 0x4003B000, 0 }, //4.0.0 - 4.1.0
    { KB_FIRMWARE_VERSION_500, 0x1900, 0x3FE0, { 1, 2, 0 }, 0x4002B000, 0x4003B000, 0 }, //5.0.0 - 5.0.2
    { KB_FIRMWARE_VERSION_600, 0x1900, 0x3FE0, { 1, 2, 0 }, 0x4002B000, 0x4003D800, 0 }, //6.0.0
    { KB_FIRMWARE_VERSION_620, 0x1900, 0x3FE0, { 1, 2, 0 }, 0x4002B000, 0x4003D800, 0 }, //6.2.0
    { 0, 0, 0, 0, 0, 0, 0 } // End.
};

void firmware();