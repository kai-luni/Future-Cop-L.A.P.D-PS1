// Categories: Graphics > Animation > Clip Control
#include "game/types.h"

struct AnimClip {
    u8 unk00[4];
    s16 start;
    s16 end;
    u8 unk08[8];
};

struct AnimTrack {
    struct AnimClip *clips;
    struct AnimClip *clip;
    s32 elapsed;
    s16 frame;
    s16 target;
    s8 step;
    u8 index;
    u8 count;
    u8 flags;
};

/**
 * @brief Selects an animation clip on a track and sets direction, target
 *        frame and start frame.
 *
 * @param track Animation track.
 * @param index Clip index (16-byte clips).
 * @param direction >0 forwards, <0 backwards, 2 keeps the elapsed time.
 * @param flags ORed into the track flags.
 * @return The selected clip, or 0 when the track has no clip table.
 *
 * @par Confirmed
 * clip = clips + (index << 4); index/clip stored. Unless direction == 2 the
 * elapsed time resets. step = +1 with target = clip->end for positive
 * direction, else -1 with target = clip->start; a clip whose start > end
 * inverts the step. frame = clip->start, flags merged.
 *
 * @par Likely role
 * The clip-switch primitive of the animation system: direction 2 is the
 * seamless reselect (same clip, keep position), the start/end inversion
 * supports reversed clip definitions without a second table.
 *
 * @par Callers
 * func_8002AF94, func_8002B0EC, func_8002B784, func_8002E2BC, func_8002E630,
 * func_8002ED7C, func_80030498, func_800306A0, func_800382B4, func_80038A00,
 * func_80044988, func_800449F8, func_80044B34, func_80051AF8, func_800549F0,
 * func_80055988.
 */
struct AnimClip *func_800448D8(struct AnimTrack *track, s32 index, s32 direction, u8 flags)
{
    register struct AnimTrack *state __asm__("$8");
    struct AnimClip *clip;
    register struct AnimClip *computed __asm__("$2");
    register s32 offset __asm__("$3");
    register u16 frame __asm__("$4");
    u8 mergedFlags;

    state = track;
    __asm__ volatile ("" : "+r" (state));
    offset = index << 4;
    if (state->clips == 0) {
        computed = 0;
        goto done;
    }
    state->index = index;
    computed = (struct AnimClip *)((u8 *)state->clips + offset);
    clip = computed;
    state->clip = computed;
    if (direction != 2) {
        state->elapsed = 0;
    }
    if (direction > 0) {
        state->step = 1;
        state->target = clip->end;
    } else {
        state->step = -1;
        state->target = clip->start;
    }
    if (clip->start > clip->end) {
        state->step = -state->step;
    }
    computed = clip;
    mergedFlags = state->flags | flags;
    frame = clip->start;
    state->frame = frame;
    state->flags = mergedFlags;
done:
    return computed;
}
