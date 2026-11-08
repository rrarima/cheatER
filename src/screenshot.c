/*
 * PSPLINK
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPLINK root for details.
 *
 * bitmap.c - PSPLINK kernel module bitmap code
 *
 * Copyright (c) 2005 James F <tyranid@gmail.com>
 *
 * $HeadURL: svn://svn.ps2dev.org/psp/trunk/psplink/psplink/bitmap.c $
 * $Id: bitmap.c 1622 2005-12-27 19:29:57Z tyranid $
 */

#include <pspdisplay.h>
#include <pspdisplay_kernel.h>
#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cheat.h"
#include "common.h"
#include "file_buffer.h"
#include "game_pause.h"
#include "plugin.h"
#include "screenshot.h"

unsigned char screen_time = 0;
unsigned int screen_no = 0;
unsigned char screen_path[64] = {0};

#define BMP_ID "BM"
static int screenshot_fd;

struct BitmapHeader {
    char id[2];
    uint32_t filesize;
    uint32_t reserved;
    uint32_t offset;
    uint32_t headsize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t comp;
    uint32_t bitmapsize;
    uint32_t hres;
    uint32_t vres;
    uint32_t colors;
    uint32_t impcolors;
} __attribute__((packed));

static int write_8888_data(void *frame, int pixel_data) {
    int line; /*uint8_t*/
    char line_output = 0;
    uint8_t *p;
    int i;
    int h;

    line = pixel_data;
    for (h = 271; h >= 0; h--) {
        p = frame + (h * 512 * 4);
        for (i = 0; i < 480; i++) {
            file_buffer[((i * 3) + 2)] = p[i * 4];
            file_buffer[((i * 3) + 1)] = p[(i * 4) + 1];
            file_buffer[((i * 3) + 0)] = p[(i * 4) + 2];
        }
        sceIoLseek(screenshot_fd, line, SEEK_SET);
        sceIoWrite(screenshot_fd, &file_buffer, 480 * 3);
        line += 480 * 3;
    }

    return 0;
}

static int write_5551_data(void *frame, int pixel_data) {
    int line; /*uint8_t*/
    char line_output = 0;
    uint16_t *p;
    int i;
    int h;

    line = pixel_data;
    for (h = 271; h >= 0; h--) {
        p = frame;
        p += (h * 512);
        for (i = 0; i < 480; i++) {
            file_buffer[((i * 3) + 2)] = (p[i] & 0x1F) << 3;
            file_buffer[((i * 3) + 1)] = ((p[i] >> 5) & 0x1F) << 3;
            file_buffer[((i * 3) + 0)] = ((p[i] >> 10) & 0x1F) << 3;
        }
        sceIoLseek(screenshot_fd, line, SEEK_SET);
        sceIoWrite(screenshot_fd, &file_buffer, 480 * 3);
        line += 480 * 3;
    }

    return 0;
}

static int write_565_data(void *frame, int pixel_data) {
    int line; /*uint8_t*/
    char line_output = 0;
    uint16_t *p;
    int i;
    int h;

    line = pixel_data;
    for (h = 271; h >= 0; h--) {
        p = frame;
        p += (h * 512);
        for (i = 0; i < 480; i++) {
            file_buffer[((i * 3) + 2)] = (p[i] & 0x1F) << 3;
            file_buffer[((i * 3) + 1)] = ((p[i] >> 5) & 0x3F) << 2;
            file_buffer[((i * 3) + 0)] = ((p[i] >> 11) & 0x1F) << 3;
        }
        sceIoLseek(screenshot_fd, line, SEEK_SET);
        sceIoWrite(screenshot_fd, &file_buffer, 480 * 3);
        line += 480 * 3;
    }

    return 0;
}

static int write_4444_data(void *frame, int pixel_data) {
    int line; /*uint8_t*/
    char line_output = 0;
    uint16_t *p;
    int i;
    int h;

    line = pixel_data;
    for (h = 271; h >= 0; h--) {
        p = frame;
        p += (h * 512);
        for (i = 0; i < 480; i++) {
            file_buffer[((i * 3) + 2)] = (p[i] & 0xF) << 4;
            file_buffer[((i * 3) + 1)] = (p[i] & 0xF) << 4;
            file_buffer[((i * 3) + 0)] = (p[i] & 0xF) << 4;
        }
        sceIoLseek(screenshot_fd, line, SEEK_SET);
        sceIoWrite(screenshot_fd, &file_buffer, 480 * 3);
        line += 480 * 3;
    }

    return 0;
}

int bitmap_write(void *frame_addr, void *ignored, int format, const char *file) {
    struct BitmapHeader bmp;
    int pixel_data = sizeof(struct BitmapHeader);

    memset(&bmp, 0, sizeof(struct BitmapHeader));
    memcpy(&bmp.id, BMP_ID, sizeof(bmp.id));
    bmp.filesize = 480 * 272 * 3 + sizeof(struct BitmapHeader);
    bmp.offset = sizeof(struct BitmapHeader);
    bmp.headsize = 0x28;
    bmp.width = 480;
    bmp.height = 272;
    bmp.planes = 1;
    bmp.bpp = 24;
    bmp.bitmapsize = 480 * 272 * 3;
    bmp.hres = 2834;
    bmp.vres = 2834;

    screenshot_fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
    if (screenshot_fd < 0) {
        printf("Could not open file '%s' for writing\n", file);
        return -1;
    }

    sceIoWrite(screenshot_fd, &bmp, sizeof(struct BitmapHeader));

    switch (format) {
        case PSP_DISPLAY_PIXEL_FORMAT_565:
            write_565_data(frame_addr, pixel_data);
            break;
        case PSP_DISPLAY_PIXEL_FORMAT_5551:
            write_5551_data(frame_addr, pixel_data);
            break;
        case PSP_DISPLAY_PIXEL_FORMAT_4444:
            write_4444_data(frame_addr, pixel_data);
            break;
        case PSP_DISPLAY_PIXEL_FORMAT_8888:
            write_8888_data(frame_addr, pixel_data);
            break;
    };

    sceIoClose(screenshot_fd);

    return 0;
}


void screenshot_path_init(void) {
    signed int fd;

    //Find which screen directory to use
    fd = sceIoDopen("ms0:/PICTURE/");
    if (fd > 0) {
        sceIoDclose(fd);

        strcpy(screen_path, "ms0:/PICTURE/screen%d.bmp");
    } else {
        fd = sceIoDopen("ms0:/PSP/PHOTO/");
        if (fd > 0) {
            sceIoDclose(fd);

            strcpy(screen_path, "ms0:/PSP/PHOTO/screen%d.bmp");
        } else {
            if (!sceIoMkdir("ms0:/PICTURE", 0777)) {
                strcpy(screen_path, "ms0:/PICTURE/screen%d.bmp");
            } else {
                screen_path[0] = 0;
            }
        }
    }

    //Set the correct screen number
    screen_no = 0;
    if (screen_path[0] != 0) {
        while (1) {
            snprintf(buffer, sizeof(buffer), screen_path, screen_no);
            fd = sceIoOpen(buffer, PSP_O_RDONLY, 0777);
            if (fd > 0) {
                sceIoClose(fd);
                screen_no++;
            } else {
                break;
            }
        }
    }
}

void screenshot_handle_request(void) {
    signed int fd;

    if ((screen_time) && (screen_path[0])) {
        screen_time = 0;
        void *block_addr;
        void *frame_addr;
        int frame_width;
        int pixel_format;
        int sync = 1;
        u32 p;

        while (1) {
            snprintf(buffer, sizeof(buffer), screen_path, screen_no);
            fd = sceIoOpen(buffer, PSP_O_RDONLY, 0777);
            if (fd > 0) {
                sceIoClose(fd);
                screen_no++;
            } else {
                break;
            }
        }

        if ((sceDisplayGetFrameBufferInternal(2, &frame_addr, &frame_width, &pixel_format, &sync) < 0) ||
            (frame_addr == NULL)) {
        } else {
            p = (u32) frame_addr;
            if (p & 0x80000000) {
                p |= 0xA0000000;
            } else {
                p |= UNCACHED;
            }

            game_pause(thid);
            bitmap_write((void *) p, NULL, pixel_format, buffer);
            game_resume(thid);

            screen_no++;
        }
    }
}
