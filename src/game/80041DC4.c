// Categories: Gameplay > Object State
#include "game/types.h"

struct Unk41DC4 {
    u8 unk00[0x16];
    u8 kind;
    u8 unk17[0x0D];
    u32 flags;
    u8 unk28[0x78];
    u8 stateA0;
    u8 unkA1[3];
    s16 valueA4;
};

extern s32 gStreamPreemptFlag __asm__("D_8009F9B0");

/**
 *  @brief Updates a value and flags on objects of kind 0x22.
 *
 *  For the matching kind, stores the value and increments a global counter.
 *  It also sets bit 3 of the object's flags and bit 0 of stateA0.
 *  @param object Object whose kind and state are accessed.
 *  @param value Signed value stored in valueA4 for a matching object.
 *  @par Globals
 *  D_8009F9B0 is incremented for each matching call.
 *  @par Control flow
 *  Branches on kind; other kinds are left unchanged.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the matching object and increments D_8009F9B0.
 *  @par Callers
 *  Confirmed: func_80029A60.
 */
void func_80041DC4(struct Unk41DC4 *object, s16 value)
{
    if (object->kind == 0x22) {
        object->valueA4 = value;
        gStreamPreemptFlag++;
        object->flags |= 8;
        object->stateA0 |= 1;
    }
}
