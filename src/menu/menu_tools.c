/*
 * Credits of toggle auto rcm
 *  
 * Copyright (c) 2018 naehrwert
 *
 * Copyright (c) 2018 Rajko Stojadinovic
 * Copyright (c) 2018 CTCaer
 * Copyright (c) 2018 Reisyukaku
 * Copyright (c) 2018 balika011
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
#include "menu_tools.h"
#include "menu_pool.h"

int is_autorcm_enabled()
{
    sdmmc_storage_t storage;
    sdmmc_t sdmmc;
    int enabled = 0;

    if (!sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4))
    {
        gfx_printf(&gfx_con, "%kError while init eMMC\n", RED);
        btn_wait();

        return 0;
    }

    u8 *tempbuf = (u8 *)malloc(0x200);
    sdmmc_storage_set_mmc_partition(&storage, 1);
    sdmmc_storage_read(&storage, 0x200 / NX_EMMC_BLOCKSIZE, 1, tempbuf);

    if (tempbuf[0x10] != 0xF7)
        enabled = 1;

    free(tempbuf);
    sdmmc_storage_end(&storage);

    return enabled;
}

int toggle_autorcm(int enable)
{
    sdmmc_storage_t storage;
    sdmmc_t sdmmc;

    u8 randomXor = 0;

    if (!sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_4, SDMMC_BUS_WIDTH_8, 4))
    {
        gfx_printf(&gfx_con, "%kError while init eMMC\n", RED);
        btn_wait();
        return -1;
    }

    u8 *tempbuf = (u8 *)malloc(0x200);
    sdmmc_storage_set_mmc_partition(&storage, 1);

    int i, sect = 0;
    for (i = 0; i < 4; i++)
    {
        sect = (0x200 + (0x4000 * i)) / NX_EMMC_BLOCKSIZE;
        sdmmc_storage_read(&storage, sect, 1, tempbuf);

        if (enable)
        {
            do
            {
                randomXor = get_tmr_us() & 0xFF; // Bricmii style of bricking.
            } while (!randomXor);                // Avoid the lottery.

            tempbuf[0x10] ^= randomXor;
        }
        else
            tempbuf[0x10] = 0xF7;
        sdmmc_storage_write(&storage, sect, 1, tempbuf);
    }

    free(tempbuf);
    sdmmc_storage_end(&storage);

    if (enable)
        gfx_printf(&gfx_con, "%kAutoRCM mode enabled!%k", GREEN, WHITE);
    else
        gfx_printf(&gfx_con, "%kAutoRCM mode disabled!%k", GREEN, WHITE);
    
    gfx_printf(&gfx_con, "\n\nPress any key...\n");

    btn_wait();
}

int enable_autorcm()
{
    return toggle_autorcm(1);
}

int disable_autorcm()
{
    return toggle_autorcm(0);
}

int reboot_normal()
{
    gfx_printf(&gfx_con, "Rebooting switch...\n");
    usleep(1000000);
	sdUnmount();
    pool_cleanup();
	display_end();
	panic(); // Bypass fuse programming in package1.
    return 0;
}

int reboot_rcm()
{
    gfx_printf(&gfx_con, "Rebooting into rcm...\n");
    usleep(1000000);
	sdUnmount();
    pool_cleanup();
	display_end();
	PMC(APBDEV_PMC_SCRATCH0) = 2; // Reboot into rcm.
	PMC(0) |= 0x10;
	while (1)
		usleep(1);
    return 1;

}

int power_off()
{
    gfx_printf(&gfx_con, "Powering switch off...\n");
    usleep(1000000);
	sdUnmount();
    pool_cleanup();
	i2c_send_byte(I2C_5, MAX77620_I2C_ADDR, MAX77620_REG_ONOFFCNFG1, MAX77620_ONOFFCNFG1_PWR_OFF);
    return 1;
}

int check_burnt_fuses()
{
    int fuses_count = 0;
    u32 burnt_fuses_data = FUSE(FUSE_RESERVED_ODMX(7));

    for (u32 currBit=0; currBit < 32; currBit++)
    {
        if (burnt_fuses_data & (1u << currBit))
            fuses_count++;
        else
            break;
    }

    gfx_printf(&gfx_con, "NUMBER OF BURNT ANTI-DOWNGRADE FUSES: %k%d%k \n(raw value: 0x%08X)\n", RED, fuses_count, WHITE, burnt_fuses_data);
    gfx_printf(&gfx_con, "\n\nPress any...\n");
    
    usleep(1000000);
    btn_wait();
    return 0;
}