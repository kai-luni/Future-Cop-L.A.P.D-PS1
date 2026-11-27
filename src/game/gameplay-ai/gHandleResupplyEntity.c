// Categories: Gameplay > Entity Script | Graphics > Model Renderer | Physics > Collision
#include "game/types.h"

typedef struct Object {
    u8 pad00[0x24];
    u32 flags;
    u8 pad28[0x8C];
    u32 unkB4;
    u8 padB8[2];
    s16 unkBA;
    u8 unkBC;
    u8 unkBD;
    u8 padBE[0x16];
    u8 unkD4[4];
} Object;

extern void *jtbl_800111DC[];
extern u32 gFrameTime __asm__("D_8009FAA4");
extern void gInitTerrainEntity(Object *, void *, s32) __asm__("func_800302E8");
extern u8 gTestEntityFieldMatch(u8, s16, void *, u8) __asm__("func_8002F8B8");
extern void gCheckResupplyPadProximity(Object *, void *) __asm__("func_80030498");
extern s32 gEntityBaseHandler(s32, Object *, void *) __asm__("func_8002AD58");
extern u8 gSubmitModelInstance(void *, u32 *, s32) __asm__("func_80017508");
extern s32 gUpdateResupplyEntity(Object *, void *) __asm__("func_800306A0");
s32 gHandleResupplyEntity(s32 message, Object *object, void *data, s32 arg3) __asm__("func_80030960");


/**
 * @brief Message handler of object type 32: spawn, conditional draw with a
 *        zoom-dependent flag, collision tick and the shared fallback.
 *
 * @param message Handler case (1 type, 2 spawn, 3 draw, 6 tick, 10 collision).
 * @param object Entity block; +0x24 flags, +0xB4 a state word, +0xBA an s16
 *        parameter, +0xBC/+0xBD two state bytes, +0xD4 the model instance.
 * @param data Spawn/collision payload.
 * @param arg3 Forwarded to the spawn helper.
 * @return Case 1: 0x134. Case 10: 1. Case 3: the draw result. Case 6: the
 *         tick result. Default: the shared hook func_8002AD58.
 *
 * @par Confirmed
 * Dispatch through jtbl_800111DC over message - 1 (10 entries). Case 2 runs
 * func_800302E8. Case 10 only acts when unkBD is 0 or 2: func_8002F8B8
 * (unkBC, unkBA, data, unkBD == 2) gates func_80030498. Case 3 requires
 * unkBD 2 or 5 and flag bit 0x20 clear, calls the shared hook, then submits
 * the model via func_80017508 with flags 0x2000000 when (unkB4 & 0xF0) and
 * (D_8009FAA4 & 0x80) are both set. Case 6 forwards to func_800306A0.
 *
 * @par Likely role
 * One of the numbered entity handlers registered by func_8002AE54 (case 32
 * returns this address): a collidable object whose draw flag follows the
 * map zoom bit 0x80.
 *
 * @par Unknown
 * What the type id 0x134 selects and the meaning of unkBD states 2/5.
 *
 * @par Callers
 * No direct callers in the call graph. func_8002AE54 returns the address
 * for object type 32, so the dispatcher reaches it indirectly.
 */
s32 gHandleResupplyEntity(s32 message, Object *object, void *data, s32 arg3)
{
    register Object *self __asm__("$16");
    register void *other __asm__("$17");
    u32 index;
    u32 flags;
    static void *const labels[] __attribute__((used)) = {
        &&case1, &&case2, &&case3, &&fallback, &&fallback,
        &&case6, &&fallback, &&fallback, &&fallback, &&case10
    };
    self = object;
    other = data;
    index = (u32)message - 1;
    if (index >= 10) goto fallback;
    goto *jtbl_800111DC[index];
case1:
    return 0x134;
case2:
    gInitTerrainEntity(self, other, arg3);
    return 0;
case10:
    if (self->unkBD == 2 || self->unkBD == 0) {
        if (gTestEntityFieldMatch(self->unkBC, self->unkBA, other, self->unkBD == 2)) {
            gCheckResupplyPadProximity(self, other);
        }
    }
    return 1;
case3:
    if (self->unkBD != 2 && self->unkBD != 5) goto fallback;
    if (self->flags & 0x20) return 0;
    gEntityBaseHandler(message, self, other);
    if ((self->unkB4 & 0xF0) && (gFrameTime & 0x80)) {
        flags = 0x2000000;
    } else {
        flags = 0;
    }
    return gSubmitModelInstance(self->unkD4, &flags, 1);
case6:
    return gUpdateResupplyEntity(self, other);
fallback:
    return gEntityBaseHandler(message, self, other);
}
