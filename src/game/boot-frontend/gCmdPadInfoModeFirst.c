// Categories: Input > DualShock | Platform > Controller
#include "game/types.h"

struct PadPort {
    u8 pad_00[0x46];
    u8 sequence_step;
    u8 info_record_index;
    u8 pad_48[0x9C];
    u8 current_mode_index;
};

extern void gSetPadModelQueryCmd(struct PadPort *port) __asm__("func_8008B73C");
extern void gSetPadModeConstCmd(struct PadPort *port, u8 index) __asm__("func_8008B750");
extern void gSetPadCombInfoCmd(struct PadPort *port, u8 index) __asm__("func_8008B790");
void gCmdPadInfoModeFirst(struct PadPort *port) __asm__("func_8008AE8C");


/**
 * @brief Continues a pad handshake at its current step (mode-first variant).
 *
 * Steps 2, 3 and 4 are advanced: step 2 asks func_8008B73C for the mode probe,
 * step 3 queues the mode table request (0x4C) with the current mode index and
 * step 4 requests the next combination record (0x47) with the info record
 * index. Other steps do nothing.
 * @param port Pad port whose sequence_step drives the handshake.
 * @par Globals
 * None.
 * @par Control flow
 * Switch over the step.
 * @par Calls
 * func_8008B73C - mode probe. func_8008B750 - 0x4C request. func_8008B790 - 0x47 request.
 * @par Side effects
 * Queues the next DualShock command.
 * @par Callers
 * Unknown: no direct edge; driven by the pad polling loop.
 *
 * @par Role and path
 * Switches on port->sequence_step: step 2 calls func_8008B73C(port), step 3
 * calls func_8008B750(port, port->current_mode_index), and step 4 calls
 * func_8008B790(port, port->info_record_index). Driven indirectly by the pad
 * polling loop.
 */
void gCmdPadInfoModeFirst(struct PadPort *port)
{
    s32 state = port->sequence_step;

    switch (state) {
    case 2:
        gSetPadModelQueryCmd(port);
        break;
    case 3:
        gSetPadModeConstCmd(port, port->current_mode_index);
        break;
    case 4:
        gSetPadCombInfoCmd(port, port->info_record_index);
        break;
    }
}
