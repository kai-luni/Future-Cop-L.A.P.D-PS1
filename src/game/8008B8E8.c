// Categories: Platform > State Initialization
#include "game/types.h"

typedef struct {
    u8 pad0[0xC];
    void *unkC;
    u8 pad10[0x26];
    u8 unk36;
} Unk8008B8E8;

extern u32 gPadUpdateByte __asm__("D_8009C7A8");
extern u32 gPadDataOffset __asm__("D_8009C740");
extern u32 D_8009C774;
extern u32 gPadMotorValue __asm__("D_8009C7A0");
extern s32 func_8008AB80(Unk8008B8E8 *, void *);

/**
 *  @brief Processes an object and derives a shared value from the result.
 *
 *  Optional callbacks operate on two offsets within the object's referenced data.
 *  A global pointer is passed to processing only when the object's byte is zero.
 *  @param arg0 Object containing the data pointer and selection byte.
 *  @return A negative processing result, -9 for a missing high nibble, or zero.
 *  @par Globals
 *  D_8009C7A8 enables calls through D_8009C740; D_8009C774 supplies optional data.
 *  D_8009C7A0 receives twice the result's low nibble, or 0x20 if that is zero.
 *  @par Control flow
 *  Optionally invokes two callbacks, then checks the processing result and nibbles.
 *  @par Calls
 *  func_8008AB80 processes the object; D_8009C740 is called at offsets 0x1E0 and 0x2D0.
 *  @par Side effects
 *  May invoke callbacks and updates D_8009C7A0 on success.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_8008B8E8(Unk8008B8E8 *arg0)
{
    s32 ret;
    void *a1;

    if (gPadUpdateByte != 0) {
        ((void (*)(void *))gPadDataOffset)((u8 *)arg0->unkC + 0x1E0);
        ((void (*)(void *))gPadDataOffset)((u8 *)arg0->unkC + 0x2D0);
    }
    a1 = 0;
    if (arg0->unk36 == 0) {
        a1 = (void *)D_8009C774;
    }
    ret = func_8008AB80(arg0, a1);
    if (ret < 0) {
        return ret;
    }
    if ((ret & 0xF0) == 0) {
        return -9;
    }
    gPadMotorValue = (ret & 0xF) << 1;
    if (gPadMotorValue == 0) {
        gPadMotorValue = 0x20;
    }
    return 0;
}