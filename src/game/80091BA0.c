// Categories: Platform > Interrupts | Game Engine > Frame
#include "game/types.h"

extern s32 D_8009DD30[];
extern volatile s32 D_8009DD50;
extern s32 *D_8009DD54;

extern void func_80091C90(s32 *dest, u32 count);
extern void func_800914B0(s32 a0, void *a1);
extern void func_80091BF8(void);
extern void func_80091C64(u32 index, s32 value);

/**
 * @brief Initializes the VSync root-counter hardware, callback table, and IRQ 0 handler.
 *
 * Confirmed: writes 0x107 to *D_8009DD54 (0x1F801114, PS1 Timer 1 mode register), clears VSync counter D_8009DD50, zeroes the 8-word callback array D_8009DD30, installs func_80091BF8 on IRQ 0 via func_800914B0, and returns func_80091C64.
 *
 * @param void No parameters.
 * @return Pointer to func_80091C64 (8-slot VSync callback registration routine).
 *
 * @par Globals
 * D_8009DD54 - pointer to PS1 T1_MODE (0x1F801114) written with 0x107; D_8009DD50 - VSync tick counter cleared; D_8009DD30 - 8-slot callback table zeroed.
 *
 * @par Control flow
 * Straight-line hardware setup, two helper calls, and return of func_80091C64; no branches or loops.
 *
 * @par Calls
 * func_80091C90 - zeroes 8 words in D_8009DD30; func_800914B0 - registers func_80091BF8 for IRQ 0 via D_8009DD10 slot +0x08.
 *
 * @par Side effects
 * Configures hardware register 0x1F801114, resets D_8009DD50 and D_8009DD30, and installs VBlank dispatcher func_80091BF8.
 *
 * @par Callers
 * Confirmed: func_80091614 (0x800916A8 in asm/us/main.s, stores returned pointer into D_8009DD10 +0x14) / Likely: none / Unknown: none.
 */
void *func_80091BA0(void)
{
    *D_8009DD54 = 0x107;
    D_8009DD50 = 0;
    func_80091C90(D_8009DD30, 8);
    func_800914B0(0, func_80091BF8);
    return func_80091C64;
}
