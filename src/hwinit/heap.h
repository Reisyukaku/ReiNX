/*
* Copyright (c) 2018 naehrwert
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

#ifndef _HEAP_H_
#define _HEAP_H_

#include "types.h"

typedef struct _hnode
{
	int used;
	u32 size;
	struct _hnode *prev;
	struct _hnode *next;
} hnode_t;

typedef struct _heap
{
	u32 start;
	hnode_t *first;
} heap_t;

void heap_init(u32 base);
void *malloc(u32 size);
void *calloc(u32 num, u32 size);
void free(void *buf);
void *memalign(u32 align, u32 size);
void *realloc(void * ptr, size_t size);

#endif
