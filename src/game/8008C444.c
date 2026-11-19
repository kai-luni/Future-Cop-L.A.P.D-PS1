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

extern void gSetPadConfigModeCmd(struct CommandState *state, u8 value) __asm__("func_8008B71C");
extern void gCmdPadInfoModeFirst(struct PadPort *port) __asm__("func_8008AE8C");
s32 gDispatchPadStep(struct C444Layout *state) __asm__("func_8008C444");


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
 *
 * @par Role and path
 * Installed into gPadDataOffset (D_8009C740) by func_8008C410 to dispatch a
 * pad sequence step. Calls func_8008B71C(state, 0) if ptr_3C[0] is 0xF3 and
 * field_E8 is 0; otherwise steps 0 and 0xFF do nothing, step 1 calls
 * func_8008B71C(state, 1), step 0xFE calls func_8008B71C(state, 0), and other
 * steps invoke state->callback_14 or fall back to func_8008AE8C. Always
 * returns 0.
 */
s32 gDispatchPadStep(struct C444Layout *state)
{
    if (state->ptr_3C[0] == 0xF3 && state->field_E8 == 0) {
        gSetPadConfigModeCmd((struct CommandState *)state, 0);
    } else {
        s32 step = state->sequence_step;

        switch (step) {
        case 0:
            break;
        case 1:
            gSetPadConfigModeCmd((struct CommandState *)state, 1);
            break;
        case 0xFE:
            gSetPadConfigModeCmd((struct CommandState *)state, 0);
            break;
        case 0xFF:
            break;
        default:
            if (state->callback_14 != 0) {
                state->callback_14(state);
            } else {
                gCmdPadInfoModeFirst((struct PadPort *)state);
            }
            break;
        }
    }

    return 0;
}
