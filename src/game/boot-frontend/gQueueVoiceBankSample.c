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

extern s32 gQueueBankSampleOnChannel(s32, s32, s32, s32, s32, struct Args *) __asm__("func_80021F98");

s32 gQueueVoiceBankSample(struct Bank *bank, struct Voice *voice, s32 pan, s32 volume, s32 extra) __asm__("func_80024358");

/**
 * @brief Queues a voice's bank sample when its source bank is valid.
 *
 * @param bank Bank containing the cursor passed to the request.
 * @param voice Voice containing the sample and slot.
 * @param pan Pan value placed in the argument record.
 * @param volume Volume value placed in the argument record.
 * @param extra Unused argument.
 * @return One if the request is made and succeeds; otherwise zero.
 *
 * @par Confirmed mechanics
 * Reads the source descriptor through voice +0x14, tests its signed bank byte against -1, then
 * passes its sample byte, record index zero, the voice channel at +0x0D and a stack mix tuple
 * beginning with -1 to gQueueBankSampleOnChannel. Normalizes the result to zero or one.
 *
 * @par Globals
 * None directly. The callee updates the sound channel in gSoundSystem.
 *
 * @par Callers
 * Direct calls from func_8004772C.
 *
 * @par Role and path
 * The voice playback path converts a sample descriptor to the bank queue call.
 *
 * @par Unknown
 * The fourth forwarded value is read from bank +0x13 and the callee ignores it.
 */
s32 gQueueVoiceBankSample(struct Bank *bank, struct Voice *voice, s32 pan, s32 volume, s32 extra)
{
    struct Args args;
    register s32 result __asm__("$3");
    args.unk04 = volume;
    args.unk08 = pan;
    args.unk00 = -1;
    if (voice->sample->bank != -1 && gQueueBankSampleOnChannel(voice->sample->bank, voice->sample->index, 0, bank->cursor, voice->slot, &args)) {
        result = 1;
    } else {
        result = 0;
    }
    return result;
}
