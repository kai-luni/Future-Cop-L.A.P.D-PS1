// Categories: Input > Controller Protocol
#include "game/types.h"

struct C444Layout {
    u8 pad_00[0x14];
    void (*callback_14)(struct C444Layout *);
    u8 pad_18[0x24];
    u8 *ptr_3C;
    u8 pad_40[0x6];
    u8 sequence_step;
    u8 pad_47[0xA1];
    u8 field_E8;
};

struct PadPort;
struct CommandState;

extern void func_8008B71C(struct CommandState *state, u8 value);
extern void func_8008AE8C(struct PadPort *port);

/**
 *  @brief Dispatches a controller command sequence step.
 *
 *  The first byte of the pointed-to data and a state field can force a
 *  zero command before the sequence step is considered.
 *  @param state Controller state containing the sequence and optional callback.
 *  @return Always zero.
 *  @par Globals
 *  None directly referenced.
 *  @par Control flow
 *  Steps 1 and 0xFE send commands; 0 and 0xFF do nothing.
 *  Other steps invoke the callback if present, otherwise the fallback handler.
 *  @par Calls
 *  func_8008B71C sends a command value; func_8008AE8C handles the fallback.
 *  The callback stored in state is invoked for other steps when present.
 *  @par Side effects
 *  Called handlers or the callback may modify state or perform I/O.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_8008C444(struct C444Layout *state)
{
    if (state->ptr_3C[0] == 0xF3 && state->field_E8 == 0) {
        func_8008B71C((struct CommandState *)state, 0);
    } else {
        s32 step = state->sequence_step;

        switch (step) {
        case 0:
            break;
        case 1:
            func_8008B71C((struct CommandState *)state, 1);
            break;
        case 0xFE:
            func_8008B71C((struct CommandState *)state, 0);
            break;
        case 0xFF:
            break;
        default:
            if (state->callback_14 != 0) {
                state->callback_14(state);
            } else {
                func_8008AE8C((struct PadPort *)state);
            }
            break;
        }
    }

    return 0;
}
