// Categories: Input > Controller Ports
#include "game/types.h"

struct PadPort;

extern struct PadPort *(*gPadLookupPortFn)(s32 slot) __asm__("D_8009C73C");
extern s32 gStartPadActAlign(struct PadPort *port, u8 *command) __asm__("func_8008B484");
s32 gSetPadActuatorAlign(s32 slot, u8 *command) __asm__("func_8008A1C0");


/**
 *  @brief Passes a slot's pad port and a command to the pad routine.
 *
 *  Resolves the slot through the configured port lookup function pointer.
 *  The resulting port and the original command are forwarded unchanged.
 *  @param slot Slot passed to the port lookup function.
 *  @param command Command buffer passed to the pad routine.
 *  @return The result returned by func_8008B484.
 *  @par Globals
 *  D_8009C73C supplies the port lookup function pointer.
 *  @par Control flow
 *  Calls the lookup, then calls the pad routine and returns its result.
 *  @par Calls
 *  D_8009C73C resolves the port; func_8008B484 handles the command.
 *  @par Side effects
 *  Any effects are those of the lookup and pad routines.
 *  @par Callers
 *  Confirmed: func_8001EC94.
 *
 * @par Role and path
 * Resolves the controller port for slot via D_8009C73C(slot) and forwards the
 * port and command buffer to func_8008B484. Returns the result from
 * func_8008B484. Called by func_8001EC94.
 */
s32 gSetPadActuatorAlign(s32 slot, u8 *command)
{
    return gStartPadActAlign(gPadLookupPortFn(slot), command);
}
