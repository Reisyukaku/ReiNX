#include "blz.h"
#include "../package.h"

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

int result = 0;
u8 * blz_compress(u8 *decompressed, u32 * isize) {
	result = 1;
	u8 * dest = malloc(*isize+1);
	u32 classic_size = *isize;

	u32 comp_size = *isize;
	
	if(*isize > sizeof(compfooter) && comp_size >= *isize) {
		
		u32 bufsize = (4098 + 4098 + 256 + 256) * sizeof(s16);
		u8 workbuf[bufsize];
		
		u32 headerSize = 4;
		do {
			compress_info info;
			init_info(&info, workbuf);
			const int maxsize = 0xF + 3;
			const u8 * psrc = decompressed + *isize;
			u8 * pdst = dest + *isize;
			while(psrc - decompressed > 0 && pdst - dest > 0) {
				u8 * pflag = --pdst;
				*pflag = 0;
				for(int i=0; i<8; i++) {
					int noff = 0;
					u32 t1 = MIN(maxsize, psrc - decompressed);
					t1 = MIN(t1, decompressed + *isize - psrc);
					int nsize = search(&info, psrc, &noff, t1);
					if(nsize <3) {
						if(pdst - dest < 1){
							result = -1;
							break;
						}
						slide(&info, psrc, 1);
						*--pdst = *--psrc;
					}
					else {
						if(pdst - dest < 2) {
							result = -2;
							break;
						}
						*pflag |= 0x80 >> i;
						slide(&info, psrc, nsize);
						psrc -= nsize;
						nsize -= 3;
						*--pdst = (nsize << 4 & 0xF0) | ((noff -3) >> 8 & 0x0F);
						*--pdst = (noff - 3) & 0xFF;
					}
					if(psrc - decompressed <= 0)
						break;
				}
				if(!result)
					break;
				
			}
			if(!result)
				break;
			comp_size = dest + *isize - pdst;
		} while(0);
	}
	else
		result = -3;
	if(result>0) {
		u32 origsize = *isize;
		u8 *compbuffer = dest + origsize - comp_size;
		u32 compbuffersize = comp_size;
		u32 origsafe = 0;
		u32 compresssafe = 0;
		int over =0;
		while(origsize > 0) {
			u8 flag = compbuffer[--compbuffersize];
			for(int i=0; i<8; i++) {
				if((flag << i & 0x80)==0) {
					compbuffersize--;
					origsize--;
				}
				else {
					int nsize = (compbuffer[--compbuffersize] >> 4 & 0x0F) + 3;
					compbuffersize--;
					origsize -= nsize;
					if(origsize < compbuffersize) {
						origsafe = origsize;
						compresssafe = compbuffersize;
						over = 1;
						break;
					}
					
				}
				if(origsize <=0)
					break;
			}
			if(over)
				break;

		}
		u32 fcompsize = comp_size - compresssafe;
		u32 padoffset = origsafe + fcompsize;
		u32 compfooteroff = (u32) Align(padoffset, 4);
		comp_size = compfooteroff + sizeof(compfooter);
		u32 top = comp_size - origsafe;
		u32 bottom = comp_size - padoffset;
		if(comp_size >= classic_size || top > 0xFFFFFF){
			result = 0;
		}
		else {
			memcpy(dest, decompressed, origsafe);
			memmove(dest + origsafe, compbuffer + compresssafe, fcompsize);
			memset(dest + padoffset, 0xFF, compfooteroff - padoffset);
			compfooter * fcompfooter = (compfooter *) (dest + compfooteroff);
			fcompfooter->compressed_size = top;
			fcompfooter->init_index = comp_size - padoffset;
			fcompfooter->uncompressed_addl_size = classic_size - comp_size;
		}
		*isize = comp_size;
		return dest;
		
	}
}

u8 * blz_decompress(u8 *compressed, u32 size) {
	
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
	u32 delta = size - compressed_size;
	if(size!=compressed_size) {
		memcpy(decomp, compressed, delta);
		decomp += delta;
		compressed +=delta;
	}
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
	return decomp - delta;
}