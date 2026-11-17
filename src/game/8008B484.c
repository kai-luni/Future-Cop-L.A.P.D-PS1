// Categories: Input > Controller Protocol
#include "game/types.h"

struct PadPort {
    u8 pad_00[0x14];
    void (*command_handler)(void *);
    s32 (*response_handler)(struct PadPort *);
    u8 pad_1C[4];
    u8 *requested_actuator_map;
    u8 pad_24[0x22];
    u8 sequence_step;
};

extern  s32 (*gReadyCheckHook)(struct PadPort *port) __asm__("D_8009C744");
extern void func_8008B4EC(struct PadPort *port);
extern s32 func_8008B508(struct PadPort *port);

/**
 *  @brief Sets up an actuator-map command sequence when the port is available.
 *
 *  A port check gates installation of command and response handlers.
 *  The supplied map pointer is retained in the port state for the sequence.
 *  @param port Controller port state to check and configure.
 *  @param command Pointer to the requested actuator map.
 *  @return One if the sequence was configured, or zero if the check failed.
 *  @par Globals
 *  D_8009C744 supplies the port-check callback.
 *  @par Control flow
 *  A nonzero callback result returns immediately; zero initializes the sequence.
 *  @par Calls
 *  The D_8009C744 callback checks the port; func_8008B4EC and func_8008B508
 *  are installed as handlers rather than called directly.
 *  @par Side effects
 *  On success, writes the sequence step, handlers, and map pointer in the port.
 *  @par Callers
 *  Confirmed: func_8008A1C0.
 */
s32 func_8008B484(struct PadPort *port, u8 *command)
{
    s32 result;
    s32 active;

    result = gReadyCheckHook(port);
    if (result != 0) {
        return 0;
    } else {
        result = 1;
        __asm__ volatile("" : "=r"(result) : "0"(result));
        active = 1;
        port->sequence_step = active;
        port->command_handler = (void (*)(void *))func_8008B4EC;
        port->requested_actuator_map = command;
        port->response_handler = func_8008B508;
    }
    return result;
}
