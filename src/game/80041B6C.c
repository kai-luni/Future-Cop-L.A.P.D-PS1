// Categories: Gameplay > Entity Script | Graphics > Model Renderer
#include "game/types.h"

struct Obj41B6C {
    u8 pad[0xC4];
    s16 count;
};

extern s32 func_8002AD58(s32 a0, struct Obj41B6C *a1, s32 a2, s32 a3);
extern void func_80041488(struct Obj41B6C *a0, s32 a1, s32 a2);
extern s32 func_80041880(struct Obj41B6C *a0, s32 a1);
extern void func_80063194(struct Obj41B6C *a0);

/**
 * @brief Entity-type 10 handler: waypoint-patrol objects with a capped hit counter; spawn, motion and teardown are specialised, everything else goes to the base handler.
 *
 * @param msg Message id (1 size, 2 spawn, 5 teardown, 6 motion, 11 hit).
 * @param obj Entity block; the s16 hit counter lives at +0xC4.
 * @param a2 Case payload (script record for spawn, dt for motion).
 * @param a3 Forwarded to the base handler.
 * @return 0xD0 for msg 1 (block size), the motion result for msg 6, 0 for msgs 2/5/11, otherwise the base handler's result.
 *
 * @par Control flow
 * Switch on msg; msgs 5 and 11 run their own step and then call the base handler explicitly before returning 0 (no fall-through).
 *
 * @par Calls
 * func_80041488 - msg 2 waypoint setup from the script record (positions, speeds, ground snap). func_80063194 - msg 5 swap-delete from the ground/interaction key table. func_80041880 - msg 6 advance along the waypoint sequence (ping-pong, dwell timers, segment events). func_8002AD58 - base handler for the remaining cases and the post-steps of 5/11.
 *
 * @par Side effects
 * Increments the hit counter up to 20 on msg 11; via callees: entity fields, matrices, sounds and the ground table.
 *
 * @par Callers
 * Confirmed: no direct calls; the address is taken at 0x8002AED0 inside func_8002AE54, the entity-type dispatcher (jump-table index 9 = type id 10). The package work list names a sibling handler and a non-existent unit as callers; neither calls this function.
 */
s32 func_80041B6C(s32 msg, struct Obj41B6C *obj, s32 a2, s32 a3)
{
    switch (msg) {
    case 1:
        return 0xD0;
    case 2:
        func_80041488(obj, a2, a3);
        return 0;
    case 5:
        func_80063194(obj);
        func_8002AD58(msg, obj, a2, a3);
        return 0;
    case 11:
        if (obj->count < 20) {
            obj->count++;
        }
        func_8002AD58(msg, obj, a2, a3);
        return 0;
    case 6:
        return func_80041880(obj, a2);
    default:
        return func_8002AD58(msg, obj, a2, a3);
    }
}
