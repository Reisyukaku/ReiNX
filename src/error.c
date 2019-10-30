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

#include "hwinit.h"
#include "error.h"

void panic() {
    // Set panic code.
    PMC(APBDEV_PMC_SCRATCH200) = 0x21;
    
    //PMC(APBDEV_PMC_CRYPTO_OP) = 1; // Disable SE.
    TMR(0x18C) = 0xC45A;
    TMR(0x80) = 0xC0000000;
    TMR(0x180) = 0x8019;
    TMR(0x188) = 1;
    while (1);
}

void error(char *errStr) {
    gfx_con.mute = 0;
    gfx_con_setcol(&gfx_con, RED, 0, 0);
    print("Error: %s", errStr);
    gfx_con_setcol(&gfx_con, DEFAULT_TEXT_COL, 0, 0);
    if (btn_wait() & BTN_POWER)
        i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_ONOFFCNFG1, MAX77620_ONOFFCNFG1_PWR_OFF);
}