#ifndef CHEATER_SEARCH_H
#define CHEATER_SEARCH_H

/*
 * Memory search state (the search logic itself lives in the menu input handler).
 */

#include "cheat.h"

extern unsigned char cheat_search;
extern unsigned char search_mode;
extern unsigned int search_no;
extern unsigned int search_max;
extern unsigned int search_history_counter;
extern Block search_history[16];
extern unsigned int search_result_counter;
extern unsigned int search_address[100];
extern unsigned char *search_mode_name[];

// Clear the search history and default every entry to 32-bit.
void search_history_reset(void);

#endif // CHEATER_SEARCH_H
