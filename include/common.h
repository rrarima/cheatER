#ifndef CHEATER_COMMON_H
#define CHEATER_COMMON_H

/*
 * Things shared by every part of the plugin.
 */

#include <pspkerneltypes.h>

// Scratch text buffer used for sprintf() before printing or writing to a file.
extern unsigned char buffer[64];

// Kernel export that the PSPSDK headers don't declare.
extern SceUID sceKernelSearchModuleByName(unsigned char *);

#endif // CHEATER_COMMON_H
