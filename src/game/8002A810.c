#include "game/types.h"

typedef struct Unk8002A810 Unk8002A810;
typedef struct Unk8002A810Sub Unk8002A810Sub;
typedef struct Unk8002A810Local Unk8002A810Local;

struct Unk8002A810Sub {
    u8 unk00[0x16];
    u8 unk16;
    u8 unk17[0x03];
    u16 unk1A;
    u8 unk1C[0x5C];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
};

struct Unk8002A810Local {
    u8 unk00[4];
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    u8 unk10[0x30];
    u16 unk40;
    u8 unk42[7];
    u8 unk49;
    u8 unk4A[6];
};

struct Unk8002A810 {
    u8 unk00[0x19];
    u8 unk19;
    u8 unk1A[0x06];
    u8 unk20;
    u8 unk21[0x03];
    u32 unk24;
    u8 unk28[0x4C];
    void *unk74;
    u8 unk78[0x14];
    u32 unk8C;
};

extern Unk8002A810Sub *func_8004D664(s32 id);
extern void func_8003B898(s32 arg0, Unk8002A810 *arg1,
                         Unk8002A810Local *arg2, s32 arg3, s32 arg4);
extern void func_80031690(Unk8002A810 *arg0);
extern void func_8004D958(Unk8002A810 *arg0);

s32 func_8002A810(Unk8002A810 *s0, s32 arg1, s32 arg2)
{
    register Unk8002A810Sub *a0 __asm__("$4");
    Unk8002A810Local local;
    Unk8002A810Local *a2;
    s32 word7C;
    register s32 word78 __asm__("$3");
    u16 half1A;
    register u8 type __asm__("$5");

    if (s0->unk24 & 0x01000000) {
        return 0;
    }

    s0->unk24 |= 0x01000010;

    a0 = 0;
    if (s0->unk8C != 0) {
        a0 = func_8004D664(s0->unk8C & 0x3FFFFFFF);
    }

    if (s0->unk20 != 0) {
        a2 = 0;
        if (a0 != 0) {
            type = a0->unk16;
            if (type == 2) {
                local.unk49 = type;
                word7C = a0->unk7C;
                word78 = a0->unk78;
                local.unk08 = word7C;
                local.unk0C = a0->unk80;
                half1A = a0->unk1A;
                a2 = &local;
                local.unk04 = word78;
                local.unk40 = half1A;
            }
        }
        if (arg2 & 0xFF) {
            register s32 two __asm__("$2");
            two = 2;
            func_8003B898(s0->unk20, s0, a2,
                          (s0->unk19 >> 3) & 0x1E, two);
        }
    }

    if (s0->unk74 != 0) {
        func_80031690(s0);
    }

    if (arg1 & 0xFF) {
        func_8004D958(s0);
    }
    return 1;
}
