/*
* Copyright (c) 2018 naehrwert, CTCaer, Reisyukaku
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
#include "hwinit/arm64.h"

#define PKG2_MAGIC 0x31324B50
#define PKG2_SEC_BASE 0x80000000
#define PKG2_SEC_KERNEL 0
#define PKG2_SEC_INI1 1

#define INI1_MAGIC 0x31494E49

#define FREE_CODE_OFF_1ST_100 0x4797C
#define FREE_CODE_OFF_1ST_200 0x6486C
#define FREE_CODE_OFF_1ST_300 0x494A4
#define FREE_CODE_OFF_1ST_302 0x494BC
#define FREE_CODE_OFF_1ST_400 0x52890
#define FREE_CODE_OFF_1ST_500 0x5C020
#define FREE_CODE_OFF_1ST_600 0x5EE00

#define ID_SND_OFF_100 0x23CC0
#define ID_SND_OFF_200 0x3F134
#define ID_SND_OFF_300 0x26080
#define ID_SND_OFF_302 0x26080
#define ID_SND_OFF_400 0x2AF64
#define ID_SND_OFF_500 0x2AD34
#define ID_SND_OFF_600 0x2BB88

#define ID_RCV_OFF_100 0x219F0
#define ID_RCV_OFF_200 0x3D1A8
#define ID_RCV_OFF_300 0x240F0
#define ID_RCV_OFF_302 0x240F0
#define ID_RCV_OFF_400 0x28F6C
#define ID_RCV_OFF_500 0x28DAC
#define ID_RCV_OFF_600 0x29B6C

#define NOP 0xD503201F
#define ADRP(r, o) 0x90000000 | ((((o) >> 12) & 0x3) << 29) | ((((o) >> 12) & 0x1FFFFC) << 3) | ((r) & 0x1F)

static u8 customSecmon = 0;
static u8 customWarmboot = 0;
static u8 customKern = 0;

typedef struct _pkg2_hdr_t
{
	u8 ctr[0x10];
	u8 sec_ctr[0x40];
	u32 magic;
	u32 base;
	u32 pad0;
	u16 version;
	u16 pad1;
	u32 sec_size[4];
	u32 sec_off[4];
	u8 sec_sha256[0x80];
	u8 data[];
} pkg2_hdr_t;

typedef struct _pkg2_ini1_t
{
	u32 magic;
	u32 size;
	u32 num_procs;
	u32 pad;
} pkg2_ini1_t;

typedef struct _pkg2_kip1_sec_t
{
	u32 offset;
	u32 size_decomp;
	u32 size_comp;
	u32 attrib;
} pkg2_kip1_sec_t;

#define KIP1_NUM_SECTIONS 6

typedef struct _pkg2_kip1_t
{
	u32 magic;
	char name[12];
	u64 tid;
	u32 proc_cat;
	u8 main_thrd_prio;
	u8 def_cpu_core;
	u8 res;
	u8 flags;
	pkg2_kip1_sec_t sections[KIP1_NUM_SECTIONS];
	u32 caps[0x20];
	u8 data[];
} pkg2_kip1_t;

typedef struct _pkg2_kip1_info_t
{
	pkg2_kip1_t *kip1;
	u32 size;
	link_t link;
} pkg2_kip1_info_t;

typedef struct {
	const char *id;
	u32 kb;
	u32 tsec_off;
	u32 pkg11_off;
	u32 sec_map[3];
	u32 secmon_base;
	u32 warmboot_base;
	int set_warmboot;
} pk11_offs;

typedef struct {
	u32 magic;
	u32 wb_size;
	u32 wb_off;
	u32 pad;
	u32 ldr_size;
	u32 ldr_off;
	u32 sm_size;
	u32 sm_off;
} pk11_header;

enum
{
	// Generic instruction patches
	SVC_VERIFY_DS = 0x10, // 0x0-0xF are RESERVED.
	DEBUG_MODE_EN,
	ATM_GEN_PATCH,
	// >4 bytes patches. Value is a pointer of a u32 array.
	ATM_ARR_PATCH,
};

typedef struct _kernel_patch_t
{
	u32 id;
	u32 off;
	u32 val;
	u32 *ptr;
} kernel_patch_t;

typedef struct _pkg2_kernel_id_t
{
	u32 crc32c_id;
	kernel_patch_t *kernel_patchset;
} pkg2_kernel_id_t;


static u32 PRC_ID_SND_100[] =
{
	0xA9BF2FEA, 0x2A0E03EB, 0xD37EF56B, 0xF86B6B8B, 0x92FFFFE9, 0x8A090168, 0xD2FFFFE9, 0x8A09016B,
	0xD2FFFFC9, 0xEB09017F, 0x54000040, 0xF9412948,	0xA8C12FEA
};
#define FREE_CODE_OFF_2ND_100 (FREE_CODE_OFF_1ST_100 + sizeof(PRC_ID_SND_100) + 4)
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
#define FREE_CODE_OFF_2ND_200 (FREE_CODE_OFF_1ST_200 + sizeof(PRC_ID_SND_200) + 4)
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
#define FREE_CODE_OFF_2ND_300 (FREE_CODE_OFF_1ST_300 + sizeof(PRC_ID_SND_300) + 4)
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
#define FREE_CODE_OFF_2ND_302 (FREE_CODE_OFF_1ST_302 + sizeof(PRC_ID_SND_302) + 4)
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
#define FREE_CODE_OFF_2ND_400 (FREE_CODE_OFF_1ST_400 + sizeof(PRC_ID_SND_400) + 4)
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
#define FREE_CODE_OFF_2ND_500 (FREE_CODE_OFF_1ST_500 + sizeof(PRC_ID_SND_500) + 4)
static u32 PRC_ID_RCV_500[] =
{
	0xF9403BED, 0x2A1503EA, 0xD37EF54A, 0xF86A69AA, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A,
	0xD2FFFFC9, 0xEB09015F, 0x54000040, 0xF9415B08, 0xF9406FEA
};

static u32 PRC_ID_SND_600[] =
{
	0x2A1703EA, 0xD37EF54A, 0xF86A6B6A, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A, 0xD2FFFFC9,
	0xEB09015F, 0x54000060, 0xF94043EA, 0xF9415948, 0xF94043EA
};
#define FREE_CODE_OFF_2ND_600 (FREE_CODE_OFF_1ST_600 + sizeof(PRC_ID_SND_600) + 4)
static u32 PRC_ID_RCV_600[] =
{
	0xF9403BED, 0x2A1503EA, 0xD37EF54A, 0xF86A69AA, 0x92FFFFE9, 0x8A090148, 0xD2FFFFE9, 0x8A09014A,
	0xD2FFFFC9, 0xEB09015F, 0x54000040, 0xF9415B08, 0xF9406FEA
};


static kernel_patch_t kern1[] = {
	{ SVC_VERIFY_DS, 0x3764C, _NOP(), NULL },          // Disable SVC verifications
	{ DEBUG_MODE_EN, 0x44074, _MOVZX(8, 1, 0), NULL }, // Enable Debug Patch
	// Atmosphère kernel patches.
	{ ATM_GEN_PATCH, ID_SND_OFF_100, _B(ID_SND_OFF_100, FREE_CODE_OFF_1ST_100), NULL},    // Send process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_1ST_100, sizeof(PRC_ID_SND_100) >> 2, PRC_ID_SND_100}, // Send process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_1ST_100 + sizeof(PRC_ID_SND_100),                      // Branch back and skip 1 instruction.
		_B(FREE_CODE_OFF_1ST_100 + sizeof(PRC_ID_SND_100), ID_SND_OFF_100 + 4), NULL},
	{ ATM_GEN_PATCH, ID_RCV_OFF_100, _B(ID_RCV_OFF_100, FREE_CODE_OFF_2ND_100), NULL},    // Receive process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_2ND_100, sizeof(PRC_ID_RCV_100) >> 2, PRC_ID_RCV_100}, // Receive process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_2ND_100 + sizeof(PRC_ID_RCV_100),                      // Branch back and skip 1 instruction.
	_B(FREE_CODE_OFF_2ND_100 + sizeof(PRC_ID_RCV_100), ID_RCV_OFF_100 + 4), NULL},
	{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, (u32*)0xFFFFFFFF}
};

static kernel_patch_t kern2[] = {
	{ SVC_VERIFY_DS, 0x54834, _NOP(), NULL },          // Disable SVC verifications
	{ DEBUG_MODE_EN, 0x6086C, _MOVZX(8, 1, 0), NULL }, // Enable Debug Patch
	// Atmosphère kernel patches.
	{ ATM_GEN_PATCH, ID_SND_OFF_200, _B(ID_SND_OFF_200, FREE_CODE_OFF_1ST_200), NULL},    // Send process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_1ST_200, sizeof(PRC_ID_SND_200) >> 2, PRC_ID_SND_200}, // Send process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_1ST_200 + sizeof(PRC_ID_SND_200),                      // Branch back and skip 1 instruction.
		_B(FREE_CODE_OFF_1ST_200 + sizeof(PRC_ID_SND_200), ID_SND_OFF_200 + 4), NULL},
	{ ATM_GEN_PATCH, ID_RCV_OFF_200, _B(ID_RCV_OFF_200, FREE_CODE_OFF_2ND_200), NULL},    // Receive process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_2ND_200, sizeof(PRC_ID_RCV_200) >> 2, PRC_ID_RCV_200}, // Receive process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_2ND_200 + sizeof(PRC_ID_RCV_200),                      // Branch back and skip 1 instruction.
		_B(FREE_CODE_OFF_2ND_200 + sizeof(PRC_ID_RCV_200), ID_RCV_OFF_200 + 4), NULL},
	{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, (u32*)0xFFFFFFFF}
};
static kernel_patch_t kern3[] = {
	{ SVC_VERIFY_DS, 0x3BD24, _NOP(), NULL },          // Disable SVC verifications
	{ DEBUG_MODE_EN, 0x483FC, _MOVZX(8, 1, 0), NULL }, // Enable Debug Patch
	// Atmosphère kernel patches.
	{ ATM_GEN_PATCH, ID_SND_OFF_300, _B(ID_SND_OFF_300, FREE_CODE_OFF_1ST_300), NULL},    // Send process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_1ST_300, sizeof(PRC_ID_SND_300) >> 2, PRC_ID_SND_300}, // Send process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_1ST_300 + sizeof(PRC_ID_SND_300),                      // Branch back and skip 1 instruction.
		_B(FREE_CODE_OFF_1ST_300 + sizeof(PRC_ID_SND_300), ID_SND_OFF_300 + 4), NULL},
	{ ATM_GEN_PATCH, ID_RCV_OFF_300, _B(ID_RCV_OFF_300, FREE_CODE_OFF_2ND_300), NULL},    // Receive process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_2ND_300, sizeof(PRC_ID_RCV_300) >> 2, PRC_ID_RCV_300}, // Receive process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_2ND_300 + sizeof(PRC_ID_RCV_300),                      // Branch back and skip 1 instruction.
		_B(FREE_CODE_OFF_2ND_300 + sizeof(PRC_ID_RCV_300), ID_RCV_OFF_300 + 4), NULL},
	{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, (u32*)0xFFFFFFFF}
};
static kernel_patch_t kern302[] = {
	{ SVC_VERIFY_DS, 0x3BD24, _NOP(), NULL },          // Disable SVC verifications
	{ DEBUG_MODE_EN, 0x48414, _MOVZX(8, 1, 0), NULL }, // Enable Debug Patch
	// Atmosphère kernel patches.
	{ ATM_GEN_PATCH, ID_SND_OFF_302, _B(ID_SND_OFF_302, FREE_CODE_OFF_1ST_302), NULL},    // Send process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_1ST_302, sizeof(PRC_ID_SND_302) >> 2, PRC_ID_SND_302}, // Send process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_1ST_302 + sizeof(PRC_ID_SND_302),                      // Branch back and skip 1 instruction.
		_B(FREE_CODE_OFF_1ST_302 + sizeof(PRC_ID_SND_302), ID_SND_OFF_302 + 4), NULL},
	{ ATM_GEN_PATCH, ID_RCV_OFF_302, _B(ID_RCV_OFF_302, FREE_CODE_OFF_2ND_302), NULL},    // Receive process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_2ND_302, sizeof(PRC_ID_RCV_302) >> 2, PRC_ID_RCV_302}, // Receive process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_2ND_302 + sizeof(PRC_ID_RCV_302),                      // Branch back and skip 1 instruction.
		_B(FREE_CODE_OFF_2ND_302 + sizeof(PRC_ID_RCV_302), ID_RCV_OFF_302 + 4), NULL},
	{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, (u32*)0xFFFFFFFF}
};
static kernel_patch_t kern4[] = {
	{ SVC_VERIFY_DS, 0x41EB4, _NOP(), NULL },          // Disable SVC verifications
	{ DEBUG_MODE_EN, 0x4EBFC, _MOVZX(8, 1, 0), NULL }, // Enable Debug Patch
	// Atmosphère kernel patches.
	{ ATM_GEN_PATCH, ID_SND_OFF_400, _B(ID_SND_OFF_400, FREE_CODE_OFF_1ST_400), NULL},    // Send process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_1ST_400, sizeof(PRC_ID_SND_400) >> 2, PRC_ID_SND_400}, // Send process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_1ST_400 + sizeof(PRC_ID_SND_400),                      // Branch back and skip 2 instructions.
		_B(FREE_CODE_OFF_1ST_400 + sizeof(PRC_ID_SND_400), ID_SND_OFF_400 + 8), NULL},
	{ ATM_GEN_PATCH, ID_RCV_OFF_400, _B(ID_RCV_OFF_400, FREE_CODE_OFF_2ND_400), NULL},    // Receive process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_2ND_400, sizeof(PRC_ID_RCV_400) >> 2, PRC_ID_RCV_400}, // Receive process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_2ND_400 + sizeof(PRC_ID_RCV_400),                      // Branch back and skip 1 instruction.
		_B(FREE_CODE_OFF_2ND_400 + sizeof(PRC_ID_RCV_400), ID_RCV_OFF_400 + 4), NULL},
	{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, (u32*)0xFFFFFFFF}
};

static kernel_patch_t kern5[] = {
	{ SVC_VERIFY_DS, 0x45E6C, _NOP(), NULL },          // Disable SVC verifications
	{ DEBUG_MODE_EN, 0x5513C, _MOVZX(8, 1, 0), NULL }, // Enable Debug Patch
	// Atmosphère kernel patches.
	{ ATM_GEN_PATCH, ID_SND_OFF_500, _B(ID_SND_OFF_500, FREE_CODE_OFF_1ST_500), NULL},    // Send process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_1ST_500, sizeof(PRC_ID_SND_500) >> 2, PRC_ID_SND_500}, // Send process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_1ST_500 + sizeof(PRC_ID_SND_500),                      // Branch back and skip 2 instructions.
		_B(FREE_CODE_OFF_1ST_500 + sizeof(PRC_ID_SND_500), ID_SND_OFF_500 + 8), NULL},
	{ ATM_GEN_PATCH, ID_RCV_OFF_500, _B(ID_RCV_OFF_500, FREE_CODE_OFF_2ND_500), NULL},    // Receive process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_2ND_500, sizeof(PRC_ID_RCV_500) >> 2, PRC_ID_RCV_500}, // Receive process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_2ND_500 + sizeof(PRC_ID_RCV_500),                      // Branch back and skip 2 instructions.
		_B(FREE_CODE_OFF_2ND_500 + sizeof(PRC_ID_RCV_500), ID_RCV_OFF_500 + 8), NULL},
	{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, (u32*)0xFFFFFFFF}
};

static kernel_patch_t kern6[] = {
	{ SVC_VERIFY_DS, 0x47E98, _NOP(), NULL },          // Disable SVC verifications
	{ DEBUG_MODE_EN, 0x52D40, _MOVZX(8, 1, 0), NULL }, // Enable Debug Patch
	// Atmosphère kernel patches.
	{ ATM_GEN_PATCH, ID_SND_OFF_600, _B(ID_SND_OFF_600, FREE_CODE_OFF_1ST_600), NULL},    // Send process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_1ST_600, sizeof(PRC_ID_SND_600) >> 2, PRC_ID_SND_600}, // Send process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_1ST_600 + sizeof(PRC_ID_SND_600),                      // Branch back and skip 2 instructions.
		_B(FREE_CODE_OFF_1ST_600 + sizeof(PRC_ID_SND_600), ID_SND_OFF_600 + 8), NULL},
	{ ATM_GEN_PATCH, ID_RCV_OFF_600, _B(ID_RCV_OFF_600, FREE_CODE_OFF_2ND_600), NULL},    // Receive process id branch.
	{ ATM_ARR_PATCH, FREE_CODE_OFF_2ND_600, sizeof(PRC_ID_RCV_600) >> 2, PRC_ID_RCV_600}, // Receive process id code.
	{ ATM_GEN_PATCH, FREE_CODE_OFF_2ND_600 + sizeof(PRC_ID_RCV_600),                      // Branch back and skip 2 instructions.
		_B(FREE_CODE_OFF_2ND_600 + sizeof(PRC_ID_RCV_600), ID_RCV_OFF_600 + 8), NULL},
	{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, (u32*)0xFFFFFFFF}
};

static const pkg2_kernel_id_t _pkg2_kernel_ids[] =
{
	{ 0x427f2647, kern1 },   //1.0.0
	{ 0xae19cf1b, kern2 },   //2.0.0 - 2.3.0
	{ 0x73c9e274, kern3 },   //3.0.0 - 3.0.1
	{ 0xe0e8cdc4, kern302 }, //3.0.2
	{ 0x485d0157, kern4 },   //4.0.0 - 4.1.0
	{ 0xf3c363f2, kern5 },   //5.0.0 - 5.1.0
    { 0x64ce1a44, kern6 },   //6.0.0
	{ 0, 0 }                              //End.
};

typedef struct kipdiff_s {
  u64 offset;              // offset from start of kip's .text segment
  u32 len;                 // length of below strings, NULL signifies end of patch
  const char *orig_bytes;  // original byte string (this must match exactly)
  const char *patch_bytes; // replacement byte string (same length)
} kipdiff_t;

// a single patch for a particular kip version
typedef struct kippatch_s {
  const char *name;        // name/id of the patch, NULL signifies end of patchset
  kipdiff_t *diffs;        // array of kipdiff_t's to apply
} kippatch_t;

// a group of patches that patch several different things in a particular kip version
typedef struct kippatchset_s {
  const char *kip_name;    // name/id of the kip, NULL signifies end of patchset list
  const char *kip_hash;    // sha256 of the right version of the kip
  kippatch_t *patches;     // set of patches for this version of the kip
} kippatchset_t;


extern kippatchset_t kip_patches[];
u8 *ReadPackage1(sdmmc_storage_t *storage);
u8 *ReadPackage2(sdmmc_storage_t *storage);
int kippatch_apply(u8 *kipdata, u64 kipdata_len, kippatch_t *patch);
int kippatch_apply_set(u8 *kipdata, u64 kipdata_len, kippatchset_t *patchset);
kippatchset_t *kippatch_find_set(u8 *kiphash, kippatchset_t *patchsets);
pkg2_hdr_t *unpackFirmwarePackage(u8 *data);
void pkg1_unpack(pk11_offs *offs, u8 *pkg1);
void buildFirmwarePackage(u8 *kernel, u32 kernel_size, link_t *kips_info);
size_t calcKipSize(pkg2_kip1_t *kip1);
void pkg2_parse_kips(link_t *info, pkg2_hdr_t *pkg2);
void loadKip(link_t *info, char *path);
const pkg2_kernel_id_t *pkg2_identify(u32 id);