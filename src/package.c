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

#include "hwinit/gfx.h"
#include "hwinit/list.h"
#include "error.h"
#include "fs.h"
#include "package.h"

pkg2_hdr_t *unpackFirmwarePackage(u8 *data) {
    print("Unpacking firmware...\n");
    pkg2_hdr_t *hdr = (pkg2_hdr_t *)(data + 0x100);
    
    //Decrypt header.
    se_aes_crypt_ctr(8, hdr, sizeof(pkg2_hdr_t), hdr, sizeof(pkg2_hdr_t), hdr);

    if (hdr->magic != PKG2_MAGIC) {
        error("Package2 Magic invalid!\n");
        return NULL;
    }

    //Decrypt body
    data += (0x100 + sizeof(pkg2_hdr_t));
    
    for (u32 i = 0; i < 4; i++) {
        if (!hdr->sec_size[i])
            continue;

        se_aes_crypt_ctr(8, data, hdr->sec_size[i], data, hdr->sec_size[i], &hdr->sec_ctr[i * 0x10]);

        data += hdr->sec_size[i];
    }

    return hdr;
}

void pkg1_unpack(pk11_offs *offs, u8 *pkg1) {
    u8 ret = 0;
    u8 *extWb;
    u8 *extSec;
    
    pk11_header *hdr = (pk11_header *)(pkg1 + offs->pkg11_off + 0x20);

    u32 sec_size[3] = { hdr->wb_size, hdr->ldr_size, hdr->sm_size };
    u8 *pdata = (u8 *)hdr + sizeof(pk11_header);
    
    for (u32 i = 0; i < 3; i++) {
        if (offs->sec_map[i] == 0 && offs->warmboot_base) {
            u8 *extWb = NULL;
            if(fopen("/ReiNX/warmboot.bin", "rb") != 0) {
                extWb = malloc(fsize());
                fread(extWb, fsize(), 1);
                fclose();
            }
            memcpy((void *)offs->warmboot_base, extWb == NULL ? pdata : extWb, sec_size[offs->sec_map[i]]);
        } else if (offs->sec_map[i] == 2 && offs->secmon_base) {
            u8 *extSec = NULL;
            if(fopen("/ReiNX/secmon.bin", "rb") != 0) {
                extSec = malloc(fsize());
                fread(extSec, fsize(), 1);
                fclose();
            }
            memcpy((u8 *)offs->secmon_base, extSec == NULL ? pdata : extSec, sec_size[offs->sec_map[i]]);
        }
        pdata += sec_size[offs->sec_map[i]];
    }
    if(extWb != NULL) {
        free(extWb); 
        customWarmboot = 1;
    }
    if(extSec != NULL) {
        free(extSec); 
        customSecmon = 1;
    }
}

void buildFirmwarePackage(u8 *kernel, u32 kernel_size, link_t *kips_info) {
    u8 *pdst = (u8 *)0xA9800000;

    // Signature.
    memset(pdst, 0, 0x100);
    pdst += 0x100;

    // Header.
    pkg2_hdr_t *hdr = (pkg2_hdr_t *)pdst;
    memset(hdr, 0, sizeof(pkg2_hdr_t));
    pdst += sizeof(pkg2_hdr_t);
    hdr->magic = PKG2_MAGIC;
    hdr->base = 0x10000000;
    print("kernel @ %08X (%08X)\n", (u32)kernel, kernel_size);

    // Kernel.
    u8 *extKern = NULL;
    if(fopen("/ReiNX/kernel.bin", "rb") != 0) {
        extKern = malloc(fsize());
        fread(extKern, fsize(), 1);
        fclose();
    }
    if(extKern != NULL) customKern = 1;
    memcpy(pdst, extKern == NULL ? kernel : extKern, kernel_size);
    hdr->sec_size[PKG2_SEC_KERNEL] = kernel_size;
    hdr->sec_off[PKG2_SEC_KERNEL] = 0x10000000;
    se_aes_crypt_ctr(8, pdst, kernel_size, pdst, kernel_size, &hdr->sec_ctr[PKG2_SEC_KERNEL * 0x10]);
    pdst += kernel_size;
    print("kernel encrypted\n");

    // INI1.
    u32 ini1_size = sizeof(pkg2_ini1_t);
    pkg2_ini1_t *ini1 = (pkg2_ini1_t *)pdst;
    memset(ini1, 0, sizeof(pkg2_ini1_t));
    ini1->magic = INI1_MAGIC;
    pdst += sizeof(pkg2_ini1_t);
    LIST_FOREACH_ENTRY(pkg2_kip1_info_t, ki, kips_info, link) {
        print("adding kip1 '%s' @ %08X (%08X)\n", ki->kip1->name, (u32)ki->kip1, ki->size);
        memcpy(pdst, ki->kip1, ki->size);
        pdst += ki->size;
        ini1_size += ki->size;
        ini1->num_procs++;
    }
    ini1->size = ini1_size;
    hdr->sec_size[PKG2_SEC_INI1] = ini1_size;
    hdr->sec_off[PKG2_SEC_INI1] = 0x14080000;
    se_aes_crypt_ctr(8, ini1, ini1_size, ini1, ini1_size, &hdr->sec_ctr[PKG2_SEC_INI1 * 0x10]);
    print("INI1 encrypted\n");

    // Encrypt header.
    *(u32 *)hdr->ctr = 0x100 + sizeof(pkg2_hdr_t) + kernel_size + ini1_size;
    se_aes_crypt_ctr(8, hdr, sizeof(pkg2_hdr_t), hdr, sizeof(pkg2_hdr_t), hdr);
    memset(hdr->ctr, 0 , 0x10);
    *(u32 *)hdr->ctr = 0x100 + sizeof(pkg2_hdr_t) + kernel_size + ini1_size;
}

size_t calcKipSize(pkg2_kip1_t *kip1) {
    u32 size = sizeof(pkg2_kip1_t);
    for (u32 j = 0; j < KIP1_NUM_SECTIONS; j++)
        size += kip1->sections[j].size_comp;
    return size;
}

void pkg2_parse_kips(link_t *info, pkg2_hdr_t *pkg2) {
    u8 *ptr = pkg2->data + pkg2->sec_size[PKG2_SEC_KERNEL];
    pkg2_ini1_t *ini1 = (pkg2_ini1_t *)ptr;
    ptr += sizeof(pkg2_ini1_t);

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