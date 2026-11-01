#include "game/types.h"

struct Unk4749CResource {
    u8 unk00[0x10];
    s32 serial;
};

struct Unk4749CHandle {
    struct Unk4749CResource *resource;
    s32 serial;
};

extern struct Unk4749CHandle D_800A0A80;
extern s32 func_800474C4(struct Unk4749CHandle *handle);

s32 func_80024188(void)
{
    register s32 value __asm__("$4");

    if ((u8)func_800474C4(&D_800A0A80) == 1) {
        value = (s32)D_800A0A80.resource;
    } else {
        value = 0;
    }
    return value;
}
