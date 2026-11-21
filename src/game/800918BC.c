// Categories: Platform > Hardware Control
#include "game/intc_globals.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

extern s32 gIrqHandlerSlots[] __asm__("D_8009CC8C");
extern volatile short *gInterruptMaskPtr __asm__("D_8009DD18");
extern void gBiosChangeClearPadAndLatch(s32) __asm__("func_8008D7B0");
extern void gBiosChangeClearRCnt(s32, s32) __asm__("func_8008CA20");
s32 gSetIrqSlotEnable(s32 index, s32 value) __asm__("func_800918BC");


/**
 *  @brief Updates an indexed control value and its hardware mask when enabled.
 *
 *  Reads the previous value from the control table and, if it changes,
 *  synchronizes the table, shadow mask, and hardware register when active.
 *  @param index Control-table index and corresponding mask bit.
 *  @param value New control value; zero clears the corresponding bit.
 *  @return Previous table value, even if no update was performed.
 *  @par Globals
 *  D_8009CC8C supplies the table and adjacent state; D_8009DD18 points to the hardware register.
 *  @par Control flow
 *  Returns early for an unchanged value; active state gates updates and index-specific calls.
 *  @par Calls
 *  func_8008D7B0 handles index 0; func_8008CA20 handles indices 0 and 4 through 6.
 *  @par Side effects
 *  May update the table, shadow mask, hardware register, and callee-managed state.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Updates the interrupt handler slot D_8009CC8C[index] to value when active,
 * atomically masking *D_8009DD18 (I_MASK) while updating the shadow repeat
 * mask at +0x30 and hardware I_MASK bit (1 << index). For indices 0 and 4
 * through 6 it also notifies BIOS stubs func_8008D7B0 and func_8008CA20,
 * returning the previous slot value.
 */
s32 gSetIrqSlotEnable(s32 index, s32 value)
{
    register s32 *base __asm__("$5") = gIrqHandlerSlots;
    s32 old = base[index];
    register u32 bits __asm__("$19");
    register u32 sample __asm__("$3");
    register char *state __asm__("$6");
    volatile short *hw;
    register u32 setmask __asm__("$3");
    register u32 clearmask __asm__("$2");
    if (value == old) return old;
    state = (char *)base - 4;
    if (*(u16 *)((char *)base - 4)) {
        hw = gInterruptMaskPtr;
        sample = (u16)*hw;
        *hw = 0;
        __asm__("" : "+r"(sample) : "m"(*hw));
        bits = (u16)sample;
        if (value) {
            setmask = 1 << index;
            base[index] = value;
            bits |= setmask;
            *(u16 *)(state + 0x30) |= setmask;
        } else {
            clearmask = ~(1 << index);
            base[index] = 0;
            bits &= clearmask;
            *(volatile u16 *)((char *)base + 0x2c) &= clearmask;
        }
        if (index == 0) {
            s32 disabled = !value;
            gBiosChangeClearPadAndLatch(disabled);
            gBiosChangeClearRCnt(3, disabled);
        }
        if (index == 4) gBiosChangeClearRCnt(0, !value);
        if (index == 5) gBiosChangeClearRCnt(1, !value);
        if (index == 6) gBiosChangeClearRCnt(2, !value);
        *gInterruptMaskPtr = bits;
    }
    return old;
}
