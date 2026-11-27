// Categories: Platform > Dispatch
#include "game/types.h"

typedef struct {
    u8 pad0[0xC];
    void *unkC;
    u8 pad10[0x26];
    volatile u8 unk36;
} Unk8008B810;

extern u32 gPadSlotIndex __asm__("D_8009C768");
extern u32 gPadStartSlot __asm__("D_8009C778");
extern u32 gPadPollPreHookFlag __asm__("D_8009C764");
extern u32 gPadPollPreHookA __asm__("D_8009C758");
extern u32 gPadPollPreHookB __asm__("D_8009C754");
extern u32 gPadUpdateByte __asm__("D_8009C7A8");
extern u32 gPadDataOffset __asm__("D_8009C740");
extern s32 gTransferSio0Byte(Unk8008B810 *, u32) __asm__("func_8008AB80");
s32 gSendPadCmdHeader(Unk8008B810 *arg0) __asm__("func_8008B810");


/**
 *  @brief Runs configured callbacks and dispatches an object with a selected code.
 *
 *  Global conditions control two no-argument callbacks and two calls on
 *  object data; the object's volatile byte selects the final dispatch code.
 *  @param arg0 Object supplying the data pointer and optional dispatch code.
 *  @return Result returned by func_8008AB80.
 *  @par Globals
 *  D_8009C768 and D_8009C778 are compared; D_8009C764 enables callbacks
 *  D_8009C758 and D_8009C754; D_8009C7A8 enables callback D_8009C740.
 *  @par Control flow
 *  Calls the first pair when the comparison and enable check pass, then calls
 *  D_8009C740 on the data pointer and its 0xF0 offset when enabled.
 *  @par Calls
 *  func_8008AB80 receives the object and its nonzero byte, or 0x42 otherwise.
 *  @par Side effects
 *  Invokes configured callbacks and the final dispatch helper.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * When gPadSlotIndex equals D_8009C778 and D_8009C764 is nonzero, invokes
 * D_8009C758() and D_8009C754(). If gPadUpdateByte is nonzero, invokes
 * gPadDataOffset on arg0->unkC and arg0->unkC + 0xF0, then calls
 * func_8008AB80 with arg0->unk36 (or 0x42 if zero).
 */
s32 gSendPadCmdHeader(Unk8008B810 *arg0)
{
    register Unk8008B810 *a0 __asm__("$4");

    if (gPadSlotIndex == gPadStartSlot) {
        if (gPadPollPreHookFlag != 0) {
            ((void (*)(void))gPadPollPreHookA)();
            ((void (*)(void))gPadPollPreHookB)();
        }
    }
    if (gPadUpdateByte != 0) {
        ((void (*)(void *))gPadDataOffset)(arg0->unkC);
        ((void (*)(void *))gPadDataOffset)((u8 *)arg0->unkC + 0xF0);
    }
    a0 = arg0;
    if (arg0->unk36 != 0) {
        return gTransferSio0Byte(a0, arg0->unk36);
    }
    return gTransferSio0Byte(a0, 0x42);
}
