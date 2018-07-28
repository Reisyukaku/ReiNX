#ifndef _KIP_H_
#define _KIP_H_

#include "hwinit/list.h"
#include "hwinit/sha256.h"
#include "hwinit/blz.h"

typedef struct kip_patch
{
	u32 off; //absolute decompressed offset
	u32 length; //bytes to be copied
	const char* source_data; //original data
	const char* patch_data; //patch data
} kip_patch;

typedef struct kip_patchlist
{
	const char* name; 
	kip_patch * patches;
} kip_patchlist;

typedef struct kip_id
{
	const char* name;
	u8 sha[16];
	kip_patchlist * patchlist;
} kip_id;


static kip_patch fs100_nosig[] =
{
	{ 0x194A0 , 4, "\xBA\x09\x00\x94", "\xE0\x03\x1F\x2A" },
	{ 0x3A79c, 4,  "\xE0\x06\x00\x36", "\x1F\x20\x03\xD5" },
	{0, 0, NULL, NULL}
};

static kip_patchlist fs_patch_100[] =
{
	{"nosig", fs100_nosig },
	{NULL, NULL}
};



static kip_id kip_id_list[] = 
{
	{ "FS", "\xde\x9f\xdd\xa4\x08\x5d\xd5\xfe\x68\xdc\xb2\x0b\x41\x09\x5b\xb4", fs_patch_100 }, // FS 1.0.0
	{ "FS", "\xfc\x3e\x80\x99\x1d\xca\x17\x96\x4a\x12\x1f\x04\xb6\x1b\x17\x5e", fs_patch_100 }, // FS 1.0.0 "exfat"
	{ NULL, "" , NULL}
};

int patch_apply(pkg2_kip1_info_t * ki, kip_patchlist plist[]) {
	u32 oldsize = ki->size;
	char * decomp = kip_decomp(ki->kip1->data, &ki->size);
	int i=0;
	while(plist[i].name!=NULL){
		int j=0;
		while(plist[i].patches[j].off!=0){
			struct kip_patch patch = plist[i].patches[j];
			if(!memcmp(decomp+patch.off, patch.source_data, patch.length)) {
				memcpy(decomp+patch.off, patch.patch_data, patch.length);
			}
			else{
				//printf("patch src does not match kip data\n");
				return -1;
			}
			j++;
		}
		
		
		i++;
	}
	
	
}

int patch_kips(link_t *info) {

	LIST_FOREACH_ENTRY(pkg2_kip1_info_t, ki, info, link) {
		struct sha256_state sha_ctx;
		u8 kip_hash[0x20];
		sha256_init(&sha_ctx);
		sha256_update(&sha_ctx, (u8 *)ki->kip1->data, ki->size);
		sha256_finalize(&sha_ctx);
		sha256_finish(&sha_ctx, kip_hash);
		int i=0;
		while(kip_id_list[i].name!=NULL) {
			if(!strcmp(kip_id_list[i].name, ki->kip1->name)) {
				if(!memcmp(kip_id_list[i].sha, kip_hash, 0x10)) {
					patch_apply(ki, kip_id_list[i].patchlist);
					//hashes are the same
				}
			}
			i++;
		}
		
	}
}


#endif