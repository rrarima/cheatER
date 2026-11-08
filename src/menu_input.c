/*
 * The cheat menu's input loop. Runs until the menu is closed;
 * each screen's button handling lives in menu_input_edit.c and menu_input_tabs.c.
 */

#include <pspctrl.h>
#include <pspdebug.h>
#include <pspdisplay_kernel.h>
#include <pspkernel.h>

#include "cheat.h"
#include "common.h"
#include "menu.h"
#include "menu_draw.h"
#include "menu_input.h"
#include "search.h"


// Wait before a held button repeats. Returns early as soon as the buttons change
// (released or another pressed), so a quick tap is never blocked.
void menu_key_delay(unsigned int us) {
    SceCtrlData now;
    unsigned int held = pad.Buttons;

    while (us > 0) {
        unsigned int step = us < 5000 ? us : 5000;
        sceKernelDelayThread(step);
        us -= step;
        sceCtrlPeekBufferPositive(&now, 1);
        if (now.Buttons != held) break;
    }
}

// Copy/paste address popup
void menu_input_copy_menu(unsigned int pad_buttons) {
    if (pad.Buttons & PSP_CTRL_UP) {
        if (copy_menu != 1) copy_menu = 1;
        menu_draw();
        menu_key_delay(150000);
    } else if (pad.Buttons & PSP_CTRL_DOWN) {
        if (copy_menu != 2) copy_menu = 2;
        menu_draw();
        menu_key_delay(150000);
    }
    if (pad.Buttons & PSP_CTRL_CROSS) {
        if (copy_menu == 1)//Copy
        {
            if (ext_menu) {
                if (ext_menu == EXT_EDIT_CHEAT) {
                    copy_data = block[ext_selected[0]].address;
                } else if (ext_menu == EXT_SEARCH) {
                    if (ext_selected[0] > 2) {
                        copy_data = search_address[ext_selected[0] - 3] - UNCACHED;
                    }
                } else if (ext_menu == EXT_DIFF_SEARCH) {
                    if (ext_selected[0] > 2) {
                        copy_data = search_address[ext_selected[0] - 3] - UNCACHED;
                    }
                } else if (ext_menu == EXT_TEXT_SEARCH) {
                    if (ext_selected[0] > 1) {
                        copy_data = search_address[ext_selected[0] - 2] - UNCACHED;
                    }
                }
            } else {
                if (tab_selected == TAB_MEM_VIEW) {
                    copy_data = browse_address + (browse_y * browse_lines);
                    copy_data -= UNCACHED;
                } else if (tab_selected == TAB_DECODER) {
                    copy_data = decode_address + (decode_y * 4);
                    copy_data -= UNCACHED;
                } else if (tab_selected == TAB_TOOLS) {
                    copy_data = track_address;
                    copy_data -= UNCACHED;
                }
            }
            copy_data &= 0xFFFFFFFC;

            if (copy_data < USER_MEM_START) {
                copy_data = USER_MEM_START;
            }
        } else//Paste
        {
            if (ext_menu) {
                if (ext_menu == EXT_EDIT_CHEAT) {
                    if (!(block[ext_selected[0]].flags & FLAG_DMA)) block[ext_selected[0]].address = copy_data;
                }
            } else {
                if (tab_selected == TAB_MEM_VIEW) {
                    browse_address = copy_data | UNCACHED;
                    if (browse_address > (END_ADDRESS - (22 * browse_lines))) {
                        browse_address = (END_ADDRESS - (22 * browse_lines));
                    }
                } else if (tab_selected == TAB_DECODER) {
                    decode_address = copy_data | UNCACHED;
                    if (decode_address > 0x49FFFFA8) {
                        decode_address = 0x49FFFFA8;
                    }
                } else if (tab_selected == TAB_TOOLS) {
                    track_address = copy_data + UNCACHED;
                }
            }
        }
        goto hide_copy_menu;
    }
    if ((pad_buttons & PSP_CTRL_CIRCLE) && !(pad.Buttons & PSP_CTRL_CIRCLE)) {
    hide_copy_menu:
        menu_clear_screen();
        copy_menu = 0;
        menu_draw();
        menu_key_delay(150000);
    }
}

void menu_input(void) {
    unsigned int pad_buttons;
    pad.Buttons = 0;
    menu_draw();

    //Loop for input
    while (1) {
        pad_buttons = pad.Buttons;
        sceCtrlPeekBufferPositive(&pad, 1);

        //Has the HOME button screwed up the VRAM blocks?
        unsigned int address = 0;
        unsigned int buffer_width = 0;
        unsigned int pixel_format = 0;
        unsigned int sync;

        sceDisplayGetFrameBufferInternal(0, (void **) &address, &buffer_width, &pixel_format, &sync);

        if (address == 0) {
            //Stop cheatER
            menu_drawn = 0;
            return;
        }

        //The 1st priority menu
        if (copy_menu) {
            menu_input_copy_menu(pad_buttons);
        } else if (ext_menu)//Do we use extended menus?
        {
            switch (ext_menu) {
                case EXT_EDIT_CHEAT:
                    menu_input_edit_cheat(pad_buttons);
                    break;
                case EXT_SEARCH:
                    menu_input_edit_search(pad_buttons);
                    break;
                case EXT_DIFF_SEARCH:
                    menu_input_edit_diff_search(pad_buttons);
                    break;
                case EXT_TEXT_SEARCH:
                    menu_input_edit_text_search(pad_buttons);
                    break;
            }
        } else {
            //Overall button inputs
            if ((pad.Buttons & PSP_CTRL_LTRIGGER) && (tab_selected != -1)) {
                menu_clear_screen();
                cheat_selected = 0;
                ext_selected[0] = ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
                ext_opt = 0;
                if (tab_selected > TAB_CHEATS) {
                    tab_selected--;
                } else if (tab_selected == TAB_CHEATS) {
                    tab_selected = TAB_LAST;
                }

                menu_draw();
                menu_key_delay(150000);
            }
            if ((pad.Buttons & PSP_CTRL_RTRIGGER) && (tab_selected != 6)) {
                menu_clear_screen();
                cheat_selected = 0;
                ext_selected[0] = ext_selected[1] = ext_selected[2] = ext_selected[3] = 0;
                ext_opt = 0;
                if (tab_selected < TAB_LAST) {
                    tab_selected++;
                } else if (tab_selected == TAB_LAST) {
                    tab_selected = TAB_CHEATS;
                }

                menu_draw();
                menu_key_delay(150000);
            }
            if ((pad_buttons & PSP_CTRL_CIRCLE) && !(pad.Buttons & PSP_CTRL_CIRCLE)) {
                //Special case for the memory viewer
                if (ext_selected[3]) {
                    ext_selected[3] = 0;
                    menu_draw();
                    menu_key_delay(150000);
                } else {
                    //Unregister the O key so that the user mode game doesn't pick it up
                    menu_drawn = 0;
                    return;
                }
            }
            if ((pad_buttons & PSP_CTRL_NOTE)) {
                LINE_CLEAR(33);
                pspDebugScreenSetTextColor(0xFF0000FF);
                pspDebugScreenPuts("ERROR: Must press Music Button in game!");
                sceKernelDelayThread(1000000);
                LINE_CLEAR(33);
                menu_draw();
                continue;
            }

            //This is for the warning message at the start
            if (lol_init) {
                if ((pad_buttons & PSP_CTRL_START) && !(pad.Buttons & PSP_CTRL_START)) {
                    //Start closes the welcome screen
                    lol_init = 0;
                    menu_clear_screen();
                    menu_draw();
                    continue;
                }

                pspDebugScreenSetXY(0, 26);
                unsigned int red = 255;
                unsigned int green = 0;
                unsigned int blue = 0;
                unsigned int color = 0xFF000000 | (red << lol_value) | (green << lol_value) | blue << lol_value | lol_value;
                pspDebugScreenSetTextColor(color);
                pspDebugScreenPuts("                        Press [START] to Continue");
                lol_value += lol_direction;
                lol_direction = lol_value % 255 == 0 ? -lol_direction : lol_direction;

                sceKernelDelayThread(20000);
                continue;
            }

            //Each tab's buttons (cases follow enum MenuTab in menu.h)
            switch (tab_selected) {
                case TAB_CHEATS:
                    menu_input_cheats_tab();
                    break;
                case TAB_SEARCHER:
                    menu_input_searcher_tab();
                    break;
                case TAB_MEM_VIEW:
                    menu_input_browser_tab();
                    break;
                case TAB_DECODER:
                    menu_input_decoder_tab();
                    break;
                case TAB_TOOLS:
                    continue;

                case TAB_SETTINGS:
                    menu_input_settings_tab();
                    break;
            }
        }

        //While the game keeps running it can draw over the menu, so repaint it
        if (!cheat_pause) menu_present();

        //Stop the game from freezing up
        if (!cheat_pause) sceKernelDelayThread(15625);
    }
}
