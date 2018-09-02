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
#include "error.h"
#include "bootloader.h"

void check_sku() {
    if (FUSE(FUSE_SKU) != 0x83)
        panic();
}

u32 get_unknown_config() {
    u32 res = 0;
    u32 deviceInfo = FUSE(FUSE_RESERVED_ODMX(4));
    u32 config = ((deviceInfo & 4u) >> 2) | 2 * ((deviceInfo & 0x100u) >> 8);
    
    if(config == 1)
        return 0;
    if(config == 2)
        return 1;
    if(config || (res = FUSE(FUSE_SPARE_BIT_5)) != 0)
        res = 3;
    return res;
}

u32 get_unit_type() {
    u32 deviceInfo = FUSE(FUSE_RESERVED_ODMX(4));
    u32 deviceType = deviceInfo & 3 | 4 * ((deviceInfo & 0x200u) >> 9);
    
    if(deviceType == 3)
        return 0;
    if(deviceType == 4)
        return 1;
    return 2;
}

void check_config_fuses() {
    u32 config = get_unknown_config();
    u32 unitType = get_unit_type();
    u32 bromVer = FUSE(FUSE_SOC_SPEEDO_1);
    
    if (config == 3 || unitType == 2 || bromVer < 0x1F)
        panic();
}

void mbist_workaround() {
    CLOCK(0x410) = (CLOCK(0x410) | 0x8000) & 0xFFFFBFFF;
    CLOCK(0xD0) |= 0x40800000u;
    CLOCK(0x2AC) = 0x40;
    CLOCK(0x294) = 0x40000;
    CLOCK(0x304) = 0x18000000;
    usleep(2);

    I2S(0x0A0) |= 0x400;
    I2S(0x088) &= 0xFFFFFFFE;
    I2S(0x1A0) |= 0x400;
    I2S(0x188) &= 0xFFFFFFFE;
    I2S(0x2A0) |= 0x400;
    I2S(0x288) &= 0xFFFFFFFE;
    I2S(0x3A0) |= 0x400;
    I2S(0x388) &= 0xFFFFFFFE;
    I2S(0x4A0) |= 0x400;
    I2S(0x488) &= 0xFFFFFFFE;
    DISPLAY_A(0xCF8) |= 4;
    VIC(0x8C) = 0xFFFFFFFF;
    usleep(2);

    CLOCK(0x2A8) = 0x40;
    CLOCK(0x300) = 0x18000000;
    CLOCK(0x290) = 0x40000;
    CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_H) = 0xC0;
    CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_L) = 0x80000130;
    CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_U) = 0x1F00200;
    CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_V) = 0x80400808;
    CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_W) = 0x402000FC;
    CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_X) = 0x23000780;
    CLOCK(CLK_RST_CONTROLLER_CLK_OUT_ENB_Y) = 0x300;
    CLOCK(0xF8) = 0;
    CLOCK(0xFC) = 0;
    CLOCK(0x3A0) = 0;
    CLOCK(0x3A4) = 0;
    CLOCK(0x554) = 0;
    CLOCK(0xD0) &= 0x1F7FFFFF;
    CLOCK(0x410) &= 0xFFFF3FFF;
    CLOCK(0x148) = CLOCK(0x148) & 0x1FFFFFFF | 0x80000000;
    CLOCK(0x180) = CLOCK(0x180) & 0x1FFFFFFF | 0x80000000;
    CLOCK(0x6A0) = CLOCK(0x6A0) & 0x1FFFFFFF | 0x80000000;
}

void config_pmc_scratch() {
    PMC(APBDEV_PMC_SCRATCH20) &= 0xFFF3FFFF;
    PMC(APBDEV_PMC_SCRATCH190) &= 0xFFFFFFFE;
    PMC(APBDEV_PMC_SECURE_SCRATCH21) |= 0x10;
}

void config_oscillators() {
    CLOCK(CLK_RST_CONTROLLER_SPARE_REG0) = CLOCK(CLK_RST_CONTROLLER_SPARE_REG0) & 0xFFFFFFF3 | 4;
    SYSCTR0(SYSCTR0_CNTFID0) = 19200000;
    TMR(0x14) = 0x45F;
    CLOCK(CLK_RST_CONTROLLER_OSC_CTRL) = 0x50000071;
    PMC(APBDEV_PMC_OSC_EDPD_OVER) = PMC(APBDEV_PMC_OSC_EDPD_OVER) & 0xFFFFFF81 | 0xE;
    PMC(APBDEV_PMC_OSC_EDPD_OVER) = PMC(APBDEV_PMC_OSC_EDPD_OVER) & 0xFFBFFFFF | 0x400000;
    PMC(APBDEV_PMC_CNTRL2) = PMC(APBDEV_PMC_CNTRL2) & 0xFFFFEFFF | 0x1000;
    PMC(APBDEV_PMC_SCRATCH188) = PMC(APBDEV_PMC_SCRATCH188) & 0xFCFFFFFF | 0x2000000;
    CLOCK(CLK_RST_CONTROLLER_CLK_SYSTEM_RATE) = 0x10;
    CLOCK(CLK_RST_CONTROLLER_PLLMB_BASE) &= 0xBFFFFFFF;
    PMC(APBDEV_PMC_TSC_MULT) = PMC(APBDEV_PMC_TSC_MULT) & 0xFFFF0000 | 0x249F; //0x249F = 19200000 * (16 / 32.768 kHz)
    CLOCK(CLK_RST_CONTROLLER_SCLK_BURST_POLICY) = 0x20004444;
    CLOCK(CLK_RST_CONTROLLER_SUPER_SCLK_DIVIDER) = 0x80000000;
    CLOCK(CLK_RST_CONTROLLER_CLK_SYSTEM_RATE) = 2;
}

void config_gpios() {
    PINMUX_AUX(PINMUX_AUX_UART2_TX) = 0;
    PINMUX_AUX(PINMUX_AUX_UART3_TX) = 0;

    PINMUX_AUX(PINMUX_AUX_GPIO_PE6) = 0x40;
    PINMUX_AUX(PINMUX_AUX_GPIO_PH6) = 0x40;

    gpio_config(GPIO_PORT_G, GPIO_PIN_0, GPIO_MODE_GPIO);
    gpio_config(GPIO_PORT_D, GPIO_PIN_1, GPIO_MODE_GPIO);
    gpio_config(GPIO_PORT_E, GPIO_PIN_6, GPIO_MODE_GPIO);
    gpio_config(GPIO_PORT_H, GPIO_PIN_6, GPIO_MODE_GPIO);
    gpio_output_enable(GPIO_PORT_G, GPIO_PIN_0, GPIO_OUTPUT_DISABLE);
    gpio_output_enable(GPIO_PORT_D, GPIO_PIN_1, GPIO_OUTPUT_DISABLE);
    gpio_output_enable(GPIO_PORT_E, GPIO_PIN_6, GPIO_OUTPUT_DISABLE);
    gpio_output_enable(GPIO_PORT_H, GPIO_PIN_6, GPIO_OUTPUT_DISABLE);

    pinmux_config_i2c(I2C_1);
    pinmux_config_i2c(I2C_5);
    pinmux_config_uart(UART_A);

    // Configure volume up/down as inputs.
    gpio_config(GPIO_PORT_X, GPIO_PIN_6, GPIO_MODE_GPIO);
    gpio_config(GPIO_PORT_X, GPIO_PIN_7, GPIO_MODE_GPIO);
    gpio_output_enable(GPIO_PORT_X, GPIO_PIN_6, GPIO_OUTPUT_DISABLE);
    gpio_output_enable(GPIO_PORT_X, GPIO_PIN_7, GPIO_OUTPUT_DISABLE);
}

void setup() {
    config_oscillators();
    APB_MISC(0x40) = 0;
    config_gpios();
    
    if (get_unit_type() == 0) {
        // TODO: devunit sub_40018D90
    }
    
    clock_enable_cl_dvfs();
    clock_enable_i2c(I2C_1);
    clock_enable_i2c(I2C_5);

    static const clock_t clock_unk1 = { 0x358, 0x360, 0x42C, 0x1F, 0, 0 };
    static const clock_t clock_unk2 = { 0x358, 0x360, 0, 0x1E, 0, 0 };
    clock_enable(&clock_unk1);
    clock_enable(&clock_unk2);

    i2c_init(I2C_1);
    i2c_init(I2C_5);

    i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_CNFGBBC, 0x40);
    i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_ONOFFCNFG1, 0x78);

    i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_CFG0, 0x38);
    i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_CFG1, 0x3A);
    i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_CFG2, 0x38);
    i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_LDO4, 0xF);
    i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_LDO8, 0xC7);
    i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_SD0, 0x4F);
    i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_SD1, 0x29);
    i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_FPS_SD3, 0x1B);

    i2c_send_byte(I2C_5, 0x3C, MAX77620_REG_SD0, 42); //42 = (1125000 - 600000) / 12500 -> 1.125V

    config_pmc_scratch();

    CLOCK(CLK_RST_CONTROLLER_SCLK_BURST_POLICY) = CLOCK(CLK_RST_CONTROLLER_SCLK_BURST_POLICY) & 0xFFFF8888 | 0x3333;

    mc_config_carveout();

    sdram_init();
    
    sdram_lp0_save_params(sdram_get_params());
}

void bootloader() { 
    mbist_workaround();
    clock_enable_se();
    
    // This makes fuse registers visible
    clock_enable_fuse(0x01);

    check_sku();

    // Check configuration fuses
    check_config_fuses();

    // Disables fuse programming until next reboot
    FUSE(FUSE_PRIVATEKEYDISABLE) = 0x10;

    // Setup memory controllers
    mc_enable();
    
    // Pre-Firmware setup
    setup();
}