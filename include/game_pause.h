#ifndef CHEATER_GAME_PAUSE_H
#define CHEATER_GAME_PAUSE_H

/*
 * Pausing the game by suspending every thread that wasn't running when the plugin started.
 */

#include <pspkerneltypes.h>

// Remember which threads exist at plugin start (these are never suspended).
void game_threads_snapshot(void);

void game_pause(SceUID thid);
void game_resume(SceUID thid);

#endif // CHEATER_GAME_PAUSE_H
