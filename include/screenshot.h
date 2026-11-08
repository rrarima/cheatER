#ifndef CHEATER_SCREENSHOT_H
#define CHEATER_SCREENSHOT_H

/*
 * Screenshots saved as 24-bit BMP files.
 */

extern unsigned char screen_time; // Set by the hotkey to request a screenshot
extern unsigned int screen_no;
extern unsigned char screen_path[64];

// Pick ms0:/PICTURE or ms0:/PSP/PHOTO and find the next free screen number.
void screenshot_path_init(void);

// Take a screenshot if one was requested.
void screenshot_handle_request(void);

int bitmap_write(void *frame_addr, void *ignored, int format, const char *file);

#endif // CHEATER_SCREENSHOT_H
