// Categories: Input > Controller Configuration
#include "game/types.h"

struct PadCapabilityRecord {
    u8 protocol[2];
    u8 payload_bytes;
    u8 activation_cost;
    u8 high_bit;
};

struct PadPort {
    u8 pad_00[4];
    struct PadCapabilityRecord *capabilities;
    u8 pad_08[0x18];
    u8 *requested_actuator_map;
    u8 pad_24[0x22];
    u8 sequence_step;
    u8 pad_47[0x16];
    u8 actuator_map[6];
    u8 pad_63[0x86];
    u8 actuator_count;
};
s32 gRespPadActAlign(struct PadPort *port) __asm__("func_8008B508");


/**
 *  @brief Validate requested actuator assignments and advance the port sequence.
 *
 *  Counts each actuator's occurrences in the six-byte requested map and writes
 *  matching output positions according to its capability payload size.
 *  A zero payload size is treated as one; insufficient assignments become 0xFF.
 *  Positions not matching an actuator are left unchanged.
 *  @param port Controller port containing the maps and capability records.
 *  @return Always 0.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Loops over actuators and scans all six map positions twice for each one.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates matching actuator map bytes and sets sequence_step to 0xFE.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * For each actuator up to actuator_count, counts occurrences in the 6-byte
 * requested_actuator_map and compares against
 * capabilities[actuator].payload_bytes (minimum 1), writing actuator or 0xFF
 * into port->actuator_map. Sets port->sequence_step to 0xFE and returns 0.
 * Installed as response_handler by func_8008B484.
 */
s32 gRespPadActAlign(struct PadPort *port)
{
    s32 actuator;
    s32 unused;
    u8 *cursor;
    s32 matched;
    s32 remaining;
    s32 written;
    s32 needed;
    register u8 *output __asm__("$6");
    register s32 limit;

    actuator = 0;
    if (port->actuator_count != 0) {
        unused = 0xFF;
        do {
            cursor = port->requested_actuator_map;
            matched = 0;
            remaining = 5;
            do {
                if (*cursor++ == actuator) {
                    matched++;
                }
                remaining--;
            } while (remaining >= 0);

            needed = port->capabilities[actuator].payload_bytes;
            cursor = port->requested_actuator_map;
            written = 0;
            if (needed == 0) {
                needed = 1;
            }
            output = (u8 *)port;
            do {
                if (*cursor++ == actuator) {
                    if (matched < needed) {
                        output[0x5D] = unused;
                        matched--;
                    } else {
                        output[0x5D] = actuator;
                    }
                }
                written++;
                output++;
            } while (written < 6);
            limit = port->actuator_count;
            actuator++;
        } while (actuator < limit);
    }
    port->sequence_step = 0xFE;
    return 0;
}
