// Categories: Graphics > Animation > Track Advance | Game Engine > Frame
#include "game/types.h"

extern int gGuidedProjectileUpdate();

struct AnimClip;

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

struct AnimClip {
    u8 next0;
    u8 type;
    u8 mode;
    u8 flags;
    s16 start;
    s16 end;
    u8 next1;
    u8 next2;
    u16 code;
    s32 duration;
};

struct AnimScript;

extern s32 func_80019668(void);
extern struct AnimClip *gSelectAnimationClip(struct AnimTrack *track, s32 index,
                                      s32 direction, u8 flags) __asm__("func_800448D8");
extern void gProcessAnimScriptCommands(struct AnimScript *script, struct AnimTrack *track) __asm__("func_800449F8");
extern void gPackAnimEventBits(u32 code, u32 key, s32 *shift, u32 *bits) __asm__("func_80044AE8");
u32 gAdvanceAnimTrack(struct AnimScript *script, struct AnimTrack *track, s32 speed,
                  s32 direction) __asm__("func_80044B34");


/**
 * @brief Advances one animation track by a time step, resolves clip changes and
 *        returns the animation events of the frame as a packed bit field.
 *
 * @param script Animation script; the four {u8 op, u8 arg} commands start at
 *               +0x0C and are applied before anything else; may be 0.
 * @param track Track: +0x00 clip array, +0x04 current clip, +0x08 elapsed time,
 *              +0x0C frame (s16), +0x0E target frame (s16), +0x10 step (s8, +1
 *              or -1), +0x11 clip index, +0x12 clip count, +0x13 flags.
 * @param speed Amount added to the elapsed accumulator, already scaled by the
 *              caller.
 * @param direction 0, 1 or 2; selects which link of the current clip is followed
 *                  in mode 0 (next0, next1, next2).
 * @return Bit 0 set when a clip reached its target this frame, plus one 6-bit
 *         event code per clip boundary crossed, packed upwards by
 *         func_80044AE8 (shift starts at 1 and grows by 6 per code; func_8002B784
 *         reads them back as value >> 1 and then value >>= 6).
 *
 * @par Confirmed
 * The body returns 0 immediately when the track has no clip array and when the
 * flags 2 bit (paused) is set; it also returns early when +0x10 (step) is 0, but
 * only after the type-0 and type-3 shortcuts have been evaluated.
 * func_800449F8(script, track) runs the script commands first: 0x32 jumps to a
 * clip, 0x33 clears the pause bit, 0x34 sets it.
 * Clip types: type 0 samples the event code of the current frame and returns;
 * type 3 is a scrub - frame = start + ((end - start) * speed >> 16) - which
 * overrides the frame without touching the accumulator and emits no event.
 * Clip flag 0x40 replaces direction with a random band: func_80019668() & 0x3F
 * mapped to 0 below 0x16, 1 below 0x2B and 2 above. Clip flag 0x20 jitters the
 * speed by up to a quarter: speed += ((0x80 - (rand & 0xFF)) * speed) >> 9.
 * The accumulator then advances (elapsed += speed) and while it exceeds the clip
 * duration the body subtracts the duration, steps the frame, samples the event
 * code and applies the clip mode. Mode 0 walks to target + step and then follows
 * the clip link (next0, next1 or next2 by direction) through
 * func_800448D8(track, next, 2, 1); when that link leads back to the same index
 * the frame is mirrored to start + end - target. Mode 1 ping-pongs (step and
 * target are reflected at the target). Mode 2 stops exactly on the target and
 * sets bit 0.
 * The top of the loop re-samples the clip while flags bit 0 is set, which is how
 * a fresh clip from func_800448D8 is entered.
 *
 * @par Likely role
 * The single animation stepper of the game: model, actor and effect code calls
 * it once per frame per track and then dispatches on the returned bits, so bit 0
 * is the "sequence finished" flag and each 6-bit code is a script command such
 * as a footstep or an attack window (func_8002B784 consumes them as events 1, 2
 * and 3).
 *
 * @par Unknown
 * The meaning of the individual 6-bit event codes, and of the clip fields that
 * this body stores but never compares (the type-1 and type-2 bytes).
 *
 * @par Callers
 * 13 direct callers: func_8002B588, func_8002B784, func_8002CC30,
 * func_8002DB9C, func_8002DEA8, func_8002E630 (four sites), func_800306A0 (two
 * sites), func_800324E0, gGuidedProjectileUpdate, func_800382B4, func_80038A00,
 * func_800549F0 and func_80055988 (two sites).
 */
u32 gAdvanceAnimTrack(struct AnimScript *script, struct AnimTrack *track, s32 speed,
                  s32 direction)
{
    struct AnimClip *clip;
    s32 shift;
    u32 bits;
    s32 next;

    if (track->clips == 0) {
        return 0;
    }

    if (script != 0) {
        gProcessAnimScriptCommands(script, track);
    }

    if (track->flags & 2) {
        return 0;
    }

    clip = track->clip;
    bits = 0;
    shift = 1;

update_clip:
    if (track->flags & 1) {
        track->flags &= ~1;
        if (clip->code != 0) {
            gPackAnimEventBits(clip->code, track->frame, &shift, &bits);
        }
    }

    if (clip->type == 0) {
        if (clip->code != 0) {
            gPackAnimEventBits(clip->code, track->frame, &shift, &bits);
        }
        return bits;
    }

    if (clip->type == 3) {
        register s32 delta __asm__("$4");
        register u32 result __asm__("$2");
        register s32 start __asm__("$2");
        register u32 base __asm__("$3");

        delta = clip->end;
        __asm__ volatile ("" : "=r" (delta) : "0" (delta));
        start = clip->start;
        __asm__ volatile ("" : "=r" (start) : "0" (start));
        delta = delta - start;
        __asm__ volatile ("" : "=r" (delta) : "0" (delta));
        delta = delta * speed;
        base = (u16)clip->start;
        result = bits;
        __asm__ volatile ("" : "=r" (delta) : "0" (delta), "r" (result), "r" (base));
        track->frame = base + (delta >> 16);
        return result;
    }

    if (clip->flags & 0x40) {
        direction = func_80019668() & 0x3F;
        if (direction >= 0x2B) {
            direction = 2;
        } else {
            direction = (direction < 0x16) ? 0 : 1;
        }
    }

    if (track->step == 0) {
        return bits;
    }

    if (clip->flags & 0x20) {
        register s32 temp __asm__("$3");

        temp = (0x80 - (func_80019668() & 0xFF)) * speed;
        __asm__ volatile ("" : "=r" (temp) : "0" (temp));
        speed += temp >> 9;
    }

    track->elapsed += speed;
    speed = 0;

    if (track->elapsed > clip->duration) {
        do {
            track->elapsed -= clip->duration;
            track->frame += track->step;

            if (clip->code != 0) {
                gPackAnimEventBits(clip->code, track->frame, &shift, &bits);
            }

            switch (clip->mode) {
            case 0:
                if (track->frame == track->target + track->step) {
                    bits |= 1;

                    if (direction != 0) {
                        if (direction == 1) {
                            next = clip->next1;
                        } else {
                            next = clip->next2;
                        }
                    } else {
                        next = clip->next0;
                    }

                    if (track->index != next) {
                        clip = gSelectAnimationClip(track, next, 2, 1);
                        goto update_clip;
                    }

                    track->frame = clip->start + clip->end - track->target;

                    if (clip->code != 0) {
                        gPackAnimEventBits(clip->code, track->frame, &shift, &bits);
                    }
                }
                break;

            case 1:
                if (track->frame == track->target) {
                    track->step = -track->step;
                    track->target = clip->start + clip->end - track->target;
                }
                break;

            case 2:
            {
                s32 target;

                target = track->target;
                if (track->frame == target + track->step) {
                    track->frame = track->target;

                    if (clip->code != 0) {
                        gPackAnimEventBits(clip->code, target, &shift, &bits);
                    }

                    bits |= 1;
                }
                break;
            }
            }
        } while (track->elapsed > clip->duration);
    }

    return bits;
}
