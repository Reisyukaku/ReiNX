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

#ifndef _UTIL_H_
#define _UTIL_H_

#include "types.h"

#define byte_swap_32(num) ((num>>24)&0xff) | ((num<<8)&0xff0000) | \
						((num>>8)&0xff00) | ((num<<24)&0xff000000) \

typedef struct _cfg_op_t
{
	u32 off;
	u32 val;
} cfg_op_t;

u32 get_tmr_s();
u32 get_tmr_ms();
u32 get_tmr_us();
void musleep(u32 milliseconds);
void usleep(u32 microseconds);
void exec_cfg(u32 *base, const cfg_op_t *ops, u32 num_ops);
uPtr memsearch(const u8 *startPos, u32 searchSize, const void *pattern, u32 patternSize);
uPtr getFreeSpace(void *start, size_t space, size_t searchSize);
u32 crc32c(const void *buf, u32 len);

#endif