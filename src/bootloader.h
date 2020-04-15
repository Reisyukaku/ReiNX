/*
* Copyright (c) 2018 Reisyukaku
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
#include "fs.h"
#include "package.h"

// TODO: Maybe find these with memsearch
static const pk11_offs _pk11_offs[] = {
    { "20161121183008", KB_FIRMWARE_VERSION_100, HOS_FIRMWARE_VERSION_100, 0x1900, 0x3FE0, { 2, 1, 0 }, 0x40014020, 0x8000D000 }, //1.0.0
    { "20170210155124", KB_FIRMWARE_VERSION_200, HOS_FIRMWARE_VERSION_200, 0x1900, 0x3FE0, { 0, 1, 2 }, 0x4002D000, 0x8000D000 }, //2.0.0 - 2.3.0
    { "20170519101410", KB_FIRMWARE_VERSION_300, HOS_FIRMWARE_VERSION_300, 0x1A00, 0x3FE0, { 0, 1, 2 }, 0x4002D000, 0x8000D000 }, //3.0.0
    { "20170710161758", KB_FIRMWARE_VERSION_301, HOS_FIRMWARE_VERSION_300, 0x1A00, 0x3FE0, { 0, 1, 2 }, 0x4002D000, 0x8000D000 }, //3.0.1 - 3.0.2
    { "20170921172629", KB_FIRMWARE_VERSION_400, HOS_FIRMWARE_VERSION_400, 0x1800, 0x3FE0, { 1, 2, 0 }, 0x4002B000, 0x4003B000 }, //4.0.0 - 4.1.0
    { "20180220163747", KB_FIRMWARE_VERSION_500, HOS_FIRMWARE_VERSION_500, 0x1900, 0x3FE0, { 1, 2, 0 }, 0x4002B000, 0x4003B000 }, //5.0.0 - 5.0.2
    { "20180802162753", KB_FIRMWARE_VERSION_600, HOS_FIRMWARE_VERSION_600, 0x1900, 0x3FE0, { 1, 2, 0 }, 0x4002B000, 0x4003D800 }, //6.0.0
    { "20181107105733", KB_FIRMWARE_VERSION_620, HOS_FIRMWARE_VERSION_620, 0x0E00, 0x6FE0, { 1, 2, 0 }, 0x4002B000, 0x4003D800 }, //6.2.0
    { "20181218175730", KB_FIRMWARE_VERSION_700, HOS_FIRMWARE_VERSION_700, 0x0F00, 0x6FE0, { 1, 2, 0 }, 0x40030000, 0x4003E000 }, //7.0.0
    { "20190208150037", KB_FIRMWARE_VERSION_701, HOS_FIRMWARE_VERSION_700, 0x0F00, 0x6FE0, { 1, 2, 0 }, 0x40030000, 0x4003E000 }, //7.0.1
    { "20190314172056", KB_FIRMWARE_VERSION_800, HOS_FIRMWARE_VERSION_800, 0x0E00, 0x6FE0, { 1, 2, 0 }, 0x40030000, 0x4003E000 }, //8.0.0
    { "20190531152432", KB_FIRWAMRE_VERSION_810, HOS_FIRMWARE_VERSION_810, 0x0E00, 0x6FE0, { 1, 2, 0 }, 0x4002B000, 0x4003E000 }, //8.1.0
    { "20190809135709", KB_FIRMWARE_VERSION_900, HOS_FIRMWARE_VERSION_900, 0x0E00, 0x6FE0, { 1, 2, 0 }, 0x40030000, 0x4003E000 }, //9.0.0 - 9.0.1
	{ "20191021113848", KB_FIRMWARE_VERSION_910, HOS_FIRMWARE_VERSION_910, 0x0E00, 0x6FE0, { 1, 2, 0 }, 0x40030000, 0x4003E000 }, //9.1.0 - 9.2.0
	{ "20200303104606",  KB_FIRMWARE_VERSION_1000, HOS_FIRMWARE_VERSION_1000, 0x0E00, 0x6FE0, { 1, 2, 0 }, 0x40030000, 0x4003E000 }, //10.0.0
    { NULL } // End.
};

static const u8 keyblob_keyseeds[][0x10] = {
    { 0xDF, 0x20, 0x6F, 0x59, 0x44, 0x54, 0xEF, 0xDC, 0x70, 0x74, 0x48, 0x3B, 0x0D, 0xED, 0x9F, 0xD3 }, //1.0.0
    { 0x0C, 0x25, 0x61, 0x5D, 0x68, 0x4C, 0xEB, 0x42, 0x1C, 0x23, 0x79, 0xEA, 0x82, 0x25, 0x12, 0xAC }, //3.0.0
    { 0x33, 0x76, 0x85, 0xEE, 0x88, 0x4A, 0xAE, 0x0A, 0xC2, 0x8A, 0xFD, 0x7D, 0x63, 0xC0, 0x43, 0x3B }, //3.0.1
    { 0x2D, 0x1F, 0x48, 0x80, 0xED, 0xEC, 0xED, 0x3E, 0x3C, 0xF2, 0x48, 0xB5, 0x65, 0x7D, 0xF7, 0xBE }, //4.0.0
    { 0xBB, 0x5A, 0x01, 0xF9, 0x88, 0xAF, 0xF5, 0xFC, 0x6C, 0xFF, 0x07, 0x9E, 0x13, 0x3C, 0x39, 0x80 }, //5.0.0
    { 0xD8, 0xCC, 0xE1, 0x26, 0x6A, 0x35, 0x3F, 0xCC, 0x20, 0xF3, 0x2D, 0x3B, 0x51, 0x7D, 0xE9, 0xC0 }  //6.0.0
};

static const u8 cmac_keyseed[0x10] = { 0x59, 0xC7, 0xFB, 0x6F, 0xBE, 0x9B, 0xBE, 0x87, 0x65, 0x6B, 0x15, 0xC0, 0x53, 0x73, 0x36, 0xA5 };
static const u8 pre400_master_keyseed[0x10] = { 0xD8, 0xA2, 0x41, 0x0A, 0xC6, 0xC5, 0x90, 0x01, 0xC6, 0x1D, 0x6A, 0x26, 0x7C, 0x51, 0x3F, 0x3C };
static const u8 console_keyseed[0x10] = { 0x4F, 0x02, 0x5F, 0x0E, 0xB6, 0x6D, 0x11, 0x0E, 0xDC, 0x32, 0x7D, 0x41, 0x86, 0xC2, 0xF4, 0x78 };
static const u8 pk21_keyseed[] = { 0xFB, 0x8B, 0x6A, 0x9C, 0x79, 0x00, 0xC8, 0x49, 0xEF, 0xD2, 0x4D, 0x85, 0x4D, 0x30, 0xA0, 0xC7 };
static const u8 pre620_master_keyseed[0x10] = { 0x2D, 0xC1, 0xF4, 0x8D, 0xF3, 0x5B, 0x69, 0x33, 0x42, 0x10, 0xAC, 0x65, 0xDA, 0x90, 0x46, 0x66 };
static const u8 console_keyseed_4xx[0x10] = { 0x0C, 0x91, 0x09, 0xDB, 0x93, 0x93, 0x07, 0x81, 0x07, 0x3C, 0xC4, 0x16, 0x22, 0x7C, 0x6C, 0x28 };
static const u8 new_master_keyseed[0x10] = { 0x37, 0x4B, 0x77, 0x29, 0x59, 0xB4, 0x04, 0x30, 0x81, 0xF6, 0xE5, 0x8C, 0x6D, 0x36, 0x17, 0x9A };

int keygen(u8 *keyblob, u32 fwVer, void * pkg1, pk11_offs * offs);
void bootloader();
