/*
 * Copyright (c) 2019 CTCaer
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

#include "sept.h"
#include "hwinit/types.h"

static u8 warmboot_reboot[] = {
    0x14, 0x00, 0x9F, 0xE5, // LDR R0, =0x7000E450
    0x01, 0x10, 0xB0, 0xE3, // MOVS R1, #1
    0x00, 0x10, 0x80, 0xE5, // STR R1, [R0]
    0x0C, 0x00, 0x9F, 0xE5, // LDR R0, =0x7000E400
    0x10, 0x10, 0xB0, 0xE3, // MOVS R1, #0x10
    0x00, 0x10, 0x80, 0xE5, // STR R1, [R0]
    0xFE, 0xFF, 0xFF, 0xEA, // LOOP
    0x50, 0xE4, 0x00, 0x70, // #0x7000E450
    0x00, 0xE4, 0x00, 0x70  // #0x7000E400
};

int keys_generated = 0;

int has_keygen_ran() {
    if(keys_generated == 1)
        return keys_generated;
    int has_ran = EMC(EMC_SCRATCH0) == 67;
    EMC(EMC_SCRATCH0) = 0;
    keys_generated = has_ran;
    return has_ran;
}

void reloc_patcher(u32 payload_dst, u32 payload_src, u32 payload_size)
{
    static const u32 START_OFF = 0x98;
    static const u32 STACK_OFF = 0x9C;
    static const u32 PAYLOAD_END_OFF = 0xA0;
    static const u32 IPL_START_OFF = 0xA4;

    memcpy((u8 *)payload_src, (u8 *)0x40008000, PATCHED_RELOC_SZ);

    *(vu32 *)(payload_src + START_OFF) = payload_dst - ALIGN(PATCHED_RELOC_SZ, 0x10);
    *(vu32 *)(payload_src + PAYLOAD_END_OFF) = payload_dst + payload_size;
    *(vu32 *)(payload_src + STACK_OFF) = 0x40008000;
    *(vu32 *)(payload_src + IPL_START_OFF) = payload_dst;

    if (payload_size == 0x7000)
    {
        memcpy((u8 *)(payload_src + ALIGN(PATCHED_RELOC_SZ, 0x10)), (u8 *)COREBOOT_ADDR, 0x7000); //Bootblock
        *(vu32 *)CBFS_SDRAM_EN_ADDR = 0x4452414D;
    }
}

int reboot_to_sept(const u8 *tsec_fw, u32 hosver)
{
    // Copy warmboot reboot code and TSEC fw.
    memcpy((u8 *)(SEPT_PK1T_ADDR - WB_RST_SIZE), (u8 *)warmboot_reboot, sizeof(warmboot_reboot));
    memcpy((void *)SEPT_PK1T_ADDR, tsec_fw, hosver == HOS_FIRMWARE_VERSION_800 ? 0x3000 : 0x3300);
    *(vu32 *)SEPT_TCSZ_ADDR = hosver == HOS_FIRMWARE_VERSION_800 ? 0x3000 : 0x3300;

    // Copy sept-primary.
    fopen("/sept/sept-primary.bin", "rb");
    fread((u8 *)SEPT_STG1_ADDR, fsize(), 1);
    fclose();

    // Copy sept-secondary.
    char *sec_path;
    switch(hosver) {
        case HOS_FIRMWARE_VERSION_800:
            sec_path = "/sept/sept-secondary_00.enc";
            break;
        default:
            sec_path = "/sept/sept-secondary_01.enc";
            break;
    }
    fopen(sec_path, "rb");
    fread((u8 *)SEPT_STG2_ADDR, fsize(), 1);
    fclose();

    sdUnmount();

    u32 pk1t_sept = SEPT_PK1T_ADDR - (ALIGN(PATCHED_RELOC_SZ, 0x10) + WB_RST_SIZE);

    void (*sept)() = (void *)pk1t_sept;

    reloc_patcher(WB_RST_ADDR, pk1t_sept, SEPT_PKG_SZ);

    // Patch SDRAM init to perform an SVC immediately after second write.
    PMC(APBDEV_PMC_SCRATCH45) = 0x2E38DFFF;
    PMC(APBDEV_PMC_SCRATCH46) = 0x6001DC28;
    // Set SVC handler to jump to sept-primary in IRAM.
    PMC(APBDEV_PMC_SCRATCH33) = SEPT_PRI_ADDR;
    PMC(APBDEV_PMC_SCRATCH40) = 0x6000F208;

    display_end();
    
    (*sept)();

    return 1;

error:
    print("oops failed to gen keys\n");

    btn_wait();

    return 0;
}
