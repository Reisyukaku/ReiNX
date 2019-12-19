/*
* Copyright (c) 2018 Reisyukaku, naehrwert
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

#include "hwinit.h"
#include "bootrom.h"

void bootrom(void) {
    // Bootrom part we skipped.
    u32 sbk[4] = { FUSE(0x1A4), FUSE(0x1A8), FUSE(0x1AC), FUSE(0x1B0) };
    se_aes_key_set(14, sbk, 0x10);

    // Lock SBK from being read.
    SE(SE_KEY_TABLE_ACCESS_REG_OFFSET + 14 * 4) = 0x7E;
    
    // Lock SSK (although it's not set and unused anyways).
    SE(SE_KEY_TABLE_ACCESS_REG_OFFSET + 15 * 4) = 0x7E;

    // This memset needs to happen here, else TZRAM will behave weirdly later on.
    memset((void *)0x7C010000, 0, 0x10000);
    PMC(APBDEV_PMC_CRYPTO_OP) = 0;
    SE(SE_INT_STATUS_REG_OFFSET) = 0x1F;

    // Clear the boot reason to avoid problems later
    PMC(APBDEV_PMC_SCRATCH200) = 0x0;
    PMC(APBDEV_PMC_RST_STATUS) = 0x0;
    APB_MISC(APB_MISC_PP_STRAPPING_OPT_A) = (APB_MISC(APB_MISC_PP_STRAPPING_OPT_A) & 0xF0) | (7 << 10);
}
