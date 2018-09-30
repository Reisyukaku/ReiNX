#ifndef _MENU_ENTRY_H
#define _MENU_ENTRY_H

#include <string.h>

#include "../hwinit.h"
#include "../hwinit/types.h"

typedef struct
{
	char text[0x100];
	u32 color;
	void *param;
	int (*handler)(void *);
} menu_entry_t;

menu_entry_t *create_menu_entry(const char *text, u32 color, int (*handler)(void *), void *param);
int cancel(void *param);

#endif
