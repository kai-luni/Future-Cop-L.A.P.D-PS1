// Categories: Input > Controller Protocol
#include "game/types.h"

struct PadPort {
    u8 pad_00[0x46];
    u8 sequence_step;
    u8 pad_47[0xC];
    u8 mode_already_selected;
};

extern void (*D_8009C72C)(struct PadPort *port);

/**
 *  @brief Checks or advances a controller mode-selection sequence.
 *
 *  Uses the port's mode-selection flag and sequence step to report completion.
 *  If no mode is selected, it delegates further processing to a callback.
 *  @param port Controller port state to inspect or update.
 *  @return One if the mode is selected and the step is 2; otherwise zero.
 *  @par Globals
 *  D_8009C72C supplies the callback used when no mode is selected.
 *  @par Control flow
 *  A selected mode at another step sets the step to 0xFE; otherwise it dispatches.
 *  @par Calls
 *  The D_8009C72C callback processes the port when selection is pending.
 *  @par Side effects
 *  May change the sequence step or invoke a callback with additional effects.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_8008B6BC(struct PadPort *port)
{
    if (port->mode_already_selected != 0) {
        if (port->sequence_step == 2) {
            return 1;
        }
        port->sequence_step = 0xFE;
        return 0;
    }

    D_8009C72C(port);
    return 0;
}
