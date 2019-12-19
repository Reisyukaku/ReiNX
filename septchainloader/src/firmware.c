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

#include "hwinit.h"
#include "fs.h"
#include "firmware.h"


void firmware() {
    //Mount SD
    if (!sdMount()) {
        i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_ONOFFCNFG1, MAX77620_ONOFFCNFG1_PWR_OFF);
    }

    //Load ReiNX
    if(fopen("/ReiNX.bin", "rb")) {
        fread((void*)PAYLOAD_ADDR, fsize(), 1);
        fclose();
        sdUnmount();
        EMC(EMC_SCRATCH0) = 67;
        ((void (*)())PAYLOAD_ADDR)();
    }
    while(1);
}
