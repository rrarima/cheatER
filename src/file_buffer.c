#include "file_buffer.h"


unsigned char file_buffer[1536];
unsigned int file_buffer_size = 0;
unsigned int file_buffer_backup = 0;
unsigned int file_buffer_file_offset = 0;
unsigned int file_buffer_offset = 1024;
