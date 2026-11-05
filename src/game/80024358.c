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
