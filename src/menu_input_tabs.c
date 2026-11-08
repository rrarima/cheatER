/*
 * Button handling for the main menu tabs (tab_selected 0-5).
 */

#include <pspctrl.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspdisplay_kernel.h>
#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <pspusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cheat.h"
#include "cheat_file.h"
#include "common.h"
#include "file_buffer.h"
#include "game_pause.h"
#include "menu.h"
#include "menu_draw.h"
#include "menu_input.h"
#include "plugin.h"
#include "search.h"
#include "usb.h"


// Tab 0: Cheats List
void menu_input_cheats_tab(void) {
    if (pad.Buttons & PSP_CTRL_UP) {
        if (cheat_selected > 0) {
            cheat_selected--;
        } else if (cheat_selected == 0) {
            cheat_selected = cheat_total - 1;
        }
        menu_draw();
        if (cheat_button_age_y < 12) cheat_button_age_y++;
        menu_key_delay(150000 - (10000 * cheat_button_age_y));
    } else if (pad.Buttons & PSP_CTRL_DOWN) {
        if (cheat_selected < (cheat_total - 1)) {
            cheat_selected++;
        } else if (cheat_selected == (cheat_total - 1)) {
            cheat_selected = 0;
        }
        menu_draw();
        if (cheat_button_age_y < 12) cheat_button_age_y++;
        menu_key_delay(150000 - (10000 * cheat_button_age_y));
    } else {
        cheat_button_age_y = 0;
    }
    if (pad.Buttons & PSP_CTRL_CROSS) {
        cheat[cheat_selected].flags = (cheat[cheat_selected].flags & (~FLAG_SELECTED)) |
                                     ((~cheat[cheat_selected].flags) & FLAG_SELECTED);
        cheat[cheat_selected].flags &= ~FLAG_CONSTANT;
        cheat[cheat_selected].flags |= FLAG_FRESH;

        menu_draw();
        cheat_apply(0);

        menu_key_delay(150000);
    } else if (pad.Buttons & PSP_CTRL_SQUARE) {
        cheat[cheat_selected].flags = (cheat[cheat_selected].flags & ~FLAG_CONSTANT) |
                                     (~cheat[cheat_selected].flags & FLAG_CONSTANT);
        cheat[cheat_selected].flags &= ~FLAG_SELECTED;
        cheat[cheat_selected].flags |= FLAG_FRESH;

        menu_draw();
        cheat_apply(0);

        menu_key_delay(150000);
    } else if (pad.Buttons & PSP_CTRL_TRIANGLE) {
        menu_clear_screen();
        ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
        ext_selected[0] = cheat[cheat_selected].block;
        ext_menu = EXT_EDIT_CHEAT;
        ext_opt = 0;
        menu_draw();

        menu_key_delay(150000);
    }
}

// Tab 1: Searcher
void menu_input_searcher_tab(void) {
    int fd;

    if (pad.Buttons & PSP_CTRL_UP) {
        if (cheat_selected > 0) {
            cheat_selected--;
        } else if (cheat_selected == 0) {
            cheat_selected = (2 + ((!cheat_search) * 2));
        }
        menu_draw();
        if (cheat_button_age_y < 12) cheat_button_age_y++;
        menu_key_delay(150000 - (10000 * cheat_button_age_y));
    } else if (pad.Buttons & PSP_CTRL_DOWN) {
        if (cheat_selected < (2 + ((!cheat_search) * 2))) {
            cheat_selected++;
        } else if (cheat_selected == (2 + ((!cheat_search) * 2))) {
            cheat_selected = 0;
        }
        menu_draw();
        if (cheat_button_age_y < 12) cheat_button_age_y++;
        menu_key_delay(150000 - (10000 * cheat_button_age_y));
    } else {
        cheat_button_age_y = 0;
    }
    if (pad.Buttons & PSP_CTRL_CROSS) {
        if (!cheat_search) {
            if (cheat_selected == 0) {
                //Goto Find exact
                menu_clear_screen();
                ext_selected[0] = ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
                ext_menu = EXT_SEARCH;
                ext_opt = 0;
                menu_draw();
                cheat_search = 1;
            } else if (cheat_selected == 4) {
                //Goto Find text
                search_result_counter = 0;
                menu_clear_screen();
                memset(file_buffer, 0, 50);
                file_buffer[0] = 'A';
                ext_selected[0] = ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
                ext_menu = EXT_TEXT_SEARCH;
                ext_opt = 0;
                menu_draw();
            } else {
                //Dump a search dump
                fd = sceIoOpen("ms0:/search.ram", PSP_O_WRONLY | PSP_O_CREAT, 0777);
                if (fd > 0) {
                    if (sceIoWrite(fd, (void *) USER_MEM_BASE, 0x1800000) == 0x1800000) {
                        sceIoClose(fd);

                        cheat_search = 1;
                        switch (cheat_selected) {
                            case 1:
                                search_history[0].flags =
                                        (search_history[0].flags & (~FLAG_DWORD)) | FLAG_BYTE;
                                break;
                            case 2:
                                search_history[0].flags =
                                        (search_history[0].flags & (~FLAG_DWORD)) | FLAG_WORD;
                                break;
                            case 3:
                                search_history[0].flags =
                                        (search_history[0].flags & (~FLAG_DWORD)) | FLAG_DWORD;
                                break;
                        }

                        menu_clear_screen();
                        tab_selected = TAB_SEARCHER;
                        cheat_selected = 1;
                        menu_draw();
                        LINE_CLEAR(33);
                        pspDebugScreenSetTextColor(0xFFFF8000);
                        pspDebugScreenPuts("Now, resume the game!");

                        sceKernelDelayThread(3000000);
                    } else {
                        sceIoClose(fd);

                        sceIoRemove("ms0:/search.ram");

                        LINE_CLEAR(33);
                        pspDebugScreenSetTextColor(0xFF0000FF);
                        pspDebugScreenPuts("ERROR: MemoryStick out of Space!");

                        sceKernelDelayThread(3000000);
                    }
                }
            }
        } else {
            if (cheat_selected == 0) {
                //Goto Find exact
                menu_clear_screen();
                ext_selected[0] = ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
                ext_menu = EXT_SEARCH;
                ext_opt = 0;
                menu_draw();
            } else if (cheat_selected == 1) {
                //Goto Find Diff
                menu_clear_screen();
                ext_selected[0] = ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
                ext_menu = EXT_DIFF_SEARCH;
                ext_opt = 0;
                menu_draw();
            } else if (cheat_selected == 2) {
                sceIoRemove("ms0:/search.ram");

                while (search_max > 0) {
                    snprintf(buffer, sizeof(buffer), "ms0:/search%d.dat", search_max);
                    sceIoRemove(buffer);
                    search_max--;
                }

                //Reset fields
                search_no = 0;
                cheat_search = 0;
                cheat_selected = 0;
                search_result_counter = 0;
            }
        }

        menu_draw();
        menu_key_delay(150000);
    }
}

// Tab 2: Mem View (memory browser)
void menu_input_browser_tab(void) {
    if (pad.Buttons & PSP_CTRL_SELECT) {
        copy_menu = 1;
        menu_draw();
        menu_key_delay(150000);
    }
    if (pad.Buttons & PSP_CTRL_CROSS) {
        ext_selected[3] = !ext_selected[3];
        menu_draw();
        menu_key_delay(150000);
    }

    if (pad.Buttons & PSP_CTRL_LEFT) {
        browse_x--;
        switch (browse_c) {
            case 0:
                if ((signed) browse_x == -1) { browse_x = 0; }
                break;
            case 1:
                if ((signed) browse_x == -1) {
                    browse_x = 7;
                    browse_c--;
                }
                break;
            case 2:
                if ((signed) browse_x == -1) {
                    browse_x = (2 * browse_lines) - 1;
                    browse_c--;
                }
                break;
        }
        menu_draw();
        if (cheat_button_age_x < 12) cheat_button_age_x++;
        menu_key_delay(150000 - (10000 * cheat_button_age_x));
    } else if (pad.Buttons & PSP_CTRL_RIGHT) {
        browse_x++;
        switch (browse_c) {
            case 0:
                if (browse_x > 7) {
                    browse_x = 0;
                    browse_c++;
                }
                break;
            case 1:
                if (browse_x > ((2 * browse_lines) - 1)) {
                    browse_x = 0;
                    browse_c++;
                }
                break;
            case 2:
                if (browse_x > (browse_lines - 1)) { browse_x = browse_lines - 1; }
                break;
        }
        menu_draw();
        if (cheat_button_age_x < 12) cheat_button_age_x++;
        menu_key_delay(150000 - (10000 * cheat_button_age_x));
    } else {
        cheat_button_age_x = 0;
    }

    if (ext_selected[3]) {
        if (pad.Buttons & PSP_CTRL_SQUARE) {
            if (browse_c == 1) {
                browse_c = 2;
                browse_x = browse_x / 2;
            } else if (browse_c == 2) {
                browse_c = 1;
                browse_x = 2 * browse_x + 1;
            }
            menu_draw();
            menu_key_delay(150000);
        }
        if (pad.Buttons & PSP_CTRL_UP) {
            switch (browse_c) {
                case 0:
                    browse_address += (1 << (4 * (7 - browse_x)));
                    if (browse_address < START_ADDRESS) {
                        browse_address = START_ADDRESS;
                    }
                    if (browse_address > (END_ADDRESS - (22 * browse_lines))) {
                        browse_address = (END_ADDRESS - (22 * browse_lines));
                    }
                    break;
                case 1:
                    if (browse_x & 1) {
                        *((unsigned char *) ((browse_address + (browse_y * browse_lines)) +
                                             (browse_x / 2))) += 0x01;
                    } else {
                        *((unsigned char *) ((browse_address + (browse_y * browse_lines)) +
                                             (browse_x / 2))) += 0x10;
                    }
                    sceKernelDcacheWritebackInvalidateRange(
                            ((unsigned char *) ((browse_address + (browse_y * browse_lines)) +
                                                (browse_x / 2))),
                            1);
                    sceKernelIcacheInvalidateRange(
                            ((unsigned char *) ((browse_address + (browse_y * browse_lines)) +
                                                (browse_x / 2))),
                            1);
                    break;
                case 2:
                    *((unsigned char *) ((browse_address + (browse_y * browse_lines)) + browse_x)) += 1;
                    sceKernelDcacheWritebackInvalidateRange(
                            ((unsigned char *) ((browse_address + (browse_y * browse_lines)) + browse_x)),
                            1);
                    sceKernelIcacheInvalidateRange(
                            ((unsigned char *) ((browse_address + (browse_y * browse_lines)) + browse_x)),
                            1);
                    break;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            switch (browse_c) {
                case 0:
                    browse_address -= (1 << (4 * (7 - browse_x)));
                    if (browse_address < START_ADDRESS) {
                        browse_address = START_ADDRESS;
                    }
                    if (browse_address > (END_ADDRESS - (22 * browse_lines))) {
                        browse_address = (END_ADDRESS - (22 * browse_lines));
                    }
                    break;
                case 1:
                    if (browse_x & 1) {
                        *((unsigned char *) ((browse_address + (browse_y * browse_lines)) +
                                             (browse_x / 2))) -= 0x01;
                    } else {
                        *((unsigned char *) ((browse_address + (browse_y * browse_lines)) +
                                             (browse_x / 2))) -= 0x10;
                    }
                    sceKernelDcacheWritebackInvalidateRange(
                            ((unsigned char *) ((browse_address + (browse_y * browse_lines)) +
                                                (browse_x / 2))),
                            1);
                    sceKernelIcacheInvalidateRange(
                            ((unsigned char *) ((browse_address + (browse_y * browse_lines)) +
                                                (browse_x / 2))),
                            1);
                    break;
                case 2:
                    *((unsigned char *) ((browse_address + (browse_y * browse_lines)) + browse_x)) -= 1;
                    sceKernelDcacheWritebackInvalidateRange(
                            ((unsigned char *) ((browse_address + (browse_y * browse_lines)) + browse_x)),
                            1);
                    sceKernelIcacheInvalidateRange(
                            ((unsigned char *) ((browse_address + (browse_y * browse_lines)) + browse_x)),
                            1);
                    break;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
    } else if (pad.Buttons & PSP_CTRL_SQUARE) {
        if (pad.Buttons & PSP_CTRL_UP) {
            browse_address -= browse_lines;
            if (browse_address < START_ADDRESS) {
                browse_address = START_ADDRESS;
            }
            if (browse_address > (END_ADDRESS - (22 * browse_lines))) {
                browse_address = (END_ADDRESS - (22 * browse_lines));
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            browse_address += browse_lines;
            if (browse_address < START_ADDRESS) {
                browse_address = START_ADDRESS;
            }
            if (browse_address > (END_ADDRESS - (22 * browse_lines))) {
                browse_address = (END_ADDRESS - (22 * browse_lines));
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
        if ((abs((signed) (pad.Ly - 127)) - 40) > 0) {
            browse_address += (((signed) (pad.Ly - 127)) / browse_lines) * 64;
            if (browse_address < START_ADDRESS) {
                browse_address = START_ADDRESS;
            }
            if (browse_address > (END_ADDRESS - (22 * browse_lines))) {
                browse_address = (END_ADDRESS - (22 * browse_lines));
            }
            menu_draw();
            sceKernelDelayThread(18750);
        }
    } else {
        if (pad.Buttons & PSP_CTRL_UP) {
            if (browse_y > 0) {
                browse_y--;
            } else if (browse_y == 0) {
                browse_address -= browse_lines;
                if (browse_address < START_ADDRESS) {
                    browse_address = START_ADDRESS;
                }
                if (browse_address > (END_ADDRESS - (22 * browse_lines))) {
                    browse_address = (END_ADDRESS - (22 * browse_lines));
                }
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            if (browse_y < 21) {
                browse_y++;
            } else if (browse_y == 21) {
                browse_address += browse_lines;
                if (browse_address < START_ADDRESS) {
                    browse_address = START_ADDRESS;
                }
                if (browse_address > (END_ADDRESS - (22 * browse_lines))) {
                    browse_address = (END_ADDRESS - (22 * browse_lines));
                }
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
    }
}

// Tab 3: Decoder
void menu_input_decoder_tab(void) {
    if (pad.Buttons & PSP_CTRL_SELECT) {
        copy_menu = 1;
        menu_draw();
        menu_key_delay(150000);
    }
    if (pad.Buttons & PSP_CTRL_CROSS) {
        ext_selected[3] = !ext_selected[3];
        menu_draw();
        menu_key_delay(150000);
    }

    if (pad.Buttons & PSP_CTRL_LEFT) {
        decode_x--;
        switch (decode_c) {
            case 0:
                if ((signed) decode_x == -1) { decode_x = 0; }
                break;
            case 1:
                if ((signed) decode_x == -1) {
                    decode_x = 7;
                    decode_c--;
                }
                break;
        }
        menu_draw();
        if (cheat_button_age_x < 12) cheat_button_age_x++;
        menu_key_delay(150000 - (10000 * cheat_button_age_x));
    } else if (pad.Buttons & PSP_CTRL_RIGHT) {
        decode_x++;
        switch (decode_c) {
            case 0:
                if (decode_x > 7) {
                    decode_x = 0;
                    decode_c++;
                }
                break;
            case 1:
                if (decode_x > 7) { decode_x = 7; }
                break;
        }
        menu_draw();
        if (cheat_button_age_x < 12) cheat_button_age_x++;
        menu_key_delay(150000 - (10000 * cheat_button_age_x));
    } else {
        cheat_button_age_x = 0;
    }

    if (ext_selected[3]) {
        if (pad.Buttons & PSP_CTRL_UP) {
            switch (decode_c) {
                case 0:
                    if (decode_x == 7) {
                        decode_address += 4;
                    } else {
                        decode_address += (1 << (4 * (7 - decode_x)));
                    }
                    if (decode_address < START_ADDRESS) {
                        decode_address = START_ADDRESS;
                    }
                    if (decode_address > 0x49FFFFA8) {
                        decode_address = 0x49FFFFA8;
                    }
                    break;
                case 1:
                    *((unsigned int *) (decode_address + (decode_y * 4))) += (1 << (4 * (7 - decode_x)));
                    sceKernelDcacheWritebackInvalidateRange(
                            ((unsigned int *) (decode_address + (decode_y * 4))), 4);
                    sceKernelIcacheInvalidateRange(((unsigned int *) (decode_address + (decode_y * 4))),
                                                   4);
                    break;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            switch (decode_c) {
                case 0:
                    if (decode_x == 7) {
                        decode_address -= 4;
                    } else {
                        decode_address -= (1 << (4 * (7 - decode_x)));
                    }
                    if (decode_address < START_ADDRESS) {
                        decode_address = START_ADDRESS;
                    }
                    if (decode_address > 0x49FFFFA8) {
                        decode_address = 0x49FFFFA8;
                    }
                    break;
                case 1:
                    *((unsigned int *) (decode_address + (decode_y * 4))) -= (1 << (4 * (7 - decode_x)));
                    sceKernelDcacheWritebackInvalidateRange(
                            ((unsigned int *) (decode_address + (decode_y * 4))), 4);
                    sceKernelIcacheInvalidateRange(((unsigned int *) (decode_address + (decode_y * 4))),
                                                   4);
                    break;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
    } else if (pad.Buttons & PSP_CTRL_SQUARE) {
        if (pad.Buttons & PSP_CTRL_UP) {
            if (decode_address > START_ADDRESS) {
                decode_address -= 4;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            if (decode_address < 0x49FFFFA8) {
                decode_address += 4;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
        if ((abs((signed) (pad.Ly - 127)) - 40) > 0) {
            decode_address += (((signed) (pad.Ly - 127)) / browse_lines) * browse_lines;
            if (decode_address < START_ADDRESS) {
                decode_address = START_ADDRESS;
            }
            if (decode_address > 0x49FFFFA8) {
                decode_address = 0x49FFFFA8;
            }
            menu_draw();
            sceKernelDelayThread(18750);
        }
    } else {
        if (pad.Buttons & PSP_CTRL_UP) {
            if (decode_y > 0) {
                decode_y--;
            } else if (decode_y == 0) {
                if (decode_address > START_ADDRESS) {
                    decode_address -= 4;
                }
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            if (decode_y < 21) {
                decode_y++;
            } else if (decode_y == 21) {
                if (decode_address < 0x49FFFFA8) {
                    decode_address += 4;
                }
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
    }
}

// Tab 5: Settings
void menu_input_settings_tab(void) {
    int fd;
    unsigned int counter = 0;
    unsigned int scounter = 0;

    if (pad.Buttons & PSP_CTRL_UP) {
        if (cheat_selected > 0) {
            cheat_selected--;
        } else if (cheat_selected == 0) {
            cheat_selected = 11;
        }
        menu_draw();
        if (cheat_button_age_y < 13) cheat_button_age_y++;
        menu_key_delay(150000 - (10000 * cheat_button_age_y));
    } else if (pad.Buttons & PSP_CTRL_DOWN) {
        if (cheat_selected < 11) {
            cheat_selected++;
        } else if (cheat_selected == 11) {
            cheat_selected = 0;
        }
        menu_draw();
        if (cheat_button_age_y < 12) cheat_button_age_y++;
        menu_key_delay(150000 - (10000 * cheat_button_age_y));
    } else {
        cheat_button_age_y = 0;
    }
    if (pad.Buttons & PSP_CTRL_LEFT) {
        if ((cheat_selected == 8) && (cheat_hz > 0)) {
            cheat_hz -= 15625;
        }
        if ((cheat_selected == 3) && (dump_no > 0)) {
            dump_no--;
        }
        if ((cheat_selected == 2) && (dump_no > 0)) {
            dump_no--;
        }
        menu_draw();
        if (cheat_button_age_x < 12) cheat_button_age_x++;
        menu_key_delay(150000 - (10000 * cheat_button_age_x));
    } else if (pad.Buttons & PSP_CTRL_RIGHT) {
        if (cheat_selected == 8) {
            cheat_hz += 15625;
        }
        if (cheat_selected == 3) {
            dump_no++;
        }
        if (cheat_selected == 2) {
            dump_no++;
        }
        menu_draw();
        if (cheat_button_age_x < 12) cheat_button_age_x++;
        menu_key_delay(150000 - (10000 * cheat_button_age_x));
    } else {
        cheat_button_age_x = 0;
    }
    if (pad.Buttons & PSP_CTRL_CROSS) {
        if (cheat_selected == 0) {
            cheat_pause = !cheat_pause;
            menu_draw();

            if (cheat_pause) {
                game_pause(thid);
            } else {
                game_resume(thid);
            }
        } else if (cheat_selected == 1) {
            cheat_new(4, 0x4000, 0);

            //Switch to the cheat editor
            menu_clear_screen();
            ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
            ext_selected[0] = cheat[cheat_total - 1].block;
            cheat_selected = cheat_total - 1;
            ext_menu = EXT_EDIT_CHEAT;
            ext_opt = EDITOR_RETURN_SETTINGS;
            menu_draw();
            menu_key_delay(150000);
        } else if (cheat_selected == 2) {
            snprintf(buffer, sizeof(buffer), "ms0:/dump%d.ram", dump_no);

            fd = sceIoOpen(buffer, PSP_O_RDONLY, 0777);
            if (fd > 0) {
                counter = 0;
                while (counter < cheat_total) {
                    scounter = cheat[counter].block;
                    cheat_dma = 0;
                    while (scounter < cheat[counter].block + cheat[counter].len) {
                        if (block[scounter].flags & FLAG_DMA) {
                            if (block[scounter].hak_val != 0xFFFFFFFF) {
                                sceIoLseek(fd, block[scounter].hak_val, SEEK_SET);
                                sceIoRead(fd, &cheat_dma, sizeof(unsigned int));
                                block[scounter].std_val = cheat_dma;
                            } else {
                                cheat_dma = 0;
                            }
                        } else {
                            sceIoLseek(fd, (cheat_dma + block[scounter].address) - USER_MEM_BASE, SEEK_SET);
                            switch (block[scounter].flags & FLAG_DWORD) {
                                case FLAG_DWORD:
                                    sceIoRead(fd, &block[scounter].std_val, sizeof(unsigned int));
                                    break;
                                case FLAG_WORD:
                                    sceIoRead(fd, &block[scounter].std_val, sizeof(unsigned short));
                                    break;
                                case FLAG_BYTE:
                                    sceIoRead(fd, &block[scounter].std_val, sizeof(unsigned char));
                                    break;
                            }
                        }

                        scounter++;
                    }

                    cheat_disable(counter);
                    counter++;
                }
                sceIoClose(fd);

                cheat_status = 0;
            } else {
                LINE_CLEAR(33);
                pspDebugScreenSetTextColor(0xFF0000FF);
                pspDebugScreenPuts("ERROR: Selected RAM Dump # does not exist!");
                sceKernelDelayThread(3000000);
            }

            menu_draw();
        } else if (cheat_selected == 3) {
            snprintf(buffer, sizeof(buffer), "ms0:/dump%d.ram", dump_no);

            fd = sceIoOpen(buffer, PSP_O_WRONLY | PSP_O_CREAT, 0777);
            if (fd > 0) {
                if (sceIoWrite(fd, (void *) USER_MEM_BASE, 0x1800000) == 0x1800000) {
                    sceIoClose(fd);

                    dump_no++;
                } else {
                    sceIoClose(fd);

                    sceIoRemove(buffer);

                    LINE_CLEAR(33);
                    pspDebugScreenSetTextColor(0xFF0000FF);
                    pspDebugScreenPuts("ERROR: MemoryStick out of Space!");
                    sceKernelDelayThread(3000000);
                }
            }
            menu_draw();
            menu_key_delay(150000);//Delay twice
        } else if (cheat_selected == 4) {
            if (browse_lines == 8) {
                browse_lines = 16;
            } else {
                browse_lines = 8;
            }
            browse_c = 0;
            browse_x = 0;
            browse_y = 0;
            if (browse_address < START_ADDRESS) {
                browse_address = START_ADDRESS;
            }
            if (browse_address > (END_ADDRESS - (22 * browse_lines))) {
                browse_address = (END_ADDRESS - (22 * browse_lines));
            }
            menu_draw();
        } else if (cheat_selected == 5) {
            browse_format = (browse_format == USER_MEM_UNCACHED) ? UNCACHED : USER_MEM_UNCACHED;
            menu_draw();
        } else if (cheat_selected == 6) {
            decode_format = (decode_format == USER_MEM_UNCACHED) ? UNCACHED : USER_MEM_UNCACHED;
            menu_draw();
        } else if (cheat_selected == 7) {
            if (track_format == USER_MEM_UNCACHED) {
                track_format = UNCACHED;
            } else {
                track_format = USER_MEM_UNCACHED;
            }
            menu_draw();
        } else if (cheat_selected == 9) {
            cheat_save();
            menu_draw();
            menu_key_delay(150000);//Delay twice
        } else if (cheat_selected == 10) {
            if (sceUsbGetState() & PSP_USB_ACTIVATED) {
                stop_usb();
                menu_draw();
            } else {
                int usb_failed_step = start_usb();
                menu_draw();
                if (usb_failed_step != 0) {
                    char usb_msg[48];
                    snprintf(usb_msg, sizeof(usb_msg), "USB start failed at step %d", usb_failed_step);
                    pspDebugScreenSetXY(0, 33);
                    pspDebugScreenPuts(usb_msg);
                    menu_key_delay(2000000);
                }
            }
        } else if (cheat_selected == 11) {
            cheat_reload();
            cheat_selected = 0;
            menu_draw();
        }
        menu_key_delay(150000);
    }
}
