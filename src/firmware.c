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

#include <string.h>
#include <stddef.h>
#include "hwinit.h"
#include "hwinit/gfx.h"
#include "fs.h"
#include "fuse.h"
#include "package.h"
#include "error.h"
#include "firmware.h"

#define VERSION "v0.1"

static pk11_offs *pk11Offs = NULL;

// TODO: Maybe find these with memsearch
static const pk11_offs _pk11_offs[] = {
    //{ "20161121183008", 0, 0x1900, 0x3FE0, { 2, 1, 0 }, 0x4002B020, 0x8000D000, 1 }, //TODO: relocator patch for 1.0.0
    { "20161121183008", 0, 0x1900, 0x3FE0, { 2, 1, 0 }, 0x40014020, 0x8000D000, 1 }, //1.0.0
    { "20170210155124", 0, 0x1900, 0x3FE0, { 0, 1, 2 }, 0x4002D000, 0x8000D000, 1 }, //2.0.0 - 2.3.0
    { "20170519101410", 1, 0x1A00, 0x3FE0, { 0, 1, 2 }, 0x4002D000, 0x8000D000, 1 }, //3.0.0
    { "20170710161758", 2, 0x1A00, 0x3FE0, { 0, 1, 2 }, 0x4002D000, 0x8000D000, 1 }, //3.0.1 - 3.0.2
    { "20170921172629", 3, 0x1800, 0x3FE0, { 1, 2, 0 }, 0x4002B000, 0x4003B000, 0 }, //4.0.0 - 4.1.0
    { "20180220163747", 4, 0x1900, 0x3FE0, { 1, 2, 0 }, 0x4002B000, 0x4003B000, 0 }, //5.0.0 - 5.0.2
    { NULL, 0, 0, 0, 0 } // End.
};

static void SE_lock() {
    for (u32 i = 0; i < 16; i++)
        se_key_acc_ctrl(i, 0x15);

    for (u32 i = 0; i < 2; i++)
        se_rsa_acc_ctrl(i, 1);

    SE(0x4) = 0; // Make this reg secure only.
    SE(SE_KEY_TABLE_ACCESS_LOCK_OFFSET) = 0; // Make all key access regs secure only.
    SE(SE_RSA_KEYTABLE_ACCESS_LOCK_OFFSET) = 0; // Make all rsa access regs secure only.
    SE(SE_SECURITY_0) &= 0xFFFFFFFB; // Make access lock regs secure only.
}

void drawSplash() {
    // Draw splashscreen to framebuffer.
    if(fopen("/ReiNX/splash.bin", "rb") != 0) {
        fread((void*)0xC0000000, fsize(), 1);
        fclose();
    }
    usleep(3000000);
}

pk11_offs *pkg11_offsentify(u8 *pkg1) {
    for (u32 i = 0; _pk11_offs[i].id; i++)
        if (!memcmp(pkg1 + 0x10, _pk11_offs[i].id, 12))
            return (pk11_offs *)&_pk11_offs[i];
    return NULL;
}

void patch(pk11_offs *pk11, pkg2_hdr_t *pkg2, link_t *kips) {
    //Secmon patches
    if(!customSecmon){
        uPtr *rlc_ptr = NULL;
        uPtr *ver_ptr = NULL;
        uPtr *pk21_ptr = NULL;
        uPtr *hdrsig_ptr = NULL;
        uPtr *sha2_ptr = NULL;
        switch(pk11->kb) {
            case KB_FIRMWARE_VERSION_100_200: {
                //u8 rlcPattern[] = {0xE0, 0xFF, 0x1D, 0xF0, 0x00, 0x00, 0x00, 0x91}; //TODO: relocator patch for 1.0.0
                u8 verPattern[] = {0x19, 0x00, 0x36, 0xE0, 0x03, 0x08, 0x91};
                u8 hdrSigPattern[] = {0xFF, 0x97, 0xC0, 0x00, 0x00, 0x34, 0xA1, 0xFF, 0xFF};
                u8 sha2Pattern[] = {0xE0, 0x03, 0x08, 0x91, 0xE1, 0x03, 0x13, 0xAA};

                ver_ptr = (uPtr*)(memsearch((void *)pk11->secmon_base, 0x10000, verPattern, sizeof(verPattern)) + 0xB);
                hdrsig_ptr = (uPtr*)(memsearch((void *)pk11->secmon_base, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x3A);
                sha2_ptr = (uPtr*)(memsearch((void *)pk11->secmon_base, 0x10000, sha2Pattern, sizeof(sha2Pattern)) + 0x10);
                break;
            }
            case KB_FIRMWARE_VERSION_300:
            case KB_FIRMWARE_VERSION_301: {
                u8 verPattern[] = {0x2B, 0xFF, 0xFF, 0x97, 0x40, 0x19, 0x00, 0x36};
                u8 hdrSigPattern[] = {0xF7, 0xFE, 0xFF, 0x97, 0x80, 0x1E, 0x00, 0x36};
                u8 sha2Pattern[] = {0x07, 0xFF, 0xFF, 0x97, 0xC0, 0x18, 0x00, 0x36};
                u8 pk21Pattern[] = {0x40, 0x19, 0x00, 0x36, 0xE0, 0x03, 0x08, 0x91};

                ver_ptr = (uPtr*)(memsearch((void *)pk11->secmon_base, 0x10000, verPattern, sizeof(verPattern)) + 0x4);
                pk21_ptr = (uPtr*)memsearch((void *)pk11->secmon_base, 0x10000, pk21Pattern, sizeof(pk21Pattern));
                hdrsig_ptr = (uPtr*)(memsearch((void *)pk11->secmon_base, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x4);
                sha2_ptr = (uPtr*)(memsearch((void *)pk11->secmon_base, 0x10000, sha2Pattern, sizeof(sha2Pattern)) + 0x4);
                break;
            }
            case KB_FIRMWARE_VERSION_400: {
                u8 verPattern[] = {0x00, 0x01, 0x00, 0x36, 0xFD, 0x7B, 0x41, 0xA9};
                u8 hdrSigPattern[] = {0xE0, 0x03, 0x13, 0xAA, 0x4B, 0x28, 0x00, 0x94};
                u8 sha2Pattern[] = {0xD3, 0xD5, 0xFF, 0x97, 0xE0, 0x03, 0x01, 0x32};
                u8 pk21Pattern[] = {0xE0, 0x00, 0x00, 0x36, 0xE0, 0x03, 0x13, 0xAA, 0x63};

                ver_ptr = (uPtr*)memsearch((void *)pk11->secmon_base, 0x10000, verPattern, sizeof(verPattern));
                pk21_ptr = (uPtr*)memsearch((void *)pk11->secmon_base, 0x10000, pk21Pattern, sizeof(pk21Pattern));
                hdrsig_ptr = (uPtr*)(memsearch((void *)pk11->secmon_base, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x8);
                sha2_ptr = (uPtr*)memsearch((void *)pk11->secmon_base, 0x10000, sha2Pattern, sizeof(sha2Pattern));
                break;
            }
            default: {
                u8 verPattern[] = {0x00, 0x01, 0x00, 0x36, 0xFD, 0x7B, 0x41, 0xA9};
                u8 hdrSigPattern[] = {0x86, 0xFE, 0xFF, 0x97, 0x80, 0x00, 0x00, 0x36};
                u8 sha2Pattern[] = {0xF2, 0xFB, 0xFF, 0x97, 0xE0, 0x03};

                ver_ptr = (uPtr*)memsearch((void *)pk11->secmon_base, 0x10000, verPattern, sizeof(verPattern));
                pk21_ptr = (uPtr*)((u32)ver_ptr - 0xC);
                hdrsig_ptr = (uPtr*)(memsearch((void *)pk11->secmon_base, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x4);
                sha2_ptr = (uPtr*)memsearch((void *)pk11->secmon_base, 0x10000, sha2Pattern, sizeof(sha2Pattern));
                break;
            }
        }
        /*if (pre2x) { //TODO: relocator patch for 1.0.0
            *rlc_ptr = ADRP(0, 0x3BFE8020);
        };*/
        if (pk11->kb != KB_FIRMWARE_VERSION_100_200) {
            *pk21_ptr = NOP;
        };
        *ver_ptr = NOP;
        *hdrsig_ptr = NOP;
        *sha2_ptr = NOP;
    }
    if(!customKern) {
        //TODO
    }
    LIST_FOREACH_ENTRY(pkg2_kip1_info_t, ki, kips, link) {        
        //Patch FS
        if(ki->kip1->tid == 0x0100000000000000) {
            print("Patching FS\n");
            //TODO
        }
    }
}

int keygen(u8 *keyblob, u32 fwVer, void *tsec_fw) {
    u8 tmp[0x10];

    se_key_acc_ctrl(0x0D, 0x15);
    se_key_acc_ctrl(0x0E, 0x15);

    // Get TSEC key.
    if (tsec_query(tmp, 1, tsec_fw) < 0)
        return 0;

    se_aes_key_set(0x0D, tmp, 0x10);

    // Derive keyblob keys from TSEC+SBK.
    se_aes_crypt_block_ecb(0x0D, 0x00, tmp, keyblob_keyseeds[0]);
    se_aes_unwrap_key(0x0F, 0x0E, tmp);
    se_aes_crypt_block_ecb(0xD, 0x00, tmp, keyblob_keyseeds[fwVer]);
    se_aes_unwrap_key(0x0D, 0x0E, tmp);

    // Clear SBK
    se_aes_key_clear(0x0E);

    se_aes_crypt_block_ecb(0x0D, 0, tmp, cmac_keyseed);
    se_aes_unwrap_key(0x0B, 0x0D, cmac_keyseed);

    // Decrypt keyblob and set keyslots.
    se_aes_crypt_ctr(0x0D, keyblob + 0x20, 0x90, keyblob + 0x20, 0x90, keyblob + 0x10);
    se_aes_key_set(0x0B, keyblob + 0x20 + 0x80, 0x10); // Package1 key
    se_aes_key_set(0x0C, keyblob + 0x20, 0x10);
    se_aes_key_set(0x0D, keyblob + 0x20, 0x10);

    se_aes_crypt_block_ecb(0x0C, 0, tmp, master_keyseed_retail);

    switch (fwVer) {
        case KB_FIRMWARE_VERSION_100_200:
        case KB_FIRMWARE_VERSION_300:
        case KB_FIRMWARE_VERSION_301:
            se_aes_unwrap_key(0x0D, 0x0F, console_keyseed);
            se_aes_unwrap_key(0x0C, 0x0C, master_keyseed_retail);
        break;

        case KB_FIRMWARE_VERSION_400:
            se_aes_unwrap_key(0x0D, 0x0F, console_keyseed_4xx);
            se_aes_unwrap_key(0x0F, 0x0F, console_keyseed);
            se_aes_unwrap_key(0x0E, 0x0C, master_keyseed_4xx);
            se_aes_unwrap_key(0x0C, 0x0C, master_keyseed_retail);
        break;

        case KB_FIRMWARE_VERSION_500:
        default:
            se_aes_unwrap_key(0x0A, 0x0F, console_keyseed_4xx);
            se_aes_unwrap_key(0x0F, 0x0F, console_keyseed);
            se_aes_unwrap_key(0x0E, 0x0C, master_keyseed_4xx);
            se_aes_unwrap_key(0x0C, 0x0C, master_keyseed_retail);
        break;
    }

    // Package2 key
    se_key_acc_ctrl(0x08, 0x15);
    se_aes_unwrap_key(0x08, 0x0C, key8_keyseed);
}

u8 loadFirm() {
    sdmmc_storage_t storage;
    sdmmc_t sdmmc;
    u32 ret = 0;

    sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4);
    sdmmc_storage_set_mmc_partition(&storage, 1);

    // Read package1.
    print("Reading Package1...\n");
    u8 *package1 = (u8 *)malloc(0x40000);
    sdmmc_storage_read(&storage, 0x100000 / NX_EMMC_BLOCKSIZE, 0x40000 / NX_EMMC_BLOCKSIZE, package1);

    // Setup firmware specific data.
    pk11Offs = pkg11_offsentify(package1);
    u8 *keyblob = (u8 *)malloc(NX_EMMC_BLOCKSIZE);
    sdmmc_storage_read(&storage, 0x180000 / NX_EMMC_BLOCKSIZE + pk11Offs->kb, 1, keyblob);
    keygen(keyblob, pk11Offs->kb, package1 + pk11Offs->tsec_off);
    free(keyblob);

    // Decrypt package1 and setup warmboot.
    print("Decrypting Package1...\n");
    u8 *pkg11 = package1 + pk11Offs->pkg11_off;
    u32 pkg11_size = *(u32 *)pkg11;
    se_aes_crypt_ctr(11, pkg11 + 0x20, pkg11_size, pkg11 + 0x20, pkg11_size, pkg11 + 0x10);
    pkg1_unpack(pk11Offs, package1);
    PMC(APBDEV_PMC_SCRATCH1) = pk11Offs->warmboot_base;
    free(package1);

    // Read GPT partition.
    LIST_INIT(gpt);
    sdmmc_storage_set_mmc_partition(&storage, 0);
    print("Parsing GPT...\n");
    nx_emmc_gpt_parse(&gpt, &storage);
    emmc_part_t *pkg2_part = nx_emmc_part_find(&gpt, "BCPKG2-1-Normal-Main");
    nx_emmc_gpt_free(&gpt);
    if (!pkg2_part) {
        error("Failed to read GPT!\n");
        return 1;
    }

    // Read Package2.
    u8 *tmp = (u8 *)malloc(NX_EMMC_BLOCKSIZE);
    print("Reading Package2 size...\n");
    nx_emmc_part_read(&storage, pkg2_part, 0x4000 / NX_EMMC_BLOCKSIZE, 1, tmp);
    u32 *hdr = (u32 *)(tmp + 0x100);
    u32 pkg2_size = hdr[0] ^ hdr[2] ^ hdr[3];
    free(tmp);
    u8 *pkg2 = malloc(ALIGN(pkg2_size, NX_EMMC_BLOCKSIZE));
    print("Reading Package2...\n");
    ret = nx_emmc_part_read(&storage, pkg2_part, 0x4000 / NX_EMMC_BLOCKSIZE, ALIGN(pkg2_size, NX_EMMC_BLOCKSIZE) / NX_EMMC_BLOCKSIZE, pkg2);
    sdmmc_storage_end(&storage);
    if (!ret) {
        error("Failed to read Package2!\n");
        return 1;
    }

    // Unpack Package2.
    print("Unpacking package2...\n");
    pkg2_hdr_t *dec_pkg2 = unpackFirmwarePackage(pkg2);
    LIST_INIT(kip1_info);
    pkg2_parse_kips(&kip1_info, dec_pkg2);

    // Patch firmware.
    print("Patching OS...\n");
    patch(pk11Offs, dec_pkg2, &kip1_info);

    // Load all KIPs.
    char **sysmods = NULL;
    size_t cnt = enumerateDir(&sysmods, "/ReiNX/sysmodules", "*.kip");
    for (u32 i = 0; i < cnt ; i++) {
        print("%kLoading %s\n%k", YELLOW, sysmods[i], ORANGE);
        loadKip(&kip1_info, sysmods[i]);
        free(sysmods[i]);
    }
    free(sysmods);

    // Build Package2.
    buildFirmwarePackage(dec_pkg2->data, dec_pkg2->sec_size[PKG2_SEC_KERNEL], &kip1_info);
}

void launch() {
    u8 pre4x = pk11Offs->kb < KB_FIRMWARE_VERSION_400;

    se_aes_key_clear(0x8);
    se_aes_key_clear(0xB);

    if (pre4x) {
        if (pk11Offs->kb == KB_FIRMWARE_VERSION_300)
            PMC(APBDEV_PMC_SECURE_SCRATCH32) = 0xE3;
        else if (pk11Offs->kb == KB_FIRMWARE_VERSION_301)
            PMC(APBDEV_PMC_SECURE_SCRATCH32) = 0x104;
        se_key_acc_ctrl(12, 0xFF);
        se_key_acc_ctrl(13, 0xFF);
    } else {
        se_key_acc_ctrl(12, 0xFF);
        se_key_acc_ctrl(15, 0xFF);
    }

    // TODO: Don't Clear 'BootConfig' for retail >1.0.0.
    //memset((void *)0x4003D000, 0, 0x3000);

    SE_lock();

    // Start boot process now that pk21 is loaded.
    *BOOT_STATE_ADDR = (pre4x ? BOOT_PKG2_LOADED : BOOT_PKG2_LOADED_4X);

    // Boot secmon and Wait for it get ready.
    cluster_boot_cpu0(pk11Offs->secmon_base);
    while (!*SECMON_STATE_ADDR)
        usleep(1);

    // Disable display.
    if (pre4x)
        display_end();

    // Signal to finish boot process.
    *BOOT_STATE_ADDR = (pre4x ? BOOT_DONE : BOOT_DONE_4X);;

    // Halt ourselves in waitevent state.
    while (1) FLOW_CTLR(0x4) = 0x50000000;
}

void firmware() {
    display_init();
    gfx_init_ctxt(&gfx_ctxt, display_init_framebuffer(), 720, 1280, 768);
    gfx_clear_color(&gfx_ctxt, 0xFF000000);
    gfx_con_init(&gfx_con, &gfx_ctxt);
    gfx_con_setcol(&gfx_con, ORANGE, 0, 0);

    if (!sd_mount()) {
        error("Failed to init SD card!\n");
        return;
    }

    print("Welcome to ReiNX %s!\n", VERSION);
    loadFirm();
    drawSplash();
    launch();
}
