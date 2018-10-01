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
#ifndef _MENU_H_
#define _MENU_H_

#include "../hwinit.h"
#include "../hwinit/types.h"
#include "menu_entry.h"

#define MAX_ENTRIES 0x10

typedef struct
{
	char title[0x100];
	int next_entry;
	int selected_index;
	menu_entry_t *entries[MAX_ENTRIES];
} menu_t;

menu_t *menu_create(const char *title);
void menu_append_entry(menu_t *menu, menu_entry_t *menu_entry);
void menu_draw(menu_t *menu);
int menu_update(menu_t *menu);
int menu_open(menu_t *menu);
menu_t *create_yes_no_menu(const char *action,
						   int (*on_yes)(void *), void *on_yes_param,
						   int (*on_no)(void *), void *on_no_param);
void menu_destroy(menu_t *menu);

#endif