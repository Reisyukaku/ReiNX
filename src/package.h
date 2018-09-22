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
u32 *getSndPayload(u32 id, size_t *size);
u32 *getRcvPayload(u32 id, size_t *size);