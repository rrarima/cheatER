#ifndef CHEATER_FILE_BUFFER_H
#define CHEATER_FILE_BUFFER_H

/*
 * Small buffered reader used when parsing files.
 * The macros expect an open file descriptor named `fd` in the calling scope.
 * `file_buffer` is also reused as general scratch space (screenshots, text entry).
 */

#include <pspiofilemgr.h>
#include <string.h>

extern unsigned char file_buffer[1536];
extern unsigned int file_buffer_size;
extern unsigned int file_buffer_backup;
extern unsigned int file_buffer_file_offset;
extern unsigned int file_buffer_offset;

#define FILE_BUFFER_PEEK(out, ahead)                  \
    if ((file_buffer_offset + ahead) >= 1024) {         \
        file_buffer_backup = sceIoLseek(fd, 0, SEEK_CUR); \
        sceIoLseek(fd, ahead, SEEK_CUR);              \
        sceIoRead(fd, &out, 1);                       \
        sceIoLseek(fd, file_buffer_backup, SEEK_SET);     \
    } else {                                            \
        out = file_buffer[file_buffer_offset + ahead]; \
    }
#define FILE_BUFFER_READ(out, size)                     \
    if (file_buffer_offset == 1024) {                       \
        file_buffer_size = sceIoRead(fd, file_buffer, 1024); \
        file_buffer_offset = 0;                             \
    }                                                     \
    memcpy(out, &file_buffer[file_buffer_offset], size); \
    file_buffer_offset += size;                           \
    file_buffer_file_offset += size;

#endif // CHEATER_FILE_BUFFER_H
