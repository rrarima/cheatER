#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <stdio.h>
#include <string.h>

#include "cheat.h"
#include "cheat_file.h"
#include "common.h"
#include "file_buffer.h"


unsigned char game_dir[] = "ms0:/seplugins/cheatER/__________.txt";
unsigned char game_id[11]; // 10-character ID + end-of-string zero

void cheat_load(void) {
    signed int fd;

    fd = sceIoOpen(game_dir, PSP_O_RDONLY, 0777);
    if (fd > 0) {
        unsigned int file_size = sceIoLseek(fd, 0, SEEK_END);
        sceIoLseek(fd, 0, SEEK_SET);
        unsigned int file_offset = 0;
        unsigned char comment_mode = 0;
        unsigned char name_mode = 0;

        while (file_offset < file_size) {
            sceKernelDelayThread(1500);

            sceIoRead(fd, &buffer[0], 1);

            if ((buffer[0] == '\r') || (buffer[0] == '\n')) {
                comment_mode = 0;
                if (name_mode) {
                    cheat_total++;
                    name_mode = 0;
                }
            } else if ((buffer[0] == ' ') && (!name_mode)) {
            } else if (buffer[0] == ';') {
                comment_mode = 1;
                if (name_mode) {
                    cheat_total++;
                    name_mode = 0;
                }
            }//Skip comments till next line
            else if (buffer[0] == '#')//Read in the cheat name
            {
                if (cheat_total >= BLOCK_MAX) { break; }
                cheat[cheat_total].block = block_total;
                cheat[cheat_total].flags = 0;
                cheat[cheat_total].len = 0;
                cheat[cheat_total].name[0] = 0;
                name_mode = 1;
            } else if ((buffer[0] == '!') && (name_mode)) {
                //Cheat's selected by default
                if (cheat[cheat_total].flags & FLAG_SELECTED)//Two ! = selected for constant on status
                {
                    cheat[cheat_total].flags |= FLAG_CONSTANT;
                    cheat[cheat_total].flags &= ~FLAG_SELECTED;
                } else//One ! = selected for music on/off button
                {
                    cheat[cheat_total].flags |= FLAG_SELECTED;
                }
            } else if ((!comment_mode) && (name_mode)) {
                if (name_mode < 32)//1 to 31 = letters, 32=Null terminator
                {
                    cheat[cheat_total].name[name_mode - 1] = buffer[0];
                    name_mode++;
                    cheat[cheat_total].name[name_mode - 1] = 0;
                }
            } else if ((!comment_mode) && (!name_mode)) {
                //Add 0xAABBCCDD 0xAABBCCDD block
                if (!block_add(fd, buffer)) {
                    //No more RAM?
                    if (cheat_total != 0) {
                        cheat_total--;
                        break;
                    }
                }
                if (cheat_total != 0) {
                    cheat[cheat_total - 1].len++;
                }
            }

            file_offset = sceIoLseek(fd, 0, SEEK_CUR);
        }
        sceIoClose(fd);
    }
}

void cheat_save() {
    unsigned char file_char = 0;
    unsigned char file_misc[3];
    unsigned int file_size = 0;
    unsigned int counter = -1;
    unsigned int scounter = 0;
    unsigned char file_mode = 0;//0=Unknown/Initial, 1=Comment, 2=Waiting for \n (ignoring)
    int fd;
    int temp_fd;

    //1) Open the original cheat file
    fd = sceIoOpen(game_dir, PSP_O_RDONLY, 0777);
    if (fd > 0) {
        //Find the file size
        file_size = sceIoLseek(fd, 0, SEEK_END);
        sceIoLseek(fd, 0, SEEK_SET);

        //Initiate the read buffer
        file_buffer_offset = 1024;
        file_buffer_file_offset = 0;

        //2) Open up the temporary and get ready to regenerate it
        temp_fd = sceIoOpen("ms0:/seplugins/cheatER/temp.txt", PSP_O_WRONLY | PSP_O_CREAT, 0777);
        if (temp_fd <= 0) {
            sceIoClose(fd);
            return;
        }

        //Add the codes that are already there
        while (file_buffer_file_offset < file_size) {
            //Read a byte
            FILE_BUFFER_READ(&file_char, 1);
            if (file_buffer_size == 0) break;

            //Interpret the byte based on the mode
            if (file_mode == 0) {
                //Pick a mode
                switch (file_char) {
                    case ';':
                        file_mode = 1;
                        sceIoWrite(temp_fd, ";", 1);
                        break;

                    case '#':
                        file_mode = 2;
                        counter++;
                        //Add a double line skip?
                        if (counter != 0) {
                            sceIoWrite(temp_fd, "\r\n", 2);
                        }

                        //Is there an error...?
                        if (counter >= cheat_total) {
                            sceIoClose(temp_fd);
                            sceIoClose(fd);
                            return;
                        }

                        //Set up the subCounter
                        scounter = cheat[counter].block;

                        //Is it on by default...?
                        if (cheat[counter].flags & FLAG_CONSTANT) {
                            sceIoWrite(temp_fd, "#!!", 3);
                        } else if (cheat[counter].flags & FLAG_SELECTED) {
                            sceIoWrite(temp_fd, "#!", 2);
                        } else {
                            sceIoWrite(temp_fd, "#", 1);
                        }
                        //Write out the name of the cheat
                        sceIoWrite(temp_fd, &cheat[counter].name, strlen(cheat[counter].name));
                        sceIoWrite(temp_fd, "\r\n", 2);
                        break;

                    case '0':
                        if ((file_buffer_file_offset) < file_size) {
                            FILE_BUFFER_PEEK(file_misc[0], 0);
                            if (file_misc[0] == 'x') {
                                //Is there an error...?
                                if (counter == (unsigned int) -1) {
                                    sceIoClose(temp_fd);
                                    sceIoClose(fd);
                                    return;
                                }
                                if (scounter >= (cheat[counter].block + cheat[counter].len)) {
                                    sceIoClose(temp_fd);
                                    sceIoClose(fd);
                                    return;
                                }

                                //Write out the address
                                if (block[scounter].flags & FLAG_DMA) {
                                    snprintf(buffer, sizeof(buffer), "0x%08lX ", (block[scounter].address));
                                    sceIoWrite(temp_fd, buffer, strlen(buffer));
                                } else {
                                    snprintf(buffer, sizeof(buffer), "0x%08lX ", (block[scounter].address - USER_MEM_BASE));
                                    sceIoWrite(temp_fd, buffer, strlen(buffer));
                                }

                                //Write out the value
                                if (block[scounter].flags & FLAG_FREEZE) {
                                    switch (block[scounter].flags & FLAG_DWORD) {
                                        case FLAG_DWORD:
                                            snprintf(buffer, sizeof(buffer), "0x________\r\n");
                                            break;

                                        case FLAG_WORD:
                                            snprintf(buffer, sizeof(buffer), "0x____\r\n");
                                            break;

                                        case FLAG_BYTE:
                                            snprintf(buffer, sizeof(buffer), "0x__\r\n");
                                            break;
                                    }
                                } else {
                                    switch (block[scounter].flags & FLAG_DWORD) {
                                        case FLAG_DWORD:
                                            snprintf(buffer, sizeof(buffer), "0x%08lX\r\n", block[scounter].hak_val);
                                            break;

                                        case FLAG_WORD:
                                            snprintf(buffer, sizeof(buffer), "0x%04hX\r\n", (unsigned short) block[scounter].hak_val);
                                            break;

                                        case FLAG_BYTE:
                                            snprintf(buffer, sizeof(buffer), "0x%02hX\r\n", (unsigned char) block[scounter].hak_val);
                                            break;
                                    }
                                }
                                sceIoWrite(temp_fd, buffer, strlen(buffer));

                                //Skip the rest
                                file_mode = 2;
                                scounter++;
                            }
                        }
                        break;
                }
            } else if (file_mode == 1) {
                //Just copy it out straight to the file
                if ((file_char == '\r') || (file_char == '\n')) {
                    sceIoWrite(temp_fd, "\r\n", 2);
                    file_mode = 0;
                } else {
                    sceIoWrite(temp_fd, &file_char, 1);
                }
            } else if (file_mode == 2) {
                //Just wait for an '\r' or '\n'
                if ((file_char == '\r') || (file_char == '\n')) {
                    file_mode = 0;
                }
            }
        }

        //Close the files
        sceIoClose(temp_fd);
        sceIoClose(fd);

        //Delete the old file, rename the temporary
        sceIoRemove(game_dir);
        sceIoRename("ms0:/seplugins/cheatER/temp.txt", game_dir);
    }

    //Open the file for appending
    fd = sceIoOpen(game_dir, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777);
    if (fd > 0) {
        //Add any new codes
        counter++;
        if (counter != 0) sceIoWrite(fd, "\r\n", 2);
        while (counter < cheat_total) {
            //Write the cheat name
            if (cheat[counter].flags & FLAG_CONSTANT) {
                sceIoWrite(fd, "#!!", 3);
            } else if (cheat[counter].flags & FLAG_SELECTED) {
                sceIoWrite(fd, "#!", 2);
            } else {
                sceIoWrite(fd, "#", 1);
            }

            //Write out the name of the cheat
            sceIoWrite(fd, &cheat[counter].name, strlen(cheat[counter].name));
            sceIoWrite(fd, "\r\n", 2);

            //Loop through the addresses
            scounter = cheat[counter].block;
            while (scounter < (cheat[counter].block + cheat[counter].len)) {
                //Write out the address
                snprintf(buffer, sizeof(buffer), "0x%08lX ", (block[scounter].address - USER_MEM_BASE));
                sceIoWrite(fd, buffer, strlen(buffer));

                //Write out the value
                switch (block[scounter].flags & FLAG_DWORD) {
                    case FLAG_DWORD:
                        snprintf(buffer, sizeof(buffer), "0x%08lX\r\n", block[scounter].hak_val);
                        break;

                    case FLAG_WORD:
                        snprintf(buffer, sizeof(buffer), "0x%04hX\r\n", (unsigned short) block[scounter].hak_val);
                        break;

                    case FLAG_BYTE:
                        snprintf(buffer, sizeof(buffer), "0x%02hX\r\n", (unsigned char) block[scounter].hak_val);
                        break;
                }
                sceIoWrite(fd, buffer, strlen(buffer));

                //Next address
                scounter++;
            }

            //Next cheat
            counter++;
            sceIoWrite(fd, "\r\n", 2);
        }

        //Close the file
        sceIoClose(fd);
    }
}
