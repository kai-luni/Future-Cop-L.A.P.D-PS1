// Categories: Graphics > GPU Primitives | Graphics > Renderer > Depth Cue Pass
#include "game/types.h"

extern s16 gModelUvOffset __asm__("D_8009F580");
void gSetModelUvOffset(s32 low, s32 high) __asm__("func_80013AA0");


/**
 * @brief Packs two bytes into the shared 16-bit depth/tag word D_8009F580.
 *
 * @param low Low byte of the packed word.
 * @param high High byte of the packed word.
 * @return Nothing.
 *
 * @par Confirmed
 * Single store: D_8009F580 = (high << 8) | low, truncated to s16.
 * func_80016A64, func_80017058 and func_800172A8 read the word as the tag
 * base of their GPU packets; func_80013A14 resets it to 0.
 *
 * @par Likely role
 * The pair encodes the depth bucket and tag flags used to order packets
 * in the ordering table; the exact split of the two bytes is not proven.
 *
 * @par Callers
 * func_8002ACB4, func_8003B2D0, func_8003CF6C, func_8004C5C8, func_800597C0.
 *
 * @par Environment path
 * Stores (high << 8) | low into D_8009F580, the UV offset that the model part
 * emitters add to every u/v coordinate (func_80016A64 and func_80017058 copy
 * it into work.value for func_80015F38; func_800172A8 adds it directly). The
 * node walk func_8003CF6C sets it from the node bytes +0x40/+0x41 just before
 * gSubmitModelInstance and resets it with (0, 0) right after, so one model
 * instance can be drawn with a shifted texture window; func_8002ACB4,
 * func_8003B2D0, func_8004C5C8 and func_800597C0 call it the same way. It
 * emits no geometry.
 */
void gSetModelUvOffset(s32 low, s32 high)
{
    gModelUvOffset = (high << 8) | low;
}
