#include "game/types.h"

struct Unk800A0BC0Entry {
    u16 unk00;
    u8 unk02;
    u8 unk03[5];
};

struct Unk800A0BC0 {
    s16 unk00;
    s16 unk02;
    struct Unk800A0BC0Entry entries[8];
};

extern u32 D_8009FA00[];
extern u8 D_8009F6B8;
extern u8 D_8009F6C0;
extern struct Unk800A0BC0 D_800A0BC0;

extern s32 func_800452C8(void);
extern void func_80029DD8(void);
extern void func_800298EC(s32 arg0);

void func_80029974(s32 arg0)
{
    s32 index;

    if (D_8009FA00[0] < 5 && func_800452C8() != 0) {
        if (D_800A0BC0.unk00 <= 0 && (D_8009F6B8 & 0x70) == 0) {
            func_800298EC(arg0);
        } else {
            if (D_8009F6B8 & 0x40) {
                D_8009F6C0 = 1;
            } else {
                func_80029DD8();
            }

            index = D_800A0BC0.unk02 - 1;
            if (index < 0) {
                index = 7;
            }

            D_800A0BC0.entries[index].unk00 = arg0;
            D_800A0BC0.entries[index].unk02 = 0;
            D_800A0BC0.unk02 = index;

            if (D_800A0BC0.unk00 < 8) {
                D_800A0BC0.unk00++;
            }
        }
    }
}
