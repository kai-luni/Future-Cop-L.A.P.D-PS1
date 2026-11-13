#include "game/types.h"

struct S1B4_5F6E8 {
    u8 pad[0xAC];
    void *unkAC;
};

struct S1_5F6E8 {
    u8 unk00[4];
    void (*unk04)(s32, struct S1_5F6E8 *, u8 *, s32);
    s32 unk08;
    u8 unk0C[0x0A];
    u8 unk16;
    u8 unk17[0x39];
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    u8 unk68[0x10];
    void *unk78;
    u8 unk7C[8];
    u32 unk84;
    u8 unk88[0x24];
    void *unkAC;
    u8 unkB0[4];
    void *unkB4;
};

struct S2_5F6E8 {
    u8 unk00;
    u8 unk01;
    u8 unk02[0x0A];
    u16 unk0C;
    u8 unk0E[4];
    s16 unk12;
    u16 unk14;
    s16 unk16;
    u8 unk18[2];
    u8 unk1A;
    u8 unk1B;
    u8 unk1C;
    u8 unk1D[2];
    u8 unk1F;
    void *unk20;
};

struct S3_5F6E8 {
    struct S2_5F6E8 *unk0;
    u16 unk4;
    u8 unk6;
    u8 unk7;
    s16 unk8;
    u8 unkA[0x0E];
    struct S1_5F6E8 *unk18;
    s32 unk1C;
    s16 unk20;
    u8 unk22;
    s8 unk23;
    u16 unk24;
    u16 unk26;
};

struct S6_5F6E8 {
    u8 unk00[0x14];
    u16 unk14;
    u8 unk16[0x12];
    u8 model[0x1C];
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
    s32 unk58;
    u8 unk5C[0x28];
    s32 unk84;
};

struct V3_5F6E8 { u32 x, y, z; };

struct Msg_5F6E8 {
    s32 unk0;
    u16 unk4;
    u8 unk6;
    u8 pad7;
    void *unk8;
};

extern u8 D_8009F378[];

extern void func_80017668(void *instance, s32 part, s32 *out);
extern s32 func_80019668(void);
extern void func_8001A180(s32 *vec);
extern s32 func_8001AB00(s32 *coord, s32 *vec);
extern s32 func_800402DC(void *owner, u8 column, s32 arg2, s32 row, s32 arg4);
extern void func_80047284(s32 id, s32 arg1, s32 arg2, s32 arg3);
extern void func_8005BDE4(void *a0, s32 a1, struct S1_5F6E8 *a2, void *a3, s32 a4, void *a5);
extern void func_8005CBCC(void *a0, s32 a1, struct S1_5F6E8 *a2, void *a3, s32 a4, void *a5);
extern s32 func_8005D690(void *a0, void *a1, s32 a2);
extern s32 func_8005E2A0(s32 a0, void *a1, s32 a2, s32 a3, void *a4, void *a5, u32 a6);
extern s32 func_8005E47C(void *a0, s32 a1, void *a2, void *a3, void *a4, void *a5, u32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11);
extern s32 func_8005E624(s32 a0, void *a1, void *a2, void *a3, s32 a4);
extern s32 func_8005E9E4(s32 a0, void *a1, void *a2, void *a3, void *a4, void *a5, void *a6, u32 a7, s32 a8);
extern s32 func_8005EB74(void *a0, void *a1, void *a2, void *a3, void *a4, void *a5, u32 a6);
extern s32 func_8005ECB8(void *a0, void *a1, void *a2, void *a3, u32 a4);
extern s32 func_8005EE04(void *a0, void *a1, void *a2, void *a3, void *a4, s32 a5, u32 a6);
extern s32 func_8005F018(void *a0, void *a1, void *a2, void *a3, s32 a4);
extern s32 func_800684AC(void *a0, void *a1, void *a2, s32 a3, s32 a4);
extern s32 func_8006F0E8(s32 a0, s32 a1, s32 a2, u32 a3, void *a4, void *a5);

s32 func_8005F6E8(struct S3_5F6E8 *a0, struct S6_5F6E8 *a1, s32 a2, s32 a3)
{
    struct S1_5F6E8 *s1;
    struct S2_5F6E8 *s2;
    u32 s4;
    u32 vec[3];
    u32 pos[3];
    u32 vec2[3];
    u8 tmp60[4];
    u8 objBuf[0x60];
    u8 flagC8;
    s32 ret;
    s32 temp;
    u32 rnd;
    s32 dist;
    s32 mask;
    s32 a1val;
    u8 s7;
    void *owner;
    void *arg2ptr;

    s1 = a0->unk18;
    if (s1 != 0 && s1->unk08 != a0->unk1C) {
        goto fail;
    }

    func_80017668((void *)((u8 *)a1 + 0x28), a0->unk23, (s32 *)pos);

    s4 = ((a0->unk4 & 0x80) != 0) << 19;
    if (a0->unk4 & 0x200) {
        s4 |= 0x200;
    }
    if (!(a0->unk4 & 0x400)) {
        s4 |= 1;
    }

    s2 = a0->unk0;
    s7 = (s2->unk1B != 0);
    flagC8 = 0;

    switch (s2->unk01) {
    case 8:
        if (a1->unk84 != 0) {
            if ((a3 & 0xFF) == 0) {
                goto FEEC;
            }
            ret = func_8005EE04((u8 *)a0->unk0 + 0x0C, (u8 *)a0->unk0 + 0x20,
                                pos, a1, 0, 0, s4);
            goto FE28;
        }
    case 1:
    case 3:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 17:
    case 18:
    case 19:
    case 0x15:
        if (s1 == 0) {
            goto L85C;
        }
    L814:
        vec[0] = s1->unk5C - pos[0];
        vec[1] = s1->unk60 - pos[1];
        vec[2] = s1->unk64 - pos[2];
        func_8001A180((s32 *)vec);
        goto L9B4;
    case 7:
    L85C:
        *(struct V3_5F6E8 *)vec = *(struct V3_5F6E8 *)&a1->unk44;
        goto L9B4;

    case 2:
        if (s1 == 0) {
            goto L85C;
        }
        if (s1->unk16 == 1) {
            if (*(s16 *)((u8 *)s1->unkAC + 0x324) > 0) {
                goto L814;
            }
        }
        if (s1->unk16 == 0x12) {
            if (*(s16 *)((u8 *)((struct S1B4_5F6E8 *)s1->unkB4)->unkAC + 0x324) > 0) {
                goto L814;
            }
        }
        func_8005E2A0((s32)pos, vec, s2->unk12 << 4, s2->unk16 << 4,
                      (u8 *)s1 + 0x5C, (u8 *)s1 + 0x78, s1->unk84);
        goto L9B4;

    case 4:
    case 20:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        ret = func_8005E9E4(s2->unk01, (u8 *)a0->unk0 + 0x0C,
                            (u8 *)a0->unk0 + 0x20, pos, a1, s1,
                            (u8 *)s1 + 0x5C, s4, s2->unk1B);
        goto FE28;

    case 5:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        ret = func_8005ECB8((u8 *)a0->unk0 + 0x0C, (u8 *)a0->unk0 + 0x20,
                            pos, a1, s4);
        goto FE28;

    case 6:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        ret = func_8005EB74((u8 *)a0->unk0 + 0x0C, (u8 *)a0->unk0 + 0x20,
                            pos, a1, s1, (u8 *)s1 + 0x5C, s4);
        goto FE28;

    default:
        goto L9B4;
    }

L9B4:
    if (a0->unk4 & 0x8) {
        rnd = func_80019668();
        vec[1] += (rnd & 0x1FFF) - 0x1000;
    }

    if (a0->unk4 & 0x10) {
        u32 r;
        r = func_80019668();
        vec[0] += (r & 0x1FFF) - 0x1000;
        vec[2] += ((r >> 16) & 0x1FFF) - 0x1000;
    }

    if ((a0->unk4 & 0x807) != 0) {
        dist = func_8001AB00(&a1->unk44, (s32 *)vec);
        if (a0->unk4 & 0x800) {
            if (dist <= 0xF22F) {
                goto fail;
            }
        } else {
            if (dist > 0xEC82) {
                mask = 1;
            } else if (dist <= (s32)0xFFFF137D) {
                mask = 2;
            } else {
                mask = 4;
            }
            if (a0->unk4 & mask) {
                goto fail;
            }
        }
    }

    switch (s2->unk01) {
    case 12:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        memcpy(tmp60, D_8009F378, 4);
        if (func_800684AC(pos, (u8 *)s1 + 0x5C, vec2, 0, 1) == 0) {
            *(struct V3_5F6E8 *)vec2 = *(struct V3_5F6E8 *)&s1->unk50;
            if (s1 != 0) {
                ((struct Msg_5F6E8 *)objBuf)->unk6 = s2->unk1F;
                ((struct Msg_5F6E8 *)objBuf)->unk4 = s2->unk0C;
                ((struct Msg_5F6E8 *)objBuf)->unk0 = a1->unk14;
                ((struct Msg_5F6E8 *)objBuf)->unk8 = a1;
                s1->unk04(0x0E, s1, objBuf, 0);
            }
        }
        s7 = 0;
        temp = func_8006F0E8(0x140, 0x60, 0x30000, *(u32 *)tmp60, pos, vec2);
        if ((u8)temp) {
            flagC8 = (s2->unk1A != 0);
            goto FE28;
        }
        goto FE2C;

    case 14:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        func_8005BDE4(a1, a2, s1, (u8 *)a1 + 0x28, a0->unk23, (u8 *)a0->unk0 + 0x0C);
        s7 = 0;
        goto FE2C;

    case 17:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        func_8005CBCC(a1, a2, s1, (u8 *)a1 + 0x28, a0->unk23, (u8 *)a0->unk0 + 0x0C);
        s7 = 0;
        goto FE2C;

    case 11:
        s7 = 0;
    case 10:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        func_8005F018(a0, a1, pos, s1, (s2->unk01 == 11) ? 8 : 1);
        ret = 0;
        goto FE28;

    case 8:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        ret = func_8005EE04((u8 *)s2 + 0x0C, (u8 *)s2 + 0x20, pos, a1, vec, 0, s4);
        goto FE28;

    case 9:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        func_8005E624(s2->unk12 << 4, vec, pos, (u8 *)s1 + 0x5C, s2->unk16 << 4);
        ret = func_8005EE04((u8 *)s2 + 0x0C, (u8 *)s2 + 0x20, pos, a1, vec, 1, s4);
        goto FE28;

    case 13:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        func_8005D690(a1, (u8 *)a0->unk0 + 0x0C, 0);
        ret = 0;
        s7 = 0;
        goto FE2C;

    default:
        if (s1 != 0) {
            if (func_800684AC(pos, (u8 *)s1 + 0x5C, 0, 0, 1) != 0) {
                goto fail;
            }
        }
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        if (s2->unk01 != 3 && s2->unk01 != 0x12 &&
            s2->unk01 != 0x13 && s2->unk01 != 0x15) {
            s1 = 0;
        }
        temp = a0->unk4 & 0x1000;
        mask = (temp != 0);
        if (a0->unk4 & 0x2000) {
            mask |= 2;
        }
        ret = func_8005E47C((u8 *)s2 + 0x0C, (s2->unk20 != 0) ? *(s32 *)s2->unk20 : 0, pos, vec, a1, s1,
                            s4, s2->unk01, a0->unk6, a0->unk8,
                            s2->unk1B, mask);
        goto FE28;
    }

FE28:
FE2C:
    if ((a3 & 0xFF) == 0) {
        goto FEEC;
    }

    a0->unk22 = 0;
    if (a0->unk20 >= 0) {
        a0->unk20 = a0->unk20 - 1;
    }

    if (s7 & 0xFF) {
        if (ret != 0) {
            owner = (void *)ret;
            arg2ptr = 0;
        } else {
            *(struct V3_5F6E8 *)(objBuf + 0x20) = *(struct V3_5F6E8 *)pos;
            owner = 0;
            arg2ptr = objBuf + 0x10;
            objBuf[0x59] = 4;
        }
        func_800402DC(owner, s2->unk1B, (s32)arg2ptr, 0, 0);
    }

    if (flagC8 != 0) {
        *(struct V3_5F6E8 *)(objBuf + 0x10) = *(struct V3_5F6E8 *)vec2;
        objBuf[0x49] = 5;
        *(u32 *)objBuf = 0x800;
        func_800402DC(0, s2->unk1A, (s32)objBuf, 0, 0);
    }

FEEC:
    func_80047284(s2->unk1C, a1->unk50, a1->unk58, -1);
    return 1;

fail:
    if ((a3 & 0xFF) != 0) {
        a0->unk22 = 0;
        a0->unk26++;
        temp = func_80019668();
        a0->unk24 -= (temp + 0xC8) & 0xFF;
    }
    return 0;
}
