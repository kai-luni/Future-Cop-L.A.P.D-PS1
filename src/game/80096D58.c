// Categories: Platform > Sound > Init | Platform > SPU
#include "game/types.h"

struct Unk96D58 {
    s32 unk0;
    u16 unk4;
    u16 unk6;
    s32 unk8;
    s32 unkC;
};

extern struct Unk96D58 D_8009EA48;
extern s32 D_8009EA38;
extern s32 gArenaOffsetFlag __asm__("D_8009EA3C");
extern s32 gArenaCursor __asm__("D_8009EA40");
extern s32 gNormalisedMode __asm__("D_8009EA34");
extern s32 D_8009EA30;
extern s32 gModeEntryWord __asm__("D_8009EA5C");
extern s32 D_8009EA58;
extern s32 gOpOrderFlag __asm__("D_8009EEC0");
extern s32 D_8009EE90;
extern s32 D_8009EF04;
extern s32 gTableCursor __asm__("D_8009EF08");
extern s32 gSpuEntryTable __asm__("D_8009EF0C");
extern u32 D_8009EF14;
extern u16 gSpuInitWords[] __asm__("D_8009EA8E");

extern void func_80091480(void);
extern void gSpuResetHardware(s32 arg) __asm__("func_80096EB8");
extern void gSpuInitDmaEvent(void) __asm__("func_80096E40");
extern void gSpuWriteRegister(s32 index, u32 value, s32 flag) __asm__("func_800977C4");

/**
 * @brief (Re)initialises the SPU sound subsystem: hardware reset, warm-start voice table fill, DMA event setup and state reset.
 *
 * The SPU register base D_8009EEA4 is 0x1F801C00; the reset callee mutes the main volumes and waits out the status busy bits, printing "SPU: T/O [wait (reset)]" on timeout.
 *
 * @param cold_start 0 = warm start (also refills the 24-entry voice table with 0xC000); nonzero skips the fill.
 *
 * @par Globals
 * D_8009EA48 - sound state block, all five fields cleared; D_8009EA30/34/38/3C/58/5C, D_8009EE90, D_8009EEC0 and block-record words D_8009EF04/08/0C are zeroed.
 * D_8009EA40 - SPU RAM arena top, loaded from D_8009EF14 (initial 0xFFFE); func_80098458 reserves the tail above it. D_8009EA8E - last of the 24 u16 slots filled on warm start.
 *
 * @par Control flow
 * Straight-line except the backwards 0x18-slot fill loop; empty asm volatile statements pin registers and order the stores.
 *
 * @par Calls
 * func_80091480 - runs the registered display callback once. gSpuResetHardware - SPU hardware reset with busy-wait timeout. gSpuInitDmaEvent - one-time SPU DMA completion event setup. gSpuWriteRegister - writes the arena limit to SPU halfword register 0xD1 (0x1F801DA2), flag 0 = unshifted.
 *
 * @par Side effects
 * SPU registers via callees; clears sound state and allocator counters; voice table written on warm start only.
 *
 * @par Callers
 * Confirmed: func_80097A28 (warm-start wrapper, passes 0). No other direct caller in edges or listing; a cold-start entry was not found.
 */
void gSpuInitSubsystem(s32 cold_start) __asm__("func_80096D58");

void gSpuInitSubsystem(s32 cold_start)
{
    u16 fill;
    u16 *p;
    s32 i;
    u32 val;
    s32 idx;
    struct Unk96D58 *q;

    func_80091480();
    gSpuResetHardware(cold_start);

    fill = 0xC000;

    if (cold_start == 0) {
        i = 0x17;
        p = gSpuInitWords;
        for (; i >= 0; i--) {
            *p = fill;
            p--;
        }
    }

    gSpuInitDmaEvent();

    idx = 0xD1;
    __asm__ volatile("" : "=r"(idx) : "0"(idx));
    q = &D_8009EA48;
    __asm__ volatile("" : "=r"(q) : "0"(q));
    val = D_8009EF14;
    D_8009EA38 = 0;
    gArenaOffsetFlag = 0;
    q->unk0 = 0;
    q->unk4 = 0;
    q->unk6 = 0;
    q->unk8 = 0;
    q->unkC = 0;
    gArenaCursor = val;
    gSpuWriteRegister(idx, val, 0);

    D_8009EF04 = 0;
    gTableCursor = 0;
    gSpuEntryTable = 0;
    gNormalisedMode = 0;
    gOpOrderFlag = 0;
    D_8009EA30 = 0;
    gModeEntryWord = 0;
    D_8009EA58 = 0;
    D_8009EE90 = 0;
    __asm__ volatile("" ::: "memory");
}
