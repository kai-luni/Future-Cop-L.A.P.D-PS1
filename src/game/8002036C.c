// Categories: Platform > Device Service | Utilities > Resource Release
#include "game/types.h"

typedef struct Unk8009AB08 {
    u8 unk00[0x8C8];
    s32 unk8C8;
    u8 unk8CC[8];
    s32 unk8D4;
    s32 unk8D8;
    s32 unk8DC;
    s32 unk8E0;
    s32 unk8E4;
    s32 unk8E8;
    s32 unk8EC;
    s32 unk8F0;
} Unk8009AB08;

extern Unk8009AB08 *gOperationState __asm__("D_8009AB08");

extern void gDisableCpuInterrupts(void) __asm__("func_8008C9C0");
extern void gBiosDisableEvent(s32 arg0) __asm__("func_8008C990");
extern void gBiosCloseEvent(s32 arg0) __asm__("func_8008C970");
extern void gEnableCpuInterrupts(void) __asm__("func_8008C9E0");
void gReleaseMcEvents(void) __asm__("func_8002036C");


/**
 * @brief Counts a shared release down and frees eight handles when it reaches zero.
 *
 * The counter at 0x8C8 is decremented; anything other than zero ends the
 * function immediately. At zero the device lock is taken and the eight words
 * at 0x8D4..0x8F0 are passed twice through the two release entry points (first
 * func_8008C990, then func_8008C970), after which the lock is released again.
 *
 * @par Globals
 * D_8009AB08 - device block; the counter and the eight handles live at 0x8C8..0x8F0.
 *
 * @par Control flow
 * One pre-decrement test, then a fixed unrolled release sequence.
 *
 * @par Calls
 * func_8008C9C0/func_8008C9E0 - lock and unlock. func_8008C990 and
 * func_8008C970 - the two release stages.
 *
 * @par Side effects
 * Releases up to eight device handles.
 *
 * @par Callers
 * Likely: func_8002078C (same block).
 *
 * @par Role and path
 * Releases the memory-card BIOS event subsystem by decrementing
 * gOperationState->unk8C8 and, when it reaches 0, disabling and closing all
 * eight event handles (unk8D4..unk8F0) via func_8008C990 and func_8008C970
 * inside a critical section. Reads and writes D_8009AB08; counterpart to
 * func_80020120, likely called by func_8002078C.
 */
void gReleaseMcEvents(void)
{
    if (--gOperationState->unk8C8 == 0) {
        gDisableCpuInterrupts();
        gBiosDisableEvent(gOperationState->unk8D4);
        gBiosDisableEvent(gOperationState->unk8D8);
        gBiosDisableEvent(gOperationState->unk8DC);
        gBiosDisableEvent(gOperationState->unk8E0);
        gBiosDisableEvent(gOperationState->unk8E4);
        gBiosDisableEvent(gOperationState->unk8E8);
        gBiosDisableEvent(gOperationState->unk8EC);
        gBiosDisableEvent(gOperationState->unk8F0);
        gBiosCloseEvent(gOperationState->unk8D4);
        gBiosCloseEvent(gOperationState->unk8D8);
        gBiosCloseEvent(gOperationState->unk8DC);
        gBiosCloseEvent(gOperationState->unk8E0);
        gBiosCloseEvent(gOperationState->unk8E4);
        gBiosCloseEvent(gOperationState->unk8E8);
        gBiosCloseEvent(gOperationState->unk8EC);
        gBiosCloseEvent(gOperationState->unk8F0);
        gEnableCpuInterrupts();
    }
}
