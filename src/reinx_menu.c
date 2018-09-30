#include "reinx_menu.h"

int printHello(void *);
menu_t *create_restore_menu(void);
menu_t *create_dump_menu(void);

void init_reinx_menu()
{
	menu_t *reinx_menu = menu_create("ReiNX Menu");

	menu_entry_t *launch_separator = create_menu_entry("--- Launch Options ---", 0xFF7AADFF, NULL, NULL);
	menu_entry_t *launc_ofw_entry = create_menu_entry("Launch OFW", WHITE, printHello, "Launching OFW");
	menu_entry_t *launc_reinx_entry = create_menu_entry("Launch ReiNX", WHITE, printHello, "Launching ReiNX");
	menu_entry_t *reboot_rcm_entry = create_menu_entry("Reboot into RCM", WHITE, printHello, "Rebooting into RCM...");
	menu_entry_t *shutdown_entry = create_menu_entry("Shutdown", WHITE, printHello, "Tuning switch of...");

	menu_entry_t *utils_separator = create_menu_entry("--- Utils Options ---", 0xFF7AADFF, NULL, NULL);
	menu_entry_t *toogle_rcm_entry = create_menu_entry("Toogle AutoRCM", WHITE, printHello, "Auto RCM enabled/disabled");
	menu_entry_t *n_burnt_fuses_entry = create_menu_entry("Check burnt fuses", WHITE, printHello, "6 burnt fuses");

	menu_t *restore_menu = create_restore_menu();
	menu_t *dump_menu = create_dump_menu();

	menu_entry_t *backup_restore_separator = create_menu_entry("--- Backup Options ---", 0xFF7AADFF, NULL, NULL);
	menu_entry_t *restore_entry = create_menu_entry("Restore...", WHITE, (int (*)(void *))menu_open, restore_menu);
	menu_entry_t *dump_entry = create_menu_entry("Dump...", WHITE, (int (*)(void *))menu_open, dump_menu);

	menu_entry_t *more_separator = create_menu_entry("--- More ---", 0xFF7AADFF, NULL, NULL);
	menu_entry_t *about_entry = create_menu_entry("About", WHITE, printHello, "About me");
	menu_entry_t *exit_entry = create_menu_entry("Exit", YELLOW, cancel, NULL);

	menu_append_entry(reinx_menu, launch_separator);
	menu_append_entry(reinx_menu, launc_ofw_entry);
	menu_append_entry(reinx_menu, launc_reinx_entry);
	menu_append_entry(reinx_menu, reboot_rcm_entry);
	menu_append_entry(reinx_menu, shutdown_entry);

	menu_append_entry(reinx_menu, utils_separator);
	menu_append_entry(reinx_menu, toogle_rcm_entry);
	menu_append_entry(reinx_menu, n_burnt_fuses_entry);

	menu_append_entry(reinx_menu, backup_restore_separator);
	menu_append_entry(reinx_menu, restore_entry);
	menu_append_entry(reinx_menu, dump_entry);

	menu_append_entry(reinx_menu, more_separator);
	menu_append_entry(reinx_menu, about_entry);

    menu_append_entry(reinx_menu, exit_entry);

	menu_open(reinx_menu);

    menu_destroy(yn_menu_res_nand);
    menu_destroy(yn_menu_res_boot);
    menu_destroy(yn_menu_backup_nand);
    menu_destroy(yn_menu_backup_boot);
    menu_destroy(restore_menu);
	menu_destroy(dump_menu);
    menu_destroy(reinx_menu);
}

menu_t *create_restore_menu()
{
	yn_menu_res_nand = create_yes_no_menu("restore NAND", printHello, "Restoring NAND...", cancel, NULL);
	yn_menu_res_boot = create_yes_no_menu("restore BOOT 0/1", printHello, "Restoring BOOT 0/1...", cancel, NULL);

	menu_t *restore_menu = menu_create("Restore Menu");
	menu_entry_t *nand_entry = create_menu_entry("Restore NAND", WHITE, (int (*)(void *))menu_open, yn_menu_res_nand);
	menu_entry_t *boot_entry = create_menu_entry("Restore Boot 0/1", WHITE, (int (*)(void *))menu_open, yn_menu_res_boot);
	menu_entry_t *back_entry = create_menu_entry("Back", WHITE, cancel, NULL);

	menu_append_entry(restore_menu, nand_entry);
	menu_append_entry(restore_menu, boot_entry);
	menu_append_entry(restore_menu, back_entry);

	return restore_menu;
}

menu_t *create_dump_menu()
{
	yn_menu_backup_nand = create_yes_no_menu("dump NAND", printHello, "Dumping NAND...", cancel, NULL);
	yn_menu_backup_boot = create_yes_no_menu("dump BOOT 0/1", printHello, "Dumping BOOT 0/1...", cancel, NULL);

	menu_t *dump_menu = menu_create("Dump Menu");
	menu_entry_t *nand_entry = create_menu_entry("Dump NAND", WHITE, (int (*)(void *))menu_open, yn_menu_backup_nand);
	menu_entry_t *boot_entry = create_menu_entry("Dump Boot 0/1", WHITE, (int (*)(void *))menu_open, yn_menu_backup_boot);
	menu_entry_t *back_entry = create_menu_entry("Back", WHITE, cancel, NULL);

	menu_append_entry(dump_menu, nand_entry);
	menu_append_entry(dump_menu, boot_entry);
	menu_append_entry(dump_menu, back_entry);

	return dump_menu;
}

int printHello(void *param)
{
	gfx_printf(&gfx_con, "%s\n", (char *)param);
	usleep(1000000);
	return 0;
}
