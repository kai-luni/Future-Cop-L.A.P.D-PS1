// Categories: Platform > CD-ROM | Platform > Interrupts
#include "game/types.h"

extern s32 D_8009C8AC;
extern s32 D_8009C8A8;
extern s32 D_8009C8B8;
extern u8 D_8008FE28[];

extern void func_80091480(void);
extern void func_800914B0(s32 arg0, u8 *arg1);

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
 */
void func_8008F89C(void)
{
    register u32 base __asm__("$1");
    register u8 *callback __asm__("$5");
    D_8009C8AC = 0;
    D_8009C8A8 = 0;
    D_8009C8B8 = 0;
    __asm__("" ::: "memory");
    base = 0x800A0000;
    __asm__("" : "+r"(base));
    *(s32 *)(base - 0x374C) = 0;
    func_80091480();
    callback = D_8008FE28;
    __asm__("" : "+r"(callback));
    func_800914B0(2, callback);
}
