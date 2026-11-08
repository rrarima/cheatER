#ifndef CHEATER_MENU_INPUT_H
#define CHEATER_MENU_INPUT_H

/*
 * Button handling for each menu screen, called once per pass of menu_input()'s loop (see menu.h).
 * `pad_buttons` is the previous pass's buttons, used to detect a button being released.
 */

// Wait before a held button repeats; returns early when the buttons change (menu_input.c)
void menu_key_delay(unsigned int us);

// Copy/paste popup (menu_input.c)
void menu_input_copy_menu(unsigned int pad_buttons);

// Sub-menus, ext_menu 1-4 (menu_input_edit.c)
void menu_input_edit_cheat(unsigned int pad_buttons);
void menu_input_edit_search(unsigned int pad_buttons);
void menu_input_edit_diff_search(unsigned int pad_buttons);
void menu_input_edit_text_search(unsigned int pad_buttons);

// Tabs, tab_selected 0-5 (menu_input_tabs.c); Tools (4) has no input
void menu_input_cheats_tab(void);
void menu_input_searcher_tab(void);
void menu_input_browser_tab(void);
void menu_input_decoder_tab(void);
void menu_input_settings_tab(void);

#endif // CHEATER_MENU_INPUT_H
