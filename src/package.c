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

#include "package.h"

bool customSecmon = false;
bool customWb = false;
bool customKernel = false;

u8 *ReadBoot0(sdmmc_storage_t *storage){
    u8 *bctBuf = (u8 *)malloc(0x4000);
    sdmmc_storage_read(storage, 0 , 0x4000 / NX_EMMC_BLOCKSIZE, bctBuf);
    return bctBuf;
}

u8 *ReadPackage1Ldr(sdmmc_storage_t *storage) {
    u8 *pk11 = malloc(0x40000);
    sdmmc_storage_read(storage, 0x100000 / NX_EMMC_BLOCKSIZE, 0x40000 / NX_EMMC_BLOCKSIZE, pk11);
    return pk11;
}

u8 *ReadPackage2(sdmmc_storage_t *storage, size_t *out_size) {
    // Read GPT partition.
    LIST_INIT(gpt);
    sdmmc_storage_set_mmc_partition(storage, 0);
    print("Parsing GPT...\n");
    nx_emmc_gpt_parse(&gpt, storage);
    emmc_part_t *pkg2_part = nx_emmc_part_find(&gpt, "BCPKG2-1-Normal-Main");
    nx_emmc_gpt_free(&gpt);
    if (!pkg2_part) {
        error("Failed to read GPT!\n");
        return 0;
    }

    // Read Package2.
    u8 *tmp = (u8 *)malloc(NX_EMMC_BLOCKSIZE);
    print("Reading Package2 size...\n");
    nx_emmc_part_read(storage, pkg2_part, 0x4000 / NX_EMMC_BLOCKSIZE, 1, tmp);
    u32 *hdr = (u32 *)(tmp + 0x100);
    u32 pkg2_size = hdr[0] ^ hdr[2] ^ hdr[3];
    *out_size = pkg2_size;
    free(tmp);
    u8 *pkg2 = malloc(ALIGN(pkg2_size, NX_EMMC_BLOCKSIZE));
    print("Reading Package2...\n");
    u32 ret = nx_emmc_part_read(storage, pkg2_part, 0x4000 / NX_EMMC_BLOCKSIZE, ALIGN(pkg2_size, NX_EMMC_BLOCKSIZE) / NX_EMMC_BLOCKSIZE, pkg2);
    sdmmc_storage_end(storage);
    if (!ret) {
        error("Failed to read Package2!\n");
        return 0;
    }
    return pkg2;
}

pkg2_hdr_t *unpackFirmwarePackage(u8 *data) {
    print("Unpacking firmware...\n");
    pkg2_hdr_t *hdr = (pkg2_hdr_t *)(data + 0x100);

    //Decrypt header.
    se_aes_crypt_ctr(8, hdr, sizeof(pkg2_hdr_t), hdr, sizeof(pkg2_hdr_t), hdr);

    if (hdr->magic != PKG2_MAGIC) {
        error("Package2 Magic invalid!\nThere is a good chance your ReiNX build is outdated\nPlease get the newest build from our guide (reinx.guide) or our discord (discord.reiswitched.team)\nMake sure you replace the ReiNX.bin file on your SD card root too\n");
        return NULL;
    }

    //Decrypt body
    data += (0x100 + sizeof(pkg2_hdr_t));

    for (u32 i = 0; i < 4; i++) {
        if (!hdr->sec_size[i]) continue;

        se_aes_crypt_ctr(8, data, hdr->sec_size[i], data, hdr->sec_size[i], &hdr->sec_ctr[i * 0x10]);

        data += hdr->sec_size[i];
    }

    return hdr;
}

u8 *LoadExtFile(char *path, size_t *size) {
    u8 *buf = NULL;
    if(fopen(path, "rb") != 0) {
        size_t fileSize = fsize();
        *size = fileSize;
        if(fileSize <= 0) {
            error("File is empty!\n");
            fclose();
            return NULL;
        }
        print("%kReading external file %s%k\n", YELLOW, path, DEFAULT_TEXT_COL);
        buf = malloc(fileSize);
        fread(buf, fileSize, 1);
        fclose();
    }
    return buf;
}

void pkg1_unpack(pk11_offs *offs, u32 pkg1Off) {
    u8 ret = 0;

    pk11_header *hdr = (pk11_header *)(pkg1Off + 0x20);

    u32 sec_size[3] = { hdr->wb_size, hdr->ldr_size, hdr->sm_size };
    u8 *pdata = (u8 *)hdr + sizeof(pk11_header);

    for (u32 i = 0; i < 3; i++) {
        if (offs->sec_map[i] == 0 && offs->warmboot_base) {
            size_t extSize = 0;
            u8 *extWb = LoadExtFile("/ReiNX/warmboot.bin", &extSize);
            if(extWb == NULL) extWb = LoadExtFile("/ReiNX/lp0fw.bin", &extSize);
            if(offs->kb >= KB_FIRMWARE_VERSION_700 && extWb == NULL)
                error("Custom warmboot required!");
            memcpy((void *)offs->warmboot_base, extWb == NULL ? pdata : extWb, extWb == NULL ? sec_size[offs->sec_map[i]] : extSize);
            free(extWb);
        }
        if (offs->sec_map[i] == 2 && offs->secmon_base) {
            size_t extSize = 0;
            u8 *extSec = LoadExtFile("/ReiNX/secmon.bin", &extSize);
            if(extSec == NULL) extSec = LoadExtFile("/ReiNX/exosphere.bin", &extSize);
            if(offs->kb >= KB_FIRMWARE_VERSION_700 && extSec == NULL)
                error("Custom secmon required!");
            memcpy((u8 *)offs->secmon_base, extSec == NULL ? pdata : extSec, extSec == NULL ? sec_size[offs->sec_map[i]] : extSize);
            free(extSec);
        }
        pdata += sec_size[offs->sec_map[i]];
    }
}

bool hasCustomWb() {
    if (customWb)
        return customWb;
    if(fopen("/ReiNX/warmboot.bin", "rb") != 0) {
        customWb = true;
        fclose();
    }
    if(fopen("/ReiNX/lp0fw.bin", "rb") != 0) {
        customWb = true;
        fclose();
    }
    return customWb;
}

bool hasCustomSecmon() {
    if (customSecmon)
        return customSecmon;
    if(fopen("/ReiNX/secmon.bin", "rb") != 0) {
        customSecmon = true;
        fclose();
    }
    if(fopen("/ReiNX/exosphere.bin", "rb") != 0) {
        customSecmon = true;
        fclose();
    }
    return customSecmon;
}

bool hasCustomKern() {
    if (customKernel)
        return customKernel;
    if(fopen("/ReiNX/kernel.bin", "rb") != 0) {
        customKernel = true;
        fclose();
    }
    return customKernel;
}

u32 pkg2_newkern_ini1_val = 0;
u32 pkg2_newkern_ini1_start = 0;
u32 pkg2_newkern_ini1_end = 0;
void pkg2_get_newkern_info(u8 *kern_data)
{
    u32 pkg2_newkern_ini1_off = 0;
	pkg2_newkern_ini1_start = 0;

	// Find static OP offset that is close to INI1 offset.
	u32 counter_ops = 0x100;
	while (counter_ops)
	{
		if (*(u32 *)(kern_data + 0x100 - counter_ops) == PKG2_NEWKERN_GET_INI1_HEURISTIC)
		{
			pkg2_newkern_ini1_off = 0x100 - counter_ops + 12; // OP found. Add 12 for the INI1 offset.
			break;
		}

		counter_ops -= 4;
	}

	// Offset not found?
	if (!counter_ops)
		return;

	u32 info_op = *(u32 *)(kern_data + pkg2_newkern_ini1_off);
	pkg2_newkern_ini1_val = ((info_op & 0xFFFF) >> 3) + pkg2_newkern_ini1_off; // Parse ADR and PC.

    pkg2_newkern_ini1_start = *(u32 *)(kern_data + pkg2_newkern_ini1_val);
    pkg2_newkern_ini1_end   = *(u32 *)(kern_data + pkg2_newkern_ini1_val + 0x8);
}

u32 GetNewKernIniStart() { return pkg2_newkern_ini1_start; }

static u32 buildIni1(pkg2_hdr_t *hdr, void *ini, link_t *kips_info)
{
    u32 ini1_size = sizeof(pkg2_ini1_t);
    pkg2_ini1_t *ini1 = (pkg2_ini1_t *)ini;
    memset(ini1, 0, sizeof(pkg2_ini1_t));
    ini1->magic = INI1_MAGIC;
    ini += sizeof(pkg2_ini1_t);
    LIST_FOREACH_ENTRY(pkg2_kip1_info_t, ki, kips_info, link)
    {
        print("adding kip1 '%s' @ %08X (%08X)\n", ki->kip1->name, (u32)ki->kip1, ki->size);
        memcpy(ini, ki->kip1, ki->size);
        ini += ki->size;
        ini1_size += ki->size;
        ini1->num_procs++;
    }
    ini1->size = ini1_size;
    
    return ini1_size;
}

void buildFirmwarePackage(u8 *kernel, u32 kernel_size, link_t *kips_info, pk11_offs *pk11Offs) {
    u8 *pdst = (u8 *)0xA9800000;
    bool new_pkg2 = pk11Offs->hos >= HOS_FIRMWARE_VERSION_800;

    //Signature.
    memset(pdst, 0, 0x100);
    pdst += 0x100;

    //Header.
    pkg2_hdr_t *hdr = (pkg2_hdr_t *)pdst;
    memset(hdr, 0, sizeof(pkg2_hdr_t));
    pdst += sizeof(pkg2_hdr_t);
    hdr->magic = PKG2_MAGIC;
    hdr->base = new_pkg2 ? 0x60000 : 0x10000000;

    //Read custom kern if applicable
    size_t extSize = 0;
    u8 *extKern = LoadExtFile("/ReiNX/kernel.bin", &extSize);
    memcpy(hdr->data, extKern == NULL ? kernel : extKern, extKern == NULL ? kernel_size : extSize);
    print("Kernel size: %X\n", kernel_size);
    
    pdst += kernel_size;
    
    //Build ini1
    size_t iniSize = buildIni1(hdr, pdst, kips_info); 
    
    //Newer (8.0+) pk21 embeds ini1 in kernel section, so add ini1 size to kernel size
    if (new_pkg2) {
        *(u32*)(hdr->data + pkg2_newkern_ini1_val) = kernel_size;
        kernel_size += iniSize;
    }
    
    //Fill in rest of the header
    hdr->sec_off[PKG2_SEC_KERNEL] = hdr->base;
    hdr->sec_size[PKG2_SEC_KERNEL] = kernel_size;
    hdr->sec_off[PKG2_SEC_INI1] = new_pkg2 ? 0 : 0x14080000;
    hdr->sec_size[PKG2_SEC_INI1] = new_pkg2 ? 0 : iniSize;

    // Encrypt header.
    *(u32 *)hdr->ctr = 0x100 + sizeof(pkg2_hdr_t) + kernel_size + hdr->sec_size[PKG2_SEC_INI1];
    memset(hdr->ctr, 0 , 0x10);
    *(u32 *)hdr->ctr = 0x100 + sizeof(pkg2_hdr_t) + kernel_size + hdr->sec_size[PKG2_SEC_INI1];
}

size_t calcKipSize(pkg2_kip1_t *kip1) {
    u32 size = sizeof(pkg2_kip1_t);
    for (u32 j = 0; j < KIP1_NUM_SECTIONS; j++)
        size += kip1->sections[j].size_comp;
    return size;
}

void pkg2_parse_kips(link_t *info, pkg2_hdr_t *pkg2) {
    print("%kParsing KIPS%k\n", WHITE, DEFAULT_TEXT_COL);
    u8 *ptr = pkg2->data + pkg2->sec_size[PKG2_SEC_KERNEL];
    if (!pkg2->sec_size[PKG2_SEC_INI1]) {
        pkg2_get_newkern_info(pkg2->data);

        ptr = pkg2->data + pkg2_newkern_ini1_start;
    }
    pkg2_ini1_t *ini1 = (pkg2_ini1_t *)ptr;
    ptr += sizeof(pkg2_ini1_t);
    if(ini1->magic != 0x31494E49) {
        error("Invalid INI1 magic!\n");
        return;
    }
    print("INI1 procs: %d\n", ini1->num_procs);
    for (u32 i = 0; i < ini1->num_procs; i++) {
        pkg2_kip1_t *kip1 = (pkg2_kip1_t *)ptr;
        pkg2_kip1_info_t *ki = (pkg2_kip1_info_t *)malloc(sizeof(pkg2_kip1_info_t));
        ki->kip1 = kip1;
        ki->size = calcKipSize(kip1);
        list_append(info, &ki->link);
        ptr += ki->size;
    }
}

void loadKip(link_t *info, char *path) {
    if(fopen(path, "rb") == 0) return;
    pkg2_kip1_t *ckip = malloc(fsize());
    fread(ckip, fsize(), 1);
    fclose();
    LIST_FOREACH_ENTRY(pkg2_kip1_info_t, ki, info, link) {
        if (ki->kip1->tid == ckip->tid) {
            ki->kip1 = ckip;
            ki->size = calcKipSize(ckip);
            return;
        }
    }
    pkg2_kip1_info_t *ki = malloc(sizeof(pkg2_kip1_info_t));
    ki->kip1 = ckip;
    ki->size = calcKipSize(ckip);
    list_append(info, &ki->link);
}

int kippatch_apply(u8 *kipdata, u64 kipdata_len, kippatch_t *patch) {
    if (!patch || !patch->diffs) return -1;

    for (kipdiff_t *diff = patch->diffs; diff->len; ++diff) {
        if (!diff->len || diff->offset + diff->len > kipdata_len)
            return 1 + (int)(diff - patch->diffs);
        u8 *start = kipdata + diff->offset;
        if (memcmp(start, diff->orig_bytes, diff->len))
            continue;
        // TODO: maybe start copying after every diff has been verified?
        memcpy(start, diff->patch_bytes, diff->len);
    }

    return 0;
}

int nca_patch(u8 * kipdata, u64 kipdata_len) {
    char pattern[8] = {0xE5, 0x07, 0x00, 0x32, 0xE0, 0x03, 0x16, 0xAA};
    char buf[0x10];
    memcpy(buf, kipdata+0x1C450, 0x10);
    u32 * addr = (u32*)memsearch(kipdata, kipdata_len, pattern, sizeof(pattern));
    int ret=0;
    int max_dist = 0x10;
    for(int i=0; i<max_dist; i++) {
        u32 op = addr[i];
        if((op & 0xFC000000) == 0x94000000) { //is a BL op
            addr[i] = NOP_v8;
            ret=1;
            break;
        }
    }
    return ret;
}

int kippatch_apply_set(u8 *kipdata, u64 kipdata_len, kippatchset_t *patchset) {
    char *patchFilter[] = { "nosigchk", "nocmac", "nogc", NULL };

    if (!fopen("/ReiNX/nogc", "rb")) {
        patchFilter[2] = NULL;
        fclose();
    }

    for (kippatch_t *p = patchset->patches; p && p->name; ++p) {
        int found = 0;
        for (char **filtname = patchFilter; filtname && *filtname; ++filtname) {
            if (!strcmp(p->name, *filtname)) {
                found = 1;
                break;
            }
        }

        if (patchFilter && !found) continue;

        int r = kippatch_apply(kipdata, kipdata_len, p);
        if (r) return r;
    }
    
    return 0;
}

kippatchset_t *kippatch_find_set(u8 *kiphash, kippatchset_t *patchsets) {
    for (kippatchset_t *ps = patchsets; ps && ps->kip_name; ++ps) {
        if (!memcmp(kiphash, ps->kip_hash, 8)) return ps;
    }
    error("KIP hash not found in list!\n");
    return NULL;
}
