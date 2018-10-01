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
#include "menu_entry.h"

menu_entry_t *create_menu_entry(const char *text, u32 color, int (*handler)(void *), void *param)
{
	menu_entry_t *menu_entry = (menu_entry_t *)malloc(sizeof(menu_entry_t));
	strcpy(menu_entry->text, text);
	menu_entry->color = color;
	menu_entry->handler = handler;
	menu_entry->param = param;
	return menu_entry;
}

int cancel(void *param)
{
	return -1;
}