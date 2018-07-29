#pragma once
#include "hwinit.h"

#define PKG2_MAGIC 0x31324B50
#define PKG2_SEC_BASE 0x80000000
#define PKG2_SEC_KERNEL 0
#define PKG2_SEC_INI1 1

#define INI1_MAGIC 0x31494E49

static u8 customSecmon = 0;
static u8 customWarmboot = 0;
static u8 customKern = 0;

typedef struct _pkg2_hdr_t
{
	u8 ctr[0x10];
	u8 sec_ctr[0x40];
	u32 magic;
	u32 base;
	u32 pad0;
	u16 version;
	u16 pad1;
	u32 sec_size[4];
	u32 sec_off[4];
	u8 sec_sha256[0x80];
	u8 data[];
} pkg2_hdr_t;

typedef struct _pkg2_ini1_t
{
	u32 magic;
	u32 size;
	u32 num_procs;
	u32 pad;
} pkg2_ini1_t;

typedef struct _pkg2_kip1_sec_t
{
	u32 offset;
	u32 size_decomp;
	u32 size_comp;
	u32 attrib;
} pkg2_kip1_sec_t;

#define KIP1_NUM_SECTIONS 6

typedef struct _pkg2_kip1_t
{
	u32 magic;
	char name[12];
	u64 tid;
	u32 proc_cat;
	u8 main_thrd_prio;
	u8 def_cpu_core;
	u8 res;
	u8 flags;
	pkg2_kip1_sec_t sections[KIP1_NUM_SECTIONS];
	u32 caps[0x20];
	u8 data[];
} pkg2_kip1_t;

typedef struct _pkg2_kip1_info_t
{
	pkg2_kip1_t *kip1;
	u32 size;
	link_t link;
} pkg2_kip1_info_t;

typedef struct {
	const char *id;
	u32 kb;
	u32 tsec_off;
	u32 pkg11_off;
	u32 sec_map[3];
	u32 secmon_base;
	u32 warmboot_base;
	int set_warmboot;
} pk11_offs;

typedef struct {
	u32 magic;
	u32 wb_size;
	u32 wb_off;
	u32 pad;
	u32 ldr_size;
	u32 ldr_off;
	u32 sm_size;
	u32 sm_off;
} pk11_header;

pkg2_hdr_t *unpackFirmwarePackage(u8 *data);
void pkg1_unpack(pk11_offs *offs, u8 *pkg1);
void buildFirmwarePackage(u8 *kernel, u32 kernel_size, link_t *kips_info);
size_t calcKipSize(pkg2_kip1_t *kip1);
void pkg2_parse_kips(link_t *info, pkg2_hdr_t *pkg2);
void loadKip(link_t *info, char *path);