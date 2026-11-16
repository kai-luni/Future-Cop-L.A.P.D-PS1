// Categories: Input > DualShock | Platform > Controller
#include "game/types.h"

struct PadPort {
    u8 pad_00[0x46];
    u8 sequence_step;
    u8 info_record_index;
    u8 pad_48[0x9C];
    u8 current_mode_index;
};

extern void func_8008B73C(struct PadPort *port);
extern void func_8008B750(struct PadPort *port, u8 index);
extern void func_8008B790(struct PadPort *port, u8 index);

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
 */
void func_8008AE8C(struct PadPort *port)
{
    s32 state = port->sequence_step;

    switch (state) {
    case 2:
        func_8008B73C(port);
        break;
    case 3:
        func_8008B750(port, port->current_mode_index);
        break;
    case 4:
        func_8008B790(port, port->info_record_index);
        break;
    }
}
