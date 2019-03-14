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

    print("Patching FS\n");

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
    u8 *kipDecompText = blz_decompress(moddedKip->data, moddedKip->sections[0].size_comp);

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

void patchWarmboot(u32 warmbootBase) {
    //Patch warmboot
    if(!hasCustomWb()) {
        print("Patching Warmboot...\n");
        uPtr *fuseCheck = NULL;
        uPtr *segmentID = NULL;
        u8 fuseCheckPat[] = {0x44, 0x12, 0x80, 0xE5};
        u8 segmentIDPat[] = {0x60, 0x03, 0x91, 0xE5};
        fuseCheck = (uPtr*)(memsearch((void *)warmbootBase, 0x1000, fuseCheckPat, sizeof(fuseCheckPat)) + 20);
        segmentID = (uPtr*)(memsearch((void *)warmbootBase, 0x1000, segmentIDPat, sizeof(segmentIDPat)) + 12);

        *fuseCheck = NOP_v7;
        if(segmentID != NULL)
            *segmentID = NOP_v7;
    }else{
        print("Using custom warmboot.\n");
    }
}

void patchSecmon(u32 secmonBase, u32 fw){
    //Patch Secmon
    if(!hasCustomSecmon()){
        print("Patching Secmon...\n");
        uPtr *rlc_ptr = NULL;
        uPtr *ver_ptr = NULL;
        uPtr *pk21_ptr = NULL;
        uPtr *hdrsig_ptr = NULL;
        uPtr *sha2_ptr = NULL;

        //Version
        switch(fw) {
            //case KB_FIRMWARE_VERSION_100:
            case KB_FIRMWARE_VERSION_200: {
                u8 verPattern[] = {0x19, 0x00, 0x36, 0xE0, 0x03, 0x08, 0x91};
                ver_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, verPattern, sizeof(verPattern)) + 0xB);
                break;
            }
            case KB_FIRMWARE_VERSION_300:
            case KB_FIRMWARE_VERSION_301: {
                u8 verPattern[] = {0x2B, 0xFF, 0xFF, 0x97, 0x40, 0x19, 0x00, 0x36};
                ver_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, verPattern, sizeof(verPattern)) + 0x4);
                break;
            }
            case KB_FIRMWARE_VERSION_400: {
                u8 verPattern[] = {0x00, 0x01, 0x00, 0x36, 0xFD, 0x7B, 0x41, 0xA9};
                ver_ptr = (uPtr*)memsearch((void *)secmonBase, 0x10000, verPattern, sizeof(verPattern));
                break;
            }
            default:{
                u8 verPattern[] = {0x00, 0x01, 0x00, 0x36, 0xFD, 0x7B, 0x41, 0xA9};
                ver_ptr = (uPtr*)memsearch((void *)secmonBase, 0x10000, verPattern, sizeof(verPattern));
                break;
            }
        }

        //header sig
        switch(fw) {
            //case KB_FIRMWARE_VERSION_100:
            case KB_FIRMWARE_VERSION_200: {
                u8 hdrSigPattern[] = {0xFF, 0x97, 0xC0, 0x00, 0x00, 0x34, 0xA1, 0xFF, 0xFF};
                hdrsig_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x3A);
                break;
            }
            case KB_FIRMWARE_VERSION_300:
            case KB_FIRMWARE_VERSION_301: {
                u8 hdrSigPattern[] = {0xF7, 0xFE, 0xFF, 0x97, 0x80, 0x1E, 0x00, 0x36};
                hdrsig_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x4);
                break;
            }
            case KB_FIRMWARE_VERSION_400: {
                u8 hdrSigPattern[] = {0xE0, 0x03, 0x13, 0xAA, 0x4B, 0x28, 0x00, 0x94};
                hdrsig_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x8);
                break;
            }
            case KB_FIRMWARE_VERSION_500: {
                u8 hdrSigPattern[] = {0x86, 0xFE, 0xFF, 0x97, 0x80, 0x00, 0x00, 0x36};
                hdrsig_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x4);
                break;
            }
            default:{
                u8 hdrSigPattern[] = {0x9A, 0xFF, 0xFF, 0x97, 0x80, 0x00, 0x00, 0x36};
                hdrsig_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, hdrSigPattern, sizeof(hdrSigPattern)) + 0x4);
                break;
            }
        }

        //Sha2
        switch(fw) {
            //case KB_FIRMWARE_VERSION_100:
            case KB_FIRMWARE_VERSION_200: {
                u8 sha2Pattern[] = {0xE0, 0x03, 0x08, 0x91, 0xE1, 0x03, 0x13, 0xAA};
                sha2_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, sha2Pattern, sizeof(sha2Pattern)) + 0x10);
                break;
            }
            case KB_FIRMWARE_VERSION_300:
            case KB_FIRMWARE_VERSION_301: {
                u8 sha2Pattern[] = {0x07, 0xFF, 0xFF, 0x97, 0xC0, 0x18, 0x00, 0x36};
                sha2_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, sha2Pattern, sizeof(sha2Pattern)) + 0x4);
                break;
            }
            case KB_FIRMWARE_VERSION_400: {
                u8 sha2Pattern[] = {0xD3, 0xD5, 0xFF, 0x97, 0xE0, 0x03, 0x01, 0x32};
                sha2_ptr = (uPtr*)memsearch((void *)secmonBase, 0x10000, sha2Pattern, sizeof(sha2Pattern));
                break;
            }
            case KB_FIRMWARE_VERSION_500: {
                u8 sha2Pattern[] = {0xF2, 0xFB, 0xFF, 0x97, 0xE0, 0x03};
                sha2_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, sha2Pattern, sizeof(sha2Pattern)));
                break;
            }
            case KB_FIRMWARE_VERSION_600: {
                u8 sha2Pattern[] = {0x81, 0x00, 0x80, 0x72, 0xB5, 0xFB, 0xFF, 0x97};
                sha2_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, sha2Pattern, sizeof(sha2Pattern)) + 0x4);
                break;
            }
            default:{
                u8 sha2Pattern[] = {0x81, 0x00, 0x80, 0x72, 0x3C, 0xFC, 0xFF, 0x97};
                sha2_ptr = (uPtr*)(memsearch((void *)secmonBase, 0x10000, sha2Pattern, sizeof(sha2Pattern)) + 0x4);
                break;
            }
        }

        //Pkg2
        switch(fw) {
            //case KB_FIRMWARE_VERSION_100:
            case KB_FIRMWARE_VERSION_200: break;
            case KB_FIRMWARE_VERSION_300:
            case KB_FIRMWARE_VERSION_301: {
                u8 pk21Pattern[] = {0x40, 0x19, 0x00, 0x36, 0xE0, 0x03, 0x08, 0x91};
                pk21_ptr = (uPtr*)memsearch((void *)secmonBase, 0x10000, pk21Pattern, sizeof(pk21Pattern));
                break;
            }
            case KB_FIRMWARE_VERSION_400: {
                u8 pk21Pattern[] = {0xE0, 0x00, 0x00, 0x36, 0xE0, 0x03, 0x13, 0xAA, 0x63};
                pk21_ptr = (uPtr*)memsearch((void *)secmonBase, 0x10000, pk21Pattern, sizeof(pk21Pattern));
                break;
            }
            default:{
                pk21_ptr = (uPtr*)((u32)ver_ptr - 0xC);
                break;
            }
        }

        if (fw > KB_FIRMWARE_VERSION_200) {
            *pk21_ptr = NOP_v8;
        };
        *ver_ptr = NOP_v8;
        *hdrsig_ptr = NOP_v8;
        *sha2_ptr = NOP_v8;
    }else{
        print("Using custom secmon.\n");
    }
}

void patchKernel(pkg2_hdr_t *pkg2){
    //Patch Kernel
    if(!hasCustomKern()) {
        print("Patching Kernel...\n");
        u8 hash[0x20];
        se_calc_sha256(hash, pkg2->data, pkg2->sec_size[PKG2_SEC_KERNEL]);
        uPtr kern = (uPtr)&pkg2->data;
        uPtr sendOff, recvOff, codeRcvOff, codeSndOff, svcVerifOff, svcDebugOff;

        int i; for(i = 0; i < sizeof(kernelInfo)/sizeof(KernelMeta); i++) {
            if(memcmp(hash, kernelInfo[i].Hash, 0x20)) continue;
            print("Patching kernel %d\n", i);

            //ID Send
            uPtr freeSpace = getFreeSpace((void*)(kern+0x45000), 0x200, 0x20000) + 0x45000;     //Find area to write payload
            print("Kernel Freespace: 0x%08X\n", freeSpace);
            size_t payloadSize;
            u32 *sndPayload = getSndPayload(i, &payloadSize);
            *(vu32*)(kern + kernelInfo[i].SendOff) = _B(kernelInfo[i].SendOff, freeSpace);      //write hook to payload
            memcpy((void*)(kern + freeSpace), sndPayload, payloadSize);                         //Copy payload to free space
            *(vu32*)(kern + freeSpace + payloadSize) = _B(freeSpace + payloadSize, kernelInfo[i].SendOff + kernelInfo[i].CodeSndOff);  //Jump back skipping the hook

            //ID Receive
            freeSpace += (payloadSize+4);
            u32 *rcvPayload = getRcvPayload(i, &payloadSize);
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
    }else{
        print("Using custom kernel.\n");
    }
}

void patchKernelExtensions(link_t *kips){
    print("Patching KIPs...\n");
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