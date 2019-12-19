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

#include <string.h>
#include "util.h"
#include "t210.h"


u32 get_tmr_s()
{
	return RTC(0x8); //RTC_SECONDS
}

u32 get_tmr_ms()
{
	// The registers must be read with the following order:
	// -> RTC_MILLI_SECONDS (0x10) -> RTC_SHADOW_SECONDS (0xC)
	return (RTC(0x10) | (RTC(0xC)<< 10));
}

u32 get_tmr_us()
{
	return TMR(0x10); //TIMERUS_CNTR_1US
}

void msleep(u32 milliseconds)
{
	u32 start = RTC(0x10) | (RTC(0xC)<< 10);
	while (((RTC(0x10) | (RTC(0xC)<< 10)) - start) <= milliseconds)
		;
}

void usleep(u32 microseconds)
{
	u32 start = TMR(0x10);
	while ((TMR(0x10) - start) <= microseconds)
		;
}

void exec_cfg(u32 *base, const cfg_op_t *ops, u32 num_ops)
{
	for(u32 i = 0; i < num_ops; i++)
		base[ops[i].off] = ops[i].val;
}

uPtr memsearch(void *startPos, size_t searchSize, void *pattern, size_t patternSize) {
    if(!searchSize) return 0;
    for (u8 *pos = (u8*)startPos; pos <= (u8*)startPos + searchSize - patternSize; pos++) {
        if (memcmp(pos, pattern, patternSize) == 0) return (uPtr)pos;
    }
    return 0;
}

//probably could be more optimized :<
uPtr getFreeSpace(void *start, size_t space, size_t searchSize) {
    for(int i = 0; i < searchSize; i++) {
        if(*(u8*)(start+i) == 0) {
            for(int j=0;j<space;j++) {
                if(*(u8*)(start+i+j) != 0) break;
                if(j==space-1) return (uPtr)i;
            }
        }
    }
    return 0;
}