// Categories: Input > Controller Protocol
#include "game/types.h"

struct PadPort {
    u8 pad_00[0x3C];
    u8 *response;
    u8 pad_40[6];
    u8 sequence_step;
    u8 info_record_index;
    u8 combination_bytes_remaining;
    u8 communication_state;
    u8 pad_4A[0x99];
    u8 mode_count;
    u8 current_mode_index;
    u8 pad_E5;
    u16 current_extended_id;
    u8 current_id;
    u8 actuator_count;
    u8 combination_count;
    u8 pad_EB;
    s32 combination_storage_bytes;
};

#define PAD_RESPONSE(port) \
    (*(u8 *volatile *)&(port)->response)

extern s32 func_8008B05C(struct PadPort *port);
extern s32 func_8008B094(struct PadPort *port, u8 *buffer);

/**
 *  @brief Processes a controller information response for the current sequence step.
 *
 *  Records mode and actuator information, an extended ID, or combination
 *  storage requirements in the port's protocol state.
 *  @param port Controller port state and response buffer pointer.
 *  @return Zero while combination processing continues or restarts; otherwise one.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Steps 2 and 3 record response fields; step 4 accumulates aligned record sizes.
 *  After the last combination, a size check selects an error state or a restart.
 *  @par Calls
 *  func_8008B05C checks the accumulated state; func_8008B094 receives the port
 *  and its internal buffer when the sequence is restarted.
 *  @par Side effects
 *  Updates port information, sequence and communication fields as applicable.
 *  @par Callers
 *  Confirmed: func_8008C518.
 */
s32 func_8008AF10(struct PadPort *port)
{
    u32 chunk;
    register u32 next __asm__("$2");
    u32 response_value;

    switch (port->sequence_step) {
    case 2:
        port->mode_count = PAD_RESPONSE(port)[3];
        port->current_mode_index = PAD_RESPONSE(port)[4];
        port->current_extended_id = 0;
        port->actuator_count = PAD_RESPONSE(port)[5];
        port->combination_count = PAD_RESPONSE(port)[6];
        port->combination_storage_bytes = 0;
        break;

    case 3:
        response_value = port->response[4];
        next = port->response[5];
        port->info_record_index = 0;
        port->current_extended_id = (response_value << 8) + next;
        break;

    case 4:
        chunk = port->combination_storage_bytes;
        next = port->info_record_index;
        response_value = port->response[4];
        next++;
        port->info_record_index = next;
        chunk += 8;
        chunk += (response_value + 3) & 0x1FC;
        port->combination_storage_bytes = chunk;
        if ((next & 0xFF) < port->combination_count) {
return_zero:
            return 0;
        }

        if (func_8008B05C(port) >= 0x81) {
            port->sequence_step = 0xFE;
            port->communication_state = 2;
            goto return_zero;
        }

        port->sequence_step = 0xFF;
        func_8008B094(port, (u8 *)port + 0x63);
        port->sequence_step = 2;
        goto return_zero;
    }

    return 1;
}
