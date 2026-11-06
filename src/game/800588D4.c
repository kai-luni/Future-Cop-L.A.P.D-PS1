#include "game/types.h"

struct Unk5A190;
struct Unk584BC;
struct Unk55E10;

struct Unk588D4Sub28 {
    u8 unk00[0x34];
    u8 *unk34;
};

struct Unk588D4SubDC {
    u8 unk00[0x24];
    u32 unk24;
};

struct Unk588D4Global {
    u8 unk00[0x5C];
    s32 unk5C;
    u8 unk60[0x24];
    s32 unk84[2];
};

struct Unk588D4Data {
    u8 unk00[0x24];
    u32 unk24;
    s32 unk28[17];
    s32 unk6C;
    u8 unk70[0x3D];
    s8 unkAD;
    u8 unkAE[0x13];
    s8 unkC1;
    u8 unkC2[0x1C];
    u8 unkDE;
    u8 unkDF[0x21D];
    s8 unk2FC;
    u8 unk2FD[0x1B];
    s16 unk318;
    s16 unk31A;
    u8 unk31C[8];
    s16 unk324;
    s16 unk326;
};

struct Unk588D4 {
    u8 unk00[0x0C];
    u8 unk0C;
    u8 unk0D[0x07];
    u16 unk14;
    u8 unk16[0x06];
    s16 unk1C;
    u8 unk1E[0x06];
    u32 unk24;
    struct Unk588D4Sub28 *unk28;
    u8 unk2C[0x24];
    u8 unk50[0x04];
    s32 unk54;
    u8 unk58[0x04];
    u8 unk5C[0x14];
    s16 unk70;
    u8 unk72[0x3A];
    struct Unk588D4Data *unkAC;
    u8 unkB0[0x06];
    s8 unkB6;
    s8 unkB7;
    u8 unkB8[0x24];
    struct Unk588D4SubDC *unkDC;
};

struct Unk588D4Msg {
    u8 unk00[0x44];
    u8 *unk44;
    u8 unk48;
    u8 unk49;
    u8 unk4A[6];
};

struct Unk584BCHitLocal {
    u32 unk00;
    s16 damage;
    u8 kind;
    u8 unk07;
    union {
        void *attacker;
        s32 source;
    } from;
    u8 unk0C[4];
};

struct Unk588D4Locals {
    s32 out18;
    s32 out1C;
    struct Unk588D4Msg msg;
    struct Unk584BCHitLocal hit;
};

extern s32 D_8009FA00;
extern struct Unk588D4Global *D_8009FA94;
extern u32 D_8009FAA4;

extern void func_8005A190(struct Unk5A190 *object, s32 index, s32 value);
extern void func_800585B8(struct Unk588D4 *object);
extern s32 func_80066F78(void *object, s32 arg1, s32 arg2, s32 *arg3);
extern void func_80058378(struct Unk584BC *object, s32 amount, s32 source);
extern void func_80058714(struct Unk588D4 *object);
extern void func_800576A8(void *object);
extern s32 func_800586D4(void *object, s32 arg1, s32 arg2, s32 *arg3);
extern s32 func_800402DC(void *owner, u8 column, void *arg2, s32 row, s32 arg4);
extern s32 func_80046C84(s32 id, s32 arg1);
extern s32 func_800584BC(s32 message, struct Unk584BC *object,
                         struct Unk584BCHitLocal *hit, s32 arg3, s32 arg4);
extern void func_8006C34C(struct Unk55E10 *object, s32 arg1);
extern void func_80010750(void *arg);
extern void func_80010234(void *arg0, void *arg1, s32 arg2);
extern void func_80057E40(void *object, s32 arg1);
extern void func_8006C3E0(void *object, s32 arg1);
extern void func_80055E10(struct Unk55E10 *object, s32 arg1);
extern void func_800569EC(void *object, s32 arg1, s32 arg2);

s32 func_800588D4(struct Unk588D4 *object, s32 arg1)
{
    struct Unk588D4Data *data;
    register s32 s2 __asm__("$18");
    register s32 s4 __asm__("$20");
    register s32 v __asm__("$2");
    register u32 hitcode __asm__("$3");
    register s32 hitarg __asm__("$7");
    register s32 offset __asm__("$4");
    struct Unk588D4Locals locals;
    s32 ret;
    s32 c1;

    object->unkAC->unk2FC = -1;
    func_8005A190((struct Unk5A190 *)object->unkAC, 0, object->unk1C);

    if (object->unk0C != 0) {
        func_800585B8(object);
    }

    if (object->unkAC->unkDE != 0) {
        ret = func_80066F78(object->unk50, 0, 0, &locals.out18);
        if (object->unk54 + 0x3800 < ret) {
            func_80058378((struct Unk584BC *)object, object->unk1C + 1, 0);
        }
    }

    if (object->unkAC->unk31A > 0) {
        object->unkAC->unk31A -= arg1;
    }

    if (object->unkAC->unk318 > 0) {
        object->unkAC->unk318 -= arg1;
        if (object->unkAC->unk318 > 0) {
            return 0;
        }
        object->unk24 &= -0xA6;
        object->unkDC->unk24 &= -0xA6;
        func_80058714(object);
        return 0;
    }

    v = object->unkB6;
    data = *(struct Unk588D4Data *volatile *)&object->unkAC;
    s2 = arg1 + v;
    object->unkB6 = s2 & 7;
    s2 >>= 3;
    v = object->unkB7;
    s4 = arg1 + v;
    object->unkB7 = s4 & 1;
    s4 >>= 1;

    c1 = data->unkC1;
    if (D_8009FA94->unk5C == 1) {
        *(s32 *)((u32)data + (data->unkAD << 2) + 0x28) += arg1;
        if (object->unkAC->unk24 == 0 && *(s32 *)((c1 << 2) + (u32)D_8009FA94 + 0x84) > 0) {
            object->unkAC->unk24 = D_8009FAA4;
        }
    }

    if (c1 < 2) {
        if (D_8009FA00 == 4) {
            func_800576A8((u8 *)data + 0x30);
            s2 = func_800586D4(object, arg1, s2, &locals.out1C);
            if (locals.out1C & 1) {
                locals.msg.unk49 = 0x40;
                locals.msg.unk44 = (u8 *)object->unkDC + 0x28;
                func_800402DC(object, 4, &locals.msg, 1, 0);
                func_80046C84(0x15, object);
            }
            if (s2 != 0) {
                hitarg = 0;
                hitcode = object->unk14;
                locals.hit.kind = 1;
                locals.hit.damage = s2;
                locals.hit.from.attacker = object;
                locals.hit.unk00 = hitcode;
                func_800584BC(0xE, (struct Unk584BC *)object, &locals.hit, hitarg, 0);
            }
        }
        if (data->unk6C == 0) {
            func_8006C34C((struct Unk55E10 *)object, 1);
        }
    }

    func_80010750(object->unk2C);
    offset = object->unk70 << 4;
    __asm__("" : "=r"(offset) : "0"(offset));
    offset += 8;
    v = (s32)object->unk28->unk34;
    func_80010234((void *)(v + offset), object->unk5C, 1);
    func_80057E40(object->unkDC, arg1);
    func_8006C3E0(object, arg1);
    if (D_8009FA00 == 4) {
        func_80055E10((struct Unk55E10 *)object, arg1);
    }
    func_800569EC(object, arg1, s4);

    if (data->unk324 > 0) {
        data->unk324 -= arg1;
    }
    if (data->unk326 > 0) {
        data->unk326 -= arg1;
    }
    return 0;
}
