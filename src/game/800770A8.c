#include "game/types.h"

typedef struct {
    u8 unk00[3];
    u8 unk03;
    u8 unk04;
    u8 unk05;
    u8 unk06;
    u8 unk07;
    s16 unk08;
    s16 unk0A;
    s16 unk0C;
    s16 unk0E;
} Unk770A8;

extern u8 D_8009B314[];

extern void func_80092C8C(s32 arg0);
extern void func_80093224(u8 *arg0);

void func_800770A8(s32 arg0, s32 arg1, s32 arg2)
{
    u8 flags;
    Unk770A8 local;

    flags = D_8009B314[arg2];

    local.unk03 = 3;
    local.unk07 = 0x40;
    local.unk04 = 0x80;
    local.unk05 = 0x80;
    local.unk06 = 0x80;

    func_80092C8C(0);

    if (flags & 1) {
        local.unk0A = arg1 + 1;
        local.unk08 = arg0;
        local.unk0C = arg0;
        local.unk0E = arg1 + 5;
        func_80093224((u8 *)&local);
        func_80092C8C(0);
    }

    if (flags & 2) {
        local.unk0A = arg1 + 7;
        local.unk08 = arg0;
        local.unk0C = arg0;
        local.unk0E = arg1 + 0xC;
        func_80093224((u8 *)&local);
        func_80092C8C(0);
    }

    if (flags & 4) {
        local.unk08 = arg0 + 1;
        local.unk0A = arg1 + 0xD;
        local.unk0C = arg0 + 4;
        local.unk0E = arg1 + 0xD;
        func_80093224((u8 *)&local);
        func_80092C8C(0);
    }

    if (flags & 8) {
        local.unk08 = arg0 + 5;
        local.unk0A = arg1 + 7;
        local.unk0C = arg0 + 5;
        local.unk0E = arg1 + 0xC;
        func_80093224((u8 *)&local);
        func_80092C8C(0);
    }

    if (flags & 0x10) {
        local.unk08 = arg0 + 5;
        local.unk0A = arg1 + 1;
        local.unk0C = arg0 + 5;
        local.unk0E = arg1 + 5;
        func_80093224((u8 *)&local);
        func_80092C8C(0);
    }

    if (flags & 0x20) {
        local.unk08 = arg0 + 1;
        local.unk0A = arg1;
        local.unk0C = arg0 + 4;
        local.unk0E = arg1;
        func_80093224((u8 *)&local);
        func_80092C8C(0);
    }

    {
        register u8 *p __asm__("$4");

        p = (u8 *)&local;
        if (flags & 0x40) {
            local.unk08 = arg0 + 1;
            local.unk0A = arg1 + 6;
            local.unk0C = arg0 + 4;
            local.unk0E = arg1 + 6;
            func_80093224(p);
            func_80092C8C(0);
        }
    }
}
