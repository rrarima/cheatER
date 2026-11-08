#ifndef CHEATER_CHEAT_H
#define CHEATER_CHEAT_H

/*
 * Cheat engine: the loaded cheats, their memory blocks, and turning them on/off.
 */

// *** STRUCTURES ***

// One address/value pair of a cheat.
typedef struct Block {
    unsigned char flags;
    unsigned int address;
    unsigned int std_val; // Original (OFF) value
    unsigned int hak_val; // Cheat (ON) value
} Block;

// A named cheat made up of `len` blocks starting at index `block`.
typedef struct Cheat {
    unsigned short block;
    unsigned short len;
    unsigned char flags;
    unsigned char name[32];
} Cheat;


// *** DEFINES ***

// Block flags
#define FLAG_DMA (1 << 4)
#define FLAG_FREEZE (1 << 5)
#define FLAG_DWORD (3 << 6)
#define FLAG_UWORD (2 << 6)
#define FLAG_WORD (1 << 6)
#define FLAG_BYTE (0 << 6)

// Cheat flags
#define FLAG_SELECTED (1 << 0)//If selected, will be disabled/enabled by music button
#define FLAG_CONSTANT (1 << 1)//If 1, cheat is constantly on regardless of music button
#define FLAG_FRESH (1 << 2)   //Cheat was just recently enabled/disabled

#define BLOCK_MAX 2048

// PSP memory map
#define USER_MEM_BASE 0x08800000                         // Start of user RAM; cheat file addresses are offsets from here
#define USER_MEM_START 0x08804000                        // First address a game uses (USER_MEM_BASE + 0x4000)
#define UNCACHED 0x40000000                              // OR'd into an address to bypass the CPU cache
#define USER_MEM_UNCACHED (USER_MEM_BASE | UNCACHED)     // 0x48800000

#define START_ADDRESS 0x48804000 // USER_MEM_START | UNCACHED
#define END_ADDRESS 0x4A000000


// *** GLOBALS ***

extern unsigned int block_total;
extern unsigned int cheat_total;
extern Block block[BLOCK_MAX];
extern Cheat cheat[BLOCK_MAX];
extern unsigned char cheat_status; // Cheats currently ON (music button)
extern unsigned char cheat_saved;  // OFF values have been captured
extern unsigned int cheat_hz;      // Re-apply delay in microseconds (0 = off)
extern unsigned char cheat_flash;  // Extra re-applies still to do
extern unsigned int cheat_dma;
extern unsigned int cheat_no;      // Counter used to name "NEW CHEAT %d"


// *** FUNCTIONS ***

unsigned int char2hex(unsigned char *data, unsigned int *type);
void cheat_new(unsigned char size, unsigned int address, unsigned int value);
unsigned int block_add(int fd, unsigned char *data);
void cheat_toggle(unsigned int cheat_index, unsigned char enable);
void cheat_enable(unsigned int cheat_index);
void cheat_disable(unsigned int cheat_index);
void cheat_apply(unsigned char type);//0=Enable/Disable FLAG_FRESH, -1=Enable only all (Freeze on all)
void cheat_reload(void);

#endif // CHEATER_CHEAT_H
