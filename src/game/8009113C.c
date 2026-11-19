// Categories: Platform > Device Polling
#include "game/types.h"

extern s32 gReadWindowTable[] __asm__("D_8009CBEC");
extern s32 gVSyncWait(s32 arg0) __asm__("func_80091270");
extern void gStartCdSectorRead(s32 arg0) __asm__("func_80090C18");
extern s32 gCdReady(s32 mode, u8 *result) __asm__("func_8008E174");
s32 gCdReadSync(s32 mode, u8 *result) __asm__("func_8009113C");


/**
 * @brief Polls a service result word with two timeout limits.
 *
 * The loop reads the current time through func_80091270(-1) and compares it
 * with the value at D_8009CBEC + 4 (0x4B0 = 1200 ticks). If that window is
 * exceeded the result becomes -1. Otherwise the status word at -8 is checked:
 * negative status or an expired 0x3C (60 tick) window calls func_80090C18(1)
 * and returns the value at -0x1C, else the status at -8 is returned. With mode
 * 0 the loop repeats while the flag word is set and the result is still 0 or
 * positive. Finally the result is handed to func_8008E174(1, result), and an
 * "empty but pending" combination is normalised to 1.
 *
 * @param mode 0 keeps polling until settled, non-zero does a single pass.
 * @param result Buffer that receives the converted result.
 * @return The raw or converted status value.
 *
 * @par Globals
 * D_8009CBEC - service status block (base pointer with negative offsets).
 *
 * @par Control flow
 * Do-while loop with two timeout branches inside.
 *
 * @par Calls
 * func_80091270 - time query. func_80090C18 - timeout/abort handling.
 * func_8008E174 - converts a status into the caller's buffer.
 *
 * @par Side effects
 * Writes result and may advance service state through the abort call.
 *
 * @par Callers
 * Likely: func_8009081C (same address family).
 *
 * @par Role and path
 * Polls the CD-ROM read window table D_8009CBEC using func_80091270(-1),
 * returning -1 if the 0x4B0-tick total window expires or calling
 * func_80090C18(1) if status is negative or the 0x3C-tick stall window
 * expires. In mode 0 it loops until settled, then converts the status via
 * func_8008E174(1, result) and normalizes an empty-but-pending state to 1.
 * Driven by func_8009081C.
 */
s32 gCdReadSync(s32 mode, u8 *result)
{
    register s32 ret __asm__("$16");
    register s32 *ptr __asm__("$17");
    register s32 *flag __asm__("$18");
    register s32 saved_mode __asm__("$19");
    register u8 *saved_result __asm__("$20");
    s32 now;

    saved_mode = mode;
    saved_result = result;
    ptr = gReadWindowTable;
    flag = ptr + 2;
    do {
        now = gVSyncWait(-1);
        if (ptr[0] + 0x4b0 < now) {
            ret = -1;
        } else {
            if (ptr[-2] < 0) goto timeout;
            now = gVSyncWait(-1);
            if (ptr[-1] + 0x3c < now) {
            timeout:
                gStartCdSectorRead(1);
                ret = ptr[-7];
            } else {
                ret = ptr[-2];
            }
        }
    } while (saved_mode == 0 && ((*flag != 0 && ret == 0) || ret > 0));
    gCdReady(1, saved_result);
    if (*flag != 0 && ret == 0) {
        ret = 1;
    }
    return ret;
}
