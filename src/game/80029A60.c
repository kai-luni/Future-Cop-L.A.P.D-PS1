#include "game/types.h"

struct Unk41DC4;

struct Unk800A0BC0Entry {
    u16 unk00;
    u8 unk02;
    u8 pad03;
    struct Unk41DC4 *unk04;
};

struct Unk800A0BC0 {
    s16 unk00;
    s16 unk02;
    struct Unk800A0BC0Entry entries[8];
};

extern struct Unk800A0BC0 D_800A0BC0;
extern u32 D_8009FA00;
extern u8 D_8009F6B8;

extern void func_80029DD8(void);
extern void func_80041DC4(struct Unk41DC4 *object, s32 value);

s32 func_80029A60(s32 arg0, struct Unk41DC4 *arg1, u8 arg2)
{
    s32 index;

    if (arg2 == 0) {
        if (D_800A0BC0.unk00 > 0) {
            return 0;
        }
        if ((D_8009F6B8 & 0x70) != 0) {
            return 0;
        }
    }

    if (D_8009FA00 >= 5) {
        return 0;
    }

    if (arg2 == 0 || D_800A0BC0.unk00 <= 0) {
        func_80041DC4(arg1, arg0);

        index = (D_800A0BC0.unk02 + D_800A0BC0.unk00) & 7;
        D_800A0BC0.entries[index].unk00 = arg0;
        D_800A0BC0.entries[index].unk02 = 1;
        D_800A0BC0.entries[index].unk04 = arg1;
        D_800A0BC0.unk00++;
    } else {
        func_80029DD8();
        func_80041DC4(arg1, arg0);

        index = D_800A0BC0.unk02 - 1;
        if (index < 0) {
            index = 7;
        }

        D_800A0BC0.entries[index].unk00 = arg0;
        D_800A0BC0.entries[index].unk02 = 1;
        D_800A0BC0.entries[index].unk04 = arg1;
        D_800A0BC0.unk02 = index;

        if (D_800A0BC0.unk00 < 8) {
            D_800A0BC0.unk00++;
        }
    }

    return 1;
}
