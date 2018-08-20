/*
* Copyright (c) 2018 Reisyukaku
* Copyright (c) 2018 neonsea
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

#include <stddef.h> 
#include "splash.h"
#include "fs.h"
#include "hwinit.h"

int drawSplash() {
    // Draw splashscreen to framebuffer.
    if(fopen("/ReiNX/splash.bin", "rb") != 0) {
        fread((void*)0xC0000000, fsize(), 1);
        fclose();
        return 0;
    }
    return -1;
}

int loadBmp() {
    if (fopen("/ReiNX/splash.bmp", "rb") == 0)
        return -1;

    unsigned char * bmp;
    bmp = malloc(fsize());

    // Read data
    fread(bmp, fsize(), 1);
    fclose();

    // Get details of the image & validate them
    static int size, offset, width, height, colordep;
    size     = fsize();
    offset   = (bmp[11] << 8) | bmp[10];
    width    = (bmp[19] << 8) | bmp[18];
    height   = (bmp[23] << 8) | bmp[22];
    colordep =  bmp[28];

    if (width   != 720 
    || height   != 1280
    || colordep != 24) {
        free(bmp);
        return -1;
    }

    // Init destination for decoded .bmp
    unsigned char * out = malloc(0x3C0008);
    strncpy(out, "00921600", 8);

    // Swap bytes and pad where necessary
    int index = 8;
    int pad_counter = 0;
    for (int i = offset; i < size - offset - 2; i += 3){
        out[  index  ] = bmp[i + 2];
        out[index + 1] = bmp[i + 1];
        out[index + 2] = bmp[  i  ];
        out[index + 3] = 0;
        index += 4;
        pad_counter++;
        if (pad_counter == 720){
            index += 192;
            pad_counter = 0;
        }
    }

    // Save to file
    if(fopen("/ReiNX/splash.bin", "wb") != 0) {
        fwrite(out, 0x3C0008, 1);
        fclose();
    }
    
    // Finish
    free(out);
    free(bmp);
    drawSplash();
    return 0;
}

void splash() {
    if (drawSplash() != 0){
        // Failed to open splash.bin, try .bmp
        loadBmp();
    }
}