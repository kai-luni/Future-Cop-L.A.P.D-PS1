// Categories: Platform > Memory Card | Game Engine > Loading
#include "game/types.h"

extern volatile s32 gExecutableImageBase __asm__("D_80010000");
s32 gWaitCdLoaderIdle(void) __asm__("func_8001C478");


/**
 * @brief Spins until the loader busy word at 0x80010000 clears.
 *
 * The block at 0x80010000 - the first word of the program image, declared as D_80010000 in the card code - is non-zero while an asynchronous card command is in flight, so func_8001CE54 waits here before starting a file operation.
 *
 * @return Always 0.
 *
 * @par Globals
 * D_80010000 - the loader busy/state word, polled in a tight loop.
 *
 * @par Control flow
 * One backward branch (bnez) around the load; no timeout and no work inside the loop, so a stuck request hangs the console. The argument func_8001CE54 passes (0x11) is not read.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed; the word is only read.
 *
 * @par Callers
 * Confirmed: func_8001CE54 (0x8001CEB4) before a card file operation, func_8001D424 (0x8001D52C) and func_8001D6D0 (0x8001D7B0) in the card loader paths.
 *
 * @par Role and path
 * Polls D_80010000 in a tight loop until it becomes zero and then returns 0,
 * making no outgoing calls. Called by func_8001CE54, func_8001D424, and
 * func_8001D6D0 before issuing CD-ROM file operations.
 */
s32 gWaitCdLoaderIdle(void)
{
    while (gExecutableImageBase != 0) {
    }
    return 0;
}
