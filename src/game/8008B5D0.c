// Categories: Input > Controller Protocol
#include "game/types.h"

struct PadPort {
    u8 pad_00[0x14];
    void (*command_handler)(void *);
    s32 (*response_handler)(struct PadPort *);
    u8 pad_1C[0x2A];
    u8 sequence_step;
    u8 pad_47[0xA];
    u8 requested_mode_index;
    u8 requested_mode_lock;
    u8 mode_already_selected;
    u8 pad_54[0x90];
    u8 current_mode_index;
};

extern s32 (*D_8009C744)(struct PadPort *port);
extern void func_8008B668(struct PadPort *port);
extern s32 func_8008B6BC(struct PadPort *port);

/**
 *  @brief Starts a controller mode-selection sequence when the port is available.
 *
 *  A port check gates installation of command and response handlers and storage
 *  of the requested mode, lock, and current-mode comparison.
 *  @param port Controller port state to check and configure.
 *  @param mode Requested mode index, stored as a byte.
 *  @param lock Requested mode lock, stored as a byte.
 *  @return Zero if the port check rejects the request; otherwise one.
 *  @par Globals
 *  D_8009C744 is the indirect port-check function pointer.
 *  @par Control flow
 *  Returns immediately on a nonzero check result; otherwise initializes sequence step 1.
 *  @par Calls
 *  D_8009C744 checks the port; func_8008B668 and func_8008B6BC are installed as handlers.
 *  @par Side effects
 *  On success, updates the port's handlers, request fields, and mode comparison flag.
 *  @par Callers
 *  Confirmed: func_8008A1F8.
 */
s32 func_8008B5D0(struct PadPort *port, s32 mode, s32 lock)
{
    register s32 compare_mode __asm__("$19");
    register s32 flag __asm__("$3");
    register s32 result __asm__("$2");
    s32 current_mode;

    compare_mode = mode;
    result = D_8009C744(port);
    if (result != 0) {
        return 0;
    }

    result = 1;
    __asm__("" : "=r"(result) : "0"(result));
    current_mode = port->current_mode_index;
    __asm__("" : "=r"(current_mode) : "0"(current_mode));
    flag = 1;
    port->sequence_step = flag;
    port->command_handler = (void (*)(void *))func_8008B668;
    port->response_handler = func_8008B6BC;
    __asm__("" : "=r"(compare_mode) : "0"(compare_mode) : "memory");
    flag = compare_mode & 0xFF;
    __asm__("" : "=r"(flag) : "0"(flag));
    port->requested_mode_index = mode;
    port->requested_mode_lock = lock;
    __asm__("" : "=r"(flag), "=r"(current_mode)
            : "0"(flag), "1"(current_mode) : "memory");
    flag ^= current_mode;
    flag = (u32)flag < 1;
    port->mode_already_selected = flag;
    return result;
}
