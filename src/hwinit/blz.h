#ifndef _BLZ_H_
#define _BLZ_H_

#include <string.h>
#include "types.h"
#include "heap.h"


typedef struct kipseg {
	u32 loc;
	u32 size;
	u32 filesize;
} kipseg;

typedef struct {
    u8  magic[4];
    u8  name[0xC];
    u64 title_id;
    u32 process_category;
    u8  thread_priority;
    u8  cpu_id;
    u8  unk;
    u8  flags;
    kipseg segments[6];
    u32 capabilities[0x20];   
} kiphdr;

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
char * blz_decompress(unsigned char * compressed, u32 * isize);
char * blz_compress(unsigned char * decompressed, u32 * isize);
char * kip_decomp(char * bytes, u32 * sz);
char * kip_comp(char * bytes, u32 * sz);


#endif
