#pragma once

#include "hwinit/types.h"

typedef struct kipdiff_s {
  u64 offset;              // offset from start of kip's .text segment
  u32 len;                 // length of below strings, NULL signifies end of patch
  const char *orig_bytes;  // original byte string (this must match exactly)
  const char *patch_bytes; // replacement byte string (same length)
} kipdiff_t;

// a single patch for a particular kip version
typedef struct kippatch_s {
  const char *name;        // name/id of the patch, NULL signifies end of patchset
  kipdiff_t *diffs;        // array of kipdiff_t's to apply
} kippatch_t;

// a group of patches that patch several different things in a particular kip version
typedef struct kippatchset_s {
  const char *kip_name;    // name/id of the kip, NULL signifies end of patchset list
  const char *kip_hash;    // sha256 of the right version of the kip
  kippatch_t *patches;     // set of patches for this version of the kip
} kippatchset_t;

// fs

extern kippatchset_t kip_patches[];

int kippatch_apply(u8 *kipdata, u64 kipdata_len, kippatch_t *patch);
int kippatch_apply_set(u8 *kipdata, u64 kipdata_len, kippatchset_t *patchset, char **filter);
kippatchset_t *kippatch_find_set(u8 *kiphash, kippatchset_t *patchsets);
