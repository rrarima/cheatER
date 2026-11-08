#ifndef CHEATER_PLUGIN_H
#define CHEATER_PLUGIN_H

/*
 * Plugin lifecycle (module entry points live in main.c).
 */

#include <pspkerneltypes.h>

extern unsigned char running;
extern SceUID thid; // cheatER's own thread

void restart_plugin(const char *plugin_path);

#endif // CHEATER_PLUGIN_H
