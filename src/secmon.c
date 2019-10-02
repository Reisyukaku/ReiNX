/*
 * Copyright (C) 2018-2019 CTCaer
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
#include "secmon.h"
#include "hwinit.h"

typedef struct _exo_cfg_t
{
    vu32 magic;
    vu32 fwno;
    vu32 flags;
    vu32 rsvd;
} exo_cfg_t;

typedef struct _atm_meta_t
{
    uint32_t magic;
    uint32_t fwno;
} wb_cfg_t;

#define ATM_WB_HEADER_OFF 0x244
#define  ATM_WB_MAGIC 0x30544257

// Exosphère mailbox defines.
#define EXO_CFG_DEPR_ADDR 0x40002E40 // Deprecated.
#define EXO_CFG_ADDR      0x8000F000
#define  EXO_MAGIC_DEPR_VAL 0x31434258
#define  EXO_MAGIC_VAL      0x304F5845
#define  EXO_FLAG_620_KGN   (1 << 0)
#define  EXO_FLAG_DBG_PRIV  (1 << 1)
#define  EXO_FLAG_DBG_USER  (1 << 2)

void config_exosphere(pk11_offs *pkoff) {
    u32 exoFwNo = 0;
    u32 exoFlags = 0;

    volatile exo_cfg_t *exo_cfg_depr = (exo_cfg_t *)EXO_CFG_DEPR_ADDR;
    volatile exo_cfg_t *exo_cfg = (exo_cfg_t *)EXO_CFG_ADDR;

    exoFwNo = pkoff->hos;
    print("Exo firm #: %d\n", exoFwNo);

    if (pkoff->hos == HOS_FIRMWARE_VERSION_620)
        exoFlags |= EXO_FLAG_620_KGN;

    // To avoid problems, make private debug mode always on.
    exoFlags |= EXO_FLAG_DBG_PRIV;

    // Set mailbox values.
    exo_cfg_depr->magic = EXO_MAGIC_VAL;
    exo_cfg->magic = EXO_MAGIC_VAL;

    exo_cfg_depr->fwno = pkoff->hos;
    exo_cfg->fwno = pkoff->hos;

    exo_cfg_depr->flags = exoFlags;
    exo_cfg->flags = exoFlags;

    volatile wb_cfg_t *wb_cfg = (wb_cfg_t *)((void *)pkoff->warmboot_base + ATM_WB_HEADER_OFF);

    if (wb_cfg->magic == ATM_WB_MAGIC) {
        wb_cfg->fwno = pkoff->hos;

        sdmmc_storage_t storage;
        sdmmc_t sdmmc;

        u8 *rsa_mod = malloc(0x200);

        sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4);
        sdmmc_storage_set_mmc_partition(&storage, 1);
        sdmmc_storage_read(&storage, 1, 1, rsa_mod);
        sdmmc_storage_end(&storage);

        // Patch AutoRCM out.
        if ((FUSE(FUSE_RESERVED_ODMX(4)) & 3) != 3)
            rsa_mod[0x10] = 0xF7;
        else
            rsa_mod[0x10] = 0x37;

        memcpy((void*)pkoff->warmboot_base + 0x10, rsa_mod + 0x10, 0x100);
    }
}
