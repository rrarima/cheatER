#ifndef CHEATER_HOOKS_H
#define CHEATER_HOOKS_H

/*
 * System function hooks: MAC address spoofing and the OpenPSID patch.
 */

#include "module.h"

typedef struct Hook {
    ModuleFunc modfunc;
    char modname[32];
    char libname[32];
    u32 nid;
    void *func;
} Hook;

// Config block patched into the binary by external tools ("CFG" signature).
// Bytes 5-10: MAC address, 11-14: trigger key, 15-18: menu key, 20-23: screenshot key.
extern unsigned char cfg[24];

// Install the MAC address hook if enabled in cfg.
void mac_hook_install(void);

// Patch sceOpenPSIDGetOpenPSID to return the PSID saved in cheatERimportant.bin.
void psid_patch_apply(void);

#endif // CHEATER_HOOKS_H
