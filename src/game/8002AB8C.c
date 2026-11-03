#include "game/types.h"

struct Unk2AB8C {
    u8 unk00[0x17];
    u8 unk17;
    u8 unk18;
    u8 unk19[0x3];
    s16 unk1C;
    u8 unk1E[0x32];
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C[3];
    s32 unk68;
    u8 unk6C[0x18];
    s32 unk84;
};

extern struct Unk2AB8C *D_8009FA58[2];

extern s32 func_8001A4E8(s32 *a, s32 *b);

s32 func_8002AB8C(struct Unk2AB8C *object, s32 query)
{
    s32 min;
    s32 i;
    register struct Unk2AB8C **p __asm__("$16");
    s32 dist;

    switch (query) {
    case 0:
        return object->unk1C;
    case 1:
        return object->unk50;
    case 2:
        return object->unk54;
    case 3:
        return object->unk58;
    case 4:
        return object->unk84;
    case 5:
        min = 0x7FFFFFFF;
        i = 0;
        p = D_8009FA58;
        do {
            dist = func_8001A4E8(object->unk5C, (*p)->unk5C) -
                   (object->unk68 + (*p)->unk68);
            if (dist < min) {
                min = dist;
            }
            i++;
            p++;
        } while (i < 2);
        return min - ((min >> 31) & min);
    case 7:
        return object->unk17;
    case 8:
        return object->unk18;
    case 6:
    case 9:
    default:
        return 0;
    }
}
