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
#include "error.h"
#include "fs.h"
#include "bootloader.h"

#define PKG2_MAGIC 0x31324B50
#define PKG2_SEC_BASE 0x80000000
#define PKG2_SEC_KERNEL 0
#define PKG2_SEC_INI1 1
#define PKG2_NEWKERN_GET_INI1_HEURISTIC 0xD2800015 // Offset of OP + 12 is the INI1 offset.
#define INI1_MAGIC 0x31494E49

#define NOP_v8 0xD503201F
#define NOP_v7 0xE320F000
#define ADRP(r, o) 0x90000000 | ((((o) >> 12) & 0x3) << 29) | ((((o) >> 12) & 0x1FFFFC) << 3) | ((r) & 0x1F)

u8 *ReadBoot0(sdmmc_storage_t *storage);
u8 *ReadPackage1Ldr(sdmmc_storage_t *storage);
u8 *ReadPackage2(sdmmc_storage_t *storage, size_t *out_size);
int kippatch_apply(u8 *kipdata, u64 kipdata_len, kippatch_t *patch);
int kippatch_apply_set(u8 *kipdata, u64 kipdata_len, kippatchset_t *patchset);
kippatchset_t *kippatch_find_set(u8 *kiphash, kippatchset_t *patchsets);
pkg2_hdr_t *unpackFirmwarePackage(u8 *data, u8 fwVer);
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
u32 GetNewKernIniStart();