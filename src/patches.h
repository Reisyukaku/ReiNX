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

#include "hwinit.h"
#include "fs.h"
#include "package.h"
#include "error.h"
#include "kippatches/fs.inc"

#define PKG2_NEWKERN_START 0x800

//FS_MITM
static u32 PRC_ID_SND_100[] =
{
    0xA9BF2FEA, 0x2A0E03EB, 0xD37EF56B, 0xF86B6B8B, 0x92FFFFE9, 0x8A090168, 0xD2FFFFE9, 0x8A09016B,
    0xD2FFFFC9, 0xEB09017F, 0x54000040, 0xF9412948, 0xA8C12FEA
};

static u32 PRC_ID_RCV_100[] =
{
    0xA9BF2FEA, 0x2A1C03EA, 0xD37EF54A, 0xF86A69AA, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A,
    0xD2FFFFC9, 0xEB09015F, 0x54000040, 0xF9412968, 0xA8C12FEA
};

static u32 PRC_ID_SND_200[] =
{
    0xA9BF2FEA, 0x2A1803EB, 0xD37EF56B, 0xF86B6B8B, 0x92FFFFE9, 0x8A090168, 0xD2FFFFE9, 0x8A09016B,
    0xD2FFFFC9, 0xEB09017F, 0x54000040, 0xF9413148, 0xA8C12FEA
};

static u32 PRC_ID_RCV_200[] =
{
    0xA9BF2FEA, 0x2A0F03EA, 0xD37EF54A, 0xF9405FEB, 0xF86A696A, 0xF9407BEB, 0x92FFFFE9, 0x8A090148,
    0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000040, 0xF9413168, 0xA8C12FEA
};

static u32 PRC_ID_SND_300[] =
{
    0xA9BF2FEA, 0x2A1803EB, 0xD37EF56B, 0xF86B6B8B, 0x92FFFFE9, 0x8A090168, 0xD2FFFFE9, 0x8A09016B,
    0xD2FFFFC9, 0xEB09017F, 0x54000040, 0xF9415548, 0xA8C12FEA
};

static u32 PRC_ID_RCV_300[] =
{
    0xA9BF2FEA, 0x2A0F03EA, 0xD37EF54A, 0xF9405FEB, 0xF86A696A, 0xF9407BEB, 0x92FFFFE9, 0x8A090148,
    0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000040, 0xF9415568, 0xA8C12FEA
};

static u32 PRC_ID_SND_302[] =
{
    0xA9BF2FEA, 0x2A1803EB, 0xD37EF56B, 0xF86B6B8B, 0x92FFFFE9, 0x8A090168, 0xD2FFFFE9, 0x8A09016B,
    0xD2FFFFC9, 0xEB09017F, 0x54000040, 0xF9415548, 0xA8C12FEA
};

static u32 PRC_ID_RCV_302[] =
{
    0xA9BF2FEA, 0x2A0F03EA, 0xD37EF54A, 0xF9405FEB, 0xF86A696A, 0xF9407BEB, 0x92FFFFE9, 0x8A090148,
    0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000040, 0xF9415568, 0xA8C12FEA
};

static u32 PRC_ID_SND_400[] =
{
    0x2A1703EA, 0xD37EF54A, 0xF86A6B8A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9,
    0xEB09015F, 0x54000060, 0xF94053EA, 0xF9415948, 0xF94053EA
};

static u32 PRC_ID_RCV_400[] =
{
    0xF9403BED, 0x2A0E03EA, 0xD37EF54A, 0xF86A69AA, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A,
    0xD2FFFFC9, 0xEB09015F, 0x54000040, 0xF9415B28, 0xD503201F
};

static u32 PRC_ID_SND_500[] =
{
    0x2A1703EA, 0xD37EF54A, 0xF86A6B6A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9,
    0xEB09015F, 0x54000060, 0xF94043EA, 0xF9415948, 0xF94043EA
};

static u32 PRC_ID_RCV_500[] =
{
    0xF9403BED, 0x2A1503EA, 0xD37EF54A, 0xF86A69AA, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A,
    0xD2FFFFC9, 0xEB09015F, 0x54000040, 0xF9415B08, 0xF9406FEA
};

static u32 PRC_ID_SND_600[] =
{
    0xA9BF2FEA, 0xF94037EB, 0x2A1503EA, 0xD37EF54A, 0xF86A696A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000100, 0xA9BF27E8, 0xF9400308, 0xF9401D08, 0xAA1803E0, 0xD63F0100, 0xA8C127E8, 0xAA0003E8, 0xA8C12FEA, 0xAA0803E0
};

static u32 PRC_ID_RCV_600[] =
{
    0xA9BF2FEA, 0xF94043EB, 0x2A1503EA, 0xD37EF54A, 0xF86A696A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000100, 0xA9BF27E8, 0xF9400308, 0xF9401D08, 0xAA1803E0, 0xD63F0100, 0xA8C127E8, 0xAA0003E8, 0xA8C12FEA, 0xAA0803E0
};

static u32 PRC_ID_SND_700[] =
{
    0xA9BF2FEA, 0xF9403BEB, 0x2A1903EA, 0xD37EF54A, 0xF86A696A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000100, 0xA9BF27E8, 0xF94002A8, 0xF9401D08, 0xAA1503E0, 0xD63F0100, 0xA8C127E8, 0xAA0003E8, 0xA8C12FEA, 0xAA0803E0
};

static u32 PRC_ID_RCV_700[] =
{
    0xA9BF2FEA, 0xF9404FEB, 0x2A1603EA, 0xD37EF54A, 0xF86A696A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000100, 0xA9BF27E8, 0xF9400368, 0xF9401D08, 0xAA1B03E0, 0xD63F0100, 0xA8C127E8, 0xAA0003E8, 0xA8C12FEA, 0xAA0803E0
};

static u32 PRC_ID_SND_800[] =
{
    0xA9BF2FEA, 0xF9403BEB, 0x2A1903EA, 0xD37EF54A, 0xF86A696A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000100, 0xA9BF27E8, 0xF94002A8, 0xF9401D08, 0xAA1503E0, 0xD63F0100, 0xA8C127E8, 0xAA0003E8, 0xA8C12FEA, 0xAA0803E0
};

static u32 PRC_ID_RCV_800[] =
{
    0xA9BF2FEA, 0xF9404FEB, 0x2A1603EA, 0xD37EF54A, 0xF86A696A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000100, 0xA9BF27E8, 0xF9400368, 0xF9401D08, 0xAA1B03E0, 0xD63F0100, 0xA8C127E8, 0xAA0003E8, 0xA8C12FEA, 0xAA0803E0
};

static u32 PRC_ID_SND_900[] =
{
    0xA9BF2FEA, 0xF94037EB, 0x2A1603EA, 0xD37EF54A, 0xF86A696A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000100, 0xA9BF27E8, 0xF94002E8, 0xF9401D08, 0xAA1703E0, 0xD63F0100, 0xA8C127E8, 0xAA0003E8, 0xA8C12FEA, 0xAA0803E0
};

static u32 PRC_ID_RCV_900[] =
{
    0xA9BF2FEA, 0xF9404BEB, 0x2A1703EA, 0xD37EF54A, 0xF86A696A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000100, 0xA9BF27E8, 0xF9400368, 0xF9401D08, 0xAA1B03E0, 0xD63F0100, 0xA8C127E8, 0xAA0003E8, 0xA8C12FEA, 0xAA0803E0
};
static u32 PRC_ID_SND_1000[] =
{
	0xA9BF2FEA, 0xF94063EB, 0x2A1603EA, 0xD37EF54A, 0xF86A696A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000100, 0xA9BF27E8, 0xF94002E8, 0xF9401D08, 0xAA1703E0, 0xD63F0100, 0xA8C127E8, 0xAA0003E8, 0xA8C12FEA, 0xAA0803E0
};
static u32 PRC_ID_RCV_1000[] =
{
	0xA9BF2FEA, 0xF94067EB, 0x2A1A03EA, 0xD37EF54A, 0xF86A696A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9, 0xEB09015F, 0x54000100, 0xA9BF27E8, 0xF9400388, 0xF9401D08, 0xAA1C03E0, 0xD63F0100, 0xA8C127E8, 0xAA0003E8, 0xA8C12FEA, 0xAA0803E0
};

static const KernelMeta kernelInfo[] = {
    {   //0 [1.0.0]
        {0xB8, 0xC5, 0x0C, 0x68, 0x25, 0xA9, 0xB9, 0x5B},
        0x3764C,
        0x44074,
        0x23CC0,
        0x219F0,
        0,
        4,
        4,
        0x4797C,
        PRC_ID_SND_100, sizeof(PRC_ID_SND_100),
        PRC_ID_RCV_100, sizeof(PRC_ID_RCV_100)
    },
    {   //1 [2.0.0]
        {0x64, 0x0B, 0x51, 0xFF, 0x28, 0x01, 0xB8, 0x30},
        0x54834,
        0x6086C,
        0x3F134,
        0x3D1A8,
        0,
        4,
        4,
        0x6486C,
        PRC_ID_SND_200, sizeof(PRC_ID_SND_200),
        PRC_ID_RCV_200, sizeof(PRC_ID_RCV_200)
    },
    {   //2 [3.0.0]
        {0x50, 0x84, 0x23, 0xAC, 0x6F, 0xA1, 0x5D, 0x3B},
        0x3BD24,
        0x483FC,
        0x26080,
        0x240F0,
        0,
        4,
        4,
        0x494A4,
        PRC_ID_SND_300, sizeof(PRC_ID_SND_300),
        PRC_ID_RCV_300, sizeof(PRC_ID_RCV_300)
    },
    {   //3 [3.0.2]
        {0x81, 0x9D, 0x08, 0xBE, 0xE4, 0x5E, 0x1F, 0xBB},
        0x3BD24,
        0x48414,
        0x26080,
        0x240F0,
        0,
        4,
        4,
        0x494BC,
        PRC_ID_SND_302, sizeof(PRC_ID_SND_302),
        PRC_ID_RCV_302, sizeof(PRC_ID_RCV_302)
    },
    {   //4 [4.0.0]
        {0xE6, 0xC0, 0xB7, 0xE3, 0x2F, 0xF9, 0x44, 0x51},
        0x41EB4,
        0x4EBFC,
        0x2AF64,
        0x28F6C,
        0,
        8,
        4,
        0x52890,
        PRC_ID_SND_400, sizeof(PRC_ID_SND_400),
        PRC_ID_RCV_400, sizeof(PRC_ID_RCV_400)
    },
    {   //5 [5.0.0]
        {0xB2, 0x38, 0x61, 0xA8, 0xE1, 0xE2, 0xE4, 0xE4},
        0x45E6C,
        0x5513C,
        0x2AD34,
        0x28DAC,
        0x38C2C,
        8,
        8,
        0x5C020,
        PRC_ID_SND_500, sizeof(PRC_ID_SND_500),
        PRC_ID_RCV_500, sizeof(PRC_ID_RCV_500),
        0x54E30
    },
    {   //6 [6.0.0]
        {0x85, 0x97, 0x40, 0xF6, 0xC0, 0x3E, 0x3D, 0x44},
        0x47EA0,
        0x57548,
        0x2BB8C,
        0x29B6C,
        0x3A8CC,
        0x10,
        0x10,
        0x5EE00,
        PRC_ID_SND_600, sizeof(PRC_ID_SND_600),
        PRC_ID_RCV_600, sizeof(PRC_ID_RCV_600),
        0x57330
    },
    {   //7 [7.0.0]
        {0xA2, 0x5E, 0x47, 0x0C, 0x8E, 0x6D, 0x2F, 0xD7},
        0x49E5C,
        0x581B0,
        0x2D044,
        0x2B23C,
        0x3C6E0,
        0x10,
        0x10,
        0x5FEC0,
        PRC_ID_SND_700, sizeof(PRC_ID_SND_700),
        PRC_ID_RCV_700, sizeof(PRC_ID_RCV_700),
        0x57F98
    },
    {   //8 [8.0.0]
        {0xF1, 0x5E, 0xC8, 0x34, 0xFD, 0x68, 0xF0, 0xF0},
        0x4D15C,
        0x5BFAC,
        0x2F1FC,
        0x2D424,
        0x3FAD0,
        0x10,
        0x10,
        0x607F0,
        PRC_ID_SND_800, sizeof(PRC_ID_SND_800),
        PRC_ID_RCV_800, sizeof(PRC_ID_RCV_800),
        0x5F9A4
    },
    {
        //9 [9.0.0 - 9.1.0]
        {0x69, 0x00, 0x39, 0xDF, 0x21, 0x56, 0x70, 0x6B},
        0x50628,
        0x609E8,
        0x329A0,
        0x309B4,
        0x43DFC,
        0x10,
        0x10,
        0x65780,
        PRC_ID_SND_900, sizeof(PRC_ID_SND_900),
        PRC_ID_RCV_900, sizeof(PRC_ID_RCV_900),
        0x6493C
    },
	    {
        //9 [9.2.0]
        {0xA2, 0xE3, 0xAD, 0x1C, 0x98, 0xD8, 0x7A, 0x62},
        0x50628,
        0x609E8,
        0x329A0,
        0x309B4,
        0x43DFC,
        0x10,
        0x10,
        0x65780,
        PRC_ID_SND_900, sizeof(PRC_ID_SND_900),
        PRC_ID_RCV_900, sizeof(PRC_ID_RCV_900),
        0x6493C
    },
		{
        //10 [10.0.0]
        {0x21, 0xC1, 0xD7, 0x24, 0x8E, 0xCD, 0xBD, 0xA8},
        0x523E4,
        0x62B14,
        0x34404,
        0x322F8,
        0x45DAC,
        0x10,
        0x10,
        0x67790,
        PRC_ID_SND_1000, sizeof(PRC_ID_SND_1000),
        PRC_ID_RCV_1000, sizeof(PRC_ID_RCV_1000),
        0x66950
    }
};

// TODO: get full hashes somewhere and not just the first 16 bytes
// every second one is the exfat version
static kippatchset_t kip_patches[] = {
    { "FS", "\xde\x9f\xdd\xa4\x08\x5d\xd5\xfe", fs_kip_patches_100 },
    { "FS", "\xfc\x3e\x80\x99\x1d\xca\x17\x96", fs_kip_patches_100 },
    { "FS", "\xcd\x7b\xbe\x18\xd6\x13\x0b\x28", fs_kip_patches_200 },
    { "FS", "\xe7\x66\x92\xdf\xaa\x04\x20\xe9", fs_kip_patches_200 },
    { "FS", "\x0d\x70\x05\x62\x7b\x07\x76\x7c", fs_kip_patches_210 },
    { "FS", "\xdb\xd8\x5f\xca\xcc\x19\x3d\xa8", fs_kip_patches_210 },
    { "FS", "\xa8\x6d\xa5\xe8\x7e\xf1\x09\x7b", fs_kip_patches_300 },
    { "FS", "\x98\x1c\x57\xe7\xf0\x2f\x70\xf7", fs_kip_patches_300 },
    { "FS", "\x57\x39\x7c\x06\x3f\x10\xb6\x31", fs_kip_patches_301 },
    { "FS", "\x07\x30\x99\xd7\xc6\xad\x7d\x89", fs_kip_patches_301 },
    { "FS", "\x06\xe9\x07\x19\x59\x5a\x01\x0c", fs_kip_patches_401 },
    { "FS", "\x54\x9b\x0f\x8d\x6f\x72\xc4\xe9", fs_kip_patches_401 },
    { "FS", "\x80\x96\xaf\x7c\x6a\x35\xaa\x82", fs_kip_patches_410 },
    { "FS", "\x02\xd5\xab\xaa\xfd\x20\xc8\xb0", fs_kip_patches_410 },
    { "FS", "\xa6\xf2\x7a\xd9\xac\x7c\x73\xad", fs_kip_patches_500 },
    { "FS", "\xce\x3e\xcb\xa2\xf2\xf0\x62\xf5", fs_kip_patches_500 },
    { "FS", "\x76\xf8\x74\x02\xc9\x38\x7c\x0f", fs_kip_patches_510 },
    { "FS", "\x10\xb2\xd8\x16\x05\x48\x85\x99", fs_kip_patches_510 },
    { "FS", "\x1b\x82\xcb\x22\x18\x67\xcb\x52", fs_kip_patches_600_40 },
    { "FS", "\x96\x6a\xdd\x3d\x20\xb6\x27\x13", fs_kip_patches_600_40_exfat },
    { "FS", "\x3a\x57\x4d\x43\x61\x86\x19\x1d", fs_kip_patches_600_50 },
    { "FS", "\x33\x05\x53\xf6\xb5\xfb\x55\xc4", fs_kip_patches_600_50_exfat },
    { "FS", "\x2a\xdb\xe9\x7e\x9b\x5f\x41\x77", fs_kip_patches_700 },
    { "FS", "\x2c\xce\x65\x9c\xec\x53\x6a\x8e", fs_kip_patches_700_exfat },
    { "FS", "\xb2\xf5\x17\x6b\x35\x48\x36\x4d", fs_kip_patches_800 },
    { "FS", "\xdb\xd9\x41\xc0\xc5\x3c\x52\xcc", fs_kip_patches_800_exfat },
    { "FS", "\x6b\x09\xb6\x7b\x29\xc0\x20\x24", fs_kip_patches_810 },
    { "FS", "\xb4\xca\xe1\xf2\x49\x65\xd9\x2e", fs_kip_patches_810_exfat },
    { "FS", "\x46\x87\x40\x76\x1e\x19\x3e\xb7", fs_kip_patches_900 },
    { "FS", "\x7c\x95\x13\x76\xe5\xc1\x2d\xf8", fs_kip_patches_900 },
	{ "FS", "\xB5\xE7\xA6\x4C\x6F\x5C\x4F\xE3", fs_kip_patches_910 },       // FS 9.1.0
	{ "FS", "\xF1\x96\xD1\x44\xD0\x44\x45\xB6", fs_kip_patches_910 },       // FS 9.1.0 exfat
	{ "FS", "\x3E\xEB\xD9\xB7\xBC\xD1\xB5\xE0", fs_kip_patches_1000 },      // FS 10.0.0
	{ "FS", "\x81\x7E\xA2\xB0\xB7\x02\xC1\xF3", fs_kip_patches_1000 },      // FS 10.0.0 exfat
    { NULL, NULL, NULL },
};

void patchFS(pkg2_kip1_info_t* ki);
void patchKernel(pkg2_hdr_t *pkg2, u8 fwVer);
void patchKernelExtensions(link_t *kips);
pkg2_kip1_info_t* find_by_tid(link_t* kip_list, u64 tid);