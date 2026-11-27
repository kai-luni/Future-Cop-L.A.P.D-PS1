// Categories: Gameplay > Entity Script | Graphics > Model Renderer
#include "game/types.h"

typedef struct {
    u8 pad00[0x24];
    u32 flags;
    u8 pad28[0x78];
    u8 unkA0;
} Object;

extern s32 gEntityBaseHandler(s32 message, Object *object, void *data) __asm__("func_8002AD58");
extern s32 gInitPortraitScreenEntity(s32 message, Object *object, void *data) __asm__("func_80041E0C");
extern void gDrawPortraitScreen(Object *object) __asm__("func_80042128");
extern void gStepPortraitScreen(Object *object, void *data) __asm__("func_8004228C");
s32 gHandlePortraitScreenMsg(s32 message, Object *object, void *data) __asm__("func_80042330");


/**
 * @brief Message handler of object type 34: spawn, conditional draw and tick
 *        over the shared case numbers.
 *
 * @param message Handler case (1 type, 2 spawn, 3 draw, 6 tick, rest idle).
 * @param object Entity block; +0x24 the flag word, +0xA0 a state byte.
 * @param data Spawn/tick payload.
 * @return Case 1: 0x120. Case 3: 1 when drawn. Default: the result of the
 *         shared hook func_8002AD58. All other cases: 0.
 *
 * @par Confirmed
 * Case 2 forwards to func_80041E0C. Case 3 draws via func_80042128 only when
 * state bit 0 is set and flag bit 0x20 is clear. Case 6 runs func_8004228C
 * when state bit 0 is set. Cases 4/5/7/8/9/10/11/13/14 return 0; everything
 * else falls through to func_8002AD58.
 *
 * @par Likely role
 * One of the numbered entity handlers registered by the factory
 * func_8002AE54 (case 34 returns this address): a visible object with a
 * flag-gated draw and a state-gated tick.
 *
 * @par Unknown
 * What the type id 0x120 selects in the spawn tables.
 *
 * @par Callers
 * No direct callers in the call graph. func_8002AE54 returns the address
 * for object type 34, so the dispatcher reaches it indirectly.
 */
s32 gHandlePortraitScreenMsg(s32 message, Object *object, void *data)
{
    switch (message) {
    case 1:
        return 0x120;
    case 2:
        return gInitPortraitScreenEntity(message, object, data);
    case 3:
        if (!(object->unkA0 & 1)) {
            return 0;
        }
        if (object->flags & 0x20) {
            return 0;
        }
        gDrawPortraitScreen(object);
        return 1;
    case 6:
        if (object->unkA0 & 1) {
            gStepPortraitScreen(object, data);
        }
        return 0;
    case 4:
    case 5:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 13:
    case 14:
        return 0;
    default:
        return gEntityBaseHandler(message, object, data);
    }
}
