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

#include "heap.h"
#include <string.h>
#include "util.h"
#include "t210.h"
#include "heap.h"

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

void musleep(u32 milliseconds)
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

uPtr memsearch(const u8 *startPos, u32 searchSize, const void *pattern, u32 patternSize) {
    if(!searchSize) return 0;
    for (u8 *pos = (u8 *)startPos; pos <= startPos + searchSize - patternSize; pos++) {
        if (memcmp(pos, pattern, patternSize) == 0) return (uPtr)pos;
    }
    return 0;
}


typedef struct lsf_head {
	u32 loc;
	u32 size;
	u32 filesize;
} lsf_head;


char * blz_decompress(unsigned char * compressed, u32 * isize) {
	u32 size = *isize;
	u32 compressed_size;
	u32 init_index;
	u32 uncompressed_addl_size;
	memcpy(&compressed_size, compressed+size - 0xC, 4);
	memcpy(&init_index, compressed+size - 0x8, 4);
	memcpy(&uncompressed_addl_size, compressed+size - 0x4, 4);
	
	
	u32 decompressed_size = size + uncompressed_addl_size;
	unsigned char * decomp = malloc(decompressed_size);
	memcpy(decomp, compressed, size);
	for(int i=size; i<decompressed_size; i++)
		decomp[i]=0x0;
	u32 index = compressed_size - init_index;
	
	u32 outindex = decompressed_size;
	while(outindex > 0) {
		index -= 1;
		unsigned char control;
		memcpy(&control, compressed+index,1);
		for(int i=0; i<8; i++) {
			if(control & 0x80) {
				if(index < 2) {
					//printf("ERROR: Compression out of bounds\n");
					return NULL;
				}
				index -= 2;
				unsigned short int segmentoffset = compressed[index] | (compressed[index+1] <<8);
				u32 segmentsize = ((segmentoffset >> 12) & 0xF) + 3;
				segmentoffset &= 0x0FFF;
				segmentoffset +=2;
				if(outindex < segmentsize) {
					//printf("ERROR: Compression out of bounds, outindex<segsize\n");
					return NULL;
				}
				for(int j=0; j<segmentsize; j++) {
					if(outindex +segmentoffset >= decompressed_size) {
						//printf("ERROR: Compression out of bounds, 3\n");
						return NULL;
					}
					char data = decomp[outindex+segmentoffset];
					outindex -= 1;
					decomp[outindex] = data;
				}
			}
			else{
				if(outindex < 1){
					//printf("ERROR: compression out of bounds, 4 \n");
					return NULL;
				}
				outindex -= 1;
				index -= 1;
				decomp[outindex] = compressed[index];
			}
			control <<= 1;
			control &= 0xFF;
			if(!outindex)
				break;
		}
	}
	*isize = decompressed_size;
	return decomp;
}

char * kipread(char * bytes, int * sz) {
	char magic[5];
	memcpy(magic, bytes, 4);
	magic[4]=0;
	if(strcmp(magic, "KIP1")) {
		//printf("KIP1 magic is missing, abort\n");
		return NULL;
	}
	lsf_head text_h;
	lsf_head ro_h;
	lsf_head data_h;
	
	memcpy(&text_h, bytes+0x20, 12);
	memcpy(&ro_h, bytes+0x30, 12);
	memcpy(&data_h, bytes+0x40, 12);
	u32 toff = 0x100;
	u32 roff = toff + text_h.filesize;
	u32 doff = roff + ro_h.filesize;
	u32 bsssize;
	memcpy(&bsssize, bytes+0x18, 4);
	char * text = malloc(text_h.filesize+1);
	memcpy(text, bytes+toff, text_h.filesize);
	char * ro = malloc(ro_h.filesize+1);
	memcpy(ro, bytes+roff, ro_h.filesize);
	char * data = malloc(data_h.filesize+1);
	memcpy(data, bytes+doff, data_h.filesize);
	text = blz_decompress(text, &text_h.filesize);
	ro=blz_decompress(ro, &ro_h.filesize);
	data=blz_decompress(data, &data_h.filesize);
	u32 totalsize = text_h.filesize+ro_h.filesize+data_h.filesize;
	char * out = malloc(totalsize+1);
	
	memcpy(out, text, text_h.filesize);
	memcpy(out+text_h.filesize, ro, ro_h.filesize);
	memcpy(out+text_h.filesize+ro_h.filesize,data, data_h.filesize); 
	*sz = totalsize;
	return out;
}