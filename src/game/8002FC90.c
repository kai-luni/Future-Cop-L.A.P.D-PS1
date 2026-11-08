#include "game/types.h"

typedef struct UnkFC90Sub {
    u8 unk00[0x324];
    s16 unk324;
    s16 unk326;
} UnkFC90Sub;

typedef struct UnkFC90Obj {
    u8 unk00[0x16];
    u8 unk16;
    u8 unk17[0x1C - 0x17];
    s16 unk1C;
    s16 unk1E;
    u8 unk20;
    u8 unk21[3];
    u32 unk24;
    u8 unk28[0xAC - 0x28];
    UnkFC90Sub *unkAC;
    u8 unkB0[4];
    struct UnkFC90Obj *unkB4;
} UnkFC90Obj;

typedef struct UnkFA94 {
    u8 unk00[0x5C];
    s32 unk5C;
} UnkFA94;

extern s32 D_8009F9FC;
extern s32 D_8009FA0C;
extern UnkFA94 *D_8009FA94;
extern UnkFC90Obj *D_8009FA58[2];

extern void func_8005A190(UnkFC90Sub *sub, s32 index, s32 value);
extern void func_8003BA50(UnkFC90Obj *obj, s32 value);
extern void func_8006BE90(UnkFC90Obj *obj, s32 index, s32 value, s32 flag);
extern void func_8006BDFC(UnkFC90Obj *obj, s32 index);

void func_8002FC90(UnkFC90Obj *arg0, u32 arg1, s32 arg2, s32 *arg3, s32 *arg4)
{
    register UnkFC90Obj *self __asm__("$20");
    register UnkFC90Obj *obj __asm__("$18");
    register s32 quotient __asm__("$23");
    s32 old;
    register s32 value __asm__("$6");
    register s32 other __asm__("$3");
    register s32 mask __asm__("$16");
    register s32 i __asm__("$19");
    register s32 j __asm__("$17");
    register u32 bits __asm__("$22") = arg1;
    s32 amount = arg2;
    register u32 flag __asm__("$21");
    register u32 flag0 __asm__("$6");
    register u32 temp __asm__("$2");
    register u32 temp8 __asm__("$8");
    volatile u32 flag20000;
    volatile u32 flag40000;

    __asm__("" : "+r"(bits));
    __asm__("" : "+r"(amount));
    temp = 0x1000000;
    __asm__("" : "+r"(temp));
    flag0 = bits & temp;
    if (flag0 == 0) {
        *arg3 += D_8009F9FC;
        if (*arg3 < *arg4) {
            return;
        }
        quotient = *arg3 / *arg4;
        temp8 = quotient * *arg4;
        __asm__("" : "+r"(temp8));
        temp = *arg3 - temp8;
        __asm__("" : "+r"(temp));
        *arg3 = temp;
    }

    self = arg0;
    i = 0;
    if (self->unk16 == 0x12) {
        self = self->unkB4;
        if (self == 0) {
            return;
        }
    }

    flag20000 = bits & 0x20000;
    __asm__ volatile("" : : : "memory");
    flag = flag0;
    __asm__ volatile("" : "+r"(flag));
    temp8 = 0x40000;
    __asm__("" : "+r"(temp8));
    temp8 = bits & temp8;
    __asm__("" : "+r"(temp8));
    flag40000 = temp8;
    __asm__ volatile("" : : : "memory");

    for (; i < 2; i++) {
        __asm__("" : "+r"(i));
        {
            UnkFC90Obj **table = D_8009FA58;
            __asm__ volatile("" : "+r"(table) : : "memory");
            obj = table[i];
        }
        if (obj == 0) {
            continue;
        }
        if (obj != self) {
            if (D_8009FA0C < 2) {
                continue;
            }
            if (D_8009FA94->unk5C != 0) {
                continue;
            }
        }
        temp8 = flag20000;
        __asm__("" : "+r"(temp8));
        if (temp8 != 0) {
            if (flag != 0) {
                value = obj->unk1E;
                if (amount >= 0) {
                    temp = obj->unk1C;
                    __asm__("" : "+r"(temp));
                    other = temp + amount;
                    if (other < value) {
                        value = other;
                    }
                }
            } else {
                value = obj->unk1C + quotient * 2;
                other = obj->unk1E;
                if (other < value) {
                    value = other;
                }
            }
            old = obj->unk1C;
            obj->unk1C = value;
            func_8005A190(obj->unkAC, 0, value);
            if (obj->unk20 != 0) {
                temp8 = 0x40000;
                __asm__("" : "+r"(temp8));
                if ((obj->unk24 & temp8) == 0) {
                    func_8003BA50(obj, old);
                }
            }
        }
        if (obj != self) {
            continue;
        }
        temp8 = flag40000;
        __asm__("" : "+r"(temp8));
        if (temp8 != 0) {
            obj->unkAC->unk324 = 0x34BC;
        }
        if ((bits & 0x80000) != 0) {
            obj->unkAC->unk326 = 0x1770;
        }
        mask = bits & 0xF;
        for (j = 0; mask != 0; mask >>= 1, j++) {
            if (mask & 1) {
                if (flag != 0) {
                    func_8006BE90(obj, j, amount, 1);
                } else {
                    func_8006BE90(obj, j, quotient, 1);
                }
            }
        }
        temp = bits & 0xF0;
        __asm__("" : "+r"(temp));
        mask = temp >> 4;
        for (j = 0; mask != 0; mask >>= 1, j++) {
            if (mask & 1) {
                func_8006BDFC(obj, j);
            }
        }
    }
}
