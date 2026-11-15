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
 */
struct PacketState *func_800921F4(struct PacketState *state, s16 x, s16 y,
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
