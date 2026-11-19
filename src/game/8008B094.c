// Categories: Input > Controller Configuration
#include "game/types.h"

struct PadPort;

struct PadCapabilityRecord {
    u8 protocol[2];
    u8 payload_bytes;
    u8 activation_cost;
    u8 high_bit;
};

struct PadCombinationRecord {
    u8 length;
    u8 reserved[3];
    u8 *data;
};

struct PadPort {
    u16 *mode_table;
    struct PadCapabilityRecord *capabilities;
    struct PadCombinationRecord *combinations;
    u8 pad_0C[8];
    void (*command_handler)(void *);
    s32 (*response_handler)(struct PadPort *);
    u8 pad_1C[0x2A];
    u8 sequence_step;
    u8 info_record_index;
    u8 pad_48;
    u8 communication_state;
    u8 pad_4A[0x99];
    u8 mode_count;
    u8 pad_E4[5];
    u8 actuator_count;
};

extern  s32 (*gReadyCheckHook)(void) __asm__("D_8009C744");
extern void gCmdPadTableHandshake(struct PadPort *port) __asm__("func_8008B168");
extern s32 gRespPadTableHandshake(struct PadPort *port) __asm__("func_8008B210");
s32 gInitPadHandshake(struct PadPort *port, u8 *buffer) __asm__("func_8008B094");


/**
 *  @brief Arm a controller port's configuration handshake using a buffer.
 *
 *  Aligns the buffer and partitions it into mode, capability, and combination
 *  tables, then installs the command and response handlers for the handshake.
 *  @param port Controller port to initialize.
 *  @param buffer Storage for the tables, or null to decline initialization.
 *  @return 1 if armed; 0 if the buffer is null, capabilities already exist,
 *  or the readiness callback reports a nonzero result.
 *  @par Globals
 *  D_8009C744 is the readiness callback.
 *  @par Control flow
 *  Rejects invalid or unavailable setup before calculating table addresses.
 *  @par Calls
 *  D_8009C744 checks readiness; func_8008B168 and func_8008B210 are installed
 *  as handlers rather than called here.
 *  @par Side effects
 *  Sets port state, sequence, handlers, table pointers, and record index.
 *  @par Callers
 *  Confirmed: func_8008AF10.
 *
 * @par Role and path
 * Returns 0 if buffer is null, port->capabilities is already set, or
 * gReadyCheckHook() returns nonzero. Otherwise sets communication_state to 4
 * and sequence_step to 1, installs func_8008B168 and func_8008B210 as command
 * and response handlers, and partitions the 4-byte-aligned buffer into
 * mode_table, capabilities, and combinations. Called by func_8008AF10.
 */
/*
 * Initialise a pad port on top of a raw transfer buffer.
 *
 * The configuration handshake needs three tables in one block, so the buffer is
 * sliced here: the u16 mode table (one row per mode), one 5-byte capability
 * record per actuator, then the variable-length combination records. The two
 * handlers installed below drive the handshake: func_8008B168 issues the command
 * of the current sequence step, func_8008B210 parses the reply into the tables.
 *
 * Returns 0 when the port is already set up, the buffer is missing or the global
 * readiness callback D_8009C744 reports the device as unavailable; 1 once armed.
 */
s32 gInitPadHandshake(struct PadPort *port, u8 *buffer)
{
    register s32 cursor __asm__("$5");
    s32 result;
    s32 state;
    register s32 (*process_function)(struct PadPort *);
    register s32 row_count __asm__("$3");
    s32 column_count;

    if (buffer == 0) {
        goto return_zero;
    }
    if (port->capabilities != 0) {
        return 0;
    }
    if (gReadyCheckHook() != 0) {
return_zero:
        return 0;
    }

    result = 1;
    __asm__ volatile("" : "=r"(result) : "0"(result));
    state = 4;
    cursor = ((s32)buffer + 3) >> 2;
    port->communication_state = state;
    state = 1;
    port->sequence_step = state;
    port->command_handler = (void (*)(void *))gCmdPadTableHandshake;
    __asm__ volatile("" ::: "memory");
    row_count = *(volatile u8 *)&port->mode_count;
    __asm__ volatile("" : "=r"(row_count) : "0"(row_count));
    process_function = gRespPadTableHandshake;
    port->response_handler = process_function;
    __asm__ volatile("" ::: "memory");
    column_count = port->actuator_count;
    __asm__ volatile("" : "=r"(row_count), "=r"(column_count), "=r"(result)
                     : "0"(row_count), "1"(column_count), "2"(result));

    cursor <<= 2;
    port->mode_table = (u16 *)cursor;
    port->info_record_index = 0;
    __asm__ volatile("" ::: "memory");
    cursor += ((row_count + 1) >> 1) * 4;
    port->capabilities = (struct PadCapabilityRecord *)cursor;
    cursor += (column_count * 5 + 3) & 0xFFC;
    port->combinations = (struct PadCombinationRecord *)cursor;
    return result;
}
