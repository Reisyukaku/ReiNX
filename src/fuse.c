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

#include "hwinit/t210.h"
#include "fuse.h"

u32 get_unknown_config() {
    u32 res = 0;
    u32 deviceInfo = FUSE(FUSE_RESERVED_ODMX(4));
    u32 config = ((deviceInfo & 4u) >> 2) | 2 * ((deviceInfo & 0x100u) >> 8);
    
    if(config == 1)
        return 0;
    if(config == 2)
        return 1;
    if(config || (res = FUSE(FUSE_SPARE_BIT_5)) != 0)
        res = 3;
    return res;
}

u32 get_unit_type() {
    u32 deviceInfo = FUSE(FUSE_RESERVED_ODMX(4));
    u32 deviceType = deviceInfo & 3 | 4 * ((deviceInfo & 0x200u) >> 9);
    
    if(deviceType == 3)
        return 0;
    if(deviceType == 4)
        return 1;
    return 2;
}

u32 master_key_ver() {
    return FUSE(FUSE_SPARE_BIT_5) & 1;
}