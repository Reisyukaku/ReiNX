/*
* Copyright (c) 2018 Reisyukaku
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

#include "hwinit/types.h"
#include "kippatches.h"
#include "fs.h"
#include "kippatches/fs.inc"

// TODO: get full hashes somewhere and not just the first 16 bytes
// every second one is the exfat version
kippatchset_t kip_patches[] = {
    { "FS", "\xde\x9f\xdd\xa4\x08\x5d\xd5\xfe\x68\xdc\xb2\x0b\x41\x09\x5b\xb4", fs_kip_patches_100 },
    { "FS", "\xfc\x3e\x80\x99\x1d\xca\x17\x96\x4a\x12\x1f\x04\xb6\x1b\x17\x5e", fs_kip_patches_100 },
    { "FS", "\xcd\x7b\xbe\x18\xd6\x13\x0b\x28\xf6\x2f\x19\xfa\x79\x45\x53\x5b", fs_kip_patches_200 },
    { "FS", "\xe7\x66\x92\xdf\xaa\x04\x20\xe9\xfd\xd6\x8e\x43\x63\x16\x18\x18", fs_kip_patches_200 },
    { "FS", "\x0d\x70\x05\x62\x7b\x07\x76\x7c\x0b\x96\x3f\x9a\xff\xdd\xe5\x66", fs_kip_patches_210 },
    { "FS", "\xdb\xd8\x5f\xca\xcc\x19\x3d\xa8\x30\x51\xc6\x64\xe6\x45\x2d\x32", fs_kip_patches_210 },
    { "FS", "\xa8\x6d\xa5\xe8\x7e\xf1\x09\x7b\x23\xda\xb5\xb4\xdb\xba\xef\xe7", fs_kip_patches_300 },
    { "FS", "\x98\x1c\x57\xe7\xf0\x2f\x70\xf7\xbc\xde\x75\x31\x81\xd9\x01\xa6", fs_kip_patches_300 },
    { "FS", "\x57\x39\x7c\x06\x3f\x10\xb6\x31\x3f\x4d\x83\x76\x53\xcc\xc3\x71", fs_kip_patches_301 },
    { "FS", "\x07\x30\x99\xd7\xc6\xad\x7d\x89\x83\xbc\x7a\xdd\x93\x2b\xe3\xd1", fs_kip_patches_301 },
    { "FS", "\x06\xe9\x07\x19\x59\x5a\x01\x0c\x62\x46\xff\x70\x94\x6f\x10\xfb", fs_kip_patches_401 },
    { "FS", "\x54\x9b\x0f\x8d\x6f\x72\xc4\xe9\xf3\xfd\x1f\x19\xea\xce\x4a\x5a", fs_kip_patches_401 },
    { "FS", "\x80\x96\xaf\x7c\x6a\x35\xaa\x82\x71\xf3\x91\x69\x95\x41\x3b\x0b", fs_kip_patches_410 },
    { "FS", "\x02\xd5\xab\xaa\xfd\x20\xc8\xb0\x63\x3a\xa0\xdb\xae\xe0\x37\x7e", fs_kip_patches_410 },
    { "FS", "\xa6\xf2\x7a\xd9\xac\x7c\x73\xad\x41\x9b\x63\xb2\x3e\x78\x5a\x0c", fs_kip_patches_500 },
    { "FS", "\xce\x3e\xcb\xa2\xf2\xf0\x62\xf5\x75\xf8\xf3\x60\x84\x2b\x32\xb4", fs_kip_patches_500 },
    { "FS", "\x76\xf8\x74\x02\xc9\x38\x7c\x0f\x0a\x2f\xab\x1b\x45\xce\xbb\x93", fs_kip_patches_510 },
    { "FS", "\x10\xb2\xd8\x16\x05\x48\x85\x99\xdf\x22\x42\xcb\x6b\xac\x2d\xf1", fs_kip_patches_510 },
    { NULL, NULL, NULL },
};

int kippatch_apply(u8 *kipdata, u64 kipdata_len, kippatch_t *patch) {
    if (!patch || !patch->diffs) return -1;

    for (kipdiff_t *diff = patch->diffs; diff->len; ++diff) {
        if (!diff->len || diff->offset + diff->len > kipdata_len)
            return 1 + (int)(diff - patch->diffs);
        u8 *start = kipdata + diff->offset;
        if (memcmp(start, diff->orig_bytes, diff->len))
            return 1 + (int)(diff - patch->diffs);
        // TODO: maybe start copying after every diff has been verified?
        memcpy(start, diff->patch_bytes, diff->len);
    }

    return 0;
}

int kippatch_apply_set(u8 *kipdata, u64 kipdata_len, kippatchset_t *patchset, char **filter) {
    for (kippatch_t *p = patchset->patches; p && p->name; ++p) {
        int found = 0;
        for (char **filtname = filter; filtname && *filtname; ++filtname) {
            if (!strcmp(p->name, *filtname)) {
                found = 1;
                break;
            }
        }

        if (filter && !found) continue;

        int r = kippatch_apply(kipdata, kipdata_len, p);
        if (r) return r;
    }

    return 0;
}

kippatchset_t *kippatch_find_set(u8 *kiphash, kippatchset_t *patchsets) {
    for (kippatchset_t *ps = patchsets; ps && ps->kip_name; ++ps) {
        if (!memcmp(kiphash, ps->kip_hash, 0x10)) return ps;
    }
    return NULL;
}
