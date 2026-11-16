// Categories: Platform > CD-ROM
#include "game/types.h"

extern s32 func_8008FC30(void *destination, s32 size);

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
 */
s32 func_8008E598(void *destination, s32 size)
{
    return func_8008FC30(destination, size) == 0;
}
