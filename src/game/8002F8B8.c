#include "game/types.h"

struct Inner {
    u8 pad00[0x40];
    s32 unk40;
};

struct Entity {
    u8 pad00[0x14];
    u16 unk14;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 pad19[0xAC - 0x19];
    struct Inner *unkAC;
    u8 padB0[0xC8 - 0xB0];
    struct Entity *unkC8;
};

extern struct Entity *D_8009FA58[2];
extern struct Entity *D_8009FA60[2];

s32 func_8002F8B8(u8 arg0, s16 arg1, struct Entity *arg2, u8 arg3)
{
    s32 val;
    register struct Entity **p __asm__("$3");
    register struct Entity **end __asm__("$5");

    switch (arg0) {
    case 8:
        val = arg2->unk16;
        if (val == 0x12) {
            val = 1;
        }
        break;

    case 1:
        val = arg2->unk16;
        break;

    case 2:
        val = arg2->unk14;
        if (arg2->unk16 == 0x15) {
            return 0;
        }
        break;

    case 3:
        val = arg2->unk17;
        break;

    case 4:
        val = arg2->unk18;
        break;

    case 5:
        val = arg2->unk18;
        arg1 &= (1 << val);
        break;

    case 7:
        if (arg2->unk16 == 2) {
            arg2 = arg2->unkC8;
            if (arg2 != 0 && arg2->unk16 == 0x12) {
                return 1;
            }
        }
        return 0;

    case 6:
    default:
        break;
    }

    if (val == (s16)arg1) {
        if (arg3 != 0) {
            register s32 one __asm__("$7") = 1;
            register u8 unk16 __asm__("$4") = arg2->unk16;
            p = D_8009FA58;
            end = D_8009FA60;
            do {
                if (unk16 == one) {
                    if (arg2 != *p) {
                        continue;
                    }
                }
                if (*p != 0 && (*p)->unkAC->unk40 != 0) {
                    return 0;
                }
            } while ((s32)++p < (s32)end);
        }
        return 1;
    }

    return 0;
}
