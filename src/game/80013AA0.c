// Categories: Graphics > GPU Primitives | Graphics > Renderer > Depth Cue Pass
#include "game/types.h"

extern s16 gDepthTagWord __asm__("D_8009F580");

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
 * Packs a low and a high byte into the shared 16-bit depth/tag word
 * D_8009F580 that the model packet emitters use as the base of their
 * ordering-table tag. It sets GPU state rather than drawing: the depth-cued
 * object walk func_8003CF6C calls it with the node bytes at +0x40/+0x41
 * before gSubmitModelInstance and resets it with (0, 0) right after
 * (src/game/8003CF6C.c:69, :73). func_80016A64, func_80017058 and
 * func_800172A8 read the same word, so the unit sits next to the environment
 * render path but emits no geometry itself.
 */
void func_80013AA0(s32 low, s32 high)
{
    gDepthTagWord = (high << 8) | low;
}
