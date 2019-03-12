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

#pragma once

#include "hwinit.h"
#include "fs.h"
#include "package.h"
#include "error.h"

void patchFS(pkg2_kip1_info_t* ki);
void patchWarmboot(u32 warmbootBase);
void patchSecmon(u32 secmonBase, u32 fw);
void patchKernel(pkg2_hdr_t *pkg2);
void patchKernelExtensions(link_t *kips);
pkg2_kip1_info_t* find_by_tid(link_t* kip_list, u64 tid);