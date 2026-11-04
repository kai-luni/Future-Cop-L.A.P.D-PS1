#include "game/types.h"

typedef struct Target {
    u8 pad00[0x08];
    s32 unk08;
    u8 pad0C[0x14 - 0x0C];
    u16 unk14;
    u8 pad16[2];
    u8 unk18;
    u8 unk19;
    u8 pad1A[2];
    s16 unk1C;
    u8 pad1E[0xB8 - 0x1E];
    struct Target *unkB8;
    s32 unkBC;
    s16 unkC0;
    u8 padC2[0x118 - 0xC2];
    u32 unk118;
    u8 pad11C[2];
    s16 unk11E;
    u8 pad120[0x138 - 0x120];
    struct Target *unk138;
    s32 unk13C;
    u8 pad140[0x15F - 0x140];
    u8 unk15F;
    u8 unk160;
} Target;

extern void func_8002B1DC(Target *arg0, s32 arg1, Target *arg2);
extern void func_8002B1AC(Target *arg0);

void func_8002BFB4(Target *s0, Target *s1)
{
    s16 unk1C;

    if (s0 == 0) {
        return;
    }

    unk1C = s0->unk1C;
    if (unk1C < 0) {
        return;
    }

    if (s1 == 0) {
        return;
    }

    if (s1->unk08 < 0) {
        return;
    }

    if (s1->unk18 == s0->unk18) {
        return;
    }

    if (s0->unkC0 == 0) {
        return;
    }

    if (s0->unk15F == 4 && s0->unkB8 != 0) {
        return;
    }

    if (!(s0->unk160 & 2) && unk1C < 5 && s0->unk15F != 4) {
        if (s0->unk15F == 2) {
            if (s0->unk138 != 0) {
                if ((s0->unk138->unk19 & 0xF) >= (s1->unk19 & 0xF)) {
                    return;
                }
            }
        }

        func_8002B1DC(s0, 1, s1);
        s0->unk118 = (s0->unk118 & 0xF0FFFFFF) | 0x0A000000;
        s0->unk15F = 2;
        s0->unk11E = s1->unk14 & 0x7FFF;
        s0->unk138 = s1;
        s0->unk13C = s1->unk08;
        func_8002B1AC(s0);
    } else {
        if (s0->unk15F == 0) {
            func_8002B1DC(s0, 1, s1);
        } else if (s0->unk15F == 1) {
            if (s0->unkB8 != 0 && (s0->unkB8->unk19 & 0xF) < (s1->unk19 & 0xF)) {
                s0->unkB8 = s1;
                s0->unkBC = s1->unk08;
            }
        }
    }
}
