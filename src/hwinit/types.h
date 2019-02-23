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

#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define OFFSET_OF(t, m) ((u32)&((t *)NULL)->m)
#define CONTAINER_OF(mp, t, mn) ((t *)((u32)mp - OFFSET_OF(t, mn)))

#define R_FAILED(res)   ((res)!=0)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef volatile uint8_t vu8;
typedef volatile uint16_t vu16;
typedef volatile uint32_t vu32;
typedef volatile uint64_t vu64;

typedef uintptr_t uPtr;

enum KB_FIRMWARE_VERSION {
	KB_FIRMWARE_VERSION_100 = 0,
    KB_FIRMWARE_VERSION_200 = 0,
	KB_FIRMWARE_VERSION_300 = 1,
	KB_FIRMWARE_VERSION_301 = 2,
	KB_FIRMWARE_VERSION_400 = 3,
	KB_FIRMWARE_VERSION_500 = 4,
    KB_FIRMWARE_VERSION_600 = 5,
    KB_FIRMWARE_VERSION_620 = 6,
    KB_FIRMWARE_VERSION_700 = 7
};

#endif
