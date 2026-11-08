/*
 * Button handling for the sub-menus (ext_menu 1-4).
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


// Sub-menu 1: editing the selected cheat's address/value lines
void menu_input_edit_cheat(unsigned int pad_buttons) {
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
    if (pad.Buttons & PSP_CTRL_SQUARE) {
        //Don't allow the user to cause a dword/byte misalignment!!!
        if (!(block[ext_selected[0]].address % 4)) {
            switch (block[ext_selected[0]].flags & FLAG_DWORD) {
                case FLAG_BYTE:
                    block[ext_selected[0]].flags =
                            (block[ext_selected[0]].flags & ~FLAG_DWORD) | FLAG_WORD;
                    break;
                case FLAG_WORD:
                    block[ext_selected[0]].flags =
                            (block[ext_selected[0]].flags & ~FLAG_DWORD) | FLAG_DWORD;
                    break;
                case FLAG_DWORD:
                    block[ext_selected[0]].flags =
                            (block[ext_selected[0]].flags & ~FLAG_DWORD) | FLAG_BYTE;
                    break;
            }
        }
        menu_draw();
        menu_key_delay(150000);
    }

    if (ext_selected[3]) {
        if (pad.Buttons & PSP_CTRL_UP) {
            switch (ext_selected[1]) {
                case 0:
                    if (block[ext_selected[0]].flags & FLAG_DMA) {
                        //Do nothing =o
                        break;
                    } else if (((block[ext_selected[0]].flags & FLAG_DWORD) == FLAG_DWORD) &&
                               (ext_selected[2] == 7))//Prevent user from misaligned dwords/bytes
                    {
                        block[ext_selected[0]].address += 4;
                    } else if (((block[ext_selected[0]].flags & FLAG_DWORD) == FLAG_WORD) &&
                               (ext_selected[2] == 7))//Prevent user from misaligned dwords/bytes
                    {
                        block[ext_selected[0]].address += 2;
                    } else {
                        block[ext_selected[0]].address += (1 << (4 * (7 - ext_selected[2])));
                    }
                    if (block[ext_selected[0]].address < USER_MEM_BASE) {
                        block[ext_selected[0]].address = USER_MEM_BASE;
                    }
                    if (block[ext_selected[0]].address > 0x09FFFFFF) {
                        block[ext_selected[0]].address = 0x09FFFFFC;
                    }
                    if (cheat_saved)//Re-Update the std_val
                    {
                        switch (block[ext_selected[0]].flags & FLAG_DWORD) {
                            case FLAG_BYTE:
                                block[ext_selected[0]].std_val = *((unsigned char *) (block[ext_selected[0]].address));
                                break;
                            case FLAG_WORD:
                                block[ext_selected[0]].std_val = *((unsigned short *) (block[ext_selected[0]].address));
                                break;
                            case FLAG_DWORD:
                                block[ext_selected[0]].std_val = *((unsigned int *) (block[ext_selected[0]].address));
                                break;
                            default:
                                block[block_total].flags |= FLAG_UWORD;
                        }
                    }
                    break;
                case 1:
                    if (block[ext_selected[0]].flags & FLAG_FREEZE) {
                        //Do nothing =o
                        break;
                    }
                    block[ext_selected[0]].hak_val += (1 << (4 * (7 - ext_selected[2])));
                    break;
                case 2:
                    block[ext_selected[0]].hak_val += dec_delta[ext_selected[2]];
                    break;
                case 3:
                    block[ext_selected[0]].hak_val += (1 << (8 * (ext_selected[2])));
                    break;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            switch (ext_selected[1]) {
                case 0:
                    if (block[ext_selected[0]].flags & FLAG_DMA) {
                        //Do nothing =o
                        break;
                    } else if (((block[ext_selected[0]].flags & FLAG_DWORD) == FLAG_DWORD) &&
                               (ext_selected[2] == 7))//Prevent user from misaligned dwords/bytes
                    {
                        block[ext_selected[0]].address -= 4;
                    } else if (((block[ext_selected[0]].flags & FLAG_DWORD) == FLAG_WORD) &&
                               (ext_selected[2] == 7))//Prevent user from misaligned dwords/bytes
                    {
                        block[ext_selected[0]].address -= 2;
                    } else {
                        block[ext_selected[0]].address -= (1 << (4 * (7 - ext_selected[2])));
                    }
                    if (block[ext_selected[0]].address < USER_MEM_BASE) {
                        block[ext_selected[0]].address = USER_MEM_BASE;
                    }
                    if (block[ext_selected[0]].address > 0x09FFFFFF) {
                        block[ext_selected[0]].address = 0x09FFFFFC;
                    }
                    if (cheat_saved)//Re-Update the std_val
                    {
                        switch (block[ext_selected[0]].flags & FLAG_DWORD) {
                            case FLAG_BYTE:
                                block[ext_selected[0]].std_val = *((unsigned char *) (block[ext_selected[0]].address));
                                break;
                            case FLAG_WORD:
                                block[ext_selected[0]].std_val = *((unsigned short *) (block[ext_selected[0]].address));
                                break;
                            case FLAG_DWORD:
                                block[ext_selected[0]].std_val = *((unsigned int *) (block[ext_selected[0]].address));
                                break;
                            default:
                                block[block_total].flags |= FLAG_UWORD;
                        }
                    }
                    break;
                case 1:
                    if (block[ext_selected[0]].flags & FLAG_FREEZE) {
                        //Do nothing =o
                        break;
                    }
                    block[ext_selected[0]].hak_val -= (1 << (4 * (7 - ext_selected[2])));
                    break;
                case 2:
                    block[ext_selected[0]].hak_val -= dec_delta[ext_selected[2]];
                    break;
                case 3:
                    block[ext_selected[0]].hak_val -= (1 << (8 * (ext_selected[2])));
                    break;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
    } else {
        if (pad.Buttons & PSP_CTRL_UP) {
            if (ext_selected[0] > cheat[cheat_selected].block) {
                ext_selected[0]--;
            } else if (ext_selected[0] == cheat[cheat_selected].block) {
                ext_selected[0] = (cheat[cheat_selected].block + cheat[cheat_selected].len) - 1;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            if (ext_selected[0] < ((cheat[cheat_selected].block + cheat[cheat_selected].len) - 1)) {
                ext_selected[0]++;
            } else if (ext_selected[0] ==
                       ((cheat[cheat_selected].block + cheat[cheat_selected].len) - 1)) {
                ext_selected[0] = cheat[cheat_selected].block;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
    }

    if (pad.Buttons & PSP_CTRL_LEFT) {
        ext_selected[2]--;
        switch (ext_selected[1]) {
            case 0:
                if ((signed) ext_selected[2] == -1) { ext_selected[2] = 0; }
                break;
            case 1:
                if ((signed) ext_selected[2] == -1) {
                    ext_selected[2] = 7;
                    ext_selected[1]--;
                }
                break;
            case 2:
                if ((signed) ext_selected[2] == -1) {
                    ext_selected[2] = 7;
                    ext_selected[1]--;
                }
                break;
            case 3:
                if ((signed) ext_selected[2] == -1) {
                    ext_selected[2] = 9;
                    ext_selected[1]--;
                }
                break;
        }
        menu_draw();
        if (cheat_button_age_x < 12) cheat_button_age_x++;
        menu_key_delay(150000 - (10000 * cheat_button_age_x));
    } else if (pad.Buttons & PSP_CTRL_RIGHT) {
        ext_selected[2]++;
        switch (ext_selected[1]) {
            case 0:
                if (ext_selected[2] > 7) {
                    ext_selected[2] = 0;
                    ext_selected[1]++;
                }
                break;
            case 1:
                if (ext_selected[2] > 7) {
                    ext_selected[2] = 0;
                    ext_selected[1]++;
                }
                break;
            case 2:
                if (ext_selected[2] > 9) {
                    ext_selected[2] = 0;
                    ext_selected[1]++;
                }
                break;
            case 3:
                if (ext_selected[2] > 3) { ext_selected[2] = 3; }
                break;
        }
        menu_draw();
        if (cheat_button_age_x < 12) cheat_button_age_x++;
        menu_key_delay(150000 - (10000 * cheat_button_age_x));
    } else {
        cheat_button_age_x = 0;
    }

    if ((pad_buttons & PSP_CTRL_CIRCLE) && !(pad.Buttons & PSP_CTRL_CIRCLE)) {
        if (ext_selected[3]) {
            ext_selected[3] = 0;
            menu_draw();
            menu_key_delay(150000);
        } else {
            menu_clear_screen();
            ext_selected[0] = ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
            ext_menu = EXT_NONE;
            if (ext_opt == EDITOR_RETURN_SETTINGS) {
                cheat_selected = 1;//Back on the "Add Empty Cheat?" row
            } else if (ext_opt) {
                ext_selected[0] = ext_opt_arg;
                menu_clear_screen();
                ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
                ext_menu = EXT_EDIT_CHEAT + ext_opt;//EDITOR_RETURN_SEARCH -> EXT_SEARCH, EDITOR_RETURN_DIFF_SEARCH -> EXT_DIFF_SEARCH
                cheat_search = 1;
            }
            ext_opt = 0;
            menu_draw();
            menu_key_delay(150000);
            return;
        }
    }
}

// Sub-menu 2: exact value search
void menu_input_edit_search(unsigned int pad_buttons) {
    int fd;
    int fd2;
    unsigned int counter = 0;
    unsigned int scounter = 0;
    unsigned char misc_type = 0;

    if (pad.Buttons & PSP_CTRL_SELECT) {
        copy_menu = 1;
        menu_draw();
        menu_key_delay(150000);
    }
    if (pad.Buttons & PSP_CTRL_SQUARE) {
        if (search_no == 0)//Don't allow the user to change type later on!!!
        {
            switch (search_history[0].flags & FLAG_DWORD) {
                case FLAG_BYTE:
                    search_history[0].flags = (search_history[0].flags & ~FLAG_DWORD) | FLAG_WORD;
                    break;
                case FLAG_WORD:
                    search_history[0].flags = (search_history[0].flags & ~FLAG_DWORD) | FLAG_DWORD;
                    break;
                case FLAG_DWORD:
                    search_history[0].flags = (search_history[0].flags & ~FLAG_DWORD) | FLAG_BYTE;
                    break;
            }
        }
        menu_draw();
        menu_key_delay(150000);
    }
    if (pad.Buttons & PSP_CTRL_CROSS) {
        if (ext_selected[0] == 0) {
            ext_selected[3] = !ext_selected[3];
        } else if (ext_selected[0] == 1) {
            //Update the search history!
            if (search_history_counter < 15) {
                search_history_counter++;
            }
            memmove(&search_history[1], &search_history[0], sizeof(Block) * (15));
            search_history[1].std_val = 6;

            //Move the cursor back
            ext_selected[0] = 0;

            //Is it the first search?
            if (search_no == 0) {
                //Increment the search
                search_no++;
                search_max++;

                //Setup the variables
                search_result_counter = 0;

                //Open the file
                snprintf(buffer, sizeof(buffer), "ms0:/search%d.dat", search_no);
                fd = sceIoOpen(buffer, PSP_O_WRONLY | PSP_O_CREAT, 0777);
                if (fd > 0) {
                    //Write out the search_history[0] type
                    switch (search_history[0].flags & FLAG_DWORD) {
                        case FLAG_DWORD:
                            if (sceIoWrite(fd, "4", 1) != 1) goto error_read_exact_a;
                            break;
                        case FLAG_WORD:
                            if (sceIoWrite(fd, "2", 1) != 1) goto error_read_exact_a;
                            break;
                        case FLAG_BYTE:
                            if (sceIoWrite(fd, "1", 1) != 1) goto error_read_exact_a;
                            break;
                    }

                    //Search!
                    counter = START_ADDRESS;

                    //Helper
                    while (counter < END_ADDRESS) {
                        //Helper
                        if (!((counter - START_ADDRESS) & 0xFFFF)) {
                            if (!cheat_pause) sceKernelDelayThread(1500);

                            LINE_CLEAR(33);
                            pspDebugScreenSetTextColor(0xFFFF8000);
                            snprintf(buffer, sizeof(buffer), "Searching = %02d%%; Hold () to Abort",
                                    (counter - START_ADDRESS) / ((END_ADDRESS - START_ADDRESS) / 100));
                            pspDebugScreenPuts(buffer);

                            sceCtrlPeekBufferPositive(&pad, 1);

                            if (pad.Buttons & PSP_CTRL_CIRCLE) {
                                LINE_CLEAR(33);
                                pspDebugScreenSetTextColor(0xFFFF8000);
                                pspDebugScreenPuts("Search Aborted");

                                do {
                                    sceKernelDelayThread(150000);
                                    sceCtrlPeekBufferPositive(&pad, 1);
                                } while (pad.Buttons & PSP_CTRL_CIRCLE);
                                break;
                            }
                        }

                        //Check
                        switch (search_history[0].flags & FLAG_DWORD) {
                            case FLAG_DWORD:
                                if (*((unsigned int *) (counter)) ==
                                    (unsigned int) search_history[0].hak_val) {
                                    //Add it
                                    if (sceIoWrite(fd, &counter, sizeof(unsigned int)) != 4)
                                        goto error_read_exact_a;
                                    if (sceIoWrite(fd, &search_history[0].hak_val,
                                                   sizeof(unsigned int)) != 4)
                                        goto error_read_exact_a;
                                    search_result_counter++;
                                }
                                counter += 4;
                                break;

                            case FLAG_WORD:
                                if (*((unsigned short *) (counter)) ==
                                    (unsigned short) search_history[0].hak_val) {
                                    //Add it
                                    if (sceIoWrite(fd, &counter, sizeof(unsigned int)) != 4)
                                        goto error_read_exact_a;
                                    if (sceIoWrite(fd, &search_history[0].hak_val,
                                                   sizeof(unsigned short)) != 2)
                                        goto error_read_exact_a;
                                    search_result_counter++;
                                }
                                counter += 2;
                                break;

                            case FLAG_BYTE:
                                if (*((unsigned char *) (counter)) ==
                                    (unsigned char) search_history[0].hak_val) {
                                    //Add it
                                    if (sceIoWrite(fd, &counter, sizeof(unsigned int)) != 4)
                                        goto error_read_exact_a;
                                    if (sceIoWrite(fd, &search_history[0].hak_val,
                                                   sizeof(unsigned char)) != 1)
                                        goto error_read_exact_a;
                                    search_result_counter++;
                                }
                                counter++;
                                break;
                        }
                    }
                    //Close the file since we are done with the search
                    sceIoClose(fd);

                    while (1) {
                        break;
                    //ReadShort
                    error_read_exact_a:
                        sceIoClose(fd);
                        if (search_no > 0) search_no--;
                        sceIoRemove(buffer);

                        LINE_CLEAR(33);
                        pspDebugScreenSetTextColor(0xFF0000FF);
                        pspDebugScreenPuts("ERROR: MemoryStick out of Space!");
                        sceKernelDelayThread(3000000);
                        break;
                    }
                } else {
                    //ERROR - file couldn't be opened - undo search attempt
                    if (search_no > 0) search_no--;
                }
            } else//Continue the search with a different exact number
            {
                //Increment the search
                search_no++;
                search_max++;

                //Open the files
                snprintf(buffer, sizeof(buffer), "ms0:/search%d.dat", search_no - 1);
                fd = sceIoOpen(buffer, PSP_O_RDONLY, 0777);
                snprintf(buffer, sizeof(buffer), "ms0:/search%d.dat", search_no);
                fd2 = sceIoOpen(buffer, PSP_O_WRONLY | PSP_O_CREAT, 0777);
                if ((fd > 0) && (fd2 > 0)) {
                    //Get ready to go through and check the addresses
                    sceIoLseek(fd, 1, SEEK_SET);

                    //Write out the search_history[0] type
                    switch (search_history[0].flags & FLAG_DWORD) {
                        case FLAG_DWORD:
                            if (sceIoWrite(fd2, "4", 1) != 1) goto error_read_exact_b;
                            break;
                        case FLAG_WORD:
                            if (sceIoWrite(fd2, "2", 1) != 1) goto error_read_exact_b;
                            break;
                        case FLAG_BYTE:
                            if (sceIoWrite(fd2, "1", 1) != 1) goto error_read_exact_b;
                            break;
                    }

                    //Loop through the list checking each one
                    counter = search_result_counter;
                    search_result_counter = 0;
                    while (counter > 0) {
                        //Load it
                        sceIoRead(fd, &scounter, sizeof(unsigned int));

                        //Helper
                        if (!(counter & 0x3F)) {
                            if (!cheat_pause) sceKernelDelayThread(1500);

                            LINE_CLEAR(33);
                            pspDebugScreenSetTextColor(0xFFFF8000);
                            snprintf(buffer, sizeof(buffer), "Searching = %02d%%; Hold () to Abort",
                                    (scounter - START_ADDRESS) / ((END_ADDRESS - START_ADDRESS) / 100));
                            pspDebugScreenPuts(buffer);

                            sceCtrlPeekBufferPositive(&pad, 1);

                            if (pad.Buttons & PSP_CTRL_CIRCLE) {
                                LINE_CLEAR(33);
                                pspDebugScreenSetTextColor(0xFFFF8000);
                                pspDebugScreenPuts("Search Aborted");

                                do {
                                    sceKernelDelayThread(150000);
                                    sceCtrlPeekBufferPositive(&pad, 1);
                                } while (pad.Buttons & PSP_CTRL_CIRCLE);
                                break;
                            }
                        }

                        //Check
                        switch (search_history[0].flags & FLAG_DWORD) {
                            case FLAG_DWORD:
                                sceIoLseek(fd, 4, SEEK_CUR);
                                if (*((unsigned int *) (scounter)) ==
                                    (unsigned int) search_history[0].hak_val) {
                                    //Add it
                                    if (sceIoWrite(fd2, &scounter, sizeof(unsigned int)) != 4)
                                        goto error_read_exact_b;
                                    if (sceIoWrite(fd2, &search_history[0].hak_val,
                                                   sizeof(unsigned int)) != 4)
                                        goto error_read_exact_b;
                                    search_result_counter++;
                                }
                                break;

                            case FLAG_WORD:
                                sceIoLseek(fd, 2, SEEK_CUR);
                                if (*((unsigned short *) (scounter)) ==
                                    (unsigned short) search_history[0].hak_val) {
                                    //Add it
                                    if (sceIoWrite(fd2, &scounter, sizeof(unsigned int)) != 4)
                                        goto error_read_exact_b;
                                    if (sceIoWrite(fd2, &search_history[0].hak_val,
                                                   sizeof(unsigned short)) != 2)
                                        goto error_read_exact_b;
                                    search_result_counter++;
                                }
                                break;

                            case FLAG_BYTE:
                                sceIoLseek(fd, 1, SEEK_CUR);
                                if (*((unsigned char *) (scounter)) ==
                                    (unsigned char) search_history[0].hak_val) {
                                    //Add it
                                    if (sceIoWrite(fd2, &scounter, sizeof(unsigned int)) != 4)
                                        goto error_read_exact_b;
                                    if (sceIoWrite(fd2, &search_history[0].hak_val,
                                                   sizeof(unsigned char)) != 1)
                                        goto error_read_exact_b;
                                    search_result_counter++;
                                }
                                break;
                        }

                        //Next
                        counter--;
                    }

                    //Close the files
                    sceIoClose(fd);
                    sceIoClose(fd2);

                    while (1) {
                        break;
                    //ReadShort
                    error_read_exact_b:
                        sceIoClose(fd);
                        sceIoClose(fd2);
                        if (search_no > 0) search_no--;
                        sceIoRemove(buffer);

                        LINE_CLEAR(33);
                        pspDebugScreenSetTextColor(0xFF0000FF);
                        pspDebugScreenPuts("ERROR: MemoryStick out of Space!");
                        sceKernelDelayThread(3000000);
                        break;
                    }
                } else {
                    //ERROR - files couldn't be opened - undo search attempt
                    sceIoClose(fd);
                    sceIoClose(fd2);
                    if (search_no > 0) search_no--;
                }
            }
        } else if (ext_selected[0] == 2) {
            //Reset the search
            if (search_no > 0) search_no--;

            //Move the cursor back
            ext_selected[0] = 0;
        } else {
            //Add the single cheat
            switch (search_history[0].flags & FLAG_DWORD) {
                case FLAG_DWORD:
                    cheat_new(4, search_address[ext_selected[0] - 3] - USER_MEM_UNCACHED,
                             *((unsigned int *) (search_address[ext_selected[0] - 3])));
                    break;
                case FLAG_WORD:
                    cheat_new(2, search_address[ext_selected[0] - 3] - USER_MEM_UNCACHED,
                             *((unsigned short *) (search_address[ext_selected[0] - 3])));
                    break;
                case FLAG_BYTE:
                    cheat_new(1, search_address[ext_selected[0] - 3] - USER_MEM_UNCACHED,
                             *((unsigned char *) (search_address[ext_selected[0] - 3])));
                    break;
                default:
                    break;
            }

            //Switch to the cheat editor
            ext_opt_arg = ext_selected[0];
            menu_clear_screen();
            ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
            ext_selected[0] = cheat[cheat_total - 1].block;
            cheat_selected = cheat_total - 1;
            ext_menu = EXT_EDIT_CHEAT;
            ext_opt = EDITOR_RETURN_SEARCH;
            menu_draw();
            menu_key_delay(150000);
        }

        //Load the file again, get the sample numbers
        if (search_no > 0) {
            //Open the file
            snprintf(buffer, sizeof(buffer), "ms0:/search%d.dat", search_no);
            fd = sceIoOpen(buffer, PSP_O_RDONLY, 0777);
            if (fd > 0) {
                //Get the value size
                sceIoRead(fd, &misc_type, 1);
                misc_type -= '0';

                switch (misc_type) {
                    case 1:
                        search_history[0].flags = (search_history[0].flags & ~FLAG_DWORD) | FLAG_BYTE;
                        break;
                    case 2:
                        search_history[0].flags = (search_history[0].flags & ~FLAG_DWORD) | FLAG_WORD;
                        break;
                    case 4:
                        search_history[0].flags = (search_history[0].flags & ~FLAG_DWORD) | FLAG_DWORD;
                        break;
                }

                //Get the file size
                search_result_counter = sceIoLseek(fd, 0, SEEK_END);
                sceIoLseek(fd, 1, SEEK_SET);
                search_result_counter--;
                search_result_counter /= (sizeof(unsigned int) + misc_type);

                //Only load the first 100
                if (search_result_counter > 100) {
                    for (scounter = 0; scounter < 100; scounter++) {
                        sceIoRead(fd, &search_address[scounter], sizeof(unsigned int));
                        sceIoLseek(fd, misc_type, SEEK_CUR);
                    }
                } else {
                    for (scounter = 0; scounter < search_result_counter; scounter++) {
                        sceIoRead(fd, &search_address[scounter], sizeof(unsigned int));
                        sceIoLseek(fd, misc_type, SEEK_CUR);
                    }
                }

                //Close the file since we are done with the search
                sceIoClose(fd);
            }
        } else {
            search_result_counter = 0;
        }

        menu_clear_screen();
        menu_draw();
        menu_key_delay(150000);
    }

    if (ext_selected[3]) {
        if (pad.Buttons & PSP_CTRL_UP) {
            switch (ext_selected[1]) {
                case 0:
                    search_history[0].hak_val += (1 << (4 * (7 - ext_selected[2])));
                    break;
                case 1:
                    search_history[0].hak_val += dec_delta[ext_selected[2]];
                    break;
                case 2:
                    search_history[0].hak_val += (1 << (8 * (ext_selected[2])));
                    break;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            switch (ext_selected[1]) {
                case 0:
                    search_history[0].hak_val -= (1 << (4 * (7 - ext_selected[2])));
                    break;
                case 1:
                    search_history[0].hak_val -= dec_delta[ext_selected[2]];
                    break;
                case 2:
                    search_history[0].hak_val -= (1 << (8 * (ext_selected[2])));
                    break;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
    } else {
        if (pad.Buttons & PSP_CTRL_UP) {
            if (ext_selected[0] > 0) {
                ext_selected[0]--;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            if (ext_selected[0] <
                (1 + (!!search_no) + (search_result_counter > 100 ? 100 : search_result_counter))) {
                ext_selected[0]++;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
    }

    if (ext_selected[0] == 0) {
        if (pad.Buttons & PSP_CTRL_LEFT) {
            ext_selected[2]--;
            switch (ext_selected[1]) {
                case 0:
                    if ((signed) ext_selected[2] == -1) { ext_selected[2] = 0; }
                    break;
                case 1:
                    if ((signed) ext_selected[2] == -1) {
                        ext_selected[2] = 7;
                        ext_selected[1]--;
                    }
                    break;
                case 2:
                    if ((signed) ext_selected[2] == -1) {
                        ext_selected[2] = 9;
                        ext_selected[1]--;
                    }
                    break;
            }
            menu_draw();
            if (cheat_button_age_x < 12) cheat_button_age_x++;
            menu_key_delay(150000 - (10000 * cheat_button_age_x));
        } else if (pad.Buttons & PSP_CTRL_RIGHT) {
            ext_selected[2]++;
            switch (ext_selected[1]) {
                case 0:
                    if (ext_selected[2] > 7) {
                        ext_selected[2] = 0;
                        ext_selected[1]++;
                    }
                    break;
                case 1:
                    if (ext_selected[2] > 9) {
                        ext_selected[2] = 0;
                        ext_selected[1]++;
                    }
                    break;
                case 2:
                    if (ext_selected[2] > 3) { ext_selected[2] = 3; }
                    break;
            }
            menu_draw();
            if (cheat_button_age_x < 12) cheat_button_age_x++;
            menu_key_delay(150000 - (10000 * cheat_button_age_x));
        } else {
            cheat_button_age_x = 0;
        }
    }

    if ((pad_buttons & PSP_CTRL_CIRCLE) && !(pad.Buttons & PSP_CTRL_CIRCLE)) {
        if (ext_selected[3]) {
            ext_selected[3] = 0;
            menu_draw();
            menu_key_delay(150000);
        } else//Search has been aborted!
        {
            //Do we need to act like the search didn't even happen???
            if (search_no == 0) {
                cheat_search = 0;
            }

            //Go back
            cheat_selected = 0;
            menu_clear_screen();
            ext_selected[0] = ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
            ext_menu = EXT_NONE;
            ext_opt = 0;
            menu_draw();
            menu_key_delay(150000);
            return;
        }
    }
}

// Sub-menu 3: unknown value (difference) search
void menu_input_edit_diff_search(unsigned int pad_buttons) {
    int fd;
    int fd2;
    unsigned int counter = 0;
    unsigned int scounter = 0;
    unsigned int dcounter = 0;
    unsigned char misc_type = 0;

    if (pad.Buttons & PSP_CTRL_SELECT) {
        copy_menu = 1;
        menu_draw();
        menu_key_delay(150000);
    }
    if (pad.Buttons & PSP_CTRL_CROSS) {
        if (ext_selected[0] == 0) {
            ext_selected[3] = !ext_selected[3];
        } else if (ext_selected[0] == 1) {
            //Update the search history!
            if (search_history_counter < 15) {
                search_history_counter++;
            }
            memmove(&search_history[1], &search_history[0], sizeof(Block) * (15));
            search_history[1].std_val = search_mode;

            //Move the cursor back
            ext_selected[0] = 0;

            //Is it the first search?
            if (search_no == 0) {
                //Increment the search
                search_no++;
                search_max++;

                //Setup the variables
                search_result_counter = 0;

                //Open the files
                fd = sceIoOpen("ms0:/search.ram", PSP_O_RDONLY, 0777);
                snprintf(buffer, sizeof(buffer), "ms0:/search%d.dat", search_no);
                fd2 = sceIoOpen(buffer, PSP_O_WRONLY | PSP_O_CREAT, 0777);
                if ((fd > 0) && (fd2 > 0)) {
                    //Skip the initial 0x4000 bytes
                    sceIoLseek(fd, 0x4000, SEEK_SET);

                    //Write out the search_history[0] type
                    switch (search_history[0].flags & FLAG_DWORD) {
                        case FLAG_DWORD:
                            if (sceIoWrite(fd2, "4", 1) != 1) goto error_read_diff_a;
                            misc_type = 4;
                            break;
                        case FLAG_WORD:
                            if (sceIoWrite(fd2, "2", 1) != 1) goto error_read_diff_a;
                            misc_type = 2;
                            break;
                        case FLAG_BYTE:
                            if (sceIoWrite(fd2, "1", 1) != 1) goto error_read_diff_a;
                            misc_type = 1;
                            break;
                    }

                    //Get ready
                    counter = START_ADDRESS;

                    //Go!
                    while (counter < END_ADDRESS) {
                        //Load it
                        FILE_BUFFER_READ(&scounter, misc_type);

                        //Helper
                        if (!((counter - START_ADDRESS) & 0xFFFF)) {
                            if (!cheat_pause) sceKernelDelayThread(1500);

                            LINE_CLEAR(33);
                            pspDebugScreenSetTextColor(0xFFFF8000);
                            snprintf(buffer, sizeof(buffer), "Searching = %02d%%; Hold () to Abort",
                                    (counter - START_ADDRESS) / ((END_ADDRESS - START_ADDRESS) / 100));
                            pspDebugScreenPuts(buffer);

                            sceCtrlPeekBufferPositive(&pad, 1);

                            if (pad.Buttons & PSP_CTRL_CIRCLE) {
                                LINE_CLEAR(33);
                                pspDebugScreenSetTextColor(0xFFFF8000);
                                pspDebugScreenPuts("Search Aborted");

                                do {
                                    sceKernelDelayThread(150000);
                                    sceCtrlPeekBufferPositive(&pad, 1);
                                } while (pad.Buttons & PSP_CTRL_CIRCLE);
                                break;
                            }
                        }

                        //Check
                        switch (search_history[0].flags & FLAG_DWORD) {
                            case FLAG_DWORD:
                                if (search_mode == 0) {
                                    if ((unsigned int) scounter != *((unsigned int *) (counter))) break;
                                } else if (search_mode == 1) {
                                    if ((unsigned int) scounter == *((unsigned int *) (counter))) break;
                                } else if (search_mode == 2) {
                                    if ((unsigned int) scounter >= *((unsigned int *) (counter))) break;
                                } else if (search_mode == 3) {
                                    if ((unsigned int) scounter <= *((unsigned int *) (counter))) break;
                                } else if (search_mode == 4) {
                                    if ((unsigned int) ((unsigned int) scounter +
                                                        (unsigned int) search_history[0].hak_val) !=
                                        *((unsigned int *) (counter)))
                                        break;
                                } else if (search_mode == 5) {
                                    if ((unsigned int) ((unsigned int) scounter -
                                                        (unsigned int) search_history[0].hak_val) !=
                                        *((unsigned int *) (counter)))
                                        break;
                                }
                                scounter = *((unsigned int *) (counter));

                                //Add it
                                if (sceIoWrite(fd2, &counter, sizeof(unsigned int)) != 4)
                                    goto error_read_diff_a;
                                if (sceIoWrite(fd2, &scounter, sizeof(unsigned int)) != 4)
                                    goto error_read_diff_a;
                                search_result_counter++;
                                break;

                            case FLAG_WORD:
                                if (search_mode == 0) {
                                    if ((unsigned short) scounter !=
                                        *((unsigned short *) (counter)))
                                        break;
                                } else if (search_mode == 1) {
                                    if ((unsigned short) scounter ==
                                        *((unsigned short *) (counter)))
                                        break;
                                } else if (search_mode == 2) {
                                    if ((unsigned short) scounter >=
                                        *((unsigned short *) (counter)))
                                        break;
                                } else if (search_mode == 3) {
                                    if ((unsigned short) scounter <=
                                        *((unsigned short *) (counter)))
                                        break;
                                } else if (search_mode == 4) {
                                    if ((unsigned short) ((unsigned short) scounter +
                                                          (unsigned short) search_history[0].hak_val) !=
                                        *((unsigned short *) (counter)))
                                        break;
                                } else if (search_mode == 5) {
                                    if ((unsigned short) ((unsigned short) scounter -
                                                          (unsigned short) search_history[0].hak_val) !=
                                        *((unsigned short *) (counter)))
                                        break;
                                }
                                scounter = *((unsigned short *) (counter));

                                //Add it
                                if (sceIoWrite(fd2, &counter, sizeof(unsigned int)) != 4)
                                    goto error_read_diff_a;
                                if (sceIoWrite(fd2, &scounter, sizeof(unsigned short)) != 2)
                                    goto error_read_diff_a;
                                search_result_counter++;
                                break;

                            case FLAG_BYTE:
                                if (search_mode == 0) {
                                    if ((unsigned char) scounter != *((unsigned char *) (counter)))
                                        break;
                                } else if (search_mode == 1) {
                                    if ((unsigned char) scounter == *((unsigned char *) (counter)))
                                        break;
                                } else if (search_mode == 2) {
                                    if ((unsigned char) scounter >= *((unsigned char *) (counter)))
                                        break;
                                } else if (search_mode == 3) {
                                    if ((unsigned char) scounter <= *((unsigned char *) (counter)))
                                        break;
                                } else if (search_mode == 4) {
                                    if ((unsigned char) ((unsigned char) scounter +
                                                         (unsigned char) search_history[0].hak_val) !=
                                        *((unsigned char *) (counter)))
                                        break;
                                } else if (search_mode == 5) {
                                    if ((unsigned char) ((unsigned char) scounter -
                                                         (unsigned char) search_history[0].hak_val) !=
                                        *((unsigned char *) (counter)))
                                        break;
                                }
                                scounter = *((unsigned char *) (counter));

                                //Add it
                                if (sceIoWrite(fd2, &counter, sizeof(unsigned int)) != 4)
                                    goto error_read_diff_a;
                                if (sceIoWrite(fd2, &scounter, sizeof(unsigned char)) != 1)
                                    goto error_read_diff_a;
                                search_result_counter++;
                                break;
                        }

                        //Next
                        counter += misc_type;
                    }

                    //Close the files
                    sceIoClose(fd);
                    sceIoClose(fd2);

                    while (1) {
                        break;
                    //ReadShort
                    error_read_diff_a:
                        sceIoClose(fd);
                        sceIoClose(fd2);
                        if (search_no > 0) search_no--;
                        sceIoRemove(buffer);

                        LINE_CLEAR(33);
                        pspDebugScreenSetTextColor(0xFF0000FF);
                        pspDebugScreenPuts("ERROR: MemoryStick out of Space!");
                        sceKernelDelayThread(3000000);
                        break;
                    }
                } else {
                    //ERROR - files couldn't be opened - undo search attempt
                    sceIoClose(fd);
                    sceIoClose(fd2);
                    if (search_no > 0) search_no--;
                }
            } else//Continue the search with a different Diff number
            {
                //Increment the search
                search_no++;
                search_max++;

                //Open the files
                snprintf(buffer, sizeof(buffer), "ms0:/search%d.dat", search_no - 1);
                fd = sceIoOpen(buffer, PSP_O_RDONLY, 0777);
                snprintf(buffer, sizeof(buffer), "ms0:/search%d.dat", search_no);
                fd2 = sceIoOpen(buffer, PSP_O_WRONLY | PSP_O_CREAT, 0777);
                if ((fd > 0) && (fd2 > 0)) {
                    //Get ready to go through and check the addresses
                    sceIoLseek(fd, 1, SEEK_SET);

                    //Write out the search_history[0] type
                    switch (search_history[0].flags & FLAG_DWORD) {
                        case FLAG_DWORD:
                            if (sceIoWrite(fd2, "4", 1) != 1) goto error_read_diff_b;
                            break;
                        case FLAG_WORD:
                            if (sceIoWrite(fd2, "2", 1) != 1) goto error_read_diff_b;
                            break;
                        case FLAG_BYTE:
                            if (sceIoWrite(fd2, "1", 1) != 1) goto error_read_diff_b;
                            break;
                    }

                    //Loop through the list checking each one
                    //TODO------Improve search functionality
                    counter = search_result_counter;
                    search_result_counter = 0;
                    while (counter > 0) {
                        //Load it
                        sceIoRead(fd, &scounter, sizeof(unsigned int));

                        //Helper
                        if (!(counter & 0x3F)) {
                            if (!cheat_pause) sceKernelDelayThread(1500);

                            LINE_CLEAR(33);
                            pspDebugScreenSetTextColor(0xFFFF8000);
                            snprintf(buffer, sizeof(buffer), "Searching = %02d%%; Hold () to Abort",
                                    (scounter - START_ADDRESS) / ((END_ADDRESS - START_ADDRESS) / 100));
                            pspDebugScreenPuts(buffer);

                            sceCtrlPeekBufferPositive(&pad, 1);

                            if (pad.Buttons & PSP_CTRL_CIRCLE) {
                                LINE_CLEAR(33);
                                pspDebugScreenSetTextColor(0xFFFF8000);
                                pspDebugScreenPuts("Search Aborted");

                                do {
                                    sceKernelDelayThread(150000);
                                    sceCtrlPeekBufferPositive(&pad, 1);
                                } while (pad.Buttons & PSP_CTRL_CIRCLE);
                                break;
                            }
                        }

                        //Check
                        switch (search_history[0].flags & FLAG_DWORD) {
                            case FLAG_DWORD:
                                sceIoRead(fd, &dcounter, sizeof(unsigned int));

                                if (search_mode == 0) {
                                    if ((unsigned int) dcounter != *((unsigned int *) (scounter)))
                                        break;
                                } else if (search_mode == 1) {
                                    if ((unsigned int) dcounter == *((unsigned int *) (scounter)))
                                        break;
                                } else if (search_mode == 2) {
                                    if ((unsigned int) dcounter >= *((unsigned int *) (scounter)))
                                        break;
                                } else if (search_mode == 3) {
                                    if ((unsigned int) dcounter <= *((unsigned int *) (scounter)))
                                        break;
                                } else if (search_mode == 4) {
                                    if ((unsigned int) ((unsigned int) dcounter +
                                                        (unsigned int) search_history[0].hak_val) !=
                                        *((unsigned int *) (scounter)))
                                        break;
                                } else if (search_mode == 5) {
                                    if ((unsigned int) ((unsigned int) dcounter -
                                                        (unsigned int) search_history[0].hak_val) !=
                                        *((unsigned int *) (scounter)))
                                        break;
                                }
                                dcounter = *((unsigned int *) (scounter));

                                //Add it
                                if (sceIoWrite(fd2, &scounter, sizeof(unsigned int)) != 4)
                                    goto error_read_diff_b;
                                if (sceIoWrite(fd2, &dcounter, sizeof(unsigned int)) != 4)
                                    goto error_read_diff_b;
                                search_result_counter++;
                                break;

                            case FLAG_WORD:
                                sceIoRead(fd, &dcounter, sizeof(unsigned short));

                                if (search_mode == 0) {
                                    if ((unsigned short) dcounter !=
                                        *((unsigned short *) (scounter)))
                                        break;
                                } else if (search_mode == 1) {
                                    if ((unsigned short) dcounter ==
                                        *((unsigned short *) (scounter)))
                                        break;
                                } else if (search_mode == 2) {
                                    if ((unsigned short) dcounter >=
                                        *((unsigned short *) (scounter)))
                                        break;
                                } else if (search_mode == 3) {
                                    if ((unsigned short) dcounter <=
                                        *((unsigned short *) (scounter)))
                                        break;
                                } else if (search_mode == 4) {
                                    if ((unsigned short) ((unsigned short) dcounter +
                                                          (unsigned short) search_history[0].hak_val) !=
                                        *((unsigned short *) (scounter)))
                                        break;
                                } else if (search_mode == 5) {
                                    if ((unsigned short) ((unsigned short) dcounter -
                                                          (unsigned short) search_history[0].hak_val) !=
                                        *((unsigned short *) (scounter)))
                                        break;
                                }
                                dcounter = *((unsigned short *) (scounter));

                                //Add it
                                if (sceIoWrite(fd2, &scounter, sizeof(unsigned int)) != 4)
                                    goto error_read_diff_b;
                                if (sceIoWrite(fd2, &dcounter, sizeof(unsigned short)) != 2)
                                    goto error_read_diff_b;
                                search_result_counter++;
                                break;

                            case FLAG_BYTE:
                                sceIoRead(fd, &dcounter, sizeof(unsigned char));

                                if (search_mode == 0) {
                                    if ((unsigned char) dcounter !=
                                        *((unsigned char *) (scounter)))
                                        break;
                                } else if (search_mode == 1) {
                                    if ((unsigned char) dcounter ==
                                        *((unsigned char *) (scounter)))
                                        break;
                                } else if (search_mode == 2) {
                                    if ((unsigned char) dcounter >=
                                        *((unsigned char *) (scounter)))
                                        break;
                                } else if (search_mode == 3) {
                                    if ((unsigned char) dcounter <=
                                        *((unsigned char *) (scounter)))
                                        break;
                                } else if (search_mode == 4) {
                                    if ((unsigned char) ((unsigned char) dcounter +
                                                         (unsigned char) search_history[0].hak_val) !=
                                        *((unsigned char *) (scounter)))
                                        break;
                                } else if (search_mode == 5) {
                                    if ((unsigned char) ((unsigned char) dcounter -
                                                         (unsigned char) search_history[0].hak_val) !=
                                        *((unsigned char *) (scounter)))
                                        break;
                                }
                                dcounter = *((unsigned char *) (scounter));

                                //Add it
                                if (sceIoWrite(fd2, &scounter, sizeof(unsigned int)) != 4)
                                    goto error_read_diff_b;
                                if (sceIoWrite(fd2, &dcounter, sizeof(unsigned char)) != 1)
                                    goto error_read_diff_b;
                                search_result_counter++;
                                break;
                        }

                        //Next
                        counter--;
                    }

                    //Close the files
                    sceIoClose(fd);
                    sceIoClose(fd2);

                    while (1) {
                        break;
                    //ReadShort
                    error_read_diff_b:
                        sceIoClose(fd);
                        sceIoClose(fd2);
                        if (search_no > 0) search_no--;
                        sceIoRemove(buffer);

                        LINE_CLEAR(33);
                        pspDebugScreenSetTextColor(0xFF0000FF);
                        pspDebugScreenPuts("ERROR: MemoryStick out of Space!");
                        sceKernelDelayThread(3000000);
                        break;
                    }
                } else {
                    //ERROR - files couldn't be opened - undo search attempt
                    sceIoClose(fd);
                    sceIoClose(fd2);
                    if (search_no > 0) search_no--;
                }
            }
        } else if (ext_selected[0] == 2) {
            //Reset the search
            if (search_no > 0) search_no--;

            //Move the cursor back
            ext_selected[0] = 0;
        } else {
            //Add the single cheat
            switch (search_history[0].flags & FLAG_DWORD) {
                case FLAG_DWORD:
                    cheat_new(4, search_address[ext_selected[0] - 3] - USER_MEM_UNCACHED,
                             *((unsigned int *) (search_address[ext_selected[0] - 3])));
                    break;
                case FLAG_WORD:
                    cheat_new(2, search_address[ext_selected[0] - 3] - USER_MEM_UNCACHED,
                             *((unsigned short *) (search_address[ext_selected[0] - 3])));
                    break;
                case FLAG_BYTE:
                    cheat_new(1, search_address[ext_selected[0] - 3] - USER_MEM_UNCACHED,
                             *((unsigned char *) (search_address[ext_selected[0] - 3])));
                    break;
                default:
                    break;
            }

            //Switch to the cheat editor
            ext_opt_arg = ext_selected[0];
            menu_clear_screen();
            ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
            ext_selected[0] = cheat[cheat_total - 1].block;
            cheat_selected = cheat_total - 1;
            ext_menu = EXT_EDIT_CHEAT;
            ext_opt = EDITOR_RETURN_DIFF_SEARCH;
            menu_draw();
            menu_key_delay(150000);
        }

        //Load the file again, get the sample numbers
        if (search_no > 0) {
            //Open the file
            snprintf(buffer, sizeof(buffer), "ms0:/search%d.dat", search_no);
            fd = sceIoOpen(buffer, PSP_O_RDONLY, 0777);
            if (fd > 0) {
                //Get the value size
                sceIoRead(fd, &misc_type, 1);
                misc_type -= '0';

                switch (misc_type) {
                    case 1:
                        search_history[0].flags = (search_history[0].flags & ~FLAG_DWORD) | FLAG_BYTE;
                        break;
                    case 2:
                        search_history[0].flags = (search_history[0].flags & ~FLAG_DWORD) | FLAG_WORD;
                        break;
                    case 4:
                        search_history[0].flags = (search_history[0].flags & ~FLAG_DWORD) | FLAG_DWORD;
                        break;
                }

                //Get the file size
                search_result_counter = sceIoLseek(fd, 0, SEEK_END);
                sceIoLseek(fd, 1, SEEK_SET);
                search_result_counter--;
                search_result_counter /= (sizeof(unsigned int) + misc_type);

                //Only load the first 100
                if (search_result_counter > 100) {
                    for (scounter = 0; scounter < 100; scounter++) {
                        sceIoRead(fd, &search_address[scounter], sizeof(unsigned int));
                        sceIoLseek(fd, misc_type, SEEK_CUR);
                    }
                } else {
                    for (scounter = 0; scounter < search_result_counter; scounter++) {
                        sceIoRead(fd, &search_address[scounter], sizeof(unsigned int));
                        sceIoLseek(fd, misc_type, SEEK_CUR);
                    }
                }

                //Close the file since we are done with the search
                sceIoClose(fd);
            }
        } else {
            search_result_counter = 0;
        }

        menu_clear_screen();
        menu_draw();
        menu_key_delay(150000);
    }

    if (ext_selected[3]) {
        if (pad.Buttons & PSP_CTRL_UP) {
            switch (ext_selected[1]) {
                case 0:
                    if (search_mode < 5) search_mode++;
                    break;
                case 1:
                    search_history[0].hak_val += (1 << (4 * (7 - ext_selected[2])));
                    break;
                case 2:
                    search_history[0].hak_val += dec_delta[ext_selected[2]];
                    break;
                case 3:
                    search_history[0].hak_val += (1 << (8 * (ext_selected[2])));
                    break;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            switch (ext_selected[1]) {
                case 0:
                    if (search_mode > 0) search_mode--;
                    break;
                case 1:
                    search_history[0].hak_val -= (1 << (4 * (7 - ext_selected[2])));
                    break;
                case 2:
                    search_history[0].hak_val -= dec_delta[ext_selected[2]];
                    break;
                case 3:
                    search_history[0].hak_val -= (1 << (8 * (ext_selected[2])));
                    break;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
    } else {
        if (pad.Buttons & PSP_CTRL_UP) {
            if (ext_selected[0] > 0) {
                ext_selected[0]--;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            if (ext_selected[0] <
                (1 + (!!search_no) + (search_result_counter > 100 ? 100 : search_result_counter))) {
                ext_selected[0]++;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
    }

    if (ext_selected[0] == 0) {
        if (pad.Buttons & PSP_CTRL_LEFT) {
            ext_selected[2]--;
            switch (ext_selected[1]) {
                case 0:
                    if ((signed) ext_selected[2] == -1) { ext_selected[2] = 0; }
                    break;
                case 1:
                    if ((signed) ext_selected[2] == -1) {
                        ext_selected[2] = 0;
                        ext_selected[1]--;
                    }
                    break;
                case 2:
                    if ((signed) ext_selected[2] == -1) {
                        ext_selected[2] = 7;
                        ext_selected[1]--;
                    }
                    break;
                case 3:
                    if ((signed) ext_selected[2] == -1) {
                        ext_selected[2] = 9;
                        ext_selected[1]--;
                    }
                    break;
            }
            menu_draw();
            if (cheat_button_age_x < 12) cheat_button_age_x++;
            menu_key_delay(150000 - (10000 * cheat_button_age_x));
        } else if (pad.Buttons & PSP_CTRL_RIGHT) {
            ext_selected[2]++;
            if (search_mode < 4) {
                ext_selected[2] = 0;
            } else {
                switch (ext_selected[1]) {
                    case 0:
                        if (ext_selected[2] > 0) {
                            ext_selected[2] = 0;
                            ext_selected[1]++;
                        }
                        break;
                    case 1:
                        if (ext_selected[2] > 7) {
                            ext_selected[2] = 0;
                            ext_selected[1]++;
                        }
                        break;
                    case 2:
                        if (ext_selected[2] > 9) {
                            ext_selected[2] = 0;
                            ext_selected[1]++;
                        }
                        break;
                    case 3:
                        if (ext_selected[2] > 3) { ext_selected[2] = 3; }
                        break;
                }
            }
            menu_draw();
            if (cheat_button_age_x < 12) cheat_button_age_x++;
            menu_key_delay(150000 - (10000 * cheat_button_age_x));
        } else {
            cheat_button_age_x = 0;
        }
    }

    if ((pad_buttons & PSP_CTRL_CIRCLE) && !(pad.Buttons & PSP_CTRL_CIRCLE)) {
        if (ext_selected[3]) {
            ext_selected[3] = 0;
            menu_draw();
            menu_key_delay(150000);
        } else//Search has been aborted!
        {
            //Go back
            menu_clear_screen();
            ext_selected[0] = ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
            ext_menu = EXT_NONE;
            ext_opt = 0;
            menu_draw();
            menu_key_delay(150000);
            return;
        }
    }
}

// Sub-menu 4: text search
void menu_input_edit_text_search(unsigned int pad_buttons) {
    unsigned int counter = 0;
    unsigned int scounter = 0;

    if (pad.Buttons & PSP_CTRL_SELECT) {
        copy_menu = 1;
        menu_draw();
        menu_key_delay(150000);
    }

    if (pad.Buttons & PSP_CTRL_CROSS) {
        if (ext_selected[0] == 0) {
            ext_selected[3] = !ext_selected[3];
        } else if (ext_selected[0] == 1) {
            //Move the cursor back
            ext_selected[0] = 0;

            //Setup the variables
            search_result_counter = 0;

            //Search!
            counter = START_ADDRESS;

            //Helper
            while (counter < END_ADDRESS) {
                //Helper
                if (!((counter - START_ADDRESS) & 0xFFFF)) {
                    if (!cheat_pause) sceKernelDelayThread(1500);

                    LINE_CLEAR(33);
                    pspDebugScreenSetTextColor(0xFFFF8000);
                    snprintf(buffer, sizeof(buffer), "Searching = %02d%%; Hold () to Abort",
                            (counter - START_ADDRESS) / ((END_ADDRESS - START_ADDRESS) / 100));
                    pspDebugScreenPuts(buffer);

                    sceCtrlPeekBufferPositive(&pad, 1);

                    if (pad.Buttons & PSP_CTRL_CIRCLE) {
                        LINE_CLEAR(33);
                        pspDebugScreenSetTextColor(0xFFFF8000);
                        pspDebugScreenPuts("Search Aborted");

                        do {
                            sceKernelDelayThread(150000);
                            sceCtrlPeekBufferPositive(&pad, 1);
                        } while (pad.Buttons & PSP_CTRL_CIRCLE);
                        break;
                    }
                }

                //Check
                scounter = 0;
                while (scounter < 50) {
                    if (counter + scounter <= 0x49FFFFFF) {
                        unsigned char temp_letter = *((unsigned char *) (counter + scounter));
                        if ((temp_letter >= 0x61) && (temp_letter <= 0x7A)) temp_letter -= 0x20;
                        if (temp_letter == (unsigned char) file_buffer[scounter]) {
                            scounter++;
                            if (!file_buffer[scounter + 1]) {
                                //Add it
                                search_address[search_result_counter] = counter;
                                search_result_counter++;
                                break;
                            }
                        } else {
                            if (scounter == 0) {
                                scounter = 1;
                            }
                            break;
                        }
                    } else {
                        scounter++;
                        break;
                    }
                }

                counter += scounter;

                if (search_result_counter == 100) break;
            }
        }

        menu_clear_screen();
        menu_draw();
        menu_key_delay(150000);
    }

    if (ext_selected[3]) {
        if (pad.Buttons & PSP_CTRL_UP) {
            file_buffer[ext_selected[2]]++;
            switch (file_buffer[ext_selected[2]]) {
                case 0x01:
                    file_buffer[ext_selected[2]] = 0x20;
                    break;
                case 0x21:
                    file_buffer[ext_selected[2]] = 0x41;
                    break;
                case 0x5B:
                    file_buffer[ext_selected[2]] = 0x30;
                    break;
                case 0x3A:
                    file_buffer[ext_selected[2]] = 0x21;
                    break;
                case 0x30:
                    file_buffer[ext_selected[2]] = 0x3A;
                    break;
                case 0x41:
                    file_buffer[ext_selected[2]] = 0x5B;
                    break;
                case 0x61:
                    file_buffer[ext_selected[2]] = 0x7B;
                    break;
                case 0x7F:
                    file_buffer[ext_selected[2]] = 0x20;
                    break;
            }

            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            file_buffer[ext_selected[2]]--;
            switch (file_buffer[ext_selected[2]]) {
                case 0xFF:
                    file_buffer[ext_selected[2]] = 0x7E;
                    break;
                case 0x7A:
                    file_buffer[ext_selected[2]] = 0x60;
                    break;
                case 0x5A:
                    file_buffer[ext_selected[2]] = 0x40;
                    break;
                case 0x39:
                    file_buffer[ext_selected[2]] = 0x2F;
                    break;
                case 0x20:
                    file_buffer[ext_selected[2]] = 0x39;
                    break;
                case 0x2F:
                    file_buffer[ext_selected[2]] = 0x5A;
                    break;
                case 0x40:
                    file_buffer[ext_selected[2]] = 0x20;
                    break;
                case 0x1F:
                    file_buffer[ext_selected[2]] = 0x7E;
                    break;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
    } else {
        if (pad.Buttons & PSP_CTRL_UP) {
            if (ext_selected[0] > 0) {
                ext_selected[0]--;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            if (ext_selected[0] < (1 + (search_result_counter > 100 ? 100 : search_result_counter))) {
                ext_selected[0]++;
            }
            menu_draw();
            if (cheat_button_age_y < 12) cheat_button_age_y++;
            menu_key_delay(150000 - (10000 * cheat_button_age_y));
        } else {
            cheat_button_age_y = 0;
        }
    }

    if (ext_selected[0] == 0) {
        if (pad.Buttons & PSP_CTRL_SQUARE) {
            memset(&file_buffer[ext_selected[2] + 1], 0, 49 - ext_selected[2]);
            menu_draw();
        }

        if (pad.Buttons & PSP_CTRL_LEFT) {
            ext_selected[2]--;
            if ((signed) ext_selected[2] == -1) { ext_selected[2] = 0; }
            menu_draw();
            if (cheat_button_age_x < 12) cheat_button_age_x++;
            menu_key_delay(150000 - (10000 * cheat_button_age_x));
        } else if (pad.Buttons & PSP_CTRL_RIGHT) {
            ext_selected[2]++;
            if (ext_selected[2] == 50) { ext_selected[2] = 49; }
            if (file_buffer[ext_selected[2]] == 0x00) file_buffer[ext_selected[2]] = 'A';
            menu_draw();
            if (cheat_button_age_x < 12) cheat_button_age_x++;
            menu_key_delay(150000 - (10000 * cheat_button_age_x));
        } else {
            cheat_button_age_x = 0;
        }
    } else if (ext_selected[0] >= 2) {
        if (pad.Buttons & PSP_CTRL_LEFT) {
            search_address[ext_selected[0] - 2]--;
            if (search_address[ext_selected[0] - 2] < START_ADDRESS) {
                search_address[ext_selected[0] - 2] = START_ADDRESS;
            }

            menu_draw();
            if (cheat_button_age_x < 12) cheat_button_age_x++;
            menu_key_delay(150000 - (10000 * cheat_button_age_x));
        } else if (pad.Buttons & PSP_CTRL_RIGHT) {
            search_address[ext_selected[0] - 2]++;
            if (search_address[ext_selected[0] - 2] >= END_ADDRESS) {
                search_address[ext_selected[0] - 2] = 0x49FFFFFF;
            }

            menu_draw();
            if (cheat_button_age_x < 12) cheat_button_age_x++;
            menu_key_delay(150000 - (10000 * cheat_button_age_x));
        } else {
            cheat_button_age_x = 0;
        }
    }

    if ((pad_buttons & PSP_CTRL_CIRCLE) && !(pad.Buttons & PSP_CTRL_CIRCLE)) {
        if (ext_selected[3]) {
            ext_selected[3] = 0;
            menu_draw();
            menu_key_delay(150000);
        } else//Search has been aborted!
        {
            //Go back
            cheat_selected = 0;
            menu_clear_screen();
            ext_selected[0] = ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
            ext_menu = EXT_NONE;
            ext_opt = 0;
            menu_draw();
            menu_key_delay(150000);
            return;
        }
    }
}
