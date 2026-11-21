// Categories: Gameplay > Entity Script | Game Engine > Object Setup
#include "game/types.h"

struct Object2F7C8 {
    u8 pad[0xA2];
    u8 flags;
};

extern s32 gInitMoverObject(struct Object2F7C8 *, s32, s32) __asm__("func_8002EED8");
extern void gStepMoverObject(struct Object2F7C8 *, s32, s32) __asm__("func_8002F30C");
extern void gRemoveGroundKeyEntry(struct Object2F7C8 *) __asm__("func_80063194");
extern s32 gEntityBaseHandler(s32, struct Object2F7C8 *, s32, s32) __asm__("func_8002AD58");
s32 gHandleMoverEntityMsg(s32 msg, struct Object2F7C8 *obj, s32 arg2, s32 arg3) __asm__("func_8002F7C8");


/**
 * @brief Entity-type handler for script-driven moving props: size query, spawn, per-frame motion and ground-table unregister, with all other messages delegated to the base handler.
 *
 * @param msg Message id (1 size query, 2 spawn, 5 teardown, 6 motion update).
 * @param obj Entity block; the flag byte at +0xA2 gates the ground-table removal.
 * @param arg2 Case payload (spawn descriptor, or dt for the motion step).
 * @param arg3 Extra spawn argument.
 * @return 0xC4 for msg 1 (block size), the spawn result for msg 2, 0 for msg 6, otherwise the base handler's result.
 *
 * @par Control flow
 * Switch on msg; case 5 conditionally unregisters, then falls through to the default delegation (no break).
 *
 * @par Calls
 * func_8002EED8 - msg 2 spawn/setup from a descriptor (axis, vertical or two-angle types). func_8002F30C - msg 6 one-degree-of-freedom motion step with travel/stop sounds. func_80063194 - swap-delete of obj from the ground/interaction key table when flag bit 4 is set. func_8002AD58 - shared base handler for every other message.
 *
 * @par Side effects
 * Via callees: entity fields, matrix pushes, sounds, and the D_800A31A0/D_8009F838 ground table on msg 5.
 *
 * @par Callers
 * Confirmed: no direct callers in edges or listing. Likely: reached indirectly as an object-type func24 callback (msg 1 is the spawn queue's block-size query). Unknown: which type table installs it.
 */
s32 gHandleMoverEntityMsg(s32 msg, struct Object2F7C8 *obj, s32 arg2, s32 arg3)
{
    switch ((u32)msg) {
    case 1:
        return 0xC4;
    case 2:
        return gInitMoverObject(obj, arg2, arg3);
    case 6:
        gStepMoverObject(obj, arg2, arg3);
        return 0;
    case 5:
        if (obj->flags & 4) {
            gRemoveGroundKeyEntry(obj);
        }
    default:
        return gEntityBaseHandler(msg, obj, arg2, arg3);
    }
}
