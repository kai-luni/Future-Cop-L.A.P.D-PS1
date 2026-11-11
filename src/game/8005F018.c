#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct ModelInstance;
struct Unk73500;
struct Unk402DCOwner;

struct ActorData {
    u8 pad00[1];
    u8 unk01;
    u8 pad02[0x0A];
    u16 unk0C;
    s16 unk0E;
    u8 pad10[6];
    s16 unk16;
    u8 pad18[2];
    u8 unk1A;
    u8 unk1B;
};

struct Actor5F018 {
    struct ActorData *data;
    u8 pad04[0x1F];
    s8 unk23;
};

struct Obj5F018 {
    u8 pad00[4];
    u8 unk04;
    u8 pad05[0x10];
    u8 unk15;
    u8 pad16[0x08];
    u8 unk1E;
    u8 pad1F[0x15];
    s16 unk34;
    s16 unk36;
    s16 unk38;
    u8 pad3A[1];
    u8 unk3B;
    u8 unk3C;
    u8 unk3D;
    s16 unk3E;
    s16 unk40;
    s16 unk42;
    s16 unk44;
    u8 pad46[2];
    s16 unk48;
    u16 unk4A;
};

struct Obj1_5F018 {
    u8 pad00[0x14];
    u16 unk14;
    u8 pad16[0x62];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
};

struct Coll5F018 {
    u8 pad00[4];
    void (*callback)(s32, void *, void *, s32);
    u8 pad08[0x54];
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    u8 pad68[6];
    u16 unk6E;
};

struct Effect5F018 {
    s32 unk00;
    u8 pad04[0x0C];
    struct Vec3s32 position;
    u8 pad1C[0x28];
    void *unk44;
    u8 unk48;
    u8 unk49;
    u8 pad4A[6];
};

struct Callback5F018 {
    s32 unk00;
    s16 unk04;
    u8 unk06;
    u8 pad07;
    void *unk08;
    u8 pad0C[4];
};

struct Locals5F018 {
    struct Vec3s32 vec38;
    u8 pad0[4];
    struct Vec3s32 vec48;
    u8 pad1[4];
    struct Vec3s32 vec58;
    u8 pad2[4];
    struct Effect5F018 effect;
    s32 sin1;
    s32 cos1;
    s32 sin2;
    s32 cos2;
    struct Callback5F018 callback;
    u8 unkD8;
    u8 pad3[3];
};

#define MUL16(out, a, b) \
    __asm__("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")

extern void *D_800A7040[][0x100];

extern void func_80017668(struct ModelInstance *instance, s32 part, s32 *out);
extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);
extern s32 func_80019668(void);
extern s32 func_80019904(s32 arg0);
extern s32 func_80019FCC(struct Vec3s32 *v);
extern s32 func_8001AB00(struct Vec3s32 *a, struct Vec3s32 *b);
extern struct Unk73500 *func_80073500(struct Vec3s32 *position, s32 arg1, s32 arg2, s32 arg3);
extern void func_8007377C(void *a0, void *a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11, s32 a12, s32 a13);
extern s32 func_800402DC(struct Unk402DCOwner *owner, u8 column, s32 arg2, s32 row, s32 arg4);
extern s32 func_800684AC(struct Vec3s32 *pos, struct Vec3s32 *in, struct Vec3s32 *out, s32 arg3, s32 arg4);

void func_8005F018(struct Actor5F018 *arg0, struct Obj1_5F018 *arg1, struct Vec3s32 *pos, struct Coll5F018 *arg3, u8 arg4)
{
    struct Locals5F018 locals;
    struct Obj5F018 *obj;
    s32 s0;
    register s32 t2 __asm__("$10");
    u8 flag;

    obj = 0;
    flag = 0;

    if (arg0->data->unk01 == 0xB && arg0->data->unk1B != 0) {
        obj = (struct Obj5F018 *)D_800A7040[0][arg0->data->unk1B];
        if (obj != 0 && obj->unk04 != 0x58) {
            obj = 0;
        }
    }

    if (obj != 0) {
        t2 = obj->unk15;
        __asm__("" : "=r"(t2) : "0"(t2));
        locals.unkD8 = t2;
    } else {
        locals.unkD8 = arg0->data->unk1B;
    }

    if (arg3 != 0) {
        s32 dist;
        s32 factor;
        register s32 square __asm__("$11");
        register s32 comparison __asm__("$2");

        if (obj != 0) {
            if (obj->unk1E < 4) {
                func_80017668((struct ModelInstance *)((u8 *)arg3 + 0x28), (s8)obj->unk1E, &locals.vec38.x);
                goto common;
            }
            if (arg0->data->unk0E != 0) {
                s0 = arg0->data->unk0E;
            } else if (obj->unk34 < 0) {
                s0 = ((s32)(arg3->unk6E << 16)) >> 17;
            } else {
                s0 = obj->unk34 << 4;
            }
        } else {
            if (arg0->data->unk0E != 0) {
                s0 = arg0->data->unk0E;
            } else {
                s0 = ((s32)(arg3->unk6E << 16)) >> 17;
            }
        }

        flag = 1;

        func_8001846C(func_80019668() & 0xFFF, &locals.sin1, &locals.cos1);
        func_8001846C(func_80019668() & 0xFFF, &locals.sin2, &locals.cos2);

        {
            register s32 ma __asm__("$4");
            register s32 mb __asm__("$3");
            register s32 first __asm__("$5");
            register s32 second __asm__("$11");
            register s32 temp __asm__("$2");
            ma = locals.sin1 >> 2;
            mb = locals.cos2 >> 2;
            __asm__("" : "+r"(ma), "+r"(mb));
            first = mb * ma;
            temp = locals.cos1 >> 2;
            __asm__("" : "=r"(temp) : "0"(temp));
            __asm__("" : "=r"(first) : "0"(first));
            second = mb * temp;
            locals.cos1 = temp;
            temp = first >> 12;
            __asm__("" : "=r"(temp) : "0"(temp));
            MUL16(t2, temp, s0);
            locals.vec38.x = t2;
            __asm__("" : "=r"(second) : "0"(second));
            temp = second >> 12;
            __asm__("" : "=r"(temp) : "0"(temp));
            __asm__ volatile("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8"
                             : "=r"(t2) : "r"(temp), "r"(s0) : "$8", "$9");
            locals.sin1 = ma;
            locals.cos2 = mb;
            temp = locals.sin2;
            __asm__("" : "=r"(temp) : "0"(temp));
            __asm__("" : "=r"(t2) : "0"(t2), "m"(locals.sin1), "m"(locals.cos2));
            locals.vec38.y = t2;
            MUL16(t2, temp, s0);
            locals.vec38.z = t2;
        }

        locals.vec58.x = pos->x - arg3->unk5C;
        locals.vec58.y = pos->y - arg3->unk60;
        locals.vec58.z = pos->z - arg3->unk64;

        dist = func_80019FCC(&locals.vec58);
        s0 = arg0->data->unk16;

        square = s0 * s0;
        __asm__("" : "=r"(square) : "0"(square));
        comparison = square >> 12;
        __asm__("" : "=r"(comparison) : "0"(comparison));
        if (comparison < (dist >> 4)) {
            arg3 = 0;
            {
                register s32 root __asm__("$2");
                register s32 shifted __asm__("$3");
                root = func_80019904(dist);
                shifted = s0 << 4;
                __asm__("" : "=r"(root), "=r"(shifted) : "0"(root), "1"(shifted));
                MUL16(factor, shifted, root);
            }
            {
                register s32 result __asm__("$3");
                register s32 origin __asm__("$2");
                result = locals.vec58.x;
                origin = pos->x;
                __asm__("" : "+r"(result), "+r"(origin));
                MUL16(result, result, factor);
                locals.vec38.x = origin - result;
            }
            {
                register s32 result __asm__("$3");
                register s32 origin __asm__("$2");
                result = locals.vec58.y;
                origin = pos->y;
                __asm__("" : "+r"(result), "+r"(origin));
                MUL16(result, result, factor);
                locals.vec38.y = origin - result;
            }
            {
                register s32 result __asm__("$3");
                register s32 origin __asm__("$2");
                result = locals.vec58.z;
                origin = pos->z;
                __asm__("" : "+r"(result), "+r"(origin));
                MUL16(result, result, factor);
                locals.vec38.z = origin - result;
            }
        } else if (func_8001AB00(&locals.vec58, &locals.vec38) < 0) {
            locals.vec38.x = arg3->unk5C - locals.vec38.x;
            locals.vec38.y = arg3->unk60 - locals.vec38.y;
            locals.vec38.z = arg3->unk64 - locals.vec38.z;
        } else {
            locals.vec38.x += arg3->unk5C;
            locals.vec38.y += arg3->unk60;
            locals.vec38.z += arg3->unk64;
        }
    } else {
        {
            register s32 rhs __asm__("$3");
            register s32 lhs __asm__("$2");
            rhs = (s32)arg0->data;
            lhs = arg1->unk78;
            rhs = *(s16 *)((u8 *)rhs + 0x16);
            lhs >>= 4;
            t2 = lhs * rhs;
            __asm__("" : "=r"(t2) : "0"(t2));
            lhs = pos->x;
            rhs = t2 >> 8;
            __asm__("" : "+r"(rhs), "+r"(lhs));
            locals.vec38.x = lhs + rhs;
        }
        {
            register s32 rhs __asm__("$3");
            register s32 lhs __asm__("$2");
            rhs = (s32)arg0->data;
            lhs = arg1->unk7C;
            rhs = *(s16 *)((u8 *)rhs + 0x16);
            lhs >>= 4;
            t2 = lhs * rhs;
            __asm__("" : "=r"(t2) : "0"(t2));
            lhs = pos->y;
            rhs = t2 >> 8;
            __asm__("" : "+r"(rhs), "+r"(lhs));
            locals.vec38.y = lhs + rhs;
        }
        {
            register s32 rhs __asm__("$3");
            register s32 lhs __asm__("$2");
            rhs = (s32)arg0->data;
            lhs = arg1->unk80;
            rhs = *(s16 *)((u8 *)rhs + 0x16);
            lhs >>= 4;
            t2 = lhs * rhs;
            __asm__("" : "=r"(t2) : "0"(t2));
            lhs = pos->z;
            rhs = t2 >> 8;
            __asm__("" : "+r"(rhs), "+r"(lhs));
            locals.vec38.z = lhs + rhs;
        }
    }

common:
    if (func_800684AC(pos, &locals.vec38, &locals.vec48, 0, 1) != 0) {
        register s32 x __asm__("$2");
        register s32 y __asm__("$3");
        register s32 z __asm__("$4");
        x = locals.vec48.x;
        y = locals.vec48.y;
        z = locals.vec48.z;
        __asm__("" : "=r"(x), "=r"(y), "=r"(z) : "0"(x), "1"(y), "2"(z));
        locals.vec38.x = x;
        locals.vec38.y = y;
        locals.vec38.z = z;
        __asm__ volatile("" : : "m"(locals.vec38) : "memory");
        flag = 1;
    } else if (arg3 != 0) {
        locals.callback.unk06 = arg4;
        locals.callback.unk04 = arg0->data->unk0C;
        locals.callback.unk00 = arg1->unk14;
        locals.callback.unk08 = arg1;
        arg3->callback(0xE, arg3, &locals.callback, 0);
    }

    if (arg0->data->unk01 == 0xB) {
        struct Unk73500 *node1;
        struct Unk73500 *node2;
        s32 a2;

        if (obj != 0) {
            node1 = func_80073500(pos, 0, obj->unk48, 0);
            if (obj->unk4A & 8) {
                a2 = 0;
            } else {
                a2 = obj->unk48;
            }
            node2 = func_80073500(&locals.vec38, 0, a2, 0);
            func_8007377C(node1, node2, obj->unk36, obj->unk38, obj->unk3B, obj->unk3C, obj->unk3D, obj->unk3E, obj->unk40, obj->unk42, obj->unk44, 0, 0, 1);
        } else {
            node1 = func_80073500(pos, 0, 0x18, 0);
            node2 = func_80073500(&locals.vec38, 0, 0x10, 0);
            func_8007377C(node1, node2, 0x20, 1, 0x7F, 0x7F, 0xFF, 0x2AA, 0x4000, -0x3000, 0, 0, 0, 1);
        }
    }

    if (flag) {
        register s32 x __asm__("$2");
        register s32 y __asm__("$3");
        register s32 z __asm__("$4");
        x = locals.vec38.x;
        y = locals.vec38.y;
        z = locals.vec38.z;
        __asm__("" : "=r"(x), "=r"(y), "=r"(z) : "0"(x), "1"(y), "2"(z));
        locals.effect.position.x = x;
        locals.effect.position.y = y;
        locals.effect.position.z = z;
        __asm__ volatile("" : : "m"(locals.effect.position) : "memory");
        locals.effect.unk49 = 4;
        locals.effect.unk00 = 0x300;
        if (arg0->data->unk1A != 0) {
            func_800402DC(0, arg0->data->unk1A, (s32)&locals.effect, 0, 0);
        }
    }

    obj = (struct Obj5F018 *)(u32)locals.unkD8;
    if (obj != 0) {
        struct ModelInstance *instance;

        instance = (struct ModelInstance *)((u8 *)arg1 + 0x28);
        func_80017668(instance, arg0->unk23, &locals.effect.position.x);
        locals.effect.unk00 = 0x300;
        locals.effect.unk44 = instance;
        locals.effect.unk48 = 0;
        locals.effect.unk49 = 0x65;
        func_800402DC(0, (u32)obj, (s32)&locals.effect, 0, 0);
    }
}
