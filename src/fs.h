#pragma once

u32 sd_mount();
u32 fopen(const char *path, const char *mode);
u32 fread(void *buf, size_t size, size_t ntimes);
u32 fwrite(void *buf, size_t size, size_t ntimes);
size_t fsize();
void fclose();
size_t enumerateDir(char ***output, char *path, char *pattern);
