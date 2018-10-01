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
#include "menu_pool.h"

void pool_init()
{
    menu_pool = (menu_pool_t *)malloc(sizeof(menu_pool_t));
    menu_pool->max_items = 0x16;
    menu_pool->current_items = 0;
    menu_pool->menus = (menu_t **)malloc(sizeof(menu_t *) * menu_pool->max_items);
}

void push_to_pool(menu_t *menu)
{
    if (menu != NULL)
    {
        if (menu_pool->current_items == menu_pool->max_items - 1)
        {
            // Resize the pool
            menu_pool->max_items = menu_pool->max_items << 1;
            menu_pool->menus = (menu_t **)realloc(menu_pool->menus, sizeof(menu_t *) * menu_pool->max_items);
        }
        menu_pool->menus[menu_pool->current_items] = menu;
        menu_pool->current_items++;
    }
}

void pool_cleanup()
{
    for (int i = 0; i < menu_pool->current_items; ++i)
        menu_destroy(menu_pool->menus[i]);
    free(menu_pool->menus);
    free(menu_pool);
}
