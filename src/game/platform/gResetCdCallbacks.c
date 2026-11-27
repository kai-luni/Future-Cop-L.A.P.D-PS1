// Categories: Platform > CD-ROM | Platform > Interrupts
#include "game/types.h"

extern s32 gCdReadyCallback __asm__("D_8009C8AC");
extern s32 gCdSyncCallback __asm__("D_8009C8A8");
extern s32 gCdIrqExtraByte __asm__("D_8009C8B8");
extern u8 gCdIrqHandlerAddress[] __asm__("D_8008FE28");

extern void gResetIrqDispatch(void) __asm__("func_80091480");
extern void gSetIrqCallback(s32 arg0, u8 *arg1) __asm__("func_800914B0");
void gResetCdCallbacks(void) __asm__("func_8008F89C");


/**
 * @brief Resets the CD driver callback words and installs its default handler.
 *
 * D_8009C8AC, D_8009C8A8 and D_8009C8B8 are cleared, the scratch block at
 * 0x8009C8B4 is zeroed, then func_80091480 runs the interrupt teardown and
 * func_800914B0 registers func_8008FE28 as the channel-2 handler.
 * @par Globals
 * D_8009C8A8/D_8009C8AC/D_8009C8B8 - callback and status words;
 * D_8008FE28 - default handler address.
 * @par Control flow
 * Four stores, two calls.
 * @par Calls
 * func_80091480 - interrupt teardown. func_800914B0 - handler registration.
 * @par Side effects
 * Clears the driver callbacks and installs the default handler.
 * @par Callers
 * Confirmed: the CD init path (func_8008F8E8 neighbourhood).
 *
 * @par Role and path
 * Clears the CD driver callback words D_8009C8AC, D_8009C8A8, D_8009C8B8, and
 * the status word at 0x8009C8B4, then resets interrupt dispatch via
 * func_80091480 and registers func_8008FE28 on IRQ slot 2 via func_800914B0.
 * Called by func_8008E04C when mode equals 2 to perform a callback-only CD
 * driver reset.
 */
void gResetCdCallbacks(void)
{
    register u32 base __asm__("$1");
    register u8 *callback __asm__("$5");
    gCdReadyCallback = 0;
    gCdSyncCallback = 0;
    gCdIrqExtraByte = 0;
    __asm__("" ::: "memory");
    base = 0x800A0000;
    __asm__("" : "+r"(base));
    *(s32 *)(base - 0x374C) = 0;
    gResetIrqDispatch();
    callback = gCdIrqHandlerAddress;
    __asm__("" : "+r"(callback));
    gSetIrqCallback(2, callback);
}
