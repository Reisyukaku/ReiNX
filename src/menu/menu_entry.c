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