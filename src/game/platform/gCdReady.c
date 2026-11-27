// Categories: Platform > CD-ROM > Control
#include "game/types.h"

extern s32 gCdReadyLoop(s32 mode, u8 *result) __asm__("func_8008EF7C");
s32 gCdReady(s32 mode, u8 *result) __asm__("func_8008E174");


/**
 * @brief Forwards a mode/result query to func_8008EF7C.
 *
 * @param mode Query mode.
 * @param result Output buffer.
 * @return The callee's result.
 *
 * @par Confirmed
 * Pure tail call of func_8008EF7C(mode, result).
 *
 * @par Likely role
 * A CD-library status shim: the callers sit in the draw setup and disc
 * access families, the wrapper likely adapts the calling convention of
 * the underlying BIOS-side routine.
 *
 * @par Unknown
 * Which CD command func_8008EF7C issues.
 *
 * @par Callers
 * func_8001C680, func_8001C718, func_8001CE54, func_8009113C.
 *
 * @par Role and path
 * Forwards mode and result directly to func_8008EF7C to poll or block for a
 * pending CD-ROM command result. Accesses no globals directly and is called
 * by func_8001C680, func_8001C718, func_8001CE54, and func_8009113C.
 */
s32 gCdReady(s32 mode, u8 *result)
{
    return gCdReadyLoop(mode, result);
}
