// Categories: Gameplay > Entity Timer | Game Engine > Script Hook
#include "game/types.h"

struct Unk3BObject {
    u8 unk00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 tableIndex;
    u8 unk21[3];
    u32 flags;
};

struct Unk2A940Source {
    u8 unk00[8];
    u32 unk08;
};

struct Unk2A940Arg2 {
    u32 unk00;
    u8 unk04[4];
    struct Unk2A940Source *unk08;
    u8 unk0C[2];
    u16 unk0E;
};

struct Unk2A940 {
    u8 unk00[0x1C];
    s16 unk1C;
    u8 unk1E[2];
    u8 unk20;
    u8 unk21[3];
    u32 unk24;
    u8 unk28[0x64];
    u32 unk8C;
    u8 unk90[4];
    void *unk94;
};

extern void func_8003BA50(struct Unk3BObject *object, s32 value);
extern void func_80069D68(void *arg0, s32 arg1, s32 arg2, struct Unk2A940 *arg3);

/**
 * @brief Decrements an entity countdown and reacts when it expires.
 *
 * Reads the step width from arg->unk0E and returns it unchanged when it is
 * zero or negative. Otherwise the countdown in object->unk1C is reduced by
 * that step. When the result goes negative the entity copies the pointer
 * value arg->unk08->unk08 into object->unk8C; while it stays positive and
 * the entity has an active target (object->unk20) without flag 0x40000 set,
 * func_8003BA50 is notified with the previous countdown value.
 *
 * @param object Entity state block with the counter at 0x1C and flags at 0x24.
 * @param arg Step descriptor: step width at 0x0E, selector at 0x00, payload at 0x08.
 * @return The step width that was applied (0 or negative when nothing changed).
 *
 * @par Globals
 * None directly; the entity block itself carries the state.
 *
 * @par Control flow
 * Single branch tree on the step width and on the sign of the new counter.
 *
 * @par Calls
 * func_8003BA50 - reports the old counter value to the target tracker.
 * func_80069D68 - runs the attached script with the sign of the counter.
 *
 * @par Side effects
 * Writes the counter, sets flag 0x02000000 in object->unk24, may write 0x8C.
 *
 * @par Callers
 * Confirmed: func_8002AD58 (base entity handler, message 0x0E).
 */
s32 func_8002A940(struct Unk2A940 *object, struct Unk2A940Arg2 *arg)
{
    s32 delta;
    s32 previous;
    u16 value;

    delta = arg->unk0E;
    if (delta <= 0) {
        return delta;
    }

    previous = object->unk1C;
    object->unk1C = (u16)object->unk1C - delta;

    if (object->unk1C < 0) {
        if (arg->unk08 != 0) {
            object->unk8C = arg->unk08->unk08;
        }
    } else {
        if (object->unk20 != 0 && (object->unk24 & 0x40000) == 0) {
            func_8003BA50((struct Unk3BObject *)object, previous);
        }
    }

    object->unk24 |= 0x02000000;
    if (object->unk94 != 0) {
        func_80069D68(object->unk94,
                      (u32)object->unk1C >> 31,
                      arg->unk00 & 0x7FFF,
                      object);
    }

    return delta;
}
