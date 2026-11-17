// Categories: Platform > Serial I/O
#include "game/sio_globals.h"

#include "game/intc_globals.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef struct { u8 pad[0x3c]; u8 *buffer; u8 *other; u8 index; u8 count; } State;
typedef struct { volatile u8 data; u8 pad[3]; volatile u16 status; u8 pad2[8]; volatile u16 control; } Port;
extern Port *gSio0RegBasePtr __asm__("D_8009C78C");
extern volatile u32 *gInterruptStatusPtr __asm__("D_8009C788");
extern volatile u32 gCountWrapLimit __asm__("D_800A6E18");
extern volatile u32 gTimeoutThreshold __asm__("D_800A9448");
extern int func_8008C8B0(void);

/**
 *  @brief Exchanges a byte with the hardware port or performs a negative-value handshake.
 *
 *  The normal path records the received byte in the state buffer and sends the supplied value.
 *  A negative value instead resets the state counters and sends its complement.
 *  @param arg Transfer state containing the receive buffer and counters.
 *  @param argvalue Value to send, or a negative handshake value.
 *  @return Received byte, or -20 if the normal-path wait reports a timeout.
 *  @par Globals
 *  D_8009C78C and D_8009C788 provide port and readiness registers; D_800A9448 and D_800A6E18 record timing state.
 *  @par Control flow
 *  Branches on the value and polls hardware readiness; the normal path can return early on timeout.
 *  @par Calls
 *  func_8008C8B0 supplies the wait condition or timeout indication.
 *  @par Side effects
 *  Reads and writes hardware registers, updates timing globals, and modifies the transfer state and buffers.
 *  @par Callers
 *  Confirmed: func_8008B7D0, func_8008BA34.
 */
int func_8008A9A8(State *arg, int argvalue)
{
    register State *state __asm__("$16") = arg;
    register int value __asm__("$18");
    int result;
    register int control __asm__("$6");
    register unsigned int timer __asm__("$4");
    unsigned int status;
    register Port *port __asm__("$5");
    __asm__("" : "+r"(state));
    value = argvalue;
    if (value < 0) {
        register Port *negport __asm__("$3");
        result = gSio0RegBasePtr->data;
        state->index = 0xff;
        state->count = 1;
        *state->other = ~value;
        negport = gSio0RegBasePtr;
        if (!(negport->status & 1)) {
            do { __asm__ volatile(""); } while (!(negport->status & 1));
        }
        while (!func_8008C8B0()) {}
        {
            register int inverted __asm__("$2") = ~value;
            __asm__("" : "+r"(inverted));
            negport = gSio0RegBasePtr;
            negport->data = inverted;
        }
        return result;
    }
    control = 0x88;
    if ((((int)*(signed char *)state->buffer & 255) >> 4) == 8 && state->index >= 9)
        control = 0x22;
    timer = *(volatile u16 *)0x1f801120;
    port = gSio0RegBasePtr;
    status = port->status;
    gTimeoutThreshold = 0x1ae;
    gCountWrapLimit = timer;
    if (!(status & 2)) {
        register Port *poll __asm__("$3") = port;
        do { __asm__ volatile(""); } while (!(poll->status & 2));
    }
    {
        Port *p = gSio0RegBasePtr;
        result = p->data;
        p->control = control;
    }
    while (!(*gInterruptStatusPtr & 0x80)) {
        if (func_8008C8B0()) return -20;
    }
    gSio0RegBasePtr->data = value;
    state->count++;
    state->buffer[state->index] = result;
    {
        register unsigned int idx __asm__("$2") = state->index;
        __asm__("" : "+r"(idx));
        idx++;
        __asm__("" : "+r"(idx));
        state->index = idx;
    }
    return result;
}
