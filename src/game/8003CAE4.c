// Categories: Game Engine > Object State
#include "game/types.h"

struct Unk3CAE4;
struct Unk3CA80;

struct Unk3CAE4Sub1 {
    u8 pad00[0x34];
    void *unk34;
    u8 unk38;
    u8 unk39;
};

struct Unk3CAE4Sub2 {
    u8 pad00[0x14];
    u8 unk14;
    u8 pad15[0x15];
    s16 unk2A;
};

struct Unk3CAE4 {
    u8 unk00[4];
    s16 unk04;
    u8 unk06;
    s8 unk07;
    u8 unk08[4];
    s16 unk0C;
    u8 unk0E;
    u8 flags;
    u8 unk10;
    u8 unk11[3];
    s16 unk14;
    u8 unk16[2];
    s16 unk18;
    u16 unk1A;
    struct Unk3CAE4Sub1 *unk1C;
    struct Unk3CAE4Sub2 *unk20;
    s32 (*callback)(s32 message, struct Unk3CAE4 *object, s32 arg2, s32 arg3);
};

extern void gRetireSpawnSlot(struct Unk3CA80 *object) __asm__("func_8003CA80");
s32 gStepSpawnNodeTimers(struct Unk3CAE4 *object, s32 arg1, u8 *flag) __asm__("func_8003CAE4");


/**
 *  @brief Updates an object's timers, flags, and pending event value.
 *
 *  Advances several countdowns, dispatches callbacks on selected expirations,
 *  and handles a condition on the object's linked state.
 *  @param object Object whose timers and state are updated.
 *  @param arg1 Amount subtracted from active timers.
 *  @param flag In/out event byte, set or cleared by applicable branches.
 *  @return 1 if the message-1 callback does not return 1; otherwise 0.
 *  @par Globals
 *  None directly referenced.
 *  @par Control flow
 *  Conditional timer branches update flags and invoke callbacks; a linked-state
 *  check can clear the event byte and invoke object handling.
 *  @par Calls
 *  The object callback receives messages 1 and 8; func_8003CA80 handles the
 *  qualifying linked-state condition.
 *  @par Side effects
 *  Modifies object fields and the event byte and invokes callbacks.
 *  @par Callers
 *  Confirmed: func_8003F2AC.
 */
s32 gStepSpawnNodeTimers(struct Unk3CAE4 *object, s32 arg1, u8 *flag)
{
    s32 result;

    result = 0;
    if (!(object->unk1A & 0x800) && !(object->flags & 2) &&
        (object->unk04 >= 0)) {
        object->unk04 -= arg1;
        if (object->unk04 <= 0) {
            if (object->unk06 != 0) {
                if (object->unk18 > 0) {
                    object->unk18--;
                    if (object->unk18 <= 0) {
                        goto unk06_done;
                    }
                }
                *flag = object->unk06;
                object->unk06 = 0;
unk06_done:
                ;
            }
            object->flags |= 2;
        }
    }

    if ((object->flags & 4) && (*flag == 0)) {
        if (object->callback(1, object, 0, 0) != 1) {
            *flag = object->unk20->unk14;
            result = 1;
        }
        object->flags &= ~4;
    }

    if (object->unk14 > 0) {
        object->unk14 -= arg1;
        if (object->unk14 <= 0) {
            object->unk14 = (object->unk20->unk2A * 75U) >> 10;

            if (object->unk07 > 0) {
                object->unk07--;
                if (object->unk07 == 0) {
                    object->unk07 = -1;
                    if (object->unk1A & 0x800) {
                        *flag = object->unk06;
                        object->unk06 = 0;
                        object->unk04 = -1;
                        object->flags |= 2;
                    }
                }
            }

            if (object->unk07 >= 0) {
                object->flags |= 4;
            }
        }
    }

    if (object->unk0C > 0) {
        object->unk0C -= arg1;
        if (object->unk0C <= 0) {
            object->callback(8, object, 0, 0);
        }
    }

    if (object->unk1C != 0 && object->unk1C->unk34 == 0 &&
        (object->unk1C->unk39 & 1)) {
        if (object->unk1A & 0x1000) {
            *flag = 0;
            gRetireSpawnSlot((struct Unk3CA80 *)object);
            object->flags |= 1;
        } else if (object->unk10 & 4) {
            *flag = 0;
            gRetireSpawnSlot((struct Unk3CA80 *)object);
        }
    }

    return result;
}
