#include "game/types.h"

struct V3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Owner6ACB0 {
    u8 pad00[0xA0];
    s32 x;
    s32 y;
    s32 z;
    u8 *state;
    u8 padB0[0x2C];
    u8 *unkDC;
};

struct Tmp60 {
    u8 pad00[0x10];
    struct V3 pos;
    u8 pad1C[0x28];
    void *ptr;
    u8 a;
    u8 b;
    u8 pad4A[6];
};

struct Locals6ACB0 {
    struct V3 vec;
    u8 pad3C[4];
    struct V3 posA;
    u8 pad4C[4];
    struct V3 posB;
    u8 pad5C[4];
    struct Tmp60 t60;
};

extern s32 D_800A8F90[2];

extern void func_80017668(void *instance, s32 part, void *out);
extern void func_8001A180(void *arg0);
extern void func_8005C048(void *object, void *event);
extern s32 func_80046C84(s32 id, s32 arg1);
extern void *func_8006AAEC(u8 id);
extern s32 func_800402DC(void *owner, u8 column, void *arg2, s32 row, s32 arg4);

extern void func_8005E2A0(void *a0, void *a1, s32 a2, s32 a3, void *a4, void *a5, s32 a6);
extern s32 func_8005E47C(void *a0, s32 a1, void *a2, void *a3, void *a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11);
extern s32 func_8005EE04(void *a0, void *a1, void *a2, void *a3, void *a4, s32 a5, s32 a6);
extern s32 func_8005E624(s32 a0, void *a1, void *a2, void *a3, s32 a4);
extern s32 func_8005E9E4(s32 a0, void *a1, void *a2, void *a3, void *a4, s32 a5, void *a6, s32 a7, s32 a8);
extern s32 func_8005ECB8(void *a0, void *a1, void *a2, void *a3, s32 a4);
extern s32 func_8005EB74(void *a0, void *a1, void *a2, void *a3, s32 a4, void *a5, s32 a6);
extern s32 func_8005CBCC(void *a0, void *a1, s32 a2, void *a3, s32 a4, void *a5);
extern s32 func_8005D690(void *a0, void *a1, void *a2);

s32 func_8006ACB0(struct Owner6ACB0 *owner, s32 index)
{
    struct Locals6ACB0 locals;
    register u8 *state __asm__("$19");
    u8 *config;
    u8 *def;
    u8 *result;
    s32 offset;
    s32 s4;
    s32 action;

    result = 0;
    if (D_800A8F90[1] - D_800A8F90[0] < 10) {
        return result;
    }

    state = owner->state;

    {
        s16 h;
        h = *(s16 *)(state + (index << 1) + 0x8A);
        if (h > 0) {
            offset = *(u16 *)(state + (index << 1) + 0x8A) & 1;
        } else {
            offset = *(u16 *)(state + (index << 1) + 0x84) & 1;
        }
    }

    func_80017668((void *)(state + 0x178 + offset * 76), 0, &locals.posA);

    config = (u8 *)func_8006AAEC(*(u8 *)(index + state + 0x90));

    if (index == 1) {
        *(u16 *)(state + 0x174 + offset * 2) = 0x84;
    }

    {
        u8 *model;
        model = *(u8 **)(state + 0x6C);
        if (model != 0) {
            locals.posB = *(struct V3 *)(model + 0x5C);
        } else {
            locals.posB.x = locals.posA.x + owner->x;
            locals.posB.y = locals.posA.y + owner->y;
            locals.posB.z = locals.posA.z + owner->z;
        }
    }

    if (*(s16 *)(state + (index << 1) + 0x8A) > 0) {
        def = config + 0x24;
    } else {
        def = config + 0x04;
    }

    {
        u8 id2;
        id2 = *(u8 *)(def + 0x1C);
        action = *(u8 *)(def + 0x00);
        func_80046C84(id2, (s32)owner);
    }

    if (action == 2 && *(u8 **)(state + 0x6C) == 0) {
        action = 1;
    }

    if (action == 9 || action == 1 || action == 3 ||
        action == 0x12 || action == 0x13 || action == 0x15) {
        locals.vec.x = locals.posB.x - locals.posA.x;
        locals.vec.y = locals.posB.y - locals.posA.y;
        locals.vec.z = locals.posB.z - locals.posA.z;
        func_8001A180(&locals.vec);
    }

    s4 = (*(u8 *)(def + 1) & 1) << 19;
    s4 |= 0x201;

    switch (action) {
    case 1:
    case 2:
    case 3:
    case 0x12:
    case 0x13:
    case 0x15:
        {
            s32 t1;
            s32 t2;
            s32 t0;
            u8 flags;
            u8 *p;
            s32 a1v;
            u32 m2;
            u8 *ap;

            if (action == 2) {
                func_8005E2A0(&locals.posA, &locals.vec,
                              *(s16 *)(def + 0x12),
                              (s32)*(s16 *)(def + 0x16) << 4,
                              &locals.posB,
                              (void *)(*(u8 **)(state + 0x6C) + 0x78),
                              *(s32 *)(*(u8 **)(state + 0x6C) + 0x84));
                t1 = (s32)*(u8 **)(state + 0x6C);
                t2 = 0;
            } else {
                if (action == 3 || action == 0x12 || action == 0x13 || action == 0x15) {
                    t1 = (s32)*(u8 **)(state + 0x6C);
                } else {
                    t1 = 0;
                }
                t2 = *(u8 *)(def + 0x1B);
            }

            flags = *(u8 *)(def + 1);
            m2 = flags & 2;
            t0 = m2 != 0;
            if (flags & 8) {
                t0 |= 2;
            }

            ap = def + 0x0C;
            a1v = 0;
            p = *(u8 **)(config + 0x44);
            if (p != 0) {
                a1v = *(s32 *)p;
            }

            result = (u8 *)func_8005E47C(ap, a1v, &locals.posA, &locals.vec,
                                         owner->unkDC, t1, s4, action,
                                         0xFF, 0, t2, t0);
        }
        break;

    case 8:
        result = (u8 *)func_8005EE04(def + 0x0C, config + 0x44, &locals.posA,
                                     owner->unkDC, &locals.vec, 0, s4);
        break;

    case 9:
        func_8005E624((s32)*(s16 *)(def + 0x12) << 4, &locals.vec, &locals.posA,
                      &locals.posB, (s32)*(s16 *)(def + 0x16) << 4);
        result = (u8 *)func_8005EE04(def + 0x0C, config + 0x44, &locals.posA,
                                     owner->unkDC, &locals.vec, 1, s4);
        break;

    case 4:
    case 0x14:
        result = (u8 *)func_8005E9E4(action, def + 0x0C, config + 0x44, &locals.posA,
                                     owner->unkDC,
                                     (s32)*(u8 **)(state + 0x6C), &locals.posB, s4,
                                     *(u8 *)(def + 0x1B));
        break;

    case 5:
        result = (u8 *)func_8005ECB8(def + 0x0C, config + 0x44, &locals.posA,
                                     owner->unkDC, s4);
        break;

    case 6:
        result = (u8 *)func_8005EB74(def + 0x0C, config + 0x44, &locals.posA,
                                     owner->unkDC,
                                     (s32)*(u8 **)(state + 0x6C), &locals.posB, s4);
        break;

    case 0xF:
        func_8005C048(owner, def + 0x0C);
        return 1;

    case 0x11:
        func_8005CBCC(owner, owner, (s32)*(u8 **)(owner->state + 0x6C),
                      state + 0x178 + offset * 76, 0, def + 0x0C);
        return 1;

    case 0xD:
        return func_8005D690(owner, def + 0x0C, def) & 0xFF;
    }

    if (result != 0 && (*(u8 *)(def + 1) & 4)) {
        *(s32 *)(state + 0x98) = (s32)result;
        *(s32 *)(state + 0x9C) = *(s32 *)(result + 8);
    }

    func_80017668((void *)(owner->unkDC + 0x28), offset, &locals.posA);

    if (*(u8 *)(def + 0x1B) != 0) {
        if (result != 0) {
            func_800402DC(result, *(u8 *)(def + 0x1B), 0, 1, 0);
        } else {
            locals.t60.pos = locals.posA;
            locals.t60.ptr = (void *)(state + 0x178 + offset * 76);
            locals.t60.a = 0;
            locals.t60.b = 0x64;
            func_800402DC(0, *(u8 *)(def + 0x1B), &locals.t60, 1, 0);
        }
    }

    __asm__("" : : "r"(state));
    return 1;
}
