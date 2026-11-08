#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <stdio.h>
#include <string.h>

#include "cheat.h"

//Globals

unsigned int block_total = 0;
unsigned int cheat_total = 0;
Block block[BLOCK_MAX];
Cheat cheat[BLOCK_MAX];
unsigned char cheat_status = 0;
unsigned char cheat_saved = 0;
unsigned int cheat_hz = 0;
unsigned char cheat_flash = 0;
unsigned int cheat_no = 0;
unsigned int cheat_dma = 0;

//Cheat handler
unsigned int char2hex(unsigned char *data, unsigned int *type) {
    unsigned int dword = 0;
    unsigned int power = 0;

    while (power < 8) {
        switch (data[power]) {
            case '_':
                break;//Freeze cheat
            case '0':
                dword |= 0 * (1 << (4 * (7 - power)));
                break;
            case '1':
                dword |= 1 * (1 << (4 * (7 - power)));
                break;
            case '2':
                dword |= 2 * (1 << (4 * (7 - power)));
                break;
            case '3':
                dword |= 3 * (1 << (4 * (7 - power)));
                break;
            case '4':
                dword |= 4 * (1 << (4 * (7 - power)));
                break;
            case '5':
                dword |= 5 * (1 << (4 * (7 - power)));
                break;
            case '6':
                dword |= 6 * (1 << (4 * (7 - power)));
                break;
            case '7':
                dword |= 7 * (1 << (4 * (7 - power)));
                break;
            case '8':
                dword |= 8 * (1 << (4 * (7 - power)));
                break;
            case '9':
                dword |= 9 * (1 << (4 * (7 - power)));
                break;
            case 'a':
            case 'A':
                dword |= 0xA * (1 << (4 * (7 - power)));
                break;
            case 'b':
            case 'B':
                dword |= 0xB * (1 << (4 * (7 - power)));
                break;
            case 'c':
            case 'C':
                dword |= 0xC * (1 << (4 * (7 - power)));
                break;
            case 'd':
            case 'D':
                dword |= 0xD * (1 << (4 * (7 - power)));
                break;
            case 'e':
            case 'E':
                dword |= 0xE * (1 << (4 * (7 - power)));
                break;
            case 'f':
            case 'F':
                dword |= 0xF * (1 << (4 * (7 - power)));
                break;
            default:
                dword >>= 4 * (8 - power);
                *type = power;
                return dword;
        }

        power++;
    }

    *type = 8;
    return dword;
}

void cheat_new(unsigned char size, unsigned int address, unsigned int value) {
    if ((cheat_total + 1 < BLOCK_MAX) && (block_total + 1 < BLOCK_MAX)) {
        cheat[cheat_total].block = block_total;
        cheat[cheat_total].flags = 0;
        cheat[cheat_total].len = 1;

        block[block_total].address = address;
        block[block_total].address &= 0x0FFFFFFF;
        block[block_total].address += USER_MEM_BASE;

        block[block_total].flags = 0;
        block[block_total].hak_val = value;

        switch (size) {
            case 1:
                block[block_total].flags |= FLAG_BYTE;
                if (cheat_saved) block[block_total].std_val = *((unsigned char *) (block[block_total].address));
                break;
            case 2:
                block[block_total].flags |= FLAG_WORD;
                if (cheat_saved) block[block_total].std_val = *((unsigned short *) (block[block_total].address));
                break;
            case 4:
                block[block_total].flags |= FLAG_DWORD;
                if (cheat_saved) block[block_total].std_val = *((unsigned int *) (block[block_total].address));
                break;
            default:
                block[block_total].flags |= FLAG_UWORD;
        }

        snprintf((char *) cheat[cheat_total].name, sizeof(cheat[cheat_total].name), "NEW CHEAT %d", cheat_no);
        cheat_no++;
        cheat_total++;
        block_total++;
    }
}

unsigned int block_add(int fd, unsigned char *data) {
    unsigned int type;
    unsigned int offset;
    unsigned char hex[8];

    if (block_total != BLOCK_MAX) {
        block[block_total].flags = 0;

        sceIoLseek(fd, 1, SEEK_CUR);
        sceIoRead(fd, hex, 8);

        block[block_total].address = char2hex(hex, &type);

        if (block[block_total].address == 0xFFFFFFFF) {
            block[block_total].flags |= FLAG_DMA;
            block[block_total].std_val = 0xFFFFFFFF;
        } else {
            block[block_total].address &= 0x0FFFFFFF;
            block[block_total].address += USER_MEM_BASE;
        }

        offset = sceIoLseek(fd, 3, SEEK_CUR);
        sceIoRead(fd, hex, 8);

        block[block_total].hak_val = char2hex(hex, &type);

        if (hex[0] == '_') {
            block[block_total].flags |= FLAG_FREEZE;
        }

        switch (type) {
            case 2:
                block[block_total].flags |= FLAG_BYTE;
                break;
            case 4:
                block[block_total].flags |= FLAG_WORD;
                break;
            case 8:
                block[block_total].flags |= FLAG_DWORD;
                break;
            default:
                block[block_total].flags |= FLAG_UWORD;
        }

        sceIoLseek(fd, offset + type, SEEK_SET);//Reposition the cursor depending on size of Hex value

        block_total++;

        return 1;
    }
    return 0;
}

void cheat_toggle(unsigned int cheat_index, unsigned char enable) {
    unsigned int counter;
    unsigned char reset_dma = 0;
    cheat_dma = 0;

    counter = cheat[cheat_index].block;
    while (counter < (cheat[cheat_index].block + cheat[cheat_index].len)) {
        if (block[counter].flags & FLAG_DMA) {
            if (block[counter].hak_val != 0xFFFFFFFF) {
                cheat_dma = *((unsigned int *) (USER_MEM_BASE + (block[counter].hak_val & 0x1FFFFFF))) - USER_MEM_BASE;

                if (block[counter].std_val != cheat_dma) {
                    reset_dma = 1;
                    block[counter].std_val = cheat_dma;
                } else {
                    reset_dma = 0;
                }
            } else {
                cheat_dma = 0;
                reset_dma = 0;
            }
        } else {
            if (enable) {
                if (((cheat_dma) && (reset_dma)) ||
                    ((cheat[cheat_index].flags & FLAG_FRESH) && (block[counter].flags & FLAG_FREEZE))) {
                    switch (block[counter].flags & FLAG_DWORD) {
                        case FLAG_DWORD:
                            block[counter].std_val = *((unsigned int *) (cheat_dma + block[counter].address));
                            break;
                        case FLAG_WORD:
                            block[counter].std_val = *((unsigned short *) (cheat_dma + block[counter].address));
                            break;
                        case FLAG_BYTE:
                            block[counter].std_val = *((unsigned char *) (cheat_dma + block[counter].address));
                            break;
                    }
                    if (block[counter].flags & FLAG_FREEZE) {
                        block[counter].hak_val = block[counter].std_val;
                    }
                }

                //Apply cheat!
                switch (block[counter].flags & FLAG_DWORD) {
                    case FLAG_DWORD:
                        *((unsigned int *) (cheat_dma + block[counter].address)) = block[counter].hak_val;
                        sceKernelDcacheWritebackInvalidateRange((const void *) (cheat_dma + block[counter].address), 4);
                        sceKernelIcacheInvalidateRange((const void *) (cheat_dma + block[counter].address), 4);
                        break;
                    case FLAG_WORD:
                        *((unsigned short *) (cheat_dma + block[counter].address)) = (unsigned short) block[counter].hak_val;
                        sceKernelDcacheWritebackInvalidateRange((const void *) (cheat_dma + block[counter].address), 2);
                        sceKernelIcacheInvalidateRange((const void *) (cheat_dma + block[counter].address), 2);
                        break;
                    case FLAG_BYTE:
                        *((unsigned char *) (cheat_dma + block[counter].address)) = (unsigned char) block[counter].hak_val;
                        sceKernelDcacheWritebackInvalidateRange((const void *) (cheat_dma + block[counter].address), 1);
                        sceKernelIcacheInvalidateRange((const void *) (cheat_dma + block[counter].address), 1);
                        break;
                }
            } else {
                if (!reset_dma) {
                    switch (block[counter].flags & FLAG_DWORD) {
                        case FLAG_DWORD:
                            *((unsigned int *) (cheat_dma + block[counter].address)) = block[counter].std_val;
                            sceKernelDcacheWritebackInvalidateRange((const void *) (cheat_dma + block[counter].address), 4);
                            sceKernelIcacheInvalidateRange((const void *) (cheat_dma + block[counter].address), 4);
                            break;
                        case FLAG_WORD:
                            *((unsigned short *) (cheat_dma + block[counter].address)) = (unsigned short) block[counter].std_val;
                            sceKernelDcacheWritebackInvalidateRange((const void *) (cheat_dma + block[counter].address), 2);
                            sceKernelIcacheInvalidateRange((const void *) (cheat_dma + block[counter].address), 2);
                            break;
                        case FLAG_BYTE:
                            *((unsigned char *) (cheat_dma + block[counter].address)) = (unsigned char) block[counter].std_val;
                            sceKernelDcacheWritebackInvalidateRange((const void *) (cheat_dma + block[counter].address), 1);
                            sceKernelIcacheInvalidateRange((const void *) (cheat_dma + block[counter].address), 1);
                            break;
                    }
                }
            }
            counter++;
        }
    }
}


void cheat_enable(unsigned int cheat_index) {
    cheat_toggle(cheat_index, 1);
}

void cheat_disable(unsigned int cheat_index) {
    cheat_toggle(cheat_index, 0);
}


void cheat_apply(unsigned char type)//0=Enable/Disable FLAG_FRESH, -1=Enable only all (Freeze on all)
{
    unsigned int counter;

    if (!cheat_saved) return;
    if (!type) { cheat_flash = 2; }//Write the cheat twice more even if Cheat Hz is off

    if (cheat_status) {
        //Enable cheats
        counter = 0;
        while (counter < cheat_total) {
            if ((cheat[counter].flags & FLAG_SELECTED) || (cheat[counter].flags & FLAG_CONSTANT)) {
                if (((!type) && (cheat[counter].flags & FLAG_FRESH)) || (type)) {
                    cheat_enable(counter);
                    cheat[counter].flags &= ~FLAG_FRESH;
                }
            } else if ((!type) && (cheat[counter].flags & FLAG_FRESH)) {
                cheat_disable(counter);
                cheat[counter].flags &= ~FLAG_FRESH;
            }
            counter++;
        }
    } else {
        //Disable cheats
        counter = 0;
        while (counter < cheat_total) {
            if (cheat[counter].flags & FLAG_CONSTANT) {
                if (((!type) && (cheat[counter].flags & FLAG_FRESH)) || (type)) {
                    cheat_enable(counter);
                    cheat[counter].flags &= ~FLAG_FRESH;
                }
            } else if ((!type) && (cheat[counter].flags & FLAG_FRESH)) {
                cheat_disable(counter);
                cheat[counter].flags &= ~FLAG_FRESH;
            }
            counter++;
        }
    }
}
