/*
* Copyright (c) 2018 naehrwert
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

#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "list.h"

#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define OFFSET_OF(t, m) ((u32)&((t *)NULL)->m)
#define CONTAINER_OF(mp, t, mn) ((t *)((u32)mp - OFFSET_OF(t, mn)))

#define R_FAILED(res)   ((res)!=0)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef volatile uint8_t vu8;
typedef volatile uint16_t vu16;
typedef volatile uint32_t vu32;
typedef volatile uint64_t vu64;

typedef uintptr_t uPtr;

enum KB_FIRMWARE_VERSION {
    KB_FIRMWARE_VERSION_100 = 0,
    KB_FIRMWARE_VERSION_200 = 0,
    KB_FIRMWARE_VERSION_300 = 1,
    KB_FIRMWARE_VERSION_301 = 2,
    KB_FIRMWARE_VERSION_400 = 3,
    KB_FIRMWARE_VERSION_500 = 4,
    KB_FIRMWARE_VERSION_600 = 5,
    KB_FIRMWARE_VERSION_620 = 6,
    KB_FIRMWARE_VERSION_700 = 7,
    KB_FIRMWARE_VERSION_701 = 7,
    KB_FIRMWARE_VERSION_800 = 7,
    KB_FIRMWARE_VERSION_810 = 8,
    KB_FIRMWARE_VERSION_900 = 9,
    KB_FIRMWARE_VERSION_910 = 10,
	KB_FIRMWARE_VERSION_1000 = 11,
};

enum HOS_FIRMWARE_VERSION {
    HOS_FIRMWARE_VERSION_100 = 1,
    HOS_FIRMWARE_VERSION_200 = 2,
    HOS_FIRMWARE_VERSION_300 = 3,
    HOS_FIRMWARE_VERSION_400 = 4,
    HOS_FIRMWARE_VERSION_500 = 5,
    HOS_FIRMWARE_VERSION_600 = 6,
    HOS_FIRMWARE_VERSION_620 = 7,
    HOS_FIRMWARE_VERSION_700 = 8,
    HOS_FIRMWARE_VERSION_800 = 9,
    HOS_FIRMWARE_VERSION_810 = 10,
    HOS_FIRMWARE_VERSION_900 = 11, 
	HOS_FIRMWARE_VERSION_910 = 12, 
	HOS_FIRMWARE_VERSION_1000 = 13, 
};

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
    u32 Freespace;
    u32 *SndPayload;
    u32 SndPayloadSize;
    u32 *RcvPayload;
    u32 RcvPayloadSize;
    u32 SYSM_INCR;
} KernelMeta;

#define UWU0_MAGIC (u32)0x30557755
#define METADATA_OFFSET 0xB0

typedef struct {
	u32 magic;
	u8 major;
	u8 minor;
} metadata_t;

#endif
