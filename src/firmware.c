/*
* Copyright (c) 2018 Reisyukaku
* Copyright (c) 2019 Elise
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

#include "firmware.h"


static pk11_offs *pk11Offs = NULL;
char id[15] = {0};

const volatile metadata_t __attribute__((section (".metadata"))) metadata_section = {
    .magic = UWU0_MAGIC,
    .major = VERSION_MAJOR,
    .minor = VERSION_MINOR
};

void drawSplash() {    
    // Draw splashscreen to framebuffer.
    if(gfx_con.mute && fopen("/ReiNX/splash.bmp", "rb") != 0) {
        size_t bmpHeaderSize = 0x8A;
        size_t filsize = fsize();
        u32 *buf = malloc(filsize/sizeof(u32));
        fseek(bmpHeaderSize);
        fread((void*)buf, filsize-bmpHeaderSize, 1);
        fclose();
        gfx_load_splash(buf);
        free(buf);
    }
    return;
}

u8 loadFirm() {
    print("%k\nSetting up HOS:\n%k", WHITE, DEFAULT_TEXT_COL);
    sdmmc_storage_t storage;
    sdmmc_t sdmmc;

    //Init nand
    sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4);
    sdmmc_storage_set_mmc_partition(&storage, 1);
    
    //Read Boot0
    u8 *bctBuf = ReadBoot0(&storage);
    u32 pkg11_size = *(u32 *)(bctBuf + 0x233C);
    free(bctBuf);

    // Read package1.
    u8 *pkg1ldr = ReadPackage1Ldr(&storage);
    memcpy(id, pkg1ldr + 0x10, 14);
    for (u32 i = 0; _pk11_offs[i].pkg11_off; i++) {
        if(!strcmp(_pk11_offs[i].id, id)){
            pk11Offs = (pk11_offs *)&_pk11_offs[i];
            break;
        }
    }
    
    print("Firmware kb: %d\n", pk11Offs->kb);
    print("Firmware ver: %d\n", pk11Offs->hos);
    
    // Decrypt package1 and setup warmboot.
    print("Decrypting Package1...\n");
    u8 *pkg11 = pkg1ldr + pk11Offs->pkg11_off;

    // Generate keys
    if(pk11Offs->kb < KB_FIRMWARE_VERSION_700) {
        u8 *keyblob = (u8 *)malloc(NX_EMMC_BLOCKSIZE);
        sdmmc_storage_read(&storage, 0x180000 / NX_EMMC_BLOCKSIZE + pk11Offs->kb, 1, keyblob);
        if(!keygen(keyblob, pk11Offs->kb, pkg1ldr, pk11Offs))
            print("Failed to keygen...\n");
        free(keyblob);
        //Decrypt if needed
        if(pk11Offs->kb < KB_FIRMWARE_VERSION_620)
            se_aes_crypt_ctr(11, pkg11 + 0x20, pkg11_size, pkg11 + 0x20, pkg11_size, pkg11 + 0x10);
    }
    else {
        if(!has_keygen_ran())
            reboot_to_sept(pkg1ldr + pk11Offs->tsec_off, pk11Offs->hos);
        else
            se_aes_unwrap_key(8, 12, pk21_keyseed);
    }
    
    //Draw splash only after sept
    drawSplash();

    print("Unpacking pkg1\n");
    pkg1_unpack(pk11Offs, (u32)pkg11);
    
    PMC(APBDEV_PMC_SCRATCH1) = pk11Offs->warmboot_base;
    free(pkg1ldr);

    //Read package2
    size_t pkg2_size = 0;
    u8 *pkg2 = ReadPackage2(&storage, &pkg2_size);

    // Unpack Package2.
    print("Unpacking package2...\n");
    pkg2_hdr_t *dec_pkg2 = unpackFirmwarePackage(pkg2, pk11Offs->kb);

    LIST_INIT(kip1_info);
    pkg2_parse_kips(&kip1_info, dec_pkg2);

    // Patch firmware.
    patchKernel(dec_pkg2, pk11Offs->hos);
    patchKernelExtensions(&kip1_info);

    // Build Package2.
    buildFirmwarePackage(dec_pkg2->data, dec_pkg2->sec_size[PKG2_SEC_KERNEL], &kip1_info, pk11Offs);
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

    config_exosphere((void *)pk11Offs);

    //We're done with SD now
    sdUnmount();

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
    if (pk11Offs->kb >= KB_FIRMWARE_VERSION_700) {
        *BOOT_STATE_ADDR7X = (pk11Offs->kb < KB_FIRMWARE_VERSION_400 ? BOOT_PKG2_LOADED : BOOT_PKG2_LOADED_4X);
        *SECMON_STATE_ADDR7X = 0;
    } else {
        *BOOT_STATE_ADDR = (pk11Offs->kb < KB_FIRMWARE_VERSION_400 ? BOOT_PKG2_LOADED : BOOT_PKG2_LOADED_4X);
        *SECMON_STATE_ADDR = 0;
    }
    print("%k\nLaunching HOS!\n%k", GREEN, DEFAULT_TEXT_COL);
    
    // Wait if in verbose mode
    if(!gfx_con.mute) btn_wait();
    
    // Disable display.
    display_end();

    // Boot secmon and Wait for it get ready, if aplicable.
    if (smmu_is_used())
        smmu_exit();
    else
        cluster_boot_cpu0(pk11Offs->secmon_base);

    while (!*(pk11Offs->kb >= KB_FIRMWARE_VERSION_700 ? BOOT_STATE_ADDR7X : BOOT_STATE_ADDR))
        usleep(1);

    // Signal to finish boot process.
    if (pk11Offs->kb < KB_FIRMWARE_VERSION_700)
        *BOOT_STATE_ADDR = (pk11Offs->kb < KB_FIRMWARE_VERSION_400 ? BOOT_DONE : BOOT_DONE_4X);
    else
        *BOOT_STATE_ADDR7X = BOOT_DONE_4X;

    // Halt ourselves in waitevent state.
    while (1) FLOW_CTLR(0x4) = 0x50000000;
}

void firmware() {
    //Init display
    display_init();
    gfx_init_ctxt(&gfx_ctxt, display_init_framebuffer(), 1280, 720, 720);
    gfx_clear_color(&gfx_ctxt, BLACK);
    gfx_con_init(&gfx_con, &gfx_ctxt);
    gfx_con_setcol(&gfx_con, DEFAULT_TEXT_COL, 0, 0);
    display_backlight_pwm_init();
    display_backlight_brightness(100, 5000);
    
    //Mount SD
    if (!sdMount())
        error("Failed to init SD card!\nPress POWER to power off, or any other key to continue without SD.\n");

    //Chainload ReiNX if applicable
    if(EMC(EMC_SCRATCH0) != 69 && EMC(EMC_SCRATCH0) != 67 && fopen("/ReiNX.bin", "rb")) {
        size_t size = fsize();
        u8 *payload = malloc(size);
        fread((void*)PAYLOAD_ADDR, size, 1);
        fclose();
        metadata_t *metadata = (metadata_t*)(payload + METADATA_OFFSET);
        if(metadata->magic == metadata_section.magic) {
            if(metadata->major > metadata_section.major || (metadata->major == metadata_section.major && metadata->minor > metadata_section.minor)) {
                sdUnmount();
                display_end();
                CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_V) |= 0x400; // Enable AHUB clock.
                CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_Y) |= 0x40;  // Enable APE clock.
                EMC(EMC_SCRATCH0) = 69;
                ((void (*)())PAYLOAD_ADDR)();
            }
        }
    }
    SYSREG(AHB_AHB_SPARE_REG) &= (vu32)0xFFFFFF9F;
    PMC(APBDEV_PMC_SCRATCH49) = PMC(APBDEV_PMC_SCRATCH49) & 0xFFFFFFFC;
    EMC(EMC_SCRATCH0) = 0;

    //Chainload recovery if applicable
    if(btn_read() & BTN_VOL_UP){
        if(fopen("/ReiNX/Recovery.bin", "rb") != 0) {
            fread((void*)PAYLOAD_ADDR, fsize(), 1);
            fclose();
            if(!fopen("/ReiNX.bin", "rb")) {
                memcpy((void *)0x82000000, (void *)0x40008000, fsize());
            } else {
                EMC(EMC_SCRATCH0) = 69;
                fclose();
            }
            sdUnmount();
            ((void (*)())PAYLOAD_ADDR)();
        } else {
            error("Failed to launch recovery menu!\nIs it missing from /ReiNX folder?\n");
        }
    }

    //Determine if booting in verbose mode
    gfx_con.mute = !(btn_read() & BTN_VOL_DOWN);
        
    print("%kWelcome to ReiNX %d.%d!\n%k", WHITE, VERSION_MAJOR, VERSION_MINOR, DEFAULT_TEXT_COL);
    
    //Make sure we have the needed files
    if (!hasCustomWb() && !hasCustomSecmon()) {
        error("Missing warmboot.bin or secmon.bin. These are required!\n");
    }

    //Setup cfw
    loadFirm();
    launch();
}
