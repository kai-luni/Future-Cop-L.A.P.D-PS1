#include "game/types.h"

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

struct ObjectInfo {
    u8 pad00[0xC1];
    s8 side;
};

struct Object {
    u8 pad00[0x1E];
    s16 unk1E;
    u8 pad20[0xAC - 0x20];
    struct ObjectInfo *info;
};

struct Game {
    u8 pad00[8];
    s32 mode0;
    u8 pad0C[0x98 - 0xC];
    s32 mode1;
};

extern struct Game *D_8009FA94;
extern s32 func_80095900(s32 value);

s32 func_80052F54(struct Object *obj, s32 val)
{
    s32 *modePtr;
    s32 mode;
    s32 temp;

    if (val > 0x93333) {
        if (val > 0x300000) {
            val = 0x300000;
        }
        val -= 0x93333;

        val <<= 9;
        val /= 183501;
        val <<= 7;

        if (val <= 0x8000) {
            FMUL(val, val, 0x9CC4);
        } else if (val <= 0xC000) {
            FMUL(temp, val, func_80095900(val) << 2);
            FMUL(val, temp, 0xDDB3);
        } else {
            FMUL(val, val, val);
        }

        temp = obj->unk1E;
        FMUL(val, temp, val);

        val *= 100;

        if (obj->info->side != 0) {
            modePtr = &D_8009FA94->mode1;
        } else {
            modePtr = &D_8009FA94->mode0;
        }
        mode = *modePtr;

        switch (mode) {
        case 2:
            val /= 150;
            break;
        case 1:
            val /= 125;
            break;
        case 0:
            val /= 100;
            break;
        case 3:
            val = 0;
            break;
        }

        return val;
    }

    return 0;
}
