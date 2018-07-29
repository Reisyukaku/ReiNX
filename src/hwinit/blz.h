#ifndef _BLZ_H_
#define _BLZ_H_

#include <string.h>
#include "types.h"
#include "heap.h"

typedef struct compress_info {
	u16 windowpos;
	u16 windowlen;
	s16 * offtable;
	s16 * reverse_offtable;
	s16 * bytetable;
	s16 * endtable;
} compress_info;

typedef struct compfooter {
	u32 compressed_size;
	u32 init_index;
	u32 uncompressed_addl_size;
} compfooter;

int search(compress_info * info, const u8 * psrc, int * offset, int maxsize);
void slidebyte(compress_info * info, const u8 * psrc);
void slide(compress_info * info, const u8 * psrc, int size);
u8 * blz_decompress(u8 *compressed, u32 size);
u8 * blz_compress(u8 *decompressed, u32 * isize);

#endif
