#include <string.h>

#include "search.h"


unsigned char cheat_search = 0;
unsigned char search_mode = 0;
unsigned int search_no = 0;
unsigned int search_max = 0;
unsigned int search_history_counter = 0;
Block search_history[16];
unsigned int search_result_counter = 0;
unsigned int search_address[100];
unsigned char *search_mode_name[] = {
        "  0=Same                                                        ",
        "  1=Different                                                   ",
        "  2=Greater                                                     ",
        "  3=Less                                                        ",
        "  4=Inc by    ",
        "  5=Dec by    ",
        "  6=Equal to  "};

void search_history_reset(void) {
    //Clear the search history to 0
    memset(&search_history, 0, sizeof(search_history));
    search_history[0].flags |= FLAG_DWORD;
    search_history[1].flags |= FLAG_DWORD;
    search_history[2].flags |= FLAG_DWORD;
    search_history[3].flags |= FLAG_DWORD;
    search_history[4].flags |= FLAG_DWORD;
    search_history[5].flags |= FLAG_DWORD;
    search_history[6].flags |= FLAG_DWORD;
    search_history[7].flags |= FLAG_DWORD;
    search_history[8].flags |= FLAG_DWORD;
    search_history[9].flags |= FLAG_DWORD;
    search_history[10].flags |= FLAG_DWORD;
    search_history[11].flags |= FLAG_DWORD;
    search_history[12].flags |= FLAG_DWORD;
    search_history[13].flags |= FLAG_DWORD;
    search_history[14].flags |= FLAG_DWORD;
    search_history[15].flags |= FLAG_DWORD;
}
