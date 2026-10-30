#include "game/types.h"

struct Unk4749CResource {
    u8 unk00[0xD];
    s8 owner;
    u8 unk0E[2];
    s32 serial;
};

struct Unk4749CHandle {
    struct Unk4749CResource *resource;
    s32 serial;
    s32 state;
    s32 unk0C;
    s32 unk10;
    u8 unk14[4];
    u8 flag18;
};

struct SoundVoice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
};

extern struct Unk4749CHandle D_800A0A80;
extern volatile s32 D_8009FA2C;
extern u8 D_8009F26C;

extern void func_80023E84(void);
extern s32 func_800450A4(s32 id, s8 *mix);
extern s32 func_800474C4(struct Unk4749CHandle *handle);
extern void func_80047800(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra);

void func_80023D78(void)
{
    s8 mix[0x18];

    if (D_800A0A80.unk10 != 0) {
        if (D_800A0A80.unk10 > D_8009FA2C + 0xBB8 ||
            D_800A0A80.unk10 < D_8009FA2C) {
            D_800A0A80.unk10 = 0;
            func_80023E84();
        }
    } else if (D_800A0A80.state != 0) {
        D_800A0A80.unk10 = 0;
        func_80023E84();
    }

    if (D_8009F26C != 0 &&
        D_800A0A80.state != 0 &&
        D_800A0A80.flag18 != 0) {
        if ((u8)func_800450A4(D_800A0A80.unk0C, mix) != 0 &&
            (u8)func_800474C4(&D_800A0A80) == 1) {
            func_80047800((struct SoundVoice *)D_800A0A80.resource,
                          mix[1], mix[0], -1);
        } else {
            func_80023E84();
        }
    }
}
