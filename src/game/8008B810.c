// Categories: Platform > Dispatch
#include "game/types.h"

typedef struct {
    u8 pad0[0xC];
    void *unkC;
    u8 pad10[0x26];
    volatile u8 unk36;
} Unk8008B810;

extern u32 gPadSlotIndex __asm__("D_8009C768");
extern u32 D_8009C778;
extern u32 D_8009C764;
extern u32 D_8009C758;
extern u32 D_8009C754;
extern u32 D_8009C7A8;
extern u32 gPadDataOffset __asm__("D_8009C740");
extern s32 func_8008AB80(Unk8008B810 *, u32);

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
 */
s32 func_8008B810(Unk8008B810 *arg0)
{
    register Unk8008B810 *a0 __asm__("$4");

    if (gPadSlotIndex == D_8009C778) {
        if (D_8009C764 != 0) {
            ((void (*)(void))D_8009C758)();
            ((void (*)(void))D_8009C754)();
        }
    }
    if (D_8009C7A8 != 0) {
        ((void (*)(void *))gPadDataOffset)(arg0->unkC);
        ((void (*)(void *))gPadDataOffset)((u8 *)arg0->unkC + 0xF0);
    }
    a0 = arg0;
    if (arg0->unk36 != 0) {
        return func_8008AB80(a0, arg0->unk36);
    }
    return func_8008AB80(a0, 0x42);
}
