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