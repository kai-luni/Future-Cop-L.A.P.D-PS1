// Categories: Graphics > Animation > Track Setup
#include "game/types.h"

extern int gProjectileInit();

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

extern struct AnimClip *func_800448D8(struct AnimTrack *track, s32 index, s32 direction,
                                      u8 flags);
void gSetAnimClipTable(u8 *data, struct AnimTrack *track, u8 forward, s32 index) __asm__("func_80044988");


/**
 * @brief Installs an animation clip table on a track and starts the first
 *        clip.
 *
 * @param data Clip table blob: byte 0 the clip count, +4 the clips.
 * @param track Animation track to initialise.
 * @param forward Non-zero plays forwards (direction 1), else backwards.
 * @param index Starting clip index.
 * @return Nothing.
 *
 * @par Confirmed
 * A null data pointer clears track->clips and returns. Otherwise clips =
 * data + 4, count = data[0], func_800448D8(track, index, forward ? 1 : -1,
 * 0) selects the clip, elapsed = 0 and flags = 0.
 *
 * @par Likely role
 * The animation reset used by every spawn path: the direction flag lets
 * the same clip play in reverse (e.g. retracting gear), the blob header
 * is the shared clip-table format.
 *
 * @par Callers
 * func_8002C584, func_8002DAA4, func_8002DC94, func_8002DFC4, func_800302E8,
 * func_80032180, gProjectileInit, func_800378BC, func_80038754, func_800579D4,
 * func_8005920C.
 *
 * @par Environment path
 * Installs an animation clip table on a track: it stores the clip array at
 * data + 4 and the clip count from data[0], selects the requested clip with
 * func_800448D8 in the forward or backward direction and clears the elapsed
 * time and flags; a null data pointer only clears the track's clip array. It
 * is called from the object setup paths, among them func_8002C584,
 * func_8002DAA4, func_8002DC94, func_80038754, func_800378BC and
 * func_800579D4. It touches no graphics state.
 */
void gSetAnimClipTable(u8 *data, struct AnimTrack *track, u8 forward, s32 index)
{
    if (data == 0) {
        track->clips = 0;
        return;
    }
    track->clips = (struct AnimClip *)(data + 4);
    track->count = data[0];
    func_800448D8(track, index, forward ? 1 : -1, 0);
    track->elapsed = 0;
    track->flags = 0;
}
