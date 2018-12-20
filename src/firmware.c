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
#include "fs.h"
#include "package.h"
#include "error.h"
#include "bootloader.h"
#include "firmware.h"

static pk11_offs *pk11Offs = NULL;
static u8 *bctBuf = NULL;

int drawSplash() {
    // Draw splashscreen to framebuffer.
    if(fopen("/ReiNX/splash.bin", "rb") != 0) {
        fread((void*)gfx_ctxt.fb, fsize(), 1);
        fclose();
        return 1;
    }
    return 0;
}

void patchFS(pkg2_kip1_info_t* ki) {
    print("Patching FS\n");

    u8 kipHash[0x20];

    se_calc_sha256(&kipHash, ki->kip1, ki->size);
    se_calc_sha256(&kipHash, ki->kip1, ki->size);

    //Create header
    size_t sizeDiff = ki->kip1->sections[0].size_decomp - ki->kip1->sections[0].size_comp;

    size_t newSize = ki->size + sizeDiff;
    pkg2_kip1_t *moddedKip = malloc(newSize);
    memcpy(moddedKip, ki->kip1, newSize);

    u32 pos = 0;
    //Get decomp .text segment
    u8 *kipDecompText = blz_decompress(moddedKip->data, moddedKip->sections[0].size_comp);

    kippatchset_t *pset = kippatch_find_set(kipHash, kip_patches);
    if (!pset) {
        print("could not find patchset with matching hash\n");
    } else {
        int res = kippatch_apply_set(kipDecompText, moddedKip->sections[0].size_decomp, pset);
        if (res) error("kippatch_apply_set() failed\n");
    }

    moddedKip->flags &= ~1;
    memcpy((void*)moddedKip->data, kipDecompText, moddedKip->sections[0].size_decomp);
    free(kipDecompText);
    pos += moddedKip->sections[0].size_comp;
    moddedKip->sections[0].size_comp = moddedKip->sections[0].size_decomp;

    for(int i = 1; i < KIP1_NUM_SECTIONS; i++) {
        if(moddedKip->sections[i].offset != 0) {
            memcpy((void*)moddedKip->data + pos + sizeDiff, (void*)ki->kip1->data + pos, moddedKip->sections[i].size_comp);
            pos += moddedKip->sections[i].size_comp;
        }
    }

    free(ki->kip1);
    ki->size = newSize;
    ki->kip1 = moddedKip;
}

pkg2_kip1_info_t* find_by_tid(link_t* kip_list, u64 tid) {
    LIST_FOREACH_ENTRY(pkg2_kip1_info_t, ki, kip_list, link) {
        if(ki->kip1->tid == 0x0100000000000000)
            return ki;
    }
    return NULL;
}

void patchWarmboot(u32 warmbootBase) {
    print("Patching Warmboot...\n");
    if(!customWarmboot) {
        uPtr *fuseCheck = NULL;
        uPtr *segmentID = NULL;
        u8 fuseCheckPat[] = {0x44, 0x12, 0x80, 0xE5};
        u8 segmentIDPat[] = {0x60, 0x03, 0x91, 0xE5};
        fuseCheck = (uPtr*)(memsearch((void *)warmbootBase, 0x1000, fuseCheckPat, sizeof(fuseCheckPat)) + 20);
        segmentID = (uPtr*)(memsearch((void *)warmbootBase, 0x1000, segmentIDPat, sizeof(segmentIDPat)) + 12);
        
        *fuseCheck = NOP_v7;
        if(segmentID != NULL)
            *segmentID = NOP_v7;
    }
}

void patchSecmon(u32 secmonBase, u32 fw){
    print("Patching Secmon...\n");
    //Patch Secmon
    if(!customSecmon){
        uPtr *rlc_ptr = NULL;
        uPtr *ver_ptr = NULL;
        uPtr *pk21_ptr = NULL;
        uPtr *hdrsig_ptr = NULL;
        uPtr *sha2_ptr = NULL;
        
        //Version
        switch(fw) {
            //case KB_FIRMWARE_VERSION_100:
            case KB_FIRMWARE_VERSION_200: {
                u8 verPattern[] = {0x19, 0x00, 0x36, 0xE0, 0x03, 0x08, 0x91};
                ver_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, verPattern, sizeof(verPattern)) + 0xB);
                break;
            }
            case KB_FIRMWARE_VERSION_300:
            case KB_FIRMWARE_VERSION_301: {
                u8 verPattern[] = {0x2B, 0xFF, 0xFF, 0x97, 0x40, 0x19, 0x00, 0x36};
                ver_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, verPattern, sizeof(verPattern)) + 0x4);
                break;
            }
            case KB_FIRMWARE_VERSION_400: {
                u8 verPattern[] = {0x00, 0x01, 0x00, 0x36, 0xFD, 0x7B, 0x41, 0xA9};
                ver_ptr = (uPtr*)memsearch((void *)secmonBase, 0x10000, verPattern, sizeof(verPattern));
                break;
            }
            default:{
                u8 verPattern[] = {0x00, 0x01, 0x00, 0x36, 0xFD, 0x7B, 0x41, 0xA9};
                ver_ptr = (uPtr*)memsearch((void *)secmonBase, 0x10000, verPattern, sizeof(verPattern));
                break;
            }
        }
        
        //header sig
        switch(fw) {
            //case KB_FIRMWARE_VERSION_100:
            case KB_FIRMWARE_VERSION_200: {
                u8 hdrSigPattern[] = {0xFF, 0x97, 0xC0, 0x00, 0x00, 0x34, 0xA1, 0xFF, 0xFF};
                hdrsig_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x3A);
                break;
            }
            case KB_FIRMWARE_VERSION_300:
            case KB_FIRMWARE_VERSION_301: {
                u8 hdrSigPattern[] = {0xF7, 0xFE, 0xFF, 0x97, 0x80, 0x1E, 0x00, 0x36};
                hdrsig_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x4);
                break;
            }
            case KB_FIRMWARE_VERSION_400: {
                u8 hdrSigPattern[] = {0xE0, 0x03, 0x13, 0xAA, 0x4B, 0x28, 0x00, 0x94};
                hdrsig_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x8);
                break;
            }
            case KB_FIRMWARE_VERSION_500: {
                u8 hdrSigPattern[] = {0x86, 0xFE, 0xFF, 0x97, 0x80, 0x00, 0x00, 0x36};
                hdrsig_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x4);
                break;
            }
            default:{
                u8 hdrSigPattern[] = {0x9A, 0xFF, 0xFF, 0x97, 0x80, 0x00, 0x00, 0x36};
                u8 sha2Pattern[] = {0x81, 0x00, 0x80, 0x72, 0x3C, 0xFC, 0xFF, 0x97};
                hdrsig_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x4);
                break;
            }
        }
        
        //Sha2
        switch(fw) {
            //case KB_FIRMWARE_VERSION_100:
            case KB_FIRMWARE_VERSION_200: {
                u8 sha2Pattern[] = {0xE0, 0x03, 0x08, 0x91, 0xE1, 0x03, 0x13, 0xAA};
                sha2_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, sha2Pattern, sizeof(sha2Pattern)) + 0x10);
                break;
            }
            case KB_FIRMWARE_VERSION_300:
            case KB_FIRMWARE_VERSION_301: {
                u8 sha2Pattern[] = {0x07, 0xFF, 0xFF, 0x97, 0xC0, 0x18, 0x00, 0x36};
                sha2_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, sha2Pattern, sizeof(sha2Pattern)) + 0x4);
                break;
            }
            case KB_FIRMWARE_VERSION_400: {
                u8 sha2Pattern[] = {0xD3, 0xD5, 0xFF, 0x97, 0xE0, 0x03, 0x01, 0x32};
                sha2_ptr = (uPtr*)memsearch((void *)secmonBase, 0x10000, sha2Pattern, sizeof(sha2Pattern));
                break;
            }
            case KB_FIRMWARE_VERSION_500: {
                u8 sha2Pattern[] = {0xF2, 0xFB, 0xFF, 0x97, 0xE0, 0x03};
                sha2_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, sha2Pattern, sizeof(sha2Pattern)));
                break;
            }
            default:{
                u8 sha2Pattern[] = {0x81, 0x00, 0x80, 0x72, 0x3C, 0xFC, 0xFF, 0x97};
                sha2_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, sha2Pattern, sizeof(sha2Pattern)) + 0x4);
                break;
            }
        }
        
        //Pkg2
        switch(fw) {
            //case KB_FIRMWARE_VERSION_100:
            case KB_FIRMWARE_VERSION_200: break;
            case KB_FIRMWARE_VERSION_300:
            case KB_FIRMWARE_VERSION_301: {
                u8 pk21Pattern[] = {0x40, 0x19, 0x00, 0x36, 0xE0, 0x03, 0x08, 0x91};
                pk21_ptr = (uPtr*)memsearch((void *)secmonBase, 0x10000, pk21Pattern, sizeof(pk21Pattern));
                break;
            }
            case KB_FIRMWARE_VERSION_400: {
                u8 pk21Pattern[] = {0xE0, 0x00, 0x00, 0x36, 0xE0, 0x03, 0x13, 0xAA, 0x63};
                pk21_ptr = (uPtr*)memsearch((void *)secmonBase, 0x10000, pk21Pattern, sizeof(pk21Pattern));
                break;
            }
            default:{
                pk21_ptr = (uPtr*)((u32)ver_ptr - 0xC);
                break;
            }
        }

        if (fw > KB_FIRMWARE_VERSION_200) {
            *pk21_ptr = NOP_v8;
        };
        *ver_ptr = NOP_v8;
        *hdrsig_ptr = NOP_v8;
        *sha2_ptr = NOP_v8;
    }
}

void patchKernel(pkg2_hdr_t *pkg2){
    print("Patching Kernel...\n");
    //Patch Kernel
    if(!customKern) {
        u32 crc = crc32c(pkg2->data, pkg2->sec_size[PKG2_SEC_KERNEL]);
        uPtr kern = (uPtr)&pkg2->data;
        uPtr sendOff, recvOff, codeRcvOff, codeSndOff, svcVerifOff, svcDebugOff, ver;
        switch(crc){
            case 0x427f2647:{   //1.0.0
                svcVerifOff = 0x3764C;
                svcDebugOff = 0x44074;
                sendOff = 0x23CC0;
                recvOff = 0x219F0;
                codeSndOff = 4;
                codeRcvOff = 4;
                ver = 0;
                break;
            }
            case 0xae19cf1b:{   //2.0.0
                svcVerifOff = 0x54834;
                svcDebugOff = 0x6086C;
                sendOff = 0x3F134;
                recvOff = 0x3D1A8;
                codeSndOff = 4;
                codeRcvOff = 4;
                ver = 1;
                break;
            }
            case 0x73c9e274:{   //3.0.0
                svcVerifOff = 0x3BD24;
                svcDebugOff = 0x483FC;
                sendOff = 0x26080;
                recvOff = 0x240F0;
                codeSndOff = 4;
                codeRcvOff = 4;
                ver = 2;
                break;
            }
            case 0xe0e8cdc4:{   //3.0.2
                svcVerifOff = 0x3BD24;
                svcDebugOff = 0x48414;
                sendOff = 0x26080;
                recvOff = 0x240F0;
                codeSndOff = 4;
                codeRcvOff = 4;
                ver = 3;
                break;
            }
            case 0x485d0157:{   //4.0.0
                svcVerifOff = 0x41EB4;
                svcDebugOff = 0x4EBFC;
                sendOff = 0x2AF64;
                recvOff = 0x28F6C;
                codeSndOff = 8;
                codeRcvOff = 4;
                ver = 4;
                break;
            }
            case 0xf3c363f2:{   //5.0.0
                svcVerifOff = 0x45E6C;
                svcDebugOff = 0x5513C;
                sendOff = 0x2AD34;
                recvOff = 0x28DAC;
                codeSndOff = 8;
                codeRcvOff = 8;
                ver = 5;
                break;
            }
            case 0x64ce1a44:{   //6.0.0
                svcVerifOff = 0x47EA0;
                svcDebugOff = 0x57548;
                sendOff = 0x2BB8C;
                recvOff = 0x29B6C;
                codeSndOff = 0x10;
                codeRcvOff = 0x10;
                ver = 6;
                break;
            }
            default:
                error("Kernel not supported");
                goto end;
        }

        //ID Send
        uPtr freeSpace = getFreeSpace((void*)(kern+0x45000), 0x200, 0x20000) + 0x45000;                //Find area to write payload
        print("Kernel Freespace: 0x%08X\n", freeSpace);
        size_t payloadSize;
        u32 *sndPayload = getSndPayload(ver, &payloadSize);
        *(vu32*)(kern + sendOff) = _B(sendOff, freeSpace);                                             //write hook to payload
        memcpy((void*)(kern + freeSpace), sndPayload, payloadSize);                                    //Copy payload to free space
        *(vu32*)(kern + freeSpace + payloadSize) = _B(freeSpace + payloadSize, sendOff + codeSndOff);  //Jump back skipping the hook

        //ID Receive
        freeSpace += (payloadSize+4);
        u32 *rcvPayload = getRcvPayload(ver, &payloadSize);
        *(vu32*)(kern + recvOff) = _B(recvOff, freeSpace);
        memcpy((void*)(kern + freeSpace), rcvPayload, payloadSize);
        *(vu32*)(kern + freeSpace + payloadSize) = _B(freeSpace + payloadSize, recvOff + codeRcvOff);

        //SVC patches
        *(vu32*)(kern + svcVerifOff) = NOP_v8;
        if (fopen("/ReiNX/debug", "rb")) {
            fclose();
            *(vu32*)(kern + svcDebugOff) = _MOVZX(8, 1, 0);
        }

        end:;
    }
}

void patchKernelExtensions(link_t *kips){
    print("Patching KIPs...\n");
    pkg2_kip1_info_t* FS_module = find_by_tid(kips, 0x0100000000000000);
    if(FS_module == NULL) {
        error("Could not find FS Module.\n");
    } else {
        patchFS(FS_module);
    }

    // Load all KIPs.
    char **sysmods = NULL;
    size_t cnt = enumerateDir(&sysmods, "/ReiNX/sysmodules", "*.kip");
    for (u32 i = 0; i < cnt ; i++) {
        print("%kLoading %s\n%k", YELLOW, sysmods[i], DEFAULT_TEXT_COL);
        loadKip(kips, sysmods[i]);
        free(sysmods[i]);
    }
    free(sysmods);
}

u8 loadFirm() {
    sdmmc_storage_t storage;
    sdmmc_t sdmmc;

    //Init nand
    sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4);
    sdmmc_storage_set_mmc_partition(&storage, 1);

    //Read Boot0
    bctBuf = ReadBoot0(&storage);
    u32 ver = (*(u32*)(bctBuf+0x2330)) - 1;
    u32 pkg11_size = *(u32 *)(bctBuf + 0x233C);
    for (u32 i = 0; _pk11_offs[i].pkg11_off; i++) {   //TODO distinguish 1.x & 2.x
        if(_pk11_offs[i].kb == ver){
            pk11Offs = (pk11_offs *)&_pk11_offs[i];
            break;
        }
    }
    print("Bootloader version: %d\n", pk11Offs->kb);
    free(bctBuf);

    // Read package1.
    u8 *pkg1ldr = ReadPackage1Ldr(&storage);

    // Decrypt package1 and setup warmboot.
    print("Decrypting Package1...\n");
    u8 *pkg11 = pkg1ldr + pk11Offs->pkg11_off;

    // Setup firmware specific data.
    u8 *keyblob = (u8 *)malloc(NX_EMMC_BLOCKSIZE);
    sdmmc_storage_read(&storage, 0x180000 / NX_EMMC_BLOCKSIZE + pk11Offs->kb, 1, keyblob);
    if(!keygen(keyblob, pk11Offs->kb, pkg1ldr, pk11Offs))
      print("Failed to keygen...\n");
    free(keyblob);
    //Decrypt if needed
    if(pk11Offs->kb < KB_FIRMWARE_VERSION_620)
      se_aes_crypt_ctr(11, pkg11 + 0x20, pkg11_size, pkg11 + 0x20, pkg11_size, pkg11 + 0x10);

    print("Unpacking pkg1\n");
    pkg1_unpack(pk11Offs, (u32)pkg11);
    PMC(APBDEV_PMC_SCRATCH1) = pk11Offs->warmboot_base;
    free(pkg1ldr);

    //Read package2
    u8 *pkg2 = ReadPackage2(&storage);

    // Unpack Package2.
    print("Unpacking package2...\n");
    pkg2_hdr_t *dec_pkg2 = unpackFirmwarePackage(pkg2);
    LIST_INIT(kip1_info);
    pkg2_parse_kips(&kip1_info, dec_pkg2);

    // Patch firmware.
    patchWarmboot(pk11Offs->warmboot_base);
    patchSecmon(pk11Offs->secmon_base, pk11Offs->kb);
    patchKernel(dec_pkg2);
    patchKernelExtensions(&kip1_info);

    // Build Package2.
    buildFirmwarePackage(dec_pkg2->data, dec_pkg2->sec_size[PKG2_SEC_KERNEL], &kip1_info);

    //We're done with SD now
    sdUnmount();
}

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

void launch() {
    se_aes_key_clear(0x8);
    se_aes_key_clear(0xB);

    switch(pk11Offs->kb){
        case KB_FIRMWARE_VERSION_200:
        case KB_FIRMWARE_VERSION_300:
            PMC(APBDEV_PMC_SECURE_SCRATCH32) = 0xE3;
            se_key_acc_ctrl(12, 0xFF);
            se_key_acc_ctrl(13, 0xFF);
            break;
        case KB_FIRMWARE_VERSION_301:
            PMC(APBDEV_PMC_SECURE_SCRATCH32) = 0x104;
            se_key_acc_ctrl(12, 0xFF);
            se_key_acc_ctrl(13, 0xFF);
            break;
        case KB_FIRMWARE_VERSION_400:
        case KB_FIRMWARE_VERSION_500:
        case KB_FIRMWARE_VERSION_600:
            se_key_acc_ctrl(12, 0xFF);
            se_key_acc_ctrl(15, 0xFF);
    }

    if(pk11Offs->kb < KB_FIRMWARE_VERSION_620){
        SE_lock();
    }else{
        memset((void *)IPATCH_BASE, 0, 13);
        SB(SB_CSR) = 0x10; // Protected IROM enable.
        SYSCTR0(SYSCTR0_CNTFID0) = 19200000;
        SYSCTR0(SYSCTR0_CNTCR) = 0;
        SYSCTR0(SYSCTR0_COUNTERID0) = 0;
        SYSCTR0(SYSCTR0_COUNTERID1) = 0;
        SYSCTR0(SYSCTR0_COUNTERID2) = 0;
        SYSCTR0(SYSCTR0_COUNTERID3) = 0;
        SYSCTR0(SYSCTR0_COUNTERID4) = 0;
        SYSCTR0(SYSCTR0_COUNTERID5) = 0;
        SYSCTR0(SYSCTR0_COUNTERID6) = 0;
        SYSCTR0(SYSCTR0_COUNTERID7) = 0;
        SYSCTR0(SYSCTR0_COUNTERID8) = 0;
        SYSCTR0(SYSCTR0_COUNTERID9) = 0;
        SYSCTR0(SYSCTR0_COUNTERID10) = 0;
        SYSCTR0(SYSCTR0_COUNTERID11) = 0;
    }

    // Start boot process now that pk21 is loaded.
    *BOOT_STATE_ADDR = (pk11Offs->kb < KB_FIRMWARE_VERSION_400 ? BOOT_PKG2_LOADED : BOOT_PKG2_LOADED_4X);
    *SECMON_STATE_ADDR = 0;

    // Disable display.
    display_end();

    // Boot secmon and Wait for it get ready, if aplicable.
    if (smmu_is_used())
        smmu_exit();
    else
        cluster_boot_cpu0(pk11Offs->secmon_base);

    while (!*SECMON_STATE_ADDR)
        usleep(1);

    // Signal to finish boot process.
    *BOOT_STATE_ADDR = (pk11Offs->kb < KB_FIRMWARE_VERSION_400 ? BOOT_DONE : BOOT_DONE_4X);

    // Halt ourselves in waitevent state.
    while (1) FLOW_CTLR(0x4) = 0x50000000;
}

void firmware() {
    display_init();
    gfx_init_ctxt(&gfx_ctxt, display_init_framebuffer(), 1280, 720, 768);
    gfx_clear_color(&gfx_ctxt, 0xFF000000);
    gfx_con_init(&gfx_con, &gfx_ctxt);
    gfx_con_setcol(&gfx_con, DEFAULT_TEXT_COL, 0, 0);
    
    if (!sdMount()) {
        error("Failed to init SD card!\n");
        print("Press POWER to power off, or any other key to continue without SD.\n");
        if (btn_wait() & BTN_POWER)
            i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_ONOFFCNFG1, MAX77620_ONOFFCNFG1_PWR_OFF);
        btn_wait();
    }
    
    if(PMC(APBDEV_PMC_SCRATCH49) != 69 && fopen("/ReiNX.bin", "rb")) {
        fread((void*)PAYLOAD_ADDR, fsize(), 1);
        fclose();
        sdUnmount();
        display_end();
        CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_V) |= 0x400; // Enable AHUB clock.
        CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_Y) |= 0x40;  // Enable APE clock.
        PMC(APBDEV_PMC_SCRATCH49) = 69;
        ((void (*)())PAYLOAD_ADDR)();
    }
    SYSREG(AHB_AHB_SPARE_REG) = (volatile vu32)0xFFFFFF9F;
    PMC(APBDEV_PMC_SCRATCH49) = 0;
    
    if(btn_read() & BTN_VOL_UP){
        if(fopen("/ReiNX/Recovery.bin", "rb") != 0) {
            fread((void*)PAYLOAD_ADDR, fsize(), 1);
            fclose();
            sdUnmount();
            CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_V) |= 0x400; // Enable AHUB clock.
            CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_Y) |= 0x40;  // Enable APE clock.
            ((void (*)())PAYLOAD_ADDR)();
        } else {
            error("Failed to launch recovery menu!\nIs it missing from /ReiNX folder?\n");
            btn_wait();
        }
    }

    if (btn_read() & BTN_VOL_DOWN) {
        print("Booting verbosely\n");
    } else if (drawSplash()) {
        gfx_con.mute = 1;
    }

    print("Welcome to ReiNX %s!\n", VERSION);
    loadFirm();
    launch();
}
