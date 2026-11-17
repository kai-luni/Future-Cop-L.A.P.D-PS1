// Categories: Gameplay > Entity Script | Graphics > Model Renderer
#include "game/types.h"

struct G59ABC {
    u8 pad00[0x5C];
    s32 unk5C;
};

struct C59ABC {
    u8 pad00[0x24];
    u32 flags;
};

struct O59ABC {
    u8 pad00[0xB4];
    struct C59ABC *unkB4;
};

extern struct G59ABC *gGameSession __asm__("D_8009FA94");

extern s32 func_8002AD58(s32 message, struct O59ABC *object, s32 data);
extern s32 func_800584BC(s32 message, struct C59ABC *child, s32 data, s32 arg3, s32 arg4);
extern void func_8005920C(struct O59ABC *object, s32 a, s32 b);
extern void func_800597C0(struct O59ABC *object);

/**
 * @brief Message handler of the kind-0x12 spawned class: spawn, conditional
 *        draw, link query and child forwarding.
 *
 * @param message Handler case (1 type, 2 spawn, 3 draw, 4/6/9 idle, 10 link
 *        query, 11/14 child cases).
 * @param object Entity block; +0xB4 the linked child block.
 * @param data Spawn payload / compared pointer.
 * @param arg3 Forwarded to spawn and child cases.
 * @return Case 1: 0xB8. Case 3: 5 when drawn. Case 10: 1 when the child
 *         pointer differs from data. Cases 2/4/6/9: 0. Default: the shared
 *         hook func_8002AD58.
 *
 * @par Confirmed
 * Case 2 runs func_8005920C(object, data, arg3) and falls through to 0.
 * Case 3 draws via func_800597C0 unless the child's flag bit 0x20 is set.
 * Cases 11/14 forward to func_800584BC(message, child, data, arg3, 1).
 * Case 4 returns 0 when D_8009FA94->unk5C == 1, otherwise falls through to
 * the shared hook.
 *
 * @par Likely role
 * The handler queued by func_80057914 for kind-0x12 requests: a companion
 * object that draws through its parent's model and forwards damage cases
 * to the child handler.
 *
 * @par Unknown
 * What the type id 0xB8 selects and the meaning of the unk5C == 1 gate.
 *
 * @par Callers
 * No direct callers in the call graph. func_80057914 stores the address as
 * request.handler, so the spawn dispatcher reaches it indirectly.
 */
s32 func_80059ABC(s32 message, struct O59ABC *object, s32 data, s32 arg3)
{
    switch (message) {
    case 1:
        return 0xB8;
    case 2:
        func_8005920C(object, data, arg3);
    case 6:
    case 9:
zero:
        return 0;
    case 10:
        return (s32)object->unkB4 != data;
    case 3:
        if (object->unkB4->flags & 0x20) {
            goto zero;
        }
        func_800597C0(object);
        return 5;
    case 11:
    case 14:
        return func_800584BC(message, object->unkB4, data, arg3, 1);
    case 4:
        if (gGameSession->unk5C == 1) {
            goto zero;
        }
    default:
        return func_8002AD58(message, object, data);
    }
}
