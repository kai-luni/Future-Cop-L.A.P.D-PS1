// Categories: Platform > CD-ROM | Utilities > String
#include "game/types.h"

extern s32 gBiosStrncmp(u8 *arg0, u8 *arg1, s32 arg2) __asm__("func_80090890");
s32 gMatchCdFilename(u8 *arg0, u8 *arg1) __asm__("func_800901F8");


/**
 * @brief Compares two 12-byte filenames for equality during CD directory lookup.
 *
 * Calls the BIOS strncmp stub func_80090890(arg0, arg1, 0xC) and returns 1 when the 12-byte comparison yields 0.
 *
 * @param arg0 First filename string (e.g. cached directory entry name in D_800A3E88).
 * @param arg1 Second filename string (normalized target filename segment).
 * @return 1 if the first 12 bytes match, 0 otherwise.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Single call to func_80090890(arg0, arg1, 0xC) followed by boolean == 0 test.
 *
 * @par Calls
 * func_80090890 - BIOS vector A0:0x18 (strncmp) with length 0xC.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8008FF20 (0x80090120, scans the 64-entry cached CD directory table) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Compares two 12-byte filename strings by calling the BIOS strncmp stub
 * func_80090890(arg0, arg1, 0xC) and returning 1 when the result is zero.
 * Called exclusively by func_8008FF20 while scanning the 64-entry cached
 * directory table D_800A3E80.
 */
s32 gMatchCdFilename(u8 *arg0, u8 *arg1)
{
    return gBiosStrncmp(arg0, arg1, 0xC) == 0;
}
