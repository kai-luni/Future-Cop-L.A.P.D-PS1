#include "game/types.h"

struct Unk21824 {
    u32 f0;
    u16 f4;
    u16 f6;
    u16 f8;
    u16 fA;
    u16 fC;
    u16 fE;
    u16 f10;
    u16 f12;
    u32 f14;
    u32 f18;
    u16 f1C;
    u16 f1E;
    u32 f20;
    u32 f24;
};

void func_80021824(void)
{
    struct Unk21824 attr;

    attr.f0 = 0x3FC3;
    attr.f4 = 0x3FFF;
    attr.f6 = 0x3FFF;
    attr.f8 = 0;
    attr.fA = 0;
    attr.fC = 0;
    attr.fE = 0;
    attr.f10 = 0;
    attr.f12 = 0;
    attr.f14 = 0;
    attr.f18 = 0;
    attr.f1C = 0;
    attr.f1E = 0;
    attr.f20 = 0;
    attr.f24 = 0;
    func_80098FE8(&attr);
}
