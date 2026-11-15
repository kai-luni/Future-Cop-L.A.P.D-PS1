// Categories: Graphics > Animation > Script
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

struct AnimCommand {
    u8 op;
    u8 arg;
};

struct AnimScript {
    u8 unk00[0x0C];
    struct AnimCommand commands[4];
};

extern struct AnimClip *func_800448D8(struct AnimTrack *track, s32 index, s32 direction,
                                      u8 flags);
extern void func_8002A5B4(struct AnimScript *script, s32 index);

/**
 * @brief Interprets up to four animation script commands on a track.
 *
 * @param script Command block; +0xC four {op, arg} pairs, op 0 terminates.
 * @param track Animation track.
 * @return Nothing.
 *
 * @par Confirmed
 * Op 0x32 selects clip arg forwards via func_800448D8(track, arg, 1, 0);
 * op 0x33 clears track flag 2 and sets the frame when arg >= 0; op 0x34
 * sets flag 2 likewise. After each executed command func_8002A5B4(script,
 * i) consumes it; unknown ops just advance the index.
 *
 * @par Likely role
 * The keyframe command pump of the animation tick func_80044B34: flag 2
 * likely pauses the track (0x33 resume, 0x34 hold), the consume call
 * shifts the script window so commands fire once.
 *
 * @par Unknown
 * What func_8002A5B4 does to the consumed slot.
 *
 * @par Callers
 * func_80044B34.
 */
void func_800449F8(struct AnimScript *script, struct AnimTrack *track)
{
    s32 i;
    s32 op;
    s32 arg;

    for (i = 0; i < 4;) {
        op = script->commands[i].op;
        if (op == 0) {
            break;
        }
        arg = script->commands[i].arg;
        switch (op) {
        case 0x32:
            func_800448D8(track, arg, 1, 0);
            break;
        case 0x33:
            track->flags &= ~2;
            if (arg >= 0) {
                track->frame = arg;
            }
            break;
        case 0x34:
            track->flags |= 2;
            if (arg >= 0) {
                track->frame = arg;
            }
            break;
        default:
            i++;
            continue;
        }
        func_8002A5B4(script, i);
    }
}
