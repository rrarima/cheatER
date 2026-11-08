/*
 * Sub-menus opened from a tab (ext_menu 1-4).
 */

#include <pspdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cheat.h"
#include "common.h"
#include "file_buffer.h"
#include "float_format.h"
#include "menu.h"
#include "menu_draw.h"
#include "search.h"


// Sub-menu 1: editing the selected cheat's address/value lines
void menu_draw_edit_cheat(void) {
    unsigned int counter;
    unsigned int conv_base;
    unsigned int conv_total;

    //Draw the title
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPuts("[Editing Cheat: '");
    pspDebugScreenPuts(cheat[cheat_selected].name);
    pspDebugScreenPuts("'] ");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("\n--------------------------------------------------------------------");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("  Address     Value.Hex   Value.Dec   Value.ASCII  Value.Float\n");

    //Print out the cheat lines
    conv_base = cheat[cheat_selected].block;
    conv_total = cheat[cheat_selected].len;
    counter = cheat[cheat_selected].block;
    while (counter < (cheat[cheat_selected].block + cheat[cheat_selected].len)) {
        //Skip rows outside the visible window
        if (menu_row_hidden((int) (counter - conv_base), (int) (ext_selected[0] - conv_base), 11, (int) conv_total)) {
            counter++;
            continue;
        }

        //Apply the row color
        if (counter == ext_selected[0]) {
            pspDebugScreenSetTextColor(0xFF8080BB);
        } else {
            pspDebugScreenSetTextColor(0xFFBBBBBB);
        }

        //Print out the address
        if (block[counter].flags & FLAG_DMA) {
            pspDebugScreenPuts("  0xFFFFFFFF  ");
        } else {
            snprintf(buffer, sizeof(buffer), "  0x%08lX  ", block[counter].address - USER_MEM_BASE);
            pspDebugScreenPuts(buffer);
        }

        menu_draw_value_columns(&block[counter].hak_val, block[counter].flags, 0);

        //Skip a line, draw the pointer =)
        if (counter == ext_selected[0]) {
            //Skip the initial line
            pspDebugScreenPuts("\n");

            //Skip the desired amount?
            pspDebugScreenPuts("    ");
            if (ext_selected[1] != 0)//Skip address
            {
                pspDebugScreenPuts("            ");
                if (ext_selected[1] != 1)//Skip Hex
                {
                    pspDebugScreenPuts("          ");
                    if (ext_selected[1] != 2)//Skip Dec
                    {
                        pspDebugScreenPuts("            ");
                        //Skip ASCII
                    }
                }
            }

            //Skip the minimalist amount
            unsigned char temp_counter = ext_selected[2];
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
    pspDebugScreenSetXY(0, 33);
    pspDebugScreenSetTextColor(0xFFFF8000);
    pspDebugScreenPuts(">< = Edit On/Off; \\|/ /|\\ <- -> = Cycle; [] = Alt-Type; () = Close");
}

// Sub-menu 2: exact value search and its results
void menu_draw_edit_search(void) {
    unsigned int counter;
    unsigned int conv_total;

    //Draw the title
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPuts("[Editing Search]");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("\n--------------------------------------------------------------------");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("  Value.Hex   Value.Dec   Value.ASCII  Value.Float\n");

    //Apply the row color
    if (ext_selected[0] == 0) {
        pspDebugScreenSetTextColor(0xFF8080BB);
    } else {
        pspDebugScreenSetTextColor(0xFFBBBBBB);
    }

    menu_draw_value_columns(&search_history[0].hak_val, search_history[0].flags, 1);

    //Skip a line, draw the pointer =)
    pspDebugScreenPuts("\n");
    if (ext_selected[0] == 0) {
        //Skip the desired amount?
        pspDebugScreenPuts("    ");
        if (ext_selected[1] != 0)//Skip Hex
        {
            pspDebugScreenPuts("          ");
            if (ext_selected[1] != 1)//Skip Dec
            {
                pspDebugScreenPuts("            ");
                //Skip ASCII
            }
        }

        //Skip the minimalist amount
        unsigned char temp_counter = ext_selected[2];
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
    pspDebugScreenPuts("\n");

    //Draw the misc menus
    pspDebugScreenSetTextColor(ext_selected[0] == 1 ? 0xFF0000FF : 0xFFBBBBBB);
    pspDebugScreenPuts("  Search\n");
    if (search_no) {
        pspDebugScreenSetTextColor(ext_selected[0] == 2 ? 0xFF0000FF : 0xFFBBBBBB);
        pspDebugScreenPuts("  Undo Search\n");
    }

    //Print out results
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPuts("\n[Search Results: ");
    snprintf(buffer, sizeof(buffer), "%d Found - Only showing first 100]", search_result_counter);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("\n--------------------------------------------------------------------");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("  Address     Value.Hex   Value.Dec   Value.ASCII  Value.Float\n");

    //Print out the results variables
    conv_total = ((search_result_counter > 100) ? 100 : search_result_counter);
    counter = 0;
    while (counter < conv_total) {
        //Skip rows outside the visible window
        if (menu_row_hidden((int) counter, (int) (ext_selected[0] - 3), 7, (int) conv_total)) {
            counter++;
            continue;
        }

        //Apply the row color
        if (counter == (ext_selected[0] - 3)) {
            pspDebugScreenSetTextColor(0xFF8080BB);
        } else {
            pspDebugScreenSetTextColor(0xFFBBBBBB);
        }

        //Print out the address
        snprintf(buffer, sizeof(buffer), "  0x%08lX  ", (search_address[counter] - USER_MEM_UNCACHED));
        pspDebugScreenPuts(buffer);

        menu_draw_memory_columns(search_address[counter], search_history[0].flags);

        //Goto the next cheat down under
        pspDebugScreenPuts("\n");
        counter++;
    }

    //Helper
    pspDebugScreenSetXY(0, 33);
    pspDebugScreenSetTextColor(0xFFFF8000);
    if (ext_selected[0] == 0) {
        if (search_no == 0) {
            pspDebugScreenPuts(">< = Edit On/Off; \\|/ /|\\ <- -> = Cycle; [] = Alt-Type; () = Cancel");
        } else {
            pspDebugScreenPuts(">< = Edit On/Off; \\|/ /|\\ <- -> = Cycle; () = Cancel               ");
        }
    } else if ((ext_selected[0] == 1) || (ext_selected[0] == 2)) {
        pspDebugScreenPuts(">< = Select; () = Cancel                                           ");
    } else {
        pspDebugScreenPuts(">< = Add Selected Cheat; () = Cancel                               ");
    }
}

// Sub-menu 3: unknown value (difference) search and its results
void menu_draw_edit_diff_search(void) {
    unsigned int counter;
    unsigned int conv_total;

    //Draw the title
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPuts("[Editing Search]");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("\n--------------------------------------------------------------------");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    if (search_mode > 3) {
        pspDebugScreenPuts("  Mode          Value.Hex   Value.Dec   Value.ASCII  Value.Float\n");
    } else {
        pspDebugScreenPuts("  Mode                                                          \n");
    }

    //Apply the row color
    if (ext_selected[0] == 0) {
        pspDebugScreenSetTextColor(0xFF8080BB);
    } else {
        pspDebugScreenSetTextColor(0xFFBBBBBB);
    }

    //Print out the mode name
    pspDebugScreenPuts(search_mode_name[search_mode]);

    if (search_mode > 3) {
        menu_draw_value_columns(&search_history[0].hak_val, search_history[0].flags, 1);
    }

    //Skip a line, draw the pointer =)
    pspDebugScreenPuts("\n");
    if (ext_selected[0] == 0) {
        //Skip the desired amount?
        pspDebugScreenPuts("    ");
        if (ext_selected[1] != 0)//Skip Mode
        {
            pspDebugScreenPuts("              ");
            if (ext_selected[1] != 1)//Skip Hex
            {
                pspDebugScreenPuts("          ");
                if (ext_selected[1] != 2)//Skip Dec
                {
                    pspDebugScreenPuts("            ");
                    //Skip ASCII
                }
            }
        }

        //Skip the minimalist amount
        unsigned char temp_counter = ext_selected[2];
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
    pspDebugScreenPuts("\n");

    //Draw the misc menus
    pspDebugScreenSetTextColor(ext_selected[0] == 1 ? 0xFF0000FF : 0xFFBBBBBB);
    pspDebugScreenPuts("  Search\n");
    if (search_no) {
        pspDebugScreenSetTextColor(ext_selected[0] == 2 ? 0xFF0000FF : 0xFFBBBBBB);
        pspDebugScreenPuts("  Undo Search\n");
    }

    //Print out results
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPuts("\n[Search Results: ");
    snprintf(buffer, sizeof(buffer), "%d Found - Only showing first 100]", search_result_counter);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("\n--------------------------------------------------------------------");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("  Address     Value.Hex   Value.Dec   Value.ASCII  Value.Float\n");

    //Print out the results variables
    conv_total = ((search_result_counter > 100) ? 100 : search_result_counter);
    counter = 0;
    while (counter < conv_total) {
        //Skip rows outside the visible window
        if (menu_row_hidden((int) counter, (int) (ext_selected[0] - 3), 7, (int) conv_total)) {
            counter++;
            continue;
        }

        //Apply the row color
        if (counter == (ext_selected[0] - 3)) {
            pspDebugScreenSetTextColor(0xFF8080BB);
        } else {
            pspDebugScreenSetTextColor(0xFFBBBBBB);
        }

        //Print out the address
        snprintf(buffer, sizeof(buffer), "  0x%08lX  ", (search_address[counter] - USER_MEM_UNCACHED));
        pspDebugScreenPuts(buffer);

        menu_draw_memory_columns(search_address[counter], search_history[0].flags);

        //Goto the next cheat down under
        pspDebugScreenPuts("\n");
        counter++;
    }

    //Helper
    pspDebugScreenSetXY(0, 33);
    pspDebugScreenSetTextColor(0xFFFF8000);
    if (ext_selected[0] == 0) {
        pspDebugScreenPuts(">< = Edit On/Off; \\|/ /|\\ <- -> = Cycle; () = Cancel               ");
    } else if ((ext_selected[0] == 1) || (ext_selected[0] == 2)) {
        pspDebugScreenPuts(">< = Select; () = Cancel                                           ");
    } else {
        pspDebugScreenPuts(">< = Add Selected Cheat; () = Cancel                               ");
    }
}

// Sub-menu 4: text search and its results
void menu_draw_edit_text_search(void) {
    unsigned int counter;
    unsigned int scounter;
    unsigned int conv_total;

    //Draw the title
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPuts("[Editing Search]");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("\n--------------------------------------------------------------------");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("  Text                                                          \n");

    //Apply the row color
    if (ext_selected[0] == 0) {
        pspDebugScreenSetTextColor(0xFF8080BB);
    } else {
        pspDebugScreenSetTextColor(0xFFBBBBBB);
    }

    //Print out the ASCII
    pspDebugScreenPuts("  '");
    file_buffer[50] = 0;
    pspDebugScreenPuts(file_buffer);

    //Skip a line, draw the pointer =)
    pspDebugScreenPuts("'\n");
    if (ext_selected[0] == 0) {
        //Skip the desired amount?
        pspDebugScreenPuts("   ");

        //Skip the minimalist amount
        unsigned char temp_counter = ext_selected[2];
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
    pspDebugScreenPuts("\n");

    //Draw the misc menus
    pspDebugScreenSetTextColor(ext_selected[0] == 1 ? 0xFF0000FF : 0xFFBBBBBB);
    pspDebugScreenPuts("  Search\n");

    //Print out results
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPuts("\n[Search Results: Only showing first 100]");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("\n--------------------------------------------------------------------");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("  Address   Text                                              \n");

    //Print out the results variables
    conv_total = ((search_result_counter > 100) ? 100 : search_result_counter);
    counter = 0;
    while (counter < conv_total) {
        //Skip rows outside the visible window
        if (menu_row_hidden((int) counter, (int) (ext_selected[0] - 2), 7, (int) conv_total)) {
            counter++;
            continue;
        }

        //Apply the row color
        if (counter == (ext_selected[0] - 2)) {
            pspDebugScreenSetTextColor(0xFF8080BB);
        } else {
            pspDebugScreenSetTextColor(0xFFBBBBBB);
        }

        //Print out the address
        snprintf(buffer, sizeof(buffer), "  0x%08lX  '", (search_address[counter] - USER_MEM_UNCACHED));
        pspDebugScreenPuts(buffer);

        //Print out the ASCII
        memset(buffer, 0, 17);
        scounter = 0;
        while (scounter < 16) {
            if ((search_address[counter] + scounter) < END_ADDRESS) {
                buffer[scounter] = *((unsigned char *) (search_address[counter] + scounter));
                if ((buffer[scounter] <= 0x20) || (buffer[scounter] == 0xFF)) buffer[scounter] = '.';
            } else {
                break;
            }
            scounter++;
        }
        pspDebugScreenPuts(buffer);

        //Goto the next cheat down under
        pspDebugScreenPuts("'\n");
        counter++;
    }

    //Helper
    pspDebugScreenSetXY(0, 33);
    pspDebugScreenSetTextColor(0xFFFF8000);
    if (ext_selected[0] == 0) {
        pspDebugScreenPuts(">< = Edit On/Off; \\|/ /|\\ <- -> = Cycle; [] = Trim; () = Cancel    ");
    } else if (ext_selected[0] == 1) {
        pspDebugScreenPuts(">< = Select; () = Cancel                                           ");
    } else {
        pspDebugScreenPuts("<- -> = Scroll Address; () = Cancel                                ");
    }
}
