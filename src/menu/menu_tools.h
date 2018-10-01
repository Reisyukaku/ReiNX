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
#ifndef _MENU_TOOLS_H_
#define _MENU_TOOLS_H_

#include "../hwinit.h"
#include "../fs.h"
#include "../error.h"

int is_autorcm_enabled(void);
int enable_autorcm();
int disable_autorcm();

int power_off();
int reboot_rcm();
int reboot_normal();

int check_burnt_fuses();

#endif