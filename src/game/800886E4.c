// Categories: Platform > CD-ROM
#include "game/types.h"

extern void *gFillMemory(void *destination, int value, unsigned int count) __asm__("func_8001E4C0");
extern s32 func_8008E1BC(s32 code, void *arg1, void *arg2);

/**
 * @brief Polls CD-ROM command 1 status and issues command 0xA on command failure.
 *
 * Zeroes two `0x20`-byte stack buffers and runs `func_8008E1BC(1, buffer1, buffer2)`: if command 1 succeeds, returns `0` when `buffer2[0]` is `0` or `2` (or `1` otherwise); if command 1 fails, sends CD-ROM command `0xA` via `func_8008E1BC(0xA, 0, 0)` and returns `1`. Category Platform > CD-ROM follows synchronous CD command dispatch via func_8008E1BC.
 *
 * @param void No parameters.
 * @return `0` if command 1 succeeded and `buffer2[0]` is `0` or `2`, otherwise `1`.
 *
 * @par Globals
 * None directly (func_8008E1BC uses CD-ROM state D_8009C820..D_8009C8B4).
 *
 * @par Control flow
 * Clears `buffer1`/`buffer2`, branches on `func_8008E1BC(1, ...)` then `switch (buffer2[0])` (`0`/`2` -> `0`, default -> `1`), else calls command `0xA`.
 *
 * @par Calls
 * func_8001E4C0 - zeroes `buffer1` and `buffer2` (`0x20` bytes each).
 * func_8008E1BC - sends CD-ROM command `1` (status poll) and, on failure, command `0xA`.
 *
 * @par Side effects
 * Issues CD-ROM hardware commands `1` and conditionally `0xA`.
 *
 * @par Callers
 * Confirmed: func_80089018 (0x8008909C in asm/us/main.s, spin-wait loop after command `8`) / Likely: none / Unknown: none.
 */
s32 func_800886E4(void)
{
    u8 buffer1[0x20];
    u8 buffer2[0x20];

    gFillMemory(buffer1, 0, 0x20);
    gFillMemory(buffer2, 0, 0x20);

    if (func_8008E1BC(1, buffer1, buffer2) != 0) {
        switch (buffer2[0]) {
        case 0:
        case 2:
            return 0;
        default:
            return 1;
        }
    }

    func_8008E1BC(0xA, 0, 0);
    return 1;
}
