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

#include "patches.h"

void patchFS(pkg2_kip1_info_t* ki) {
    print("%kPatching FS%k\n", WHITE, DEFAULT_TEXT_COL);

    u8 kipHash[0x20];

    se_calc_sha256(&kipHash, ki->kip1, ki->size);
    se_calc_sha256(&kipHash, ki->kip1, ki->size);

    //Create header
    size_t sizeDiff = ki->kip1->sections[0].size_decomp - ki->kip1->sections[0].size_comp;

    size_t newSize = ki->size + sizeDiff;
    pkg2_kip1_t *moddedKip = malloc(newSize);
    memcpy(moddedKip, ki->kip1, newSize);

    u32 pos = 0;
    //Get decomp .text segment

    u8 *kipDecompText = malloc(moddedKip->sections[0].size_decomp);
    if (!blz_uncompress_srcdest(moddedKip->data, moddedKip->sections[0].size_comp, kipDecompText, moddedKip->sections[0].size_decomp))
        while(1);

    kippatchset_t *pset = kippatch_find_set(kipHash, kip_patches);
    if (!pset) {
        print("could not find patchset with matching hash\n");
    } else {
        int res = kippatch_apply_set(kipDecompText, moddedKip->sections[0].size_decomp, pset);
        if (res) error("kippatch_apply_set() failed\n");
    }

    moddedKip->flags &= ~1;
    memcpy((void*)moddedKip->data, kipDecompText, moddedKip->sections[0].size_decomp);
    free(kipDecompText);
    pos += moddedKip->sections[0].size_comp;
    moddedKip->sections[0].size_comp = moddedKip->sections[0].size_decomp;

    for(int i = 1; i < KIP1_NUM_SECTIONS; i++) {
        if(moddedKip->sections[i].offset != 0) {
            memcpy((void*)moddedKip->data + pos + sizeDiff, (void*)ki->kip1->data + pos, moddedKip->sections[i].size_comp);
            pos += moddedKip->sections[i].size_comp;
        }
    }

    free(ki->kip1);
    ki->size = newSize;
    ki->kip1 = moddedKip;
}

void patchKernel(pkg2_hdr_t *pkg2){
    //Patch Kernel
    if(!hasCustomKern()) {
        print("%kPatching Kernel...%k\n", WHITE, DEFAULT_TEXT_COL);
        u8 hash[0x20];
        if(pkg2->sec_size[PKG2_SEC_INI1] == 0) {
            se_calc_sha256(hash, (void*)(pkg2->data + 0x800), GetNewKernIniStart() - 0x800); //TODO unhardcode
            //*((vu64 *)((uPtr)pkg2->data + 0x168)) = (u64)pkg2->sec_size[PKG2_SEC_KERNEL];
            printHex(hash, hash, 0x20);
        }else{
            se_calc_sha256(hash, pkg2->data, pkg2->sec_size[PKG2_SEC_KERNEL]);
        }
        uPtr kern = (uPtr)&pkg2->data;
        uPtr sendOff, recvOff, codeRcvOff, codeSndOff, svcVerifOff, svcDebugOff;
        
        bool foundKern = false;
        int i; for(i = 0; i < sizeof(kernelInfo)/sizeof(KernelMeta); i++) {
            if(memcmp(hash, kernelInfo[i].Hash, 8)) continue;
            print("Patching kernel %d\n", i);
            foundKern = true;
            print("Svc verify off: 0x%08X\n", kernelInfo[i].SvcVerify);
            print("Svc debug off: 0x%08X\n", kernelInfo[i].SvcDebug);
            print("Kernel ipc send hook off: 0x%08X\n", kernelInfo[i].SendOff);
            print("Kernel ipc recv hook off: 0x%08X\n", kernelInfo[i].RcvOff);
            print("Generic off: 0x%08X\n", kernelInfo[i].GenericOff);
            print("Kernel ipc send payload off: 0x%08X\n", kernelInfo[i].CodeSndOff);
            print("Kernel ipc recv payload off: 0x%08X\n", kernelInfo[i].CodeRcvOff);
            print("Kernel Freespace: 0x%08X\n", kernelInfo[i].Freespace);
            
            //Find free space
            u32 freeSpace = kernelInfo[i].Freespace;//getFreeSpace((void*)(kern+0x45000), 0x200, 0x20000) + 0x45000; 
            
            //ID Send
            size_t payloadSize = kernelInfo[i].SndPayloadSize;
            u32 *sndPayload = kernelInfo[i].SndPayload;
            *(vu32*)(kern + kernelInfo[i].SendOff) = _B(kernelInfo[i].SendOff, freeSpace);      //write hook to payload
            memcpy((void*)(kern + freeSpace), kernelInfo[i].SndPayload, payloadSize);                         //Copy payload to free space
            *(vu32*)(kern + freeSpace + payloadSize) = _B(freeSpace + payloadSize, kernelInfo[i].SendOff + kernelInfo[i].CodeSndOff);  //Jump back skipping the hook

            //ID Receive
            freeSpace += (payloadSize+4);
            payloadSize = kernelInfo[i].RcvPayloadSize;
            u32 *rcvPayload = kernelInfo[i].RcvPayload;
            *(vu32*)(kern + kernelInfo[i].RcvOff) = _B(kernelInfo[i].RcvOff, freeSpace);
            memcpy((void*)(kern + freeSpace), rcvPayload, payloadSize);
            *(vu32*)(kern + freeSpace + payloadSize) = _B(freeSpace + payloadSize, kernelInfo[i].RcvOff + kernelInfo[i].CodeRcvOff);

            //SVC patches
            *(vu32*)(kern + kernelInfo[i].SvcVerify) = NOP_v8;
            if (fopen("/ReiNX/debug", "rb")) {
                fclose();
                *(vu32*)(kern + kernelInfo[i].SvcDebug) = _MOVZX(8, 1, 0);
            }

            //JIT patches
            *(vu32*)(kern + kernelInfo[i].GenericOff) = NOP_v8;

            break;
        }
        if(!foundKern) {
            error("\nKernel hash doesnt match list!\n");
            printHex(kernelInfo[i].Hash, kernelInfo[i].Hash, 0x20);
        }
    }else{
        print("Using custom kernel.\n");
    }
}

void patchKernelExtensions(link_t *kips){
    print("%kPatching KIPs...%k\n", WHITE, DEFAULT_TEXT_COL);
    pkg2_kip1_info_t* FS_module = find_by_tid(kips, 0x0100000000000000);
    if(FS_module == NULL) {
        error("Could not find FS Module.\n");
    } else {
        patchFS(FS_module);
    }

    // Load all KIPs.
    char **sysmods = NULL;
    size_t cnt = enumerateDir(&sysmods, "/ReiNX/sysmodules", "*.kip");
    for (u32 i = 0; i < cnt ; i++) {
        print("%kLoading %s\n%k", YELLOW, sysmods[i], DEFAULT_TEXT_COL);
        loadKip(kips, sysmods[i]);
        free(sysmods[i]);
    }
    free(sysmods);
}

pkg2_kip1_info_t* find_by_tid(link_t* kip_list, u64 tid) {
    LIST_FOREACH_ENTRY(pkg2_kip1_info_t, ki, kip_list, link) {
        if(ki->kip1->tid == 0x0100000000000000)
            return ki;
    }
    return NULL;
}