#include "game/types.h"

struct Unk84110 {
    u8 *unk00;
    u8 unk04[0x18];
    s32 unk1C;
    u32 unk20;
};

struct Resource;

extern u32 *func_80019CA0(u8 *arg0, u32 arg1, u32 arg2, s32 arg3);
extern struct Resource *func_80019BF0(s32 group, s32 name);
extern u8 *func_80019C38(u8 *arg0, u32 arg1, u32 arg2, s32 arg3);
extern void func_80083A10(u8 *arg0, s32 arg1, s32 arg2, s32 arg3);
extern void func_800181D4(struct Unk84110 *arg0);

void func_80084110(struct Unk84110 *arg0)
{
    u32 *data;
    s32 *node;

    data = func_80019CA0(arg0->unk00, arg0->unk20, 0x6152534C, arg0->unk1C);
    data[2] = (data[1] - 0xC) >> 3;
    data += 2;
    node = (s32 *)func_80019BF0(data[1], data[2]);
    data[2] = (u32)node;
    func_80083A10(func_80019C38(arg0->unk00, arg0->unk20, 0x74414354, arg0->unk1C) - 4,
                  node[0], -1, 0);
    func_800181D4(arg0);
}
