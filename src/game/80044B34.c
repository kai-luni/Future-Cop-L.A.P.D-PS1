#include "game/types.h"

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
extern struct AnimClip *func_800448D8(struct AnimTrack *track, s32 index,
                                      s32 direction, u8 flags);
extern void func_800449F8(struct AnimScript *script, struct AnimTrack *track);
extern void func_80044AE8(u32 code, u32 key, s32 *shift, u32 *bits);

u32 func_80044B34(struct AnimScript *script, struct AnimTrack *track, s32 speed,
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
        func_800449F8(script, track);
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
            func_80044AE8(clip->code, track->frame, &shift, &bits);
        }
    }

    if (clip->type == 0) {
        if (clip->code != 0) {
            func_80044AE8(clip->code, track->frame, &shift, &bits);
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
                func_80044AE8(clip->code, track->frame, &shift, &bits);
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
                        clip = func_800448D8(track, next, 2, 1);
                        goto update_clip;
                    }

                    track->frame = clip->start + clip->end - track->target;

                    if (clip->code != 0) {
                        func_80044AE8(clip->code, track->frame, &shift, &bits);
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
                        func_80044AE8(clip->code, target, &shift, &bits);
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
