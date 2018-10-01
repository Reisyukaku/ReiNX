/*  
 * Copyright (c) 2018 Guillem96
 *
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
#include "reinx_menu.h"

int printHello(void *);
int about(void*);
menu_t *create_restore_menu(void);
menu_t *create_dump_menu(void);
menu_t *create_autorcm_menu(void);

void init_reinx_menu()
{   
    pool_init();
	menu_t *reinx_menu = menu_create("ReiNX Menu");

	menu_entry_t *launch_separator = create_menu_entry("--- Launch Options ---", 0xFF7AADFF, NULL, NULL);
	menu_entry_t *launc_reinx_entry = create_menu_entry("Launch ReiNX", WHITE, cancel, NULL);
	menu_entry_t *reboot_rcm_entry = create_menu_entry("Reboot into RCM", WHITE, reboot_rcm, NULL);
    menu_entry_t *launc_ofw_entry = create_menu_entry("Reboot normal (won't work with AutoRCM)", WHITE, reboot_normal, NULL);
	menu_entry_t *shutdown_entry = create_menu_entry("Power Off", WHITE, power_off, NULL);

    menu_t* auto_rcm_menu = create_autorcm_menu();

	menu_entry_t *utils_separator = create_menu_entry("--- Utils Options ---", 0xFF7AADFF, NULL, NULL);
	menu_entry_t *toggle_rcm_entry = create_menu_entry("Toggle AutoRCM", RED, (int (*)(void *))menu_open, auto_rcm_menu);
	menu_entry_t *n_burnt_fuses_entry = create_menu_entry("Check burnt fuses", WHITE, check_burnt_fuses, NULL);

	menu_t *restore_menu = create_restore_menu();
	menu_t *dump_menu = create_dump_menu();

	menu_entry_t *backup_restore_separator = create_menu_entry("--- Backup Options ---", 0xFF7AADFF, NULL, NULL);
	menu_entry_t *restore_entry = create_menu_entry("Restore... (In progress)", WHITE, (int (*)(void *))menu_open, restore_menu);
	menu_entry_t *dump_entry = create_menu_entry("Dump... (In progress)", WHITE, (int (*)(void *))menu_open, dump_menu);

	menu_entry_t *more_separator = create_menu_entry("--- More ---", 0xFF7AADFF, NULL, NULL);
	menu_entry_t *about_entry = create_menu_entry("About", WHITE, about, NULL);
	menu_entry_t *exit_entry = create_menu_entry("Exit", YELLOW, cancel, NULL);

	menu_append_entry(reinx_menu, launch_separator);
	menu_append_entry(reinx_menu, launc_reinx_entry);
	menu_append_entry(reinx_menu, reboot_rcm_entry);
    menu_append_entry(reinx_menu, launc_ofw_entry);
	menu_append_entry(reinx_menu, shutdown_entry);

	menu_append_entry(reinx_menu, utils_separator);
	menu_append_entry(reinx_menu, toggle_rcm_entry);
	menu_append_entry(reinx_menu, n_burnt_fuses_entry);

	menu_append_entry(reinx_menu, backup_restore_separator);
	menu_append_entry(reinx_menu, restore_entry);
	menu_append_entry(reinx_menu, dump_entry);

	menu_append_entry(reinx_menu, more_separator);
	menu_append_entry(reinx_menu, about_entry);

    menu_append_entry(reinx_menu, exit_entry);

	menu_open(reinx_menu);

    pool_cleanup();
}

menu_t *create_restore_menu()
{
	menu_t *yn_menu_res_nand = create_yes_no_menu("restore NAND", printHello, "Restoring NAND...", cancel, NULL);
	menu_t *yn_menu_res_boot = create_yes_no_menu("restore BOOT 0/1", printHello, "Restoring BOOT 0/1...", cancel, NULL);

	menu_t *restore_menu = menu_create("Restore Menu");
	menu_entry_t *nand_entry = create_menu_entry("Restore NAND (In progress)", WHITE, (int (*)(void *))menu_open, yn_menu_res_nand);
	menu_entry_t *boot_entry = create_menu_entry("Restore Boot 0/1 (In progress)", WHITE, (int (*)(void *))menu_open, yn_menu_res_boot);
	menu_entry_t *back_entry = create_menu_entry("Back", WHITE, cancel, NULL);

	menu_append_entry(restore_menu, nand_entry);
	menu_append_entry(restore_menu, boot_entry);
	menu_append_entry(restore_menu, back_entry);

	return restore_menu;
}

menu_t *create_dump_menu()
{
	menu_t *yn_menu_backup_nand = create_yes_no_menu("dump NAND", printHello, "Dumping NAND...", cancel, NULL);
	menu_t *yn_menu_backup_boot = create_yes_no_menu("dump BOOT 0/1", printHello, "Dumping BOOT 0/1...", cancel, NULL);

	menu_t *dump_menu = menu_create("Dump Menu");
	menu_entry_t *nand_entry = create_menu_entry("Dump NAND (In progress)", WHITE, (int (*)(void *))menu_open, yn_menu_backup_nand);
	menu_entry_t *boot_entry = create_menu_entry("Dump Boot 0/1 (In progress)", WHITE, (int (*)(void *))menu_open, yn_menu_backup_boot);
	menu_entry_t *back_entry = create_menu_entry("Back", WHITE, cancel, NULL);

	menu_append_entry(dump_menu, nand_entry);
	menu_append_entry(dump_menu, boot_entry);
	menu_append_entry(dump_menu, back_entry);

	return dump_menu;
}

menu_t *create_autorcm_menu()
{
    menu_t* yn_menu_autorcm;

    if (is_autorcm_enabled())
	    yn_menu_autorcm = create_yes_no_menu("disable AutoRCM?", disable_autorcm, NULL, cancel, NULL);
    else
        yn_menu_autorcm = create_yes_no_menu("enable AutoRCM?", enable_autorcm, NULL, cancel, NULL);

	return yn_menu_autorcm;
}

int printHello(void *param)
{
	gfx_printf(&gfx_con, "%s\n", (char *)param);
	usleep(1000000);
	return 0;
}

int about(void* param)
{
    static const char credits[] =
		"\n%kNaehrwert for hardware init code\n and generally being helpful!\n\n"
		"CTCaer and st4rk for their contribution\n to the hardware code aswell!\n\n"
		"%kSciresM for sysmodules!\n\n"
		"%kThe community for your support! <3%k\n\n\n\n";

	static const char reinx[] = 
        "%k _______            _   ____  _____  ____  ____  \n"
        "|_   __ \\          (_) |_   \\|_   _||_  _||_  _| \n"
        "  | |__) |  .---.  __    |   \\ | |    \\ \\  / /   \n"
        "  |  __ /  / /__\\[  |   | |\\ \\| |     > `' <    \n"
        " _| |  \\ \\_| \\__., | |  _| |_\\   |_  _/ /'`\\ \\_  \n"
        "|____| |___|'.__.'[___]|_____|\\____||____||____| %k\n";

	gfx_printf(&gfx_con, credits, 0xFFFF9191, 0xFF91E3FF, 0xFFFFA0F0, WHITE);
	gfx_con.fntsz = 10;
	gfx_printf(&gfx_con, reinx, RED, WHITE);
    
    gfx_con.fntsz = 16;
    gfx_printf(&gfx_con, "\n\nPress any key...\n");
    
    usleep(1000000);
    btn_wait();
    return 0;
}
