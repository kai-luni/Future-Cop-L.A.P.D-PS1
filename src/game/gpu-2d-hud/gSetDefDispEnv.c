// Categories: Graphics > Sprites > State | Utilities
#include "../../include/game/types.h"

struct PacketState {
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    s16 values[4];
    u8 flags[4];
};
struct PacketState *gSetDefDispEnv(struct PacketState *state, s16 x, s16 y, s16 width, s16 height) __asm__("func_800921F4");


/**
 * @brief Initialises a rectangle/state block for the sprite or draw-list layer.
 *
 * @param state  Target block (at least 0x14 bytes).
 * @param x, y   Position.
 * @param width, height Size.
 * @return The unchanged @c state pointer.
 *
 * @par Confirmed
 * Writes x, y, width, height as half-words, then clears four half-word values
 * and four flag bytes. The block therefore describes one rectangle plus four
 * scratch values and four flags - the shape used by the GPU transfer helpers
 * (rect + mode).
 *
 * @par Callers
 * func_8001BE68, func_800773D8, func_8007757C and func_80085C4C (the video
 * decoder set-up).
 *
 * @par Environment path
 * Display set-up on the draw path: initialises a 20-byte display environment
 * block with the display rectangle, a zero screen rectangle and four cleared
 * flag bytes, and returns the block. func_8001BE68 uses it for the two
 * 320x240 display buffers at y 0 and y 256, and the debug screens
 * func_800773D8 and func_8007757C and the video set-up func_80085C4C use it
 * for a 320x240 view at 0,0.
 */
struct PacketState *gSetDefDispEnv(struct PacketState *state, s16 x, s16 y,
                                  s16 width, s16 height)
{
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    state->values[0] = 0;
    state->values[1] = 0;
    state->values[2] = 0;
    state->values[3] = 0;
    state->flags[0] = state->flags[1] = 0;
    state->flags[2] = state->flags[3] = 0;
    return state;
}
