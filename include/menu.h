#ifndef CHEATER_MENU_H
#define CHEATER_MENU_H

/*
 * The on-screen cheat menu: shared state, drawing and input handling.
 */

#include <pspctrl.h>
#include <pspdebug.h>

// Main menu tabs (tab_selected), in the order they appear in the tab bar
enum MenuTab {
    TAB_CHEATS = 0,
    TAB_SEARCHER,
    TAB_MEM_VIEW,
    TAB_DECODER,
    TAB_TOOLS,
    TAB_SETTINGS,
    TAB_COUNT
};
#define TAB_LAST (TAB_COUNT - 1)

// Sub-menus opened from a tab (ext_menu)
enum ExtMenu {
    EXT_NONE = 0,
    EXT_EDIT_CHEAT,
    EXT_SEARCH,     // Exact value search
    EXT_DIFF_SEARCH,// Unknown value (difference) search
    EXT_TEXT_SEARCH
};

// Where the cheat editor goes back to when it's closed (ext_opt)
enum EditorReturn {
    EDITOR_RETURN_TAB = 0,         // The tab it was opened from (Cheats List)
    EDITOR_RETURN_SEARCH = 1,      // The exact search results (EXT_SEARCH)
    EDITOR_RETURN_DIFF_SEARCH = 2, // The unknown value search results (EXT_DIFF_SEARCH)
    EDITOR_RETURN_SETTINGS = 3     // Settings, on the "Add Empty Cheat?" row
};

#define LINE_CLEAR(line)                                                                    \
    pspDebugScreenSetXY(0, line);                                                            \
    pspDebugScreenPuts("                                                                   "); \
    pspDebugScreenSetXY(0, line);


// *** GLOBALS ***

extern SceCtrlData pad;
extern void *vram;
extern unsigned char menu_drawn;
extern unsigned char cheat_pause; // Pause the game while the menu is open
extern unsigned int cheat_selected;
extern unsigned int tab_selected;

// Extended/sub menus
extern unsigned char ext_menu;
extern unsigned int ext_selected[4];
extern unsigned char ext_opt;
extern unsigned int ext_opt_arg;
extern unsigned int dump_no;

// Browser tab
extern unsigned int browse_address;
extern unsigned int browse_y;
extern unsigned int browse_c;
extern unsigned int browse_x;
extern unsigned int browse_lines;
extern unsigned int browse_format;

// Decoder tab
extern unsigned int decode_format;
extern unsigned int decode_address;
extern unsigned int decode_y;
extern unsigned int decode_c;
extern unsigned int decode_x;

// Tracker (Settings -> "Real Addressing in Logger"; Tools tab not built yet)
extern unsigned int track_format;
extern unsigned int track_address;
extern unsigned int track_y;
extern unsigned int track_x;
extern unsigned int track_c;
extern unsigned int track_backup;
extern unsigned char track_mode;
extern unsigned char track_status;
extern unsigned char track_pause;

extern unsigned char cheat_button_age_x;
extern unsigned char cheat_button_age_y;
extern unsigned char copy_menu;//0=Menu Off, 1=Menu On, Copy selected, 2=Menu On, Paste selected
extern unsigned int copy_data;

// Welcome screen
extern signed char lol_direction;
extern unsigned char lol_value;
extern unsigned char lol_init;

extern unsigned int dec_delta[10];


// *** FUNCTIONS ***

void menu_draw(void);
void menu_input(void);

#endif // CHEATER_MENU_H
