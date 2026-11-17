// Categories: Input > Controller > Pad Poll | Game Engine > Input > Controller
#include "game/types.h"

typedef struct {
    u32 unk0;
    u8 pad[0x20];
} UnkA78F0;

extern void (*D_8009F99C)(void);
extern s32 D_8009F9CC;
extern s32 gScreenState __asm__("D_8009FA00");
extern volatile s32 gFrameTickCounter __asm__("D_8009FA2C");
extern volatile u8 D_8009FA3C;
extern s32 gPaletteCycleCounter __asm__("D_8009FA60");
extern UnkA78F0 gPadBuffers[] __asm__("D_800A78F0");

extern s32 func_8001EF34(void);
extern void func_8001EC94(u32 arg0);
extern s32 func_80092C8C(s32 arg0);
extern s32 func_8001BFE4(void);
extern void func_8001BFA8(void);

/**
 * @brief Refreshes one controller port and returns its status/button word with
 *        the bits inverted.
 *
 * The pad layer stores the BIOS pad word at the head of the port's 0x24-byte
 * buffer (status byte, id byte, two button bytes). This function checks that the
 * port holds a supported pad, runs the per-port pad state machine and then
 * returns the complement of that word - the pad hardware reports buttons
 * active-low, so the complement makes them active-high. Callers test individual
 * bits of the result (the movie player polls 0x40080000).
 *
 * @param arg0 Port index; only 0 and 1 are accepted.
 * @return The complemented 32-bit pad word, or 0 when the port index is out of
 *         range or no supported pad is present.
 *
 * @par Globals
 * D_800A78F0 - array of 0x24-byte pad buffers, indexed by port.
 *
 * @par Control flow
 * Range test, pad-type test, pad update, then the complement of the buffer's
 * first word. No loops.
 *
 * @par Calls
 * func_8001EF34 - returns the pad type nibble of the port (4 or 7), 0 when the
 * port is empty or unsupported. Written without arguments: the port is still in
 * $a0, which is what the callee reads.
 * func_8001EC94 - runs the per-port pad state machine (button edges, repeat
 * times, analog values) for the same port.
 *
 * @par Side effects
 * Updates D_800A78F0[port] and the pad state block of the port.
 *
 * @par Callers
 * Confirmed: func_800879DC (stream demux, both ports every pass). Also listed:
 * func_8001E620.
 */
u32 func_8001EF8C(u32 arg0)
{
    if (arg0 < 2 && func_8001EF34() != 0) {
        func_8001EC94(arg0);
        return ~gPadBuffers[arg0].unk0;
    }
    return 0;
}
