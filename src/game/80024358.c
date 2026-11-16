// Categories: Game Engine > Audio
#include "game/types.h"

struct Sample {
    u8 index;
    s8 bank;
};

struct Voice {
    u8 pad[0xD];
    s8 slot;
    u8 pad2[6];
    struct Sample *sample;
};

struct Bank {
    u8 pad[0x13];
    u8 cursor;
};

struct Args {
    s32 unk00;
    s32 unk04;
    s32 unk08;
};

extern s32 func_80021F98(s32, s32, s32, s32, s32, struct Args *);

/**
 *  @brief Submits a voice sample request when its sample bank is valid.
 *
 *  Builds an argument record containing volume, pan, and a -1 field, then
 *  passes the sample, bank cursor, and voice slot to the audio routine.
 *  @param bank Bank containing the cursor passed to the request.
 *  @param voice Voice containing the sample and slot.
 *  @param pan Pan value placed in the argument record.
 *  @param volume Volume value placed in the argument record.
 *  @param extra Unused argument.
 *  @return One if the request is made and succeeds; otherwise zero.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Skips the call for bank -1 and converts the call result to zero or one.
 *  @par Calls
 *  func_80021F98 processes the sample request.
 *  @par Side effects
 *  The called routine may modify state; its effects are not shown here.
 *  @par Callers
 *  Confirmed: func_8004772C.
 */
s32 func_80024358(struct Bank *bank, struct Voice *voice, s32 pan, s32 volume, s32 extra)
{
    struct Args args;
    register s32 result __asm__("$3");
    args.unk04 = volume;
    args.unk08 = pan;
    args.unk00 = -1;
    if (voice->sample->bank != -1 && func_80021F98(voice->sample->bank, voice->sample->index, 0, bank->cursor, voice->slot, &args)) {
        result = 1;
    } else {
        result = 0;
    }
    return result;
}
