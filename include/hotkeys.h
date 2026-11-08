#ifndef CHEATER_HOTKEYS_H
#define CHEATER_HOTKEYS_H

#include <psptypes.h>

/*
 * In-game button combos (open menu, toggle cheats, take screenshot).
 */

extern unsigned int menu_key;
extern unsigned int trigger_key;
extern unsigned int screen_key;

// Registered with sceCtrlRegisterButtonCallback.
void button_callback(u32 curr, u32 last, void *arg);

#endif // CHEATER_HOTKEYS_H
