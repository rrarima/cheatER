#ifndef CHEATER_MIPS_H
#define CHEATER_MIPS_H

/*
 * Minimal MIPS disassembler used by the decoder tab.
 */

// Disassemble one instruction into the shared `buffer`.
void mips_decode(unsigned int opcode);

#endif // CHEATER_MIPS_H
