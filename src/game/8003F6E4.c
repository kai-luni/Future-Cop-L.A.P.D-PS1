#include "game/types.h"

struct DestinationTriple {
    s32 second;
    s32 zero;
    s32 first;
};

struct Unk8009F9A0 {
    u8 unk00[0x50];
    struct DestinationTriple position;
};

struct Unk800A1070 {
    u8 unk00[4];
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
};

extern struct Unk8009F9A0 *D_8009F9A0;
extern struct Unk800A1070 D_800A1070;

extern void func_80069580(u32 index, struct DestinationTriple *destination);
extern void func_80073700(s32 arg);
extern void func_8006F844(s32 arg);
extern void func_8005E160(s32 arg);
extern void func_8003CF6C(s32 arg);
extern void func_8004C5C8(s32 arg0, struct DestinationTriple *arg1);
extern void func_8006E988(s32 arg0, struct DestinationTriple *arg1);
extern void func_800445B0(s32 arg0, struct DestinationTriple *arg1);
extern void func_80070158(s32 arg0, struct DestinationTriple *arg1);
extern void func_80074104(s32 arg0, struct DestinationTriple *arg1);

void func_8003F6E4(s32 arg0)
{
    struct DestinationTriple position;
    register struct Unk8009F9A0 *p __asm__("$6");

    p = D_8009F9A0;
    if (p != 0) {
        position = p->position;
    } else {
        func_80069580(arg0, &position);
    }
    func_80073700((s32)&position);
    func_8006F844((s32)&position);
    func_8005E160((s32)&position);
    func_8003CF6C((s32)&position);
    func_8004C5C8(D_800A1070.unk10, &position);
    func_8006E988(D_800A1070.unk08, &position);
    func_800445B0(D_800A1070.unk0C, &position);
    func_80070158(D_800A1070.unk04, &position);
    func_80074104(D_800A1070.unk14, &position);
}
