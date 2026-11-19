// Categories: Platform > CD-ROM
#include "game/types.h"

extern s32 gReadCdDmaSync(void *destination, s32 size) __asm__("func_8008FC30");
s32 gCdRead(void *destination, s32 size) __asm__("func_8008E598");


/**
 * @brief Reads a file through the CD file service into a buffer.
 *
 * Forwards to func_8008FC30 and turns its result into a boolean: only a zero
 * return (complete read) yields 1, so callers can branch on success without
 * knowing the loader's error codes.
 *
 * @param destination Buffer that receives the data.
 * @param size Number of bytes to read.
 * @return 1 on a complete read, 0 otherwise.
 * @par Globals
 * None directly.
 * @par Control flow
 * None.
 * @par Calls
 * func_8008FC30 - ReadFile equivalent.
 * @par Side effects
 * Fills the destination buffer.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 *
 * @par Role and path
 * Forwards destination and size to func_8008FC30 and returns 1 if it returns
 * 0, or 0 otherwise. Accesses no globals directly and has no direct callers
 * in src/.
 */
s32 gCdRead(void *destination, s32 size)
{
    return gReadCdDmaSync(destination, size) == 0;
}
