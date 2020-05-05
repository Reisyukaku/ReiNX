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

#include "fs.h"

sdmmc_t sd_sdmmc;
sdmmc_storage_t sd_storage;
FATFS sd_fs;
int sd_mounted;
FIL fp;

u32 sdMount() {
    if (sd_mounted) return 1;

    if (sdmmc_storage_init_sd(&sd_storage, &sd_sdmmc, SDMMC_1, SDMMC_BUS_WIDTH_4, 11) && f_mount(&sd_fs, "", 1) == FR_OK) {
        sd_mounted = 1;
        return 1;
    }

    return 0;
}

void sdUnmount() {
    if (!sd_mounted) return;
    f_mount(NULL, "", 1);
    sdmmc_storage_end(&sd_storage);
    sd_mounted = 0;
}

u32 fopen(const char *path, const char *mode) {
    u32 m = (mode[0] == 0x77 ? (FA_WRITE|FA_CREATE_NEW) : FA_READ);
    if (f_open(&fp, path, m) != FR_OK) 
        return 0;
    return 1;
}

u32 fread(void *buf, size_t size, size_t ntimes) {
    u8 *ptr = buf;
    while (size > 0) {
        u32 rsize = MIN(ntimes * size, size);
        if (f_read(&fp, ptr, rsize, NULL) != FR_OK) {
            error("Failed read!\n");
            return 0;
        }

        ptr += rsize;
        size -= rsize;
    }
    return 1;
}

u32 fseek(size_t off) {
    if(f_lseek(&fp, off) != FR_OK) {
        error("Failed read!\n");
        return 0;
    }
    return 1;
}

u32 fwrite(void *buf, size_t size, size_t ntimes) {
    u8 *ptr = buf;
    while (size > 0) {
        u32 rsize = MIN(ntimes * size, size);
        if (f_write(&fp, ptr, rsize, NULL) != FR_OK) {
            error("Failed write!\n");
            return 0;
        }

        ptr += rsize;
        size -= rsize;
    }
    return 1;
}

size_t fsize() {
    return f_size(&fp);
}

void fclose() {
    f_close(&fp);
}

size_t enumerateDir(char ***output, char *path, char *pattern) {
    DIR dp;
    FILINFO fno;
    FRESULT fr;
    char **out = NULL;
    size_t pathlen = strlen(path);

    if (pathlen >= FF_LFN_BUF)
        goto end;

    fr = f_findfirst(&dp, &fno, path, pattern);

    char pathb[FF_LFN_BUF] = {0};
    strcpy(pathb, path);
    pathb[pathlen] = '/';

    int i = 0; 
    while (fno.fname[0] != 0 && fr == FR_OK) {
        if (fno.fname[0] == '.') goto next;  
        out = (char **)realloc(out, (i+1) * sizeof(char *));
        out[i] = (char *)malloc(FF_LFN_BUF);
        strcpy(out[i], pathb);
        strcat(out[i], fno.fname);
        pathb[pathlen+1] = 0;
        i++;
       next:
        f_findnext(&dp, &fno);
    }

    end:
    f_closedir(&dp);
    *output = out;
    return i;
}
