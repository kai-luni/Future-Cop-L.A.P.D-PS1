#include "game/types.h"

struct SoundSystem {
    s32 bankCount;
    s32 *banks;
    s32 channelCount;
    u8 *channels;
};

extern struct SoundSystem D_800A07A8;
extern s32 D_800A07AC;
extern s32 D_800A07B0;
extern s32 D_800A07B4;

extern s32 func_80021740(s32 *table, s32 key, s32 *value, s32 *index);
extern void func_800216D8(void);

s32 func_80021554(s32 *arguments)
{
    s32 value;
    s32 bankCount;
    s32 channelCount;
    s32 i;
    s32 count;
    s32 result;
    u8 *p;
    register s32 *valuePtr __asm__("$6");

    result = 0;
    bankCount = 1;

    valuePtr = &value;
    if (func_80021740(arguments, 4, valuePtr, 0) != 0) {
        bankCount = value;
        D_800A07A8.bankCount = bankCount;
    }

    channelCount = 0x18;
    valuePtr = &value;
    if (func_80021740(arguments, 5, valuePtr, 0) != 0) {
        channelCount = value;
        D_800A07B0 = channelCount;
    }

    value = 0;
    valuePtr = &value;
    if (func_80021740(arguments, 8, valuePtr, 0) != 0) {
        D_800A07AC = value;
    }

    p = (u8 *)D_800A07AC;
    count = bankCount * 4;
    while (count--) {
        *p++ = 0;
    }

    value = 0;
    valuePtr = &value;
    if (func_80021740(arguments, 9, valuePtr, 0) != 0) {
        D_800A07B4 = value;
    }

    p = (u8 *)D_800A07B4;
    count = channelCount * 28;
    while (count--) {
        *p++ = 0;
    }

    for (i = 0; i < channelCount; i++) {
        *(s32 *)(D_800A07A8.channels + i * 0x1C + 0xC) = i;
    }

    if (result != 0) {
        func_800216D8();
    }
    return result;
}
