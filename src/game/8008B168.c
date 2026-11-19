// Categories: Input > DualShock | Platform > Controller
#include "game/types.h"

struct PadPort {
    u8 pad_00[0x46];
    u8 sequence_step;
    u8 info_record_index;
    u8 combination_bytes_remaining;
};

extern void gSetPadModeConstCmd(struct PadPort *port, u8 index) __asm__("func_8008B750");
extern void gSetPadActInfoCmd(struct PadPort *port, u8 index) __asm__("func_8008B770");
extern void gSetPadCombInfoCmd(struct PadPort *port, u8 index) __asm__("func_8008B790");
extern void gSetPadCombContCmd(struct PadPort *port) __asm__("func_8008B7B0");
void gCmdPadTableHandshake(struct PadPort *port) __asm__("func_8008B168");


/**
 * Command handler of the pad handshake, called once per sequence step.
 * Step 2 requests the mode table, step 3 the actuator capability list. Step 4
 * either starts the next combination record (func_8008B790) or continues the
 * record in flight (func_8008B7B0), depending on bytes still outstanding.
 *
 * @par Role and path
 * Dispatches on port->sequence_step: step 2 calls func_8008B750(port,
 * info_record_index), step 3 calls func_8008B770(port, info_record_index),
 * and step 4 calls func_8008B790 if combination_bytes_remaining is 0 or
 * func_8008B7B0 otherwise. Installed as command_handler by func_8008B094.
 */
void gCmdPadTableHandshake(struct PadPort *port)
{
    s32 state = port->sequence_step;

    switch (state) {
    case 2:
        gSetPadModeConstCmd(port, port->info_record_index);
        break;
    case 3:
        gSetPadActInfoCmd(port, port->info_record_index);
        break;
    case 4:
        if (port->combination_bytes_remaining == 0) {
            gSetPadCombInfoCmd(port, port->info_record_index);
        } else {
            gSetPadCombContCmd(port);
        }
        break;
    }
}
