// Categories: Platform > Interrupts | Game Engine > Frame
#include "game/timer_globals.h"

#include "game/types.h"

extern s32 gVSyncCallbackTable[] __asm__("D_8009DD30");
extern volatile s32 gVSyncTickCounter __asm__("D_8009DD50");
extern s32 *gTimer1ModePtr __asm__("D_8009DD54");

extern void func_80091C90(s32 *dest, u32 count);
extern void gSetIrqCallback(s32 a0, void *a1) __asm__("func_800914B0");
extern void func_80091BF8(void);
extern void gSetVSyncCallbackSlot(u32 index, s32 value) __asm__("func_80091C64");
void *gInitVSyncHandler(void) __asm__("func_80091BA0");


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
 *
 * @par Role and path
 * Initializes the VSync root-counter hardware and callback table by writing
 * 0x107 to *D_8009DD54 (T1_MODE at 0x1F801114), clearing D_8009DD50, zeroing
 * the 8-word table D_8009DD30 via func_80091C90, and installing func_80091BF8
 * on IRQ 0 via func_800914B0. Returns func_80091C64; called by func_80091614
 * to populate D_8009DD10 slot +0x14.
 */
void *gInitVSyncHandler(void)
{
    *gTimer1ModePtr = 0x107;
    gVSyncTickCounter = 0;
    func_80091C90(gVSyncCallbackTable, 8);
    gSetIrqCallback(0, func_80091BF8);
    return gSetVSyncCallbackSlot;
}
