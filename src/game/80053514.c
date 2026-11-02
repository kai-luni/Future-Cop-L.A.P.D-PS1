#include "game/types.h"

typedef struct {
    s32 unk00;
    s32 (*unk04)(s32, void *, s32, s32);
    u8 unk08[0x48];
    s32 unk50;
    s32 unk54;
    s32 unk58;
} Unk53514Arg;

typedef struct {
    u8 unk00[0x50];
    s32 unk50;
    s32 unk54;
    s16 unk58;
    s16 unk5A;
    s32 unk5C;
    s32 unk60;
} Unk53514Sub;

typedef struct {
    u8 unk00[0x50];
    s32 unk50;
    s32 unk54;
    s32 unk58;
    u8 unk5C[0x44];
    s32 unkA0;
    s32 unkA4;
    s32 unkA8;
    u8 *unkAC;
    u16 unkB0;
    u8 unkB2[0x12];
    void *unkC4;
} Unk53514Obj;

extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);
extern void func_8001A578(s32 angle, s32 *offset, s32 arg2);

void func_80053514(Unk53514Obj *arg0, Unk53514Arg *arg1)
{
    register Unk53514Obj *obj __asm__("$18");
    register Unk53514Arg *ent __asm__("$19");
    register Unk53514Arg *self __asm__("$5");
    Unk53514Sub *sub;
    s32 result;
    register s32 delta __asm__("$16");
    register s32 prev __asm__("$3");
    register s32 unk50v __asm__("$2");
    s32 sub54;
    s32 b0;
    s32 v[4];
    s32 av;
    u16 angle;

    obj = arg0;
    ent = arg1;
    self = arg1;
    __asm__ volatile ("" : "+r" (obj), "+r" (ent), "+r" (self));

    sub = (Unk53514Sub *)(obj->unkAC + 0xEC);
    result = 0x270F;

    if (ent != 0) {
        if (obj->unkC4 == (void *)ent) {
            obj->unk50 += ent->unk50 - sub->unk50;
            obj->unk58 += ent->unk58 - sub->unk54;

            result = ent->unk04(0xF, self, 0, 0);

            prev = sub->unk58;
            delta = result - prev;
            if (delta != 0) {
                angle = (obj->unkB0 + delta) & 0xFFF;
                obj->unkB0 = angle;
                func_8001846C(angle, &obj->unkA0, &obj->unkA8);

                av = sub->unk5C;
                unk50v = sub->unk50;
                av = av - unk50v;
                v[0] = av;
                b0 = sub->unk60;
                sub54 = sub->unk54;
                v[2] = av;
                v[1] = b0 - sub54;
                v[3] = v[1];
                func_8001A578(-delta, &v[2], sub54);

                obj->unk50 += v[2] - v[0];
                obj->unk58 += v[3] - v[1];
            }
        }
    }

    obj->unkC4 = ent;
    if (ent != 0) {
        sub->unk50 = ent->unk50;
        sub->unk54 = ent->unk58;
        if (result == 0x270F) {
            result = ent->unk04(0xF, ent, 0, 0);
        }
        sub->unk58 = result;
        sub->unk5C = obj->unk50;
        sub->unk60 = obj->unk58;
    }
}
