// Categories: Game Engine > State
#include "game/types.h"

struct Unk8009AB08 {
    u8 unk0000[0x8F8];
    s32 unk8F8;
    u8 unk8FC[0x10];
    s32 unk90C;
    s32 unk910;
    u8 unk914[0x80];
    s32 unk994;
    s32 unk998;
    s32 unk99C;
    u8 unk9A0[4];
    s32 unk9A4;
    u8 unk9A8[0x120C];
    s32 unk1BB4;
};

extern struct Unk8009AB08 *gOperationState __asm__("D_8009AB08");
extern s32 func_8001F180(u32 arg0, u32 arg1, u32 arg2, u32 arg3);

/**
 *  @brief Initializes a mode-2 operation when its arguments pass a check.
 *
 *  The four values are forwarded to a shared check and, on success, stored
 *  in the shared operation state for subsequent processing.
 *  @param arg0 First value, stored at offset 0x998 on success.
 *  @param arg1 Second value, stored at offset 0x99C on success.
 *  @param arg2 Third value, stored at offset 0x910 on success.
 *  @param arg3 Fourth value, stored at offset 0x90C on success.
 *  @par Globals
 *  D_8009AB08 points to the operation state and its status fields.
 *  @par Control flow
 *  A zero check result initializes mode 2; a nonzero result sets the error flag.
 *  @par Calls
 *  func_8001F180 checks the four supplied values.
 *  @par Side effects
 *  Updates the shared state fields or sets its field at offset 0x9A4 to 1.
 *  @par Callers
 *  Confirmed: func_80020D3C.
 */
void func_8001F1B8(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
{
    volatile struct Unk8009AB08 *p;

    if (func_8001F180(arg0, arg1, arg2, arg3) == 0) {
        p = gOperationState;
        p->unk998 = arg0;
        p->unk99C = arg1;
        p->unk910 = arg2;
        p->unk90C = arg3;
        p->unk994 = 0;
        p->unk9A4 = 0;
        p->unk8F8 = 2;
    } else {
        gOperationState->unk9A4 = 1;
    }
}
