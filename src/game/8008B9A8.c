// Categories: Platform > Dispatch
#include "game/types.h"

extern u32 gPadTxByteHook __asm__("D_8009C730");
extern s32 gTransferSio0Byte(void *s, u32 a1) __asm__("func_8008AB80");
s32 gSendPadCmdByte(void *s) __asm__("func_8008B9A8");


/**
 *  @brief Runs a callback and normalizes the result of a follow-up operation.
 *
 *  Derives a flag from fields in the supplied structure, passes it to the
 *  global callback, and forwards the callback's low byte to a helper.
 *  @param s Structure passed to both operations and inspected for the flag.
 *  @return The helper result if negative, zero, or 0x5A; otherwise -9.
 *  @par Globals
 *  D_8009C730 holds the callback address invoked with s and the flag.
 *  @par Control flow
 *  The flag is one only when the pointed-to byte has high nibble 8 and
 *  the byte at s offset 0x36 is zero; other nonnegative results become -9.
 *  @par Calls
 *  The D_8009C730 callback supplies a value; func_8008AB80 processes its low byte.
 *  @par Side effects
 *  Invokes the callback and helper; their effects are not shown.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Computes a selection flag that is 1 only when the high nibble of the byte
 * pointed to at offset 0x3C is 8 and the byte at offset 0x36 is zero, then
 * invokes gPadTxByteHook with s and that flag. Passes the low byte of the
 * hook result to func_8008AB80(s, r0 & 0xFF) and returns negative errors, 0,
 * or 0x5A unchanged while mapping any other nonnegative result to -9.
 */
s32 gSendPadCmdByte(void *s)
{
    u32 flag;
    u32 r0;
    s32 r;
    s32 ret;

    flag = 0;
    if ((*(u8 *)(*(u32 *)((u8 *)s + 0x3C)) >> 4) == 8) {
        flag = (*(u8 *)((u8 *)s + 0x36) == 0);
    }
    r0 = ((u32 (*)(void *, u32))gPadTxByteHook)(s, flag);
    r = gTransferSio0Byte(s, r0 & 0xFF);
    ret = r;
    if (r == 0x5A) {
        goto end;
    }
    if (r == 0) {
        goto end;
    }
    if (r >= 0) {
        ret = -9;
        goto end;
    }
end:
    return ret;
}
