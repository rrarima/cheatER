#ifndef CHEATER_FLOAT_FORMAT_H
#define CHEATER_FLOAT_FORMAT_H

/*
 * Float to text conversion (taken from PSPLINK util.c, BSD licensed).
 * Renamed from float.h so it no longer shadows the standard <float.h>.
 */

#define MODE_GENERIC 0
#define MODE_EXP 1
#define MODE_FLOAT_ONLY 2

// Format the float stored at `addr` into `buf`.
void f_cvt(unsigned int addr, char *buf, int bufsize, int precision, int mode);

#endif // CHEATER_FLOAT_FORMAT_H
