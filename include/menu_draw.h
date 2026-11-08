#ifndef CHEATER_MENU_DRAW_H
#define CHEATER_MENU_DRAW_H

/*
 * The individual menu screens drawn by menu_draw() (see menu.h).
 * Each one draws into the debug screen using the shared menu state.
 */

// Back buffer: open/close around the menu, copy to the screen, clear between screens (menu_draw.c)
void menu_back_buffer_open(void);
void menu_back_buffer_close(void);
void menu_present(void);
void menu_clear_screen(void);

// Shared helpers (menu_draw.c)
int menu_row_hidden(int row, int selected, int half, int total);
void menu_draw_value_columns(unsigned int *value, unsigned char flags, unsigned char indent);
void menu_draw_memory_columns(unsigned int address, unsigned char flags);

// Always shown (menu_draw.c)
void menu_draw_header(void);
void menu_draw_welcome(void);
void menu_draw_tab_bar(void);
void menu_draw_copy_menu(void);

// Sub-menus, ext_menu 1-4 (menu_draw_edit.c)
void menu_draw_edit_cheat(void);
void menu_draw_edit_search(void);
void menu_draw_edit_diff_search(void);
void menu_draw_edit_text_search(void);

// Tabs, tab_selected 0-5 (menu_draw_tabs.c)
void menu_draw_cheats_tab(void);
void menu_draw_searcher_tab(void);
void menu_draw_browser_tab(void);
void menu_draw_decoder_tab(void);
void menu_draw_tools_tab(void);
void menu_draw_settings_tab(void);

#endif // CHEATER_MENU_DRAW_H
