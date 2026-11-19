// Categories: Game Engine > Frame | Platform > Interrupts | BIOS > System Calls
#include "game/types.h"

extern volatile s32 gVSyncTickCounter __asm__("D_8009DD50");

extern void func_8008CA20(s32, s32);
extern void func_8008D7B0(s32);
extern void func_8008FF08(void *);
void gWaitVSyncTarget(s32 arg0, s32 arg1) __asm__("func_800913E8");


/**
 * @brief Spin-waits for the VSync frame counter to reach a target tick, timing out into BIOS error recovery.
 *
 * Polls D_8009DD50 until it reaches arg0 while decrementing a scaled busy-wait budget (arg1 << 15); if the budget expires at -1, prints "VSync: timeout\n" (0x800127EC) and resets VSync/pad BIOS clear state. Category Game Engine > Frame | Platform > Interrupts | BIOS > System Calls reflects the VSync counter loop and BIOS B0/C0 recovery calls.
 *
 * @param arg0 Target VSync frame count in D_8009DD50.
 * @param arg1 Timeout budget scaled by 32768 loop iterations.
 *
 * @par Globals
 * D_8009DD50 - interrupt-driven VSync counter polled in the wait loop.
 *
 * @par Control flow
 * Single while loop comparing D_8009DD50 < arg0; decrements volatile temp from arg1 << 15 and breaks after recovery at -1.
 *
 * @par Calls
 * func_8008FF08 - prints "VSync: timeout\n" via BIOS B0:3F; func_8008D7B0 - BIOS B0:5B(0); func_8008CA20 - BIOS C0:0A(3, 0).
 *
 * @par Side effects
 * Emits console output and updates BIOS pad/root-counter clear state on timeout.
 *
 * @par Callers
 * Confirmed: func_80091270 (0x80091320, 0x80091344 in asm/us/main.s) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Spin-waits until the interrupt-driven VSync counter D_8009DD50 reaches arg0
 * while decrementing a timeout counter initialized to arg1 << 15. If the
 * timeout reaches -1, it prints the VSync timeout message via func_8008FF08
 * and invokes BIOS stubs func_8008D7B0(0) (B0:5B) and func_8008CA20(3, 0)
 * (C0:0A) before breaking. Driven by func_80091270.
 */
void gWaitVSyncTarget(s32 arg0, s32 arg1)
{
    volatile s32 temp = arg1 << 15;

    while (gVSyncTickCounter < arg0) {
        if (--temp == -1) {
            register u8 *base __asm__("$4") = (u8 *)0x80010000;
            __asm__("" : "=r"(base) : "0"(base));
            func_8008FF08(base + 0x27EC);
            func_8008D7B0(0);
            func_8008CA20(3, 0);
            break;
        }
    }
}
