/*
* Copyright (c) 2018 Reisyukaku
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

#pragma once
#include <stddef.h>
#include <string.h>
#include "hwinit.h"
#include "hwinit/ff.h"
#include "error.h"

u32 sdMount();
void sdUnmount();
u32 fopen(const char *path, const char *mode);
u32 fread(void *buf, size_t size, size_t ntimes);
u32 fwrite(void *buf, size_t size, size_t ntimes);
u32 fseek(size_t off);
size_t fsize();
void fclose();
size_t enumerateDir(char ***output, char *path, char *pattern);
