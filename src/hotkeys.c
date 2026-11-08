#include <pspctrl.h>
#include <pspkernel.h>

#include "cheat.h"
#include "hotkeys.h"
#include "menu.h"
#include "screenshot.h"


unsigned int menu_key = PSP_CTRL_VOLUP | PSP_CTRL_VOLDOWN;
unsigned int trigger_key = PSP_CTRL_NOTE;
unsigned int screen_key = PSP_CTRL_LTRIGGER | PSP_CTRL_SQUARE;

void button_callback(u32 curr, u32 last, void *arg) {
    unsigned int counter;
    unsigned int scounter;
    unsigned int address;

    if (vram == NULL) return;

    if (((curr & menu_key) == menu_key) && (!menu_drawn)) {
        menu_drawn = 1;
        if (cheat_selected >= cheat_total) cheat_selected = 0;
        tab_selected = TAB_CHEATS;
    } else if (((curr & screen_key) == screen_key) && (!menu_drawn)) {
        screen_time = 1;
    } else if (((curr & trigger_key) == trigger_key) && (!menu_drawn)) {
        //Backup all the cheat "blocks"
        if (!cheat_saved) {
            counter = 0;
            scounter = 0;
            while (counter < block_total) {
                if (cheat[scounter].block == counter) {
                    cheat_dma = 0;//Reset DMA on every new cheat
                    scounter++;
                }

                if (block[counter].flags & FLAG_DMA) {
                    if (block[counter].hak_val != 0xFFFFFFFF) {
                        cheat_dma = *((unsigned int *) (USER_MEM_BASE + (block[counter].hak_val & 0x1FFFFFF))) - USER_MEM_BASE;

                        if (((cheat_dma >= 0x00004000) && (cheat_dma < 0x01800000)) ||
                            ((cheat_dma >= 0x40004000) && (cheat_dma < 0x41800000))) {
                            block[counter].std_val = cheat_dma;
                        }
                    } else {
                        cheat_dma = 0;
                    }
                } else {
                    address = cheat_dma + block[counter].address;

                    if (((address >= USER_MEM_START) && (address < 0x0A000000)) ||
                        ((address >= START_ADDRESS) && (address < END_ADDRESS))) {
                        switch (block[counter].flags & FLAG_DWORD) {
                            case FLAG_DWORD:
                                if (address % 4 == 0) {
                                    block[counter].std_val = *((unsigned int *) (address));
                                }
                                break;
                            case FLAG_WORD:
                                if (address % 2 == 0) {
                                    block[counter].std_val = *((unsigned short *) (address));
                                }
                                break;
                            case FLAG_BYTE:
                                block[counter].std_val = *((unsigned char *) (address));
                                break;
                        }
                    }
                }
                counter++;
            }
        }

        //Turn on/off cheats
        cheat_status = !cheat_status;

        //Apply the cheats accordingly (make em fresh)
        counter = 0;
        while (counter < cheat_total) {
            if ((cheat[counter].flags & FLAG_CONSTANT) && (!cheat_saved)) {
                cheat[counter].flags |= FLAG_FRESH;
                cheat_enable(counter);
                cheat[counter].flags &= ~FLAG_FRESH;
            }
            if (cheat[counter].flags & FLAG_SELECTED) {
                cheat[counter].flags |= FLAG_FRESH;
                if (cheat_status) {
                    cheat_enable(counter);
                } else {
                    cheat_disable(counter);
                }
                cheat[counter].flags &= ~FLAG_FRESH;
            }
            counter++;
        }
        cheat_saved = 1;

        //Wait 0.5 seconds
        sceKernelDelayThread(500000);
    }
}
