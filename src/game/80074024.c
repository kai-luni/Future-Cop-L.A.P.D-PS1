// Categories: Gameplay > Object Spawning | Graphics > Effects
#include "game/types.h"

struct Unk74024Def {
    u8 unk00[0x36];
    u16 width;
    u16 height;
    u8 unk3A[0x10];
    u16 flags;
};

struct Unk74024 {
    u8 unk00[8];
    s16 unk08;
    s16 unk0A;
    u8 unk0C[0x18];
    void (*update)();
    u8 unk28[0x3C];
    s32 unk64;
};

extern void gHandleBeamObjectMsg() __asm__("func_80073BBC");

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void gInitSpawnMotionState(struct Unk74024 *object, struct Unk74024Def *def) __asm__("func_8003C14C");
struct Unk74024 *gAllocRibbonBeamObject(struct Unk74024Def *def) __asm__("func_80074024");


/**
 * @brief Allocates and initializes a kind-0x58 ribbon/beam effect object.
 *
 * Allocates a 0x68-byte object from pool tag 1, initializes common motion/tuning fields
 * from def, attaches func_80073BBC as its message handler, and sets dimensions and flags.
 *
 * @param def Source definition record containing width (+0x36), height (+0x38), and flags (+0x4A).
 * @return Pointer to the new 0x68-byte object, or null if allocation fails.
 *
 * @par Globals
 * None directly.
 *
 * @par Control flow
 * Allocates 0x68 bytes; returns 0 on null, otherwise runs func_8003C14C and initializes fields.
 *
 * @par Calls
 * func_80017EBC - allocates 0x68 bytes with tag 1; func_8003C14C - initializes base fields from def.
 *
 * @par Side effects
 * Allocates heap memory, sets update = func_80073BBC, clears unk64/unk08, sets unk0A = width * height * 2, ORs 0x10 into def->flags.
 *
 * @par Callers
 * Confirmed: func_8003E984 (spawn constructor switch case 0x58).
 */
struct Unk74024 *gAllocRibbonBeamObject(struct Unk74024Def *def)
{
    struct Unk74024 *object;
    s32 size;

    object = gAllocMemory(0x68, 1);
    if (object == 0) {
        return 0;
    }
    gInitSpawnMotionState(object, def);
    object->update = gHandleBeamObjectMsg;
    object->unk64 = 0;
    size = def->height * def->width;
    def->flags |= 0x10;
    object->unk08 = 0;
    object->unk0A = size * 2;
    return object;
}
