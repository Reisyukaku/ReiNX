/*
 * Copyright (c) 2018 naehrwert
 * Copyright (c) 2018 M4xw
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

#include <string.h>
#include "heap.h"

static void _heap_create(heap_t *heap, u32 start)
{
	heap->start = start;
	heap->first = NULL;
}

static u32 _heap_alloc(heap_t *heap, u32 size, u32 alignment)
{
	hnode_t *node, *new;
	int search = 1;

	size = ALIGN(size, alignment);

	if (!heap->first)
	{
		node = (hnode_t *)heap->start;
		node->used = 1;
		node->size = size;
		node->prev = NULL;
		node->next = NULL;
		heap->first = node;

		return (u32)node + sizeof(hnode_t);
	}

	node = heap->first;
	while (search)
	{
		if (!node->used && size + sizeof(hnode_t) < node->size)
		{
			new = (hnode_t *)((u32)node + sizeof(hnode_t) + size);

			new->size = node->size - sizeof(hnode_t) - size;
			node->size = size;
			node->used = 1;
			new->used = 0;
			new->next = node->next;
			new->prev = node;
			node->next = new;

			return (u32)node + sizeof(hnode_t);
		}
		if (node->next)
			node = node->next;
		else
			search = 0;
	}

	new = (hnode_t *)((u32)node + sizeof(hnode_t) + node->size);
	new->used = 1;
	new->size = size;
	new->prev = node;
	new->next = NULL;
	node->next = new;

	return (u32)new + sizeof(hnode_t);
}

static void _heap_free(heap_t *heap, u32 addr)
{
	hnode_t *node = (hnode_t *)(addr - sizeof(hnode_t));
	node->used = 0;
	node = heap->first;
	while (node)
	{
		if (!node->used)
		{
			if (node->prev && !node->prev->used)
			{
				node->prev->size += node->size + sizeof(hnode_t);
				node->prev->next = node->next;
				if (node->next)
					node->next->prev = node->prev;
			}
		}
		node = node->next;
	}
}

heap_t _heap;

void heap_init(u32 base)
{
	_heap_create(&_heap, base);
}

void *malloc(u32 size)
{
	return (void *)_heap_alloc(&_heap, size, 0x10);
}

void *memalign(u32 align, u32 size)
{
	return (void *)_heap_alloc(&_heap, size, align);
}

void *calloc(u32 num, u32 size)
{
	void *res = (void *)_heap_alloc(&_heap, num * size, 0x10);
	memset(res, 0, num * size);
	return res;
}

void free(void *buf)
{
	if ((buf != NULL) || ((u32)buf > (_heap.start - 1)))
		_heap_free(&_heap, (u32)buf);
}
void *realloc (void * ptr, size_t size) {
	hnode_t *node = (hnode_t *) ptr - 1;
	void *new = malloc(size);
	if (new && ptr) {
		memcpy(new, ptr, node -> size);
		free(ptr);
	}
	return new;
}