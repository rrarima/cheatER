/*
 * Draws the cheat menu. menu_draw() picks which screen to show;
 * the screens themselves live in menu_draw_edit.c and menu_draw_tabs.c.
 */

#include <pspdebug.h>
#include <psppower.h>
#include <pspsysmem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cheat.h"
#include "cheat_file.h"
#include "common.h"
#include "float_format.h"
#include "menu.h"
#include "menu_draw.h"

static void menu_draw_screen(void);


// Is list row row outside the visible window of half rows either side of selected?
// Near the start or end of a list (total rows) the window shifts so it stays full.
int menu_row_hidden(int row, int selected, int half, int total) {
    //Rows above the window (the window shifts up when it would run past the end of the list)
    if (row < (selected - half) - (((selected + half) - total) > 0 ? abs((selected + half) - total) : 0)) return 1;
    //Rows below the window (the window shifts down when it would run past the start of the list)
    if (row > (selected + half) + ((selected - half) < 0 ? abs(selected - half) : 0)) return 1;
    return 0;
}

// Hex, decimal, ASCII and float columns for a cheat or search value.
// indent adds two spaces before the hex column (the search screens; the cheat editor prints the address there).
void menu_draw_value_columns(unsigned int *value, unsigned char flags, unsigned char indent) {
    //Print out the hex
    switch (flags & FLAG_DWORD) {
        case FLAG_DWORD:
            snprintf(buffer, sizeof(buffer), "%s0x%08lX  ", indent ? "  " : "", *value);
            break;
        case FLAG_WORD:
            snprintf(buffer, sizeof(buffer), "%s0x____%04hX  ", indent ? "  " : "", (unsigned short) *value);
            break;
        case FLAG_BYTE:
            snprintf(buffer, sizeof(buffer), "%s0x______%02hX  ", indent ? "  " : "", (unsigned char) *value);
            break;
    }
    pspDebugScreenPuts(buffer);

    //Print out the decimal
    switch (flags & FLAG_DWORD) {
        case FLAG_DWORD:
            snprintf(buffer, sizeof(buffer), "%010u  ", *value);
            break;
        case FLAG_WORD:
            snprintf(buffer, sizeof(buffer), "%010u  ", (unsigned short) *value);
            break;
        case FLAG_BYTE:
            snprintf(buffer, sizeof(buffer), "%010u  ", (unsigned char) *value);
            break;
    }
    pspDebugScreenPuts(buffer);

    //Print out the ASCII
    buffer[0] = *((unsigned char *) (((unsigned int) value) + 0));
    if ((buffer[0] <= 0x20) || (buffer[0] == 0xFF)) buffer[0] = '.';
    buffer[1] = *((unsigned char *) (((unsigned int) value) + 1));
    if ((buffer[1] <= 0x20) || (buffer[1] == 0xFF)) buffer[1] = '.';
    buffer[2] = *((unsigned char *) (((unsigned int) value) + 2));
    if ((buffer[2] <= 0x20) || (buffer[2] == 0xFF)) buffer[2] = '.';
    buffer[3] = *((unsigned char *) (((unsigned int) value) + 3));
    if ((buffer[3] <= 0x20) || (buffer[3] == 0xFF)) buffer[3] = '.';
    buffer[4] = 0;
    pspDebugScreenPuts(buffer);

    //Print out the float
    pspDebugScreenPuts("         ");
    f_cvt((unsigned int) value, buffer, sizeof(buffer), 6, MODE_GENERIC);
    pspDebugScreenPuts(buffer);
}

// Hex, decimal, ASCII and float columns for the game memory at a search result address.
void menu_draw_memory_columns(unsigned int address, unsigned char flags) {
    //Print out the hex
    switch (flags & FLAG_DWORD) {
        case FLAG_DWORD:
            snprintf(buffer, sizeof(buffer), "0x%08lX  ", *((unsigned int *) (address)));
            break;
        case FLAG_WORD:
            snprintf(buffer, sizeof(buffer), "0x____%04hX  ", *((unsigned short *) (address)));
            break;
        case FLAG_BYTE:
            snprintf(buffer, sizeof(buffer), "0x______%02hX  ", *((unsigned char *) (address)));
            break;
    }
    pspDebugScreenPuts(buffer);

    //Print out the decimal
    switch (flags & FLAG_DWORD) {
        case FLAG_DWORD:
            snprintf(buffer, sizeof(buffer), "%010lu  ", *((unsigned int *) (address)));
            break;
        case FLAG_WORD:
            snprintf(buffer, sizeof(buffer), "%010lu  ", *((unsigned short *) (address)));
            break;
        case FLAG_BYTE:
            snprintf(buffer, sizeof(buffer), "%010lu  ", *((unsigned char *) (address)));
            break;
    }
    pspDebugScreenPuts(buffer);

    //Print out the ASCII
    buffer[0] = *((unsigned char *) (address + 0));
    if ((buffer[0] <= 0x20) || (buffer[0] == 0xFF)) buffer[0] = '.';
    if ((flags & FLAG_DWORD) != FLAG_BYTE) {
        buffer[1] = *((unsigned char *) (address + 1));
        if ((buffer[1] <= 0x20) || (buffer[1] == 0xFF)) buffer[1] = '.';
    } else {
        buffer[1] = '.';
    }
    if ((flags & FLAG_DWORD) == FLAG_DWORD) {
        buffer[2] = *((unsigned char *) (address + 2));
        if ((buffer[2] <= 0x20) || (buffer[2] == 0xFF)) buffer[2] = '.';
        buffer[3] = *((unsigned char *) (address + 3));
        if ((buffer[3] <= 0x20) || (buffer[3] == 0xFF)) buffer[3] = '.';
    } else {
        buffer[2] = buffer[3] = '.';
    }
    buffer[4] = 0;
    pspDebugScreenPuts(buffer);

    //Print out the float
    pspDebugScreenPuts("         ");
    f_cvt(address, buffer, sizeof(buffer), 6, MODE_GENERIC);
    pspDebugScreenPuts(buffer);
}

// Game ID, battery, title and cheat ON/OFF status
void menu_draw_header(void) {
    //Draw the menu
    pspDebugScreenSetXY(49, 0);
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    snprintf(buffer, sizeof(buffer), "Game ID: %s", game_id);
    pspDebugScreenPuts(buffer);

    pspDebugScreenSetXY(49, 1);
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    snprintf(buffer, sizeof(buffer), "Battery: ");
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetTextColor(0x0000FF00);
    snprintf(buffer, sizeof(buffer), "%02d%%", scePowerGetBatteryLifePercent());
    pspDebugScreenPuts(buffer);

    pspDebugScreenSetXY(0, 0);
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPuts("cheatER");
    pspDebugScreenSetXY(0, 1);
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPuts("The NitePR Revamp\n\n");

    pspDebugScreenSetTextColor(0xFF00FFFF);
    pspDebugScreenSetTextColor(cheat_status ? 0xFF00FF00 : 0xFF0000FF);
    pspDebugScreenPuts(
            cheat_status ? "[CHEATS ARE ON]\n\n" : (!cheat_saved ? "[ALL CHEATS ARE OFF]\n\n" : "[CHEATS ARE OFF]\n\n"));
}

// First-open welcome screen
void menu_draw_welcome(void) {
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPuts("[Welcome!]");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("\n--------------------------------------------------------------------");
    pspDebugScreenSetTextColor(0xFF0000FF);
    pspDebugScreenPuts(
            "Please tell CheatER when to generate OFF codes by pressing\nthe MUSICAL NOTE button while inside the game at least once\nand NOT when the cheat menu is showing!!!\n\n");
    pspDebugScreenSetTextColor(0xFF00FF00);
    pspDebugScreenPuts(
            "HINTs:\n - To take a screenshot, press DOWN + SQUARE\n\n - Some cheats need the Settings->Cheat Hz option to be set to 15/1000\n   This is for cheats that overwrite a changing value\n\n");
    pspDebugScreenSetTextColor(0xFFFFFFFF);
}

// Tab names along the top, current tab highlighted
void menu_draw_tab_bar(void) {
    //Draw the tabs
    //The order here must match enum MenuTab in menu.h
    pspDebugScreenSetTextColor(tab_selected == TAB_CHEATS ? 0xFFFFFFFF : 0xFFBBBBBB);
    pspDebugScreenPuts("[Cheats List] ");
    pspDebugScreenSetTextColor(tab_selected == TAB_SEARCHER ? 0xFFFFFFFF : 0xFFBBBBBB);
    pspDebugScreenPuts("[Searcher] ");
    pspDebugScreenSetTextColor(tab_selected == TAB_MEM_VIEW ? 0xFFFFFFFF : 0xFFBBBBBB);
    pspDebugScreenPuts("[Mem View] ");
    pspDebugScreenSetTextColor(tab_selected == TAB_DECODER ? 0xFFFFFFFF : 0xFFBBBBBB);
    pspDebugScreenPuts("[Decoder] ");
    pspDebugScreenSetTextColor(tab_selected == TAB_TOOLS ? 0xFFFFFFFF : 0xFFBBBBBB);
    pspDebugScreenPuts("[Tools] ");
    pspDebugScreenSetTextColor(tab_selected == TAB_SETTINGS ? 0xFFFFFFFF : 0xFFBBBBBB);
    pspDebugScreenPuts("[Settings] ");
    pspDebugScreenSetTextColor(0xFFBBBBBB);
    pspDebugScreenPuts("\n--------------------------------------------------------------------");
}

// Copy/paste address popup
void menu_draw_copy_menu(void) {
    pspDebugScreenSetXY(25, 15);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    snprintf(buffer, sizeof(buffer), "%c", 0xC9);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetBackColor(0xFFFF0000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    snprintf(buffer, sizeof(buffer), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3,
            0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    snprintf(buffer, sizeof(buffer), "%c", 0xC6);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetXY(25, 16);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    snprintf(buffer, sizeof(buffer), "%c", 0xC9);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetBackColor(0xFFFF0000);
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    snprintf(buffer, sizeof(buffer), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0x20, 0x20, 0x20, '-', 0x20, 'C', 'o', 'p', 'i', 'e', 'r',
            0x20, '-', 0x20, 0x20, 0x20);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    snprintf(buffer, sizeof(buffer), "%c", 0xC6);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetXY(25, 17);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    snprintf(buffer, sizeof(buffer), "%c", 0xC9);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetBackColor(0xFFFF0000);
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    snprintf(buffer, sizeof(buffer), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
            0x20, 0x20, 0x20, 0x20, 0x20, 0x20);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    snprintf(buffer, sizeof(buffer), "%c", 0xC6);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetXY(25, 18);
    snprintf(buffer, sizeof(buffer), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0xC9, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3,
            0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC6);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetXY(25, 19);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    snprintf(buffer, sizeof(buffer), "%c%c", 0xC9, 0x20);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    if (copy_menu == 1) {
        pspDebugScreenSetTextColor(0xFF0000FF);
        pspDebugScreenPuts(">");
    } else {
        pspDebugScreenPuts(" ");
    }
    pspDebugScreenSetBackColor(0xFF000000);
    snprintf(buffer, sizeof(buffer), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 'C', 'o', 'p', 'y', 0x20, 'A', 'd', 'd', 'r', 'e', 's', 's',
            0x20, 0x20);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    snprintf(buffer, sizeof(buffer), "%c", 0xC6);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetXY(25, 20);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    snprintf(buffer, sizeof(buffer), "%c%c", 0xC9, 0x20);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    if (copy_menu == 2) {
        pspDebugScreenSetTextColor(0xFF0000FF);
        pspDebugScreenPuts(">");
    } else {
        pspDebugScreenPuts(" ");
    }
    pspDebugScreenSetBackColor(0xFF000000);
    snprintf(buffer, sizeof(buffer), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 'P', 'a', 's', 't', 'e', 0x20, 'A', 'd', 'd', 'r', 'e', 's',
            's', 0x20);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
    snprintf(buffer, sizeof(buffer), "%c", 0xC6);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetXY(25, 21);
    snprintf(buffer, sizeof(buffer), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0xC9, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
            0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xC6);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetXY(25, 22);
    snprintf(buffer, sizeof(buffer), "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0x20, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3,
            0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0x20);
    pspDebugScreenPuts(buffer);
    pspDebugScreenSetBackColor(0xFF000000);
    pspDebugScreenSetTextColor(0xFFFF8000);
}

// *** BACK BUFFER ***
// The menu is drawn off-screen and then copied to the screen in one go, so it never shows
// a half-drawn or blank frame. Same format as the screen: 16-bit 565, 512 pixels per line.

#define MENU_FRAME_BYTES (512 * 272 * 2)

static SceUID menu_back_block = -1;
static void *menu_back_buffer = NULL;

// Get the back buffer from kernel memory. If there's no room, menu_back_buffer stays NULL
// and the menu draws straight onto the screen instead.
void menu_back_buffer_open(void) {
    if (menu_back_buffer) return;

    menu_back_block = sceKernelAllocPartitionMemory(1, "cheatERmenu", PSP_SMEM_Low, MENU_FRAME_BYTES, NULL);
    if (menu_back_block >= 0) menu_back_buffer = sceKernelGetBlockHeadAddr(menu_back_block);
}

void menu_back_buffer_close(void) {
    if (menu_back_block >= 0) sceKernelFreePartitionMemory(menu_back_block);
    menu_back_block = -1;
    menu_back_buffer = NULL;
}

// Copy the finished menu to the screen, then point text output back at the screen
// for anything printed outside menu_draw() (search progress, error messages).
void menu_present(void) {
    if (!menu_back_buffer) return;

    memcpy(vram, menu_back_buffer, MENU_FRAME_BYTES);
    pspDebugScreenSetBase((u32 *) vram);
}

// Clear the screen before drawing a different screen. With the back buffer this isn't
// needed (menu_draw() always starts from a clear buffer), and skipping it avoids a blank flash.
void menu_clear_screen(void) {
    if (!menu_back_buffer) pspDebugScreenInitEx(vram, 0, 0);
}

void menu_draw(void) {
    if (menu_back_buffer) pspDebugScreenInitEx(menu_back_buffer, 0, 0);
    menu_draw_screen();
    menu_present();
}

static void menu_draw_screen(void) {
    menu_draw_header();

    //User friendly note
    if (lol_init) {
        menu_draw_welcome();
        return;
    }

    //Extended/sub menus?
    if (ext_menu) {
        switch (ext_menu) {
            case EXT_EDIT_CHEAT:
                menu_draw_edit_cheat();
                break;
            case EXT_SEARCH:
                menu_draw_edit_search();
                break;
            case EXT_DIFF_SEARCH:
                menu_draw_edit_diff_search();
                break;
            case EXT_TEXT_SEARCH:
                menu_draw_edit_text_search();
                break;
        }
    } else {
        menu_draw_tab_bar();

        //Draw the options for the respective tab
        switch (tab_selected) {
            case TAB_CHEATS:
                menu_draw_cheats_tab();
                break;
            case TAB_SEARCHER:
                menu_draw_searcher_tab();
                break;
            case TAB_MEM_VIEW:
                menu_draw_browser_tab();
                break;
            case TAB_DECODER:
                menu_draw_decoder_tab();
                break;
            case TAB_TOOLS:
                menu_draw_tools_tab();
                break;
            case TAB_SETTINGS:
                menu_draw_settings_tab();
                break;
        }
    }

    if (copy_menu) menu_draw_copy_menu();
}
