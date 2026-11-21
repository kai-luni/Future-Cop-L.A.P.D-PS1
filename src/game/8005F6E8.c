// Categories: Gameplay > AI > Attack Dispatch | Graphics > Effects > Dispatcher | Physics > Collision
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

extern u8 gTracerCoreColour[] __asm__("D_8009F378");

extern void gTransformPartVertex(void *instance, s32 part, s32 *out) __asm__("func_80017668");
extern s32 func_80019668(void);
extern void gNormalizeVector(s32 *vec) __asm__("func_8001A180");
extern s32 gVectorDot(s32 *coord, s32 *vec) __asm__("func_8001AB00");
extern s32 gSpawnTypeAtOwner(void *owner, u8 column, s32 arg2, s32 row, s32 arg4) __asm__("func_800402DC");
extern void gPlayPositionalSoundEffect(s32 id, s32 arg1, s32 arg2, s32 arg3) __asm__("func_80047284");
extern void gAttachNearPlayerEffect(void *a0, s32 a1, struct S1_5F6E8 *a2, void *a3, s32 a4, void *a5) __asm__("func_8005BDE4");
extern void gAttachPartEffect(void *a0, s32 a1, struct S1_5F6E8 *a2, void *a3, s32 a4, void *a5) __asm__("func_8005CBCC");
extern s32 gWeaponEffectUpdate(void *a0, void *a1, s32 a2) __asm__("func_8005D690");
extern s32 gSolveLeadDirection(s32 a0, void *a1, s32 a2, s32 a3, void *a4, void *a5, u32 a6) __asm__("func_8005E2A0");
extern s32 gSpawnDirectShot(void *a0, s32 a1, void *a2, void *a3, void *a4, void *a5, u32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11) __asm__("func_8005E47C");
extern s32 gSolveLobVector(s32 a0, void *a1, void *a2, void *a3, s32 a4) __asm__("func_8005E624");
extern s32 gSpawnLobShot(s32 a0, void *a1, void *a2, void *a3, void *a4, void *a5, void *a6, u32 a7, s32 a8) __asm__("func_8005E9E4");
extern s32 gSpawnTargetedShot(void *a0, void *a1, void *a2, void *a3, void *a4, void *a5, u32 a6) __asm__("func_8005EB74");
extern s32 gSpawnStaticProjectile(void *a0, void *a1, void *a2, void *a3, u32 a4) __asm__("func_8005ECB8");
extern s32 gSpawnJitterProjectile(void *a0, void *a1, void *a2, void *a3, void *a4, s32 a5, u32 a6) __asm__("func_8005EE04");
extern s32 gFireRayAttack(void *a0, void *a1, void *a2, void *a3, s32 a4) __asm__("func_8005F018");
extern s32 gTraceMapRay(void *a0, void *a1, void *a2, s32 a3, s32 a4) __asm__("func_800684AC");
extern s32 func_8006F0E8(s32 a0, s32 a1, s32 a2, u32 a3, void *a4, void *a5);
s32 gFireEntityWeapon(struct S3_5F6E8 *a0, struct S6_5F6E8 *a1, s32 a2, s32 a3) __asm__("func_8005F6E8");


/**
 * @brief Actor attack dispatcher: resolves the muzzle and aim direction,
 *        applies jitter and angle gates, then fires the attack-type-specific
 *        effect and decrements the ammo.
 *
 * @param a0 Attack descriptor; +0 the weapon definition, +4 the flag word,
 *        +6/+8 two spawn parameters, +0x18 the target object, +0x1C the
 *        target id, +0x20 the ammo counter, +0x22/+0x24/+0x26 cooldown
 *        fields, +0x23 the model part index.
 * @param a1 Actor block; +0x28 the model instance, +0x44 the facing vector,
 *        +0x50/+0x58 position words, +0x84 a state word.
 * @param a2 Forwarded to the proximity/sprite spawners.
 * @param a3 Low byte 0 skips the fire (still runs the feedback call).
 * @return 1 when the attack sequence ran, 0 on a gate failure.
 *
 * @par Confirmed
 * The target id check (s1->unk08 != a0->unk1C) fails early. The muzzle
 * position comes from func_80017668(a1 + 0x28, a0->unk23). Flags s4 pack
 * bits 0x80/0x200/0x400 of a0->unk4 into 0x80000/0x200/1. The first switch
 * over the attack type (s2->unk01) builds the direction: types 1/3/9..14/
 * 17..19/0x15 normalise (target - muzzle) via func_8001A180; type 7 uses
 * the actor's own vector; type 2 runs the lead prediction gSolveLeadDirection
 * unless the target class (unk16 1 or 0x12) has a positive timer at
 * +0x324; types 4/20, 5, 6, 8 fire their spawners directly. Jitter: flag
 * 0x8 adds (rand & 0x1FFF) - 0x1000 to y, flag 0x10 to x/z. The angle gate
 * dots the actor facing with the aim (func_8001AB00): flag 0x800 fails
 * below 0xF22F, otherwise three bands (> 0xEC82, <= 0xFFFF137D, between)
 * map to mask bits 1/2/4 tested against flags 0x807. The second switch:
 * type 12 clips via func_800684AC, delivers message 0x0E on a miss and
 * spawns the tracer func_8006F0E8(0x140, 0x60, 0x30000, ...); 14/17 call
 * func_8005BDE4/gAttachPartEffect; 10/11 call func_8005F018 (subtype 1/8);
 * 8/9 func_8005EE04 (9 with gSolveLobVector first); 13 gWeaponEffectUpdate; the
 * default requires a clear func_800684AC line of sight and fires
 * gSpawnDirectShot. The tail decrements ammo, spawns the secondary object
 * via func_800402DC (column s2->unk1B, impact column unk1A when flagged)
 * and always calls func_80047284(s2->unk1C, ...). The fail path bumps
 * unk26 and drains unk24 by (rand + 0xC8) & 0xFF.
 *
 * @par Likely role
 * The enemy weapon controller: each actor class carries attack descriptors
 * whose type byte selects beams, projectiles, mines or sprite effects, and
 * the angle bands prevent firing outside the actor's facing cone.
 *
 * @par Unknown
 * The exact angle units of the 0xF22F/0xEC82/0xFFFF137D thresholds and the
 * feedback semantics of func_80047284.
 *
 * @par Callers
 * func_8002B784, func_8002E630, func_80038A00, func_80060238.
 */
s32 gFireEntityWeapon(struct S3_5F6E8 *a0, struct S6_5F6E8 *a1, s32 a2, s32 a3)
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

    gTransformPartVertex((void *)((u8 *)a1 + 0x28), a0->unk23, (s32 *)pos);

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
            ret = gSpawnJitterProjectile((u8 *)a0->unk0 + 0x0C, (u8 *)a0->unk0 + 0x20,
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
        gNormalizeVector((s32 *)vec);
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
        gSolveLeadDirection((s32)pos, vec, s2->unk12 << 4, s2->unk16 << 4,
                      (u8 *)s1 + 0x5C, (u8 *)s1 + 0x78, s1->unk84);
        goto L9B4;

    case 4:
    case 20:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        ret = gSpawnLobShot(s2->unk01, (u8 *)a0->unk0 + 0x0C,
                            (u8 *)a0->unk0 + 0x20, pos, a1, s1,
                            (u8 *)s1 + 0x5C, s4, s2->unk1B);
        goto FE28;

    case 5:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        ret = gSpawnStaticProjectile((u8 *)a0->unk0 + 0x0C, (u8 *)a0->unk0 + 0x20,
                            pos, a1, s4);
        goto FE28;

    case 6:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        ret = gSpawnTargetedShot((u8 *)a0->unk0 + 0x0C, (u8 *)a0->unk0 + 0x20,
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
        dist = gVectorDot(&a1->unk44, (s32 *)vec);
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
        memcpy(tmp60, gTracerCoreColour, 4);
        if (gTraceMapRay(pos, (u8 *)s1 + 0x5C, vec2, 0, 1) == 0) {
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
        gAttachNearPlayerEffect(a1, a2, s1, (u8 *)a1 + 0x28, a0->unk23, (u8 *)a0->unk0 + 0x0C);
        s7 = 0;
        goto FE2C;

    case 17:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        gAttachPartEffect(a1, a2, s1, (u8 *)a1 + 0x28, a0->unk23, (u8 *)a0->unk0 + 0x0C);
        s7 = 0;
        goto FE2C;

    case 11:
        s7 = 0;
    case 10:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        gFireRayAttack(a0, a1, pos, s1, (s2->unk01 == 11) ? 8 : 1);
        ret = 0;
        goto FE28;

    case 8:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        ret = gSpawnJitterProjectile((u8 *)s2 + 0x0C, (u8 *)s2 + 0x20, pos, a1, vec, 0, s4);
        goto FE28;

    case 9:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        gSolveLobVector(s2->unk12 << 4, vec, pos, (u8 *)s1 + 0x5C, s2->unk16 << 4);
        ret = gSpawnJitterProjectile((u8 *)s2 + 0x0C, (u8 *)s2 + 0x20, pos, a1, vec, 1, s4);
        goto FE28;

    case 13:
        if ((a3 & 0xFF) == 0) {
            goto FEEC;
        }
        gWeaponEffectUpdate(a1, (u8 *)a0->unk0 + 0x0C, 0);
        ret = 0;
        s7 = 0;
        goto FE2C;

    default:
        if (s1 != 0) {
            if (gTraceMapRay(pos, (u8 *)s1 + 0x5C, 0, 0, 1) != 0) {
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
        ret = gSpawnDirectShot((u8 *)s2 + 0x0C, (s2->unk20 != 0) ? *(s32 *)s2->unk20 : 0, pos, vec, a1, s1,
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
        gSpawnTypeAtOwner(owner, s2->unk1B, (s32)arg2ptr, 0, 0);
    }

    if (flagC8 != 0) {
        *(struct V3_5F6E8 *)(objBuf + 0x10) = *(struct V3_5F6E8 *)vec2;
        objBuf[0x49] = 5;
        *(u32 *)objBuf = 0x800;
        gSpawnTypeAtOwner(0, s2->unk1A, (s32)objBuf, 0, 0);
    }

FEEC:
    gPlayPositionalSoundEffect(s2->unk1C, a1->unk50, a1->unk58, -1);
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
