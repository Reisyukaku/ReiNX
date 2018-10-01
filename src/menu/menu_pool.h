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
#ifndef _MENU_POOL_H_
#define _MENU_POOL_H_

#include "menu.h"

typedef struct {
    int max_items;
    int current_items;
    menu_t** menus;
} menu_pool_t;

menu_pool_t* menu_pool;

void pool_init();
void push_to_pool(menu_t * menu);
void pool_cleanup();

#endif