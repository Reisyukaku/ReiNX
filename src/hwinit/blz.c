#include "blz.h"

s64 Align(s64 data, s64 alignment){
	return (data + alignment - 1) / alignment * alignment;
}

void init_info(compress_info * info, void * buf) {
	info->windowpos = 0;
	info->windowlen = 0;
	info->offtable = (s16 *)buf;
	info->reverse_offtable = (s16 *)(buf) + 4098;
	info->bytetable = (s16 *)(buf) + 4098*2;
	info->endtable = (s16 *)(buf)+4098*2+256;
	for(int i=0; i< 256; i++){
		info->bytetable[i] = -1;
		info->endtable[i] = -1;
	}
}

int search(compress_info * info, const u8 * psrc, int * noffset, int maxsize) {
		if(maxsize < 3)
			return 0;
		const u8 * psearch = NULL;
		int size = 2;
		const u16 windowpos = info->windowpos;
		const u16 windowlen = info->windowlen;
		s16 * reverse_offtable = info->reverse_offtable;
		for(s16 offset = info->endtable[*(psrc -1)]; offset != -1; offset = reverse_offtable[offset]) {
			if(offset < windowpos)
				psearch = psrc + windowpos - offset;
			else
				psearch = psrc + windowlen + windowpos - offset;
			if(psearch - psrc < 3)
				continue;
			if(*(psearch - 2) != *(psrc - 2) || *(psearch -3) != *(psrc -3))
				continue;
			int max = (int) MIN(maxsize, (psearch-psrc));
			int cursize = 3;
			while(cursize < max && *(psearch - cursize - 1) == *(psrc - cursize - 1))
				cursize++;
			if(cursize > size) {
				size = cursize;
				*noffset = (int) (psearch-psrc);
				if(size == maxsize)
					break;
			}
		
		}
		if(size < 3)
			return 0;
		return size;
}

void slidebyte(compress_info * info, const u8 * psrc) {
	u8 indata = *(psrc - 1);
	u16 insertoff = 0;
	const u16 windowpos = info->windowpos;
	const u16 windowlen = info->windowlen;
	s16 * offtable = info->offtable;
	s16 * reverse_offtable = info->reverse_offtable;
	s16 * bytetable = info->bytetable;
	s16 * endtable = info->endtable;
	if(windowlen == 4098) {
		u8 outdata = *(psrc + 4097);
		if((bytetable[outdata] = offtable[bytetable[outdata]]) == -1)
			endtable[outdata] = -1;
		else
			reverse_offtable[bytetable[outdata]] = -1;
		insertoff = windowpos;
			
	}
	else
		insertoff = windowlen;
	
	s16 noff = endtable[indata];
	if(noff == -1)
		bytetable[indata] = insertoff;
	else
		offtable[noff] = insertoff;
	endtable[indata] = insertoff;
	offtable[insertoff] = -1;
	reverse_offtable[insertoff] = noff;
	if(windowlen == 4098)
		info->windowpos = (windowpos + 1) % 4098;
	else
		info->windowlen++;
}

void slide(compress_info * info, const u8 * psrc, int size) {
	for(int i=0; i<size; i++)
		slidebyte(info, psrc--);
}

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

char * kip_comp(char * bytes, u32 * sz) {
	kiphdr header;
	kipseg * text_h;
	kipseg * ro_h;
	kipseg * data_h;
	memcpy(&header, bytes, 0x100);
	if(strncmp(header.magic, "KIP1", 4)) {
		return NULL;
	}
	text_h = &header.segments[0];
	ro_h = &header.segments[1];
	data_h = &header.segments[2];
	
	u32 toff = sizeof(kiphdr);
	u32 roff = toff + text_h->filesize;
	u32 doff = roff + ro_h->filesize;
	u32 bsssize;
	memcpy(&bsssize, bytes+0x18, 4);
	char * text = malloc(text_h->filesize+1);
	memcpy(text, bytes+toff, text_h->filesize);
	char * ro = malloc(ro_h->filesize+1);
	memcpy(ro, bytes+roff, ro_h->filesize);
	char * data = malloc(data_h->filesize+1);
	memcpy(data, bytes+doff, data_h->filesize);
	
	text = blz_compress(text, &text_h->filesize);
	ro = blz_compress(ro, &ro_h->filesize);
	data = blz_compress(data, &data_h->filesize);
	
	u32 totalsize = sizeof(kiphdr)+text_h->filesize+ro_h->filesize+data_h->filesize;
	char * out = malloc(totalsize+1);
	
	header.flags |= 7; //set first 3 bits to 1
	
	memcpy(out, &header, sizeof(kiphdr));
	memcpy(out+sizeof(kiphdr), text, text_h->filesize);
	memcpy(out+sizeof(kiphdr)+text_h->filesize, ro, ro_h->filesize);
	memcpy(out+sizeof(kiphdr)+text_h->filesize+ro_h->filesize,data, data_h->filesize); 
	
	*sz = totalsize;
	return out;
}


char * kip_decomp(char * bytes, u32 * sz) {
	kiphdr header;
	kipseg * text_h;
	kipseg * ro_h;
	kipseg * data_h;
	
	memcpy(&header, bytes, 0x100);
	if(strncmp(header.magic, "KIP1", 4)) {
		return NULL;
	}
	text_h = &header.segments[0];
	ro_h = &header.segments[1];
	data_h = &header.segments[2];

	u32 toff = sizeof(kiphdr);
	u32 roff = toff + text_h->filesize;
	u32 doff = roff + ro_h->filesize;
	u32 bsssize;
	memcpy(&bsssize, bytes+0x18, 4);
	char * text = malloc(text_h->filesize+1);
	memcpy(text, bytes+toff, text_h->filesize);
	char * ro = malloc(ro_h->filesize+1);
	memcpy(ro, bytes+roff, ro_h->filesize);
	char * data = malloc(data_h->filesize+1);
	memcpy(data, bytes+doff, data_h->filesize);
	
	text = blz_decompress(text, &text_h->filesize);
	ro = blz_decompress(ro, &ro_h->filesize);
	data = blz_decompress(data, &data_h->filesize);
		
	u32 totalsize = sizeof(kiphdr)+text_h->filesize+ro_h->filesize+data_h->filesize;
	char * out = malloc(totalsize+1);
	
	header.flags &= ~7;  //AND NOT 7 = zero first 3 bits
	
	memcpy(out, &header, sizeof(kiphdr));
	memcpy(out+sizeof(kiphdr), text, text_h->filesize);
	memcpy(out+sizeof(kiphdr)+text_h->filesize, ro, ro_h->filesize);
	memcpy(out+sizeof(kiphdr)+text_h->filesize+ro_h->filesize,data, data_h->filesize);  
	*sz = totalsize;
	return out;
}