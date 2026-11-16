// Categories: Input > Controller
#include "game/types.h"

struct PadPort;

extern struct PadPort *(*D_8009C73C)(s32 slot);

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
 */
s32 func_8008A1F8(s32 slot, u8 mode, u8 lock)
{
    return func_8008B5D0(D_8009C73C(slot), mode, lock);
}
