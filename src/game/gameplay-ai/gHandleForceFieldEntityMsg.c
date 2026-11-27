// Categories: Gameplay > Entity Script | Physics > Force Fields
#include "game/types.h"

extern void gInitForceFieldEntity(s32 message, void *object, void *data) __asm__("func_80039E8C");
extern void gUpdateForceField(void *object, void *data) __asm__("func_8003A160");
extern s32 gEntityBaseHandler(s32 message, void *object, void *data) __asm__("func_8002AD58");
s32 gHandleForceFieldEntityMsg(u32 message, void *object, void *data) __asm__("func_8003A844");


/**
 * @brief Message table of the force-field entity class: block size, spawn,
 *        per-tick update and base fallback.
 *
 * The counterpart of func_8003190C for the field class. Message 1 answers the block
 * size, message 2 runs the spawn half func_80039E8C - which forwards to the base
 * handler, snaps the object to the ground and copies the field parameters out of the
 * definition - and message 6 runs the per-frame tick. Every other message goes to
 * the base handler.
 *
 * @param message Handler case number.
 * @param object Field object; +0x50 position, +0x134..+0x144 field limits.
 * @param data Definition/payload; the spawn case reads record fields through it.
 * @return 0x148 for case 1, 0 for cases 2 and 6, otherwise the base handler's result.
 *
 * @par Globals
 * None; all state is reached through the object.
 *
 * @par Control flow
 * Three-case switch with a default; cases 2 and 6 call one helper and return 0.
 *
 * @par Calls
 * func_80039E8C - spawn/initialise: base case 2, ground snap, field limits, phase
 * timers.
 * func_8003A160 - the per-tick force accumulation and velocity injection.
 * func_8002AD58 - base class for all remaining messages.
 *
 * @par Side effects
 * Only through the callees; this function writes nothing itself.
 *
 * @par Callers
 * Unknown: no direct jal. Confirmed reference: the base dispatcher func_8002AD58
 * returns this address (lui/addiu at 0x8002AF58) in its handler-selection case,
 * which is how the class is chained.
 */
s32 gHandleForceFieldEntityMsg(u32 message, void *object, void *data)
{
    switch (message) {
    case 1:
        return 0x148;
    case 2:
        gInitForceFieldEntity(2, object, data);
        return 0;
    case 6:
        gUpdateForceField(object, data);
        return 0;
    default:
        return gEntityBaseHandler(message, object, data);
    }
}
