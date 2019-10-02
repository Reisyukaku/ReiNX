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

#define PKG2_MAGIC 0x31324B50
#define PKG2_SEC_BASE 0x80000000
#define PKG2_SEC_KERNEL 0
#define PKG2_SEC_INI1 1

#define INI1_MAGIC 0x31494E49

#define NOP_v8 0xD503201F
#define NOP_v7 0xE320F000
#define ADRP(r, o) 0x90000000 | ((((o) >> 12) & 0x3) << 29) | ((((o) >> 12) & 0x1FFFFC) << 3) | ((r) & 0x1F)

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
    char id[15];
    u32 kb;
    u32 hos;
    u32 tsec_off;
    u32 pkg11_off;
    u32 sec_map[3];
    u32 secmon_base;
    u32 warmboot_base;
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

typedef struct {
    u32 ini_off;
    u32 krnlLdr_off;
} KernelNewOffs;

typedef struct {
    u8 Hash[0x20];
    u32 SvcVerify;
    u32 SvcDebug;
    u32 SendOff;
    u32 RcvOff;
    u32 GenericOff;
    u8 CodeSndOff;
    u8 CodeRcvOff;
    u32 freespace;
} KernelMeta;

u8 *ReadBoot0(sdmmc_storage_t *storage);
u8 *ReadPackage1Ldr(sdmmc_storage_t *storage);
u8 *ReadPackage2(sdmmc_storage_t *storage, size_t *out_size);
int kippatch_apply(u8 *kipdata, u64 kipdata_len, kippatch_t *patch);
int kippatch_apply_set(u8 *kipdata, u64 kipdata_len, kippatchset_t *patchset);
kippatchset_t *kippatch_find_set(u8 *kiphash, kippatchset_t *patchsets);
pkg2_hdr_t *unpackFirmwarePackage(u8 *data);
void pkg1_unpack(pk11_offs *offs, u32 pkg1Off);
void buildFirmwarePackage(u8 *kernel, u32 kernel_size, link_t *kips_info, pk11_offs *pk11Offs);
size_t calcKipSize(pkg2_kip1_t *kip1);
void pkg2_parse_kips(link_t *info, pkg2_hdr_t *pkg2);
bool hasCustomKern();
bool hasCustomSecmon();
bool hasCustomWb();
u8 *LoadExtFile(char *path, size_t *size);
void loadKip(link_t *info, char *path);
u32 *getSndPayload(u32 id, size_t *size);
u32 *getRcvPayload(u32 id, size_t *size);