// Categories: Graphics > HUD | Platform > Sound
#include "game/types.h"

struct Frame { u8 pad[0x16]; u8 length; u8 pad17[3]; u16 stride; };
struct State { u8 pad[8]; struct Frame *frames[2]; u8 pad10[2]; u8 count; u8 current; };
extern struct State gReleaseQueues __asm__("D_800A0C30");
extern s32 gPortraitAudioByteCount[] __asm__("D_800A0C44");
extern s32 gFrameTime __asm__("D_8009FAA4");
extern u8 gBitFieldState __asm__("D_8009F6B8");
extern s32 gOutStreamId __asm__("D_8009F6BC");
extern s32 gPortraitStartTick __asm__("D_8009F6C8");
extern void gAcquireTrackedSoundVoice(s32, s32, s32) __asm__("func_80024064");
extern void gStartTrackedSoundVoice(void) __asm__("func_80023F08");
extern void gQueueVramUpload(s32, void *, s32, s32, s32) __asm__("func_8001BBEC");

s32 gStepSynchronizedPortrait(s32 *out) __asm__("func_80029748");

/**
 * @brief Starts the portrait's companion voice and advances the timed VRAM frame uploads.
 *
 * @param out Output pointer receiving the active stream/track id D_8009F6BC.
 * @return 1 while frame playback (D_8009F6B8 bit 2) stays within state->count; 0 when inactive
 * or finished.
 *
 * @par Confirmed mechanics
 * Writes the stream ID through a0, checks bits 0x04 and 0x08 and starts sound once while bits
 * 0x10 and 0x20 are clear. Computes frame index as (current tick minus start tick) divided by
 * 30 minus one. On a changed valid frame, resolves the chunk stride and calls gQueueVramUpload
 * for 0x3C0, 0x1CC. Clears the animation bit at expiry.
 *
 * @par Globals
 * Uses gOutStreamId, gBitFieldState, gPortraitAudioByteCount, gPortraitStartTick, gFrameTime
 * and the animation fields in gReleaseQueues.
 *
 * @par Callers
 * Direct calls from func_800486FC.
 *
 * @par Role and path
 * The HUD overlay calls this to advance the synchronized portrait animation and audio.
 *
 * @par Unknown
 * The stored frame-bank lengths constrain the accepted indices.
 */
s32 gStepSynchronizedPortrait(s32 *out)
{
    register struct State *state __asm__("$6");
    register struct Frame **frame __asm__("$3");
    register s32 index __asm__("$5");
    s32 length;
    register s32 quotient __asm__("$3");
    register u32 base __asm__("$3");
    *out = gOutStreamId;
    if (!(gBitFieldState & 4)) goto return0;
    if (gBitFieldState & 8) return 0;
    if (!(gBitFieldState & 0x30)) {
        gBitFieldState |= 0x30;
        gAcquireTrackedSoundVoice(gOutStreamId, gPortraitAudioByteCount[0], 0);
        gStartTrackedSoundVoice();
        if (gBitFieldState & 2) gPortraitStartTick = gFrameTime - 30;
        else gBitFieldState &= 0xDF;
    }
    if (!(gBitFieldState & 2)) goto return0;
    base = 0x800A0000;
    __asm__("" : "=r"(base) : "0"(base));
    state = (struct State *)(base + 0xC30);
    quotient = (gFrameTime - gPortraitStartTick) / 30;
    index = quotient - 1;
    if (index >= state->count) {
        gBitFieldState &= 0xDF;
        if (!(gBitFieldState & 0x10)) gBitFieldState |= 8;
        return 0;
    }
    if (index == state->current) return 1;
    state->current = index;
    frame = state->frames;
    length = (*frame)->length;
    if (index >= length) {
        do {
            index -= (*frame)->length;
            frame++;
        } while (index >= length);
    }
    index = (*frame)->stride * index + 0x28;
    gQueueVramUpload(0, (u8 *)*frame + index, 0x3C0, 0x1CC, 0);
    return 1;
return0:
    return 0;
}
