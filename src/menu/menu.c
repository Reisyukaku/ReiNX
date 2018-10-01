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

#include "menu.h"
#include "menu_pool.h"

menu_t *menu_create(const char *title)
{
	menu_t *menu = (menu_t *)malloc(sizeof(menu_t));
	strcpy(menu->title, title);
	menu->next_entry = 0;
	menu->selected_index = 0;
    push_to_pool(menu);
	return menu;
}

menu_t *create_yes_no_menu(const char *action,
						   int (*on_yes)(void *), void *on_yes_param,
						   int (*on_no)(void *), void *on_no_param)
{
	char buffer[0x100];
	strcpy(buffer, "Do you want to ");
	strcat(buffer, action);
	menu_t *menu = menu_create(buffer);

	// Create yes entry
	menu_entry_t *yes_entry = create_menu_entry("Yes", 0xFF, on_yes, on_yes_param);

	// Create no entry
	menu_entry_t *no_entry = create_menu_entry("No", 0xFF, on_no, on_no_param);

	menu_append_entry(menu, yes_entry);
	menu_append_entry(menu, no_entry);

    push_to_pool(menu);
	return menu;
}

void menu_append_entry(menu_t *menu, menu_entry_t *menu_entry)
{
	if (menu->next_entry == MAX_ENTRIES)
		return;

	menu->entries[menu->next_entry] = menu_entry;
	menu->next_entry++;
}

void menu_draw(menu_t *menu)
{
	gfx_con_setpos(&gfx_con, 20, 50);

	gfx_con.fntsz = 16;
	gfx_printf(&gfx_con, "%k----- %s -----%k\n\n", 0xFFF45642, menu->title, WHITE);

	for (size_t i = 0; i < menu->next_entry; i++)
	{
		if (i == menu->selected_index)
		{
			gfx_printf(&gfx_con, "%k-> %k%s%k\n", WHITE, menu->entries[i]->color, menu->entries[i]->text, WHITE);
		}
		else if (menu->entries[i]->handler == NULL)
		{
			gfx_printf(&gfx_con, "\n %k%s%k\n", menu->entries[i]->color, menu->entries[i]->text, WHITE);
		}
		else
		{
			gfx_printf(&gfx_con, "%k-> %k%s%k\n", BLACK, menu->entries[i]->color, menu->entries[i]->text, WHITE);
		}
	}
}

void skip_null_handlers(menu_t *menu, int direction)
{
	while (menu->entries[menu->selected_index]->handler == NULL &&
		   ((direction > 0 && menu->selected_index < menu->next_entry - 1) ||
			(direction < 0 && menu->selected_index > 0)))
	{
		menu->selected_index += direction;
	}
	if (menu->entries[menu->selected_index]->handler == NULL)
		menu->selected_index -= direction;
}

int menu_update(menu_t *menu)
{
	menu_entry_t *entry = NULL;
	u32 input;

    menu_draw(menu);

    input = btn_wait();

	if ((input & BTN_VOL_UP) && menu->selected_index > 0)
	{
		menu->selected_index--;
		skip_null_handlers(menu, -1);
	}
	else if ((input & BTN_VOL_DOWN) && menu->selected_index < menu->next_entry - 1)
	{
		menu->selected_index++;
		skip_null_handlers(menu, 1);
	}
	else if (input & BTN_POWER)
	{
		entry = menu->entries[menu->selected_index];
		if (entry->handler != NULL)
		{
			gfx_clear_color(&gfx_ctxt, BLACK);
            gfx_con_setpos(&gfx_con, 20, 50);
			if (entry->handler(entry->param) != 0)
				return 0;

            gfx_clear_color(&gfx_ctxt, BLACK);
            menu_draw(menu);
		}
	}
	return 1;
}

int menu_open(menu_t *menu)
{
	skip_null_handlers(menu, 1);
	while (menu_update(menu))
		;

	return 0;
}

void menu_destroy(menu_t *menu)
{
	for (int i = 0; i < menu->next_entry; i++)
		free(menu->entries[i]);
	free(menu->entries);
	free(menu);
}