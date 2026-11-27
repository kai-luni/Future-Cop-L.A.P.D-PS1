// Categories: Input > Controller
#include "game/types.h"

struct PadPort;

extern struct PadPort *(*gPadLookupPortFn)(s32 slot) __asm__("D_8009C73C");
extern s32 gStartPadModeSelect(struct PadPort *port, s32 mode, s32 lock) __asm__("func_8008B5D0");
s32 gSetPadMainMode(s32 slot, u8 mode, u8 lock) __asm__("func_8008A1F8");


/**
 *  @brief Resolves a controller slot and forwards its port, mode, and lock values.
 *
 *  The port lookup is dispatched through a global function pointer before the
 *  resulting port is passed to the controller operation.
 *  @param slot Slot passed to the port lookup.
 *  @param mode Mode value forwarded to the controller operation.
 *  @param lock Lock value forwarded to the controller operation.
 *  @return Result returned by the controller operation.
 *  @par Globals
 *  D_8009C73C supplies the slot-to-port lookup function.
 *  @par Control flow
 *  Performs the lookup and forwards its result without local validation.
 *  @par Calls
 *  D_8009C73C resolves the port; func_8008B5D0 performs the operation.
 *  @par Side effects
 *  Any effects are those of the dispatched lookup and controller operation.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Resolves the controller port for slot via D_8009C73C(slot) and forwards the
 * port, mode, and lock arguments to func_8008B5D0. Returns the result from
 * func_8008B5D0. No direct callers are present in the codebase.
 */
s32 gSetPadMainMode(s32 slot, u8 mode, u8 lock)
{
    return gStartPadModeSelect(gPadLookupPortFn(slot), mode, lock);
}
