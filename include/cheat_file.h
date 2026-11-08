#ifndef CHEATER_CHEAT_FILE_H
#define CHEATER_CHEAT_FILE_H

/*
 * Reading and writing the per-game cheat file (ms0:/seplugins/cheatER/<GAME ID>.txt).
 */

// Path of the cheat file; the 10 underscores are replaced with the game ID at boot.
extern unsigned char game_dir[];
extern unsigned char game_id[11]; // 10-character ID from UMD_DATA.BIN, zero-terminated

// Parse the cheat file into `cheat` / `block`.
void cheat_load(void);

// Write the current cheats back to the cheat file.
void cheat_save(void);

#endif // CHEATER_CHEAT_FILE_H
