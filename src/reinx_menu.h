#ifndef _REINX_MENU_H_
#define _REINX_MENU_H_

#include "menu/menu.h"
#include "menu/menu_entry.h"

menu_t* yn_menu_res_nand;
menu_t* yn_menu_res_boot;
menu_t* yn_menu_backup_nand;
menu_t* yn_menu_backup_boot;

void init_reinx_menu(void);

#endif