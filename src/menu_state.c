#include "cheat.h"
#include "menu.h"


SceCtrlData pad;
void *vram;
unsigned char menu_drawn = 0;
unsigned char cheat_pause = 0;
unsigned int cheat_selected = 0;
unsigned int tab_selected = TAB_CHEATS;
unsigned char ext_menu = EXT_NONE;
unsigned int ext_selected[4] = {0, 0, 0, 0};
unsigned char ext_opt = 0;
unsigned int ext_opt_arg = 0;
unsigned int dump_no = 0;
unsigned int browse_address = START_ADDRESS;
unsigned int browse_y = 0;
unsigned int browse_c = 0;
unsigned int browse_x = 0;
unsigned int browse_lines = 16;
unsigned int decode_format = USER_MEM_UNCACHED;
unsigned int track_format = USER_MEM_UNCACHED;
unsigned int browse_format = USER_MEM_UNCACHED;
unsigned int decode_address = START_ADDRESS;
unsigned int decode_y = 0;
unsigned int decode_c = 0;
unsigned int decode_x = 0;
unsigned int track_address = START_ADDRESS;
unsigned int track_y = 0;
unsigned int track_x = 0;
unsigned int track_c = 0;
unsigned int track_backup = 0;
unsigned char track_mode = 0;
unsigned char track_status = 0;
unsigned char track_pause = 0;
unsigned char cheat_button_age_x = 0;
unsigned char cheat_button_age_y = 0;
unsigned char copy_menu = 0;//0=Menu Off, 1=Menu On, Copy selected, 2=Menu On, Paste selected
unsigned int copy_data = USER_MEM_START;

signed char lol_direction = 1;
unsigned char lol_value = 0;
unsigned char lol_init = 1;

unsigned int dec_delta[10] = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1};
