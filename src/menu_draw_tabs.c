/*
 * The main menu tabs (tab_selected 0-5).
 */

#include <pspdebug.h>
#include <pspusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cheat.h"
#include "common.h"
#include "file_buffer.h"
#include "float_format.h"
#include "menu.h"
#include "menu_draw.h"
#include "mips.h"
#include "search.h"


// Tab 0: Cheats List
void menu_draw_cheats_tab(void) {
    unsigned int counter;

    counter = 0;
    while (counter < cheat_total) {
        //Skip rows outside the visible window
        if (menu_row_hidden((int) counter, (int) cheat_selected, 11, (int) cheat_total)) {
            counter++;
            continue;
        }

        pspDebugScreenPuts("  ");
        if (cheat_selected == counter) {
            //Highlight the selection
            if (cheat[cheat_selected].flags & FLAG_SELECTED) {
                pspDebugScreenSetTextColor(0xFF0000FF);
            } else if (cheat[cheat_selected].flags & FLAG_CONSTANT) {
                pspDebugScreenSetTextColor(0xFFFF40FF);
            } else {
                pspDebugScreenSetTextColor(0xFF000080);
            }
        } else {
            //Don't highlight the selection
            if (cheat[counter].flags & FLAG_SELECTED) {
                pspDebugScreenSetTextColor(0xFFFFFFFF);
            } else if (cheat[counter].flags & FLAG_CONSTANT) {
                pspDebugScreenSetTextColor(0xFFFF0000);
            } else {
                pspDebugScreenSetTextColor(0xFFBBBBBB);
            }
        }
        pspDebugScreenPuts(cheat[counter].name);
        pspDebugScreenPuts("\n");
        counter++;
    }

    //Helper
    pspDebugScreenSetTextColor(0xFFFF8000);
    pspDebugScreenSetXY(0, 31);
    pspDebugScreenPuts(">< = Select Cheat for On/Off mode;");
    pspDebugScreenSetXY(0, 32);
    pspDebugScreenPuts("[] = Select Cheat for Always On mode; /\\ = Edit Cheat;");
    pspDebugScreenSetXY(0, 33);
    pspDebugScreenPuts("() = Cancel/Return to Game");
}

// Tab 1: Searcher (search options and history)
void menu_draw_searcher_tab(void) {
    unsigned int counter;
    unsigned int scounter;

    counter = 0;
    while (counter < (3 + ((!cheat_search) * 2))) {
        //Skip rows outside the visible window
        if (menu_row_hidden((int) counter, (int) cheat_selected, 12, (int) cheat_total)) {
            counter++;
            continue;
        }

        if (cheat_selected == counter) {
            //Highlight the selection
            pspDebugScreenSetTextColor(0xFF0000FF);
        } else {
            //Don't highlight the selection
            pspDebugScreenSetTextColor(0xFFBBBBBB);
        }
        if (!cheat_search) {
            switch (counter) {
                case 0:
                    pspDebugScreenPuts("  Find Exact Value\n");
                    break;
                case 1:
                    pspDebugScreenPuts("  Find Unknown Value - 8bit\n");
                    break;
                case 2:
                    pspDebugScreenPuts("  Find Unknown Value - 16bit\n");
                    break;
                case 3:
                    pspDebugScreenPuts("  Find Unknown Value - 32bit\n");
                    break;
                case 4:
                    pspDebugScreenPuts("  Find Text\n");
                    break;
            }
        } else {
            switch (counter) {
                case 0:
                    pspDebugScreenPuts("  Continue to find Exact Value\n");
                    break;
                case 1:
                    pspDebugScreenPuts("  Continue to find Unknown Value\n");
                    break;
                case 2:
                    pspDebugScreenPuts("  Reset search\n");
                    break;
            }
        }
        counter++;
    }

    //Print out search history
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPuts("\n[Search History]");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("\n--------------------------------------------------------------------");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("  Mode          Value.Hex   Value.Dec   Value.ASCII  Value.Float\n");
    scounter = 0;
    while (scounter < search_history_counter) {
        //Apply the row color
        pspDebugScreenSetTextColor(0xFF8080BB - (scounter * 0x00080808));

        //Print out the mode
        pspDebugScreenPuts(search_mode_name[search_history[scounter + 1].std_val]);
        if (search_history[scounter + 1].std_val > 3) {
            menu_draw_value_columns(&search_history[scounter + 1].hak_val, search_history[scounter + 1].flags, 1);
        }

        //Goto the next line
        pspDebugScreenPuts("\n");

        //Increment scounter
        scounter++;
    }

    //Helper
    pspDebugScreenSetTextColor(0xFFFF8000);
    pspDebugScreenSetXY(0, 33);
    pspDebugScreenPuts(">< = Select; () = Cancel/Return to Game");
}

// Tab 2: Mem View (memory browser)
void menu_draw_browser_tab(void) {
    unsigned int counter;
    unsigned int scounter;

    pspDebugScreenSetTextColor(0xFFBBBBBB);
    if (browse_lines == 8) {
        pspDebugScreenPuts("  Address     00  01  02  03  04  05  06  07    ASCII\n");
    } else {
        pspDebugScreenPuts("  Address     000102030405060708090A0B0C0D0E0F  ASCII\n");
    }

    //Write out the RAM
    counter = 0;
    while (counter < 22) {
        //Apply the row color
        if (counter == browse_y) {
            pspDebugScreenSetTextColor(0xFF8080BB);
        } else {
            pspDebugScreenSetTextColor(0xFFBBBBBB);
        }

        //Print out the address
        snprintf(buffer, sizeof(buffer), "  0x%08lX  ", (browse_address + (counter * browse_lines)) - browse_format);
        pspDebugScreenPuts(buffer);

        //Print out the bytes per line
        scounter = 0;
        while (scounter < browse_lines) {
            //Apply the row color
            if ((scounter & 1) || (browse_y == counter)) {
                pspDebugScreenSetTextColor(0xFF8080BB);
            } else {
                pspDebugScreenSetTextColor(0xFFBBBBBB);
            }

            snprintf(buffer, sizeof(buffer), "%02hX",
                    *((unsigned char *) ((browse_address + (counter * browse_lines)) + scounter)));
            pspDebugScreenPuts(buffer);
            if (browse_lines == 8) pspDebugScreenPuts("  ");

            buffer[3 + scounter] = *((unsigned char *) ((browse_address + (counter * browse_lines)) +
                                                        scounter));
            if ((buffer[3 + scounter] <= 0x20) || (buffer[3 + scounter] == 0xFF))
                buffer[3 + scounter] = '.';

            scounter++;
        }

        //Apply the row color
        if (counter == browse_y) {
            pspDebugScreenSetTextColor(0xFF8080BB);
        } else {
            pspDebugScreenSetTextColor(0xFFBBBBBB);
        }

        //Print out the ASCII
        buffer[3 + browse_lines] = 0;
        pspDebugScreenPuts("  ");
        pspDebugScreenPuts(&buffer[3]);

        //Skip a line, draw the pointer =)
        if (counter == browse_y) {
            //Skip the initial line
            pspDebugScreenPuts("\n");

            //Skip the desired amount?
            pspDebugScreenPuts("    ");
            if (browse_c != 0)//Skip Hex
            {
                pspDebugScreenPuts("          ");
                if (browse_c != 1)//Skip Bytes
                {
                    pspDebugScreenPuts("                                  ");
                    //Skip ASCII
                }
            }

            //Skip the minimalist amount
            unsigned char temp_counter = browse_x;
            while (temp_counter) {
                if ((temp_counter != 0) && ((temp_counter % 2) == 0) && (browse_lines == 8) &&
                    (browse_c == 1)) {
                    pspDebugScreenPuts("   ");
                } else {
                    pspDebugScreenPuts(" ");
                }
                temp_counter--;
            }

            //Draw the symbol (Finally!!)
            if (ext_selected[3]) {
                pspDebugScreenSetTextColor(0xFF0000FF);
            } else {
                pspDebugScreenSetTextColor(0xFFFF0000);
            }
            pspDebugScreenPuts("^");
        }

        //Goto the next cheat down under
        pspDebugScreenPuts("\n");
        counter++;
    }

    //Helper
    pspDebugScreenSetTextColor(0xFFFF8000);
    pspDebugScreenSetXY(0, 32);
    pspDebugScreenPuts(">< = Edit On/Off; \\|/ /|\\ <- -> = Cycle/Scroll; [] = Teleport Cursor");
    pspDebugScreenSetXY(0, 33);
    pspDebugScreenPuts("[] + Analog/Digital = Scroll; () = Cancel/Return to Game");
}

// Tab 3: Decoder (MIPS disassembly)
void menu_draw_decoder_tab(void) {
    unsigned int counter;

    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("  Address     Hex       Opcode   Args\n");

    //Write out the RAM
    counter = 0;
    while (counter < 22) {
        //Apply the row color
        if (counter == decode_y) {
            pspDebugScreenSetTextColor(0xFF8080BB);
        } else {
            pspDebugScreenSetTextColor(0xFFBBBBBB);
        }

        //Print out the address
        snprintf(buffer, sizeof(buffer), "  0x%08lX  ", (decode_address + (counter * 4)) - decode_format);
        pspDebugScreenPuts(buffer);

        //Print out the dword of memory
        snprintf(buffer, sizeof(buffer), "%08lX  ", *((unsigned int *) (decode_address + (counter * 4))));
        pspDebugScreenPuts(buffer);

        //Print out the opcode
        mips_decode(*((unsigned int *) (decode_address + (counter * 4))));
        pspDebugScreenPuts(buffer);

        //Skip a line, draw the pointer =)
        if (counter == decode_y) {
            //Skip the initial line
            pspDebugScreenPuts("\n");

            //Skip the desired amount?
            pspDebugScreenPuts("    ");
            if (decode_c != 0)//Skip Address
            {
                pspDebugScreenPuts("          ");
                //Skip Hex
            }

            //Skip the minimalist amount
            unsigned char temp_counter = decode_x;
            while (temp_counter) {
                pspDebugScreenPuts(" ");
                temp_counter--;
            }

            //Draw the symbol (Finally!!)
            if (ext_selected[3]) {
                pspDebugScreenSetTextColor(0xFF0000FF);
            } else {
                pspDebugScreenSetTextColor(0xFFFF0000);
            }
            pspDebugScreenPuts("^");
        }

        //Goto the next cheat down under
        pspDebugScreenPuts("\n");
        counter++;
    }

    //Helper
    pspDebugScreenSetTextColor(0xFFFF8000);
    pspDebugScreenSetXY(0, 32);
    pspDebugScreenPuts(">< = Edit On/Off; \\|/ /|\\ <- -> = Cycle/Scroll;");
    pspDebugScreenSetXY(0, 33);
    pspDebugScreenPuts("[] + Analog/Digital = Scroll; () = Cancel/Return to Game");
}

// Tab 4: Tools
void menu_draw_tools_tab(void) {
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPuts("\nComing Soon!");

    //Helper
    LINE_CLEAR(33);
    pspDebugScreenSetTextColor(0xFFFF8000);
    if (track_y == 0) {
        pspDebugScreenPuts(">< = Edit On/Off; \\|/ /|\\ <- -> = Cycle; () = Cancel");
    } else if (track_y == 3) {
        pspDebugScreenPuts(">< = Select; () = Cancel");
    } else {
        pspDebugScreenPuts(">< = Toggle; () = Cancel");
    }
}

// Tab 5: Settings
void menu_draw_settings_tab(void) {
    unsigned int counter;

    counter = 0;
    while (counter < 12) {
        if (cheat_selected == counter) {
            //Highlight the selection
            pspDebugScreenSetTextColor(0xFF0000FF);
        } else {
            //Don't highlight the selection
            pspDebugScreenSetTextColor(0xFFBBBBBB);
        }
        switch (counter) {
            case 0:
                pspDebugScreenPuts("  Pause Game? ");
                if (cheat_pause) {
                    pspDebugScreenPuts("True\n");
                } else {
                    pspDebugScreenPuts("False\n");
                }
                break;
            case 1:
                pspDebugScreenPuts("  Add Empty Cheat?\n");
                break;
            case 2:
                snprintf(buffer, sizeof(buffer), "  Reset Codes? Slot #%d\n", dump_no);
                pspDebugScreenPuts(buffer);
                break;
            case 3:
                snprintf(buffer, sizeof(buffer), "  Dump RAM? Slot #%d\n", dump_no);
                pspDebugScreenPuts(buffer);
                break;
            case 4:
                snprintf(buffer, sizeof(buffer), "  Bytes per Line in Browser? %d\n", browse_lines);
                pspDebugScreenPuts(buffer);
                break;
            case 5:
                pspDebugScreenPuts("  Real Addressing in Browser? ");
                if (browse_format == UNCACHED) {
                    pspDebugScreenPuts("True\n");
                } else {
                    pspDebugScreenPuts("False\n");
                }
                break;
            case 6:
                pspDebugScreenPuts("  Real Addressing in Decoder? ");
                if (decode_format == UNCACHED) {
                    pspDebugScreenPuts("True\n");
                } else {
                    pspDebugScreenPuts("False\n");
                }
                break;
            case 7:
                pspDebugScreenPuts("  Real Addressing in Logger? ");
                if (track_format == UNCACHED) {
                    pspDebugScreenPuts("True\n");
                } else {
                    pspDebugScreenPuts("False\n");
                }
                break;
            case 8:
                snprintf(buffer, sizeof(buffer), "  Cheat Hz? %d/1000 seconds\n", (cheat_hz / 1000));
                pspDebugScreenPuts(buffer);
                break;
            case 9:
                pspDebugScreenPuts("  Save cheats\n");
                break;

            case 10:
                pspDebugScreenPuts("  Enable USB?  ");
                if (sceUsbGetState() & PSP_USB_ACTIVATED) {
                    pspDebugScreenPuts("True\n");
                } else {
                    pspDebugScreenPuts("False\n");
                }
                break;

            case 11:
                pspDebugScreenPuts("  Restart Plugin\n");
                break;
        }
        counter++;
    }
    //Helper
    pspDebugScreenSetTextColor(0xFFFF8000);
    LINE_CLEAR(31);
    LINE_CLEAR(32);
    switch (cheat_selected) {
        case 0:
            pspDebugScreenPuts("Pauses the game while cheatER's menu is showing");
            break;
        case 1:
            pspDebugScreenPuts("Adds an empty cheat to the Cheater for you to edit");
            break;
        case 2:
            pspDebugScreenPuts("Uses the selected 'RAM dump' to regenerate OFF codes");
            break;
        case 3:
            pspDebugScreenPuts("Saves the Game's RAM to MemoryStick");
            break;
        case 4:
            pspDebugScreenPuts("Alters the number of bytes displayed in the Browser");
            break;
        case 5:
            pspDebugScreenPuts("If enabled, REAL PSP hardware addresses will be used in Browser");
            break;
        case 6:
            pspDebugScreenPuts("If enabled, REAL PSP hardware addresses will be used in Decoder");
            break;
        case 7:
            pspDebugScreenPuts("If enabled, REAL PSP hardware addresses will be used in Logger");
            break;
        case 8:
            LINE_CLEAR(31);
            pspDebugScreenPuts("cheatER only applies a cheat once - set this to anything other");
            LINE_CLEAR(32);
            pspDebugScreenPuts("than 0 to make it apply the same cheat every X amount of seconds");
            break;
        case 9:
            pspDebugScreenPuts("Save your cheats");
            break;
        case 10:
            pspDebugScreenPuts("Enable USB");
            break;

        case 11:
            pspDebugScreenPuts("Restart the Plugin");
            break;
    }
    LINE_CLEAR(33);
    if ((cheat_selected != 8) && (cheat_selected != 3) && (cheat_selected != 2)) {
        pspDebugScreenPuts(">< = Toggle; () = Cancel/Return to Game");
    } else {
        pspDebugScreenPuts("<- -> = Decrement/Increment; () = Cancel/Return to Game");
    }
}
