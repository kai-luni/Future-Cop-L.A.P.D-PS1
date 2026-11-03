#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Basis {
    struct Vec3 rows[3];
};

struct OtherChild {
    u8 pad00[0x14];
    u16 unk14;
};

struct Other {
    s32 unk00;
    u8 pad04[4];
    struct OtherChild *unk08;
    u8 pad0C[8];
    u16 unk14;
    u8 pad16[2];
    s16 unk18;
    u8 unk1A;
    u8 pad1B[2];
    u8 unk1D;
    u16 unk1E;
    u16 unk20;
    u8 pad22[2];
    s32 unk24;
};

struct Self {
    u8 pad00[0x19];
    u8 unk19;
    u8 pad1A[2];
    u16 unk1C;
    u16 unk1E;
    u8 pad20[4];
    u32 unk24;
    u8 pad28[4];
    struct Basis unk2C;
    struct Vec3 unk50;
    u8 pad5C[0x1C];
    struct Vec3 unk78;
    s32 unk84;
    u8 pad88[0x48];
    struct Vec3 unkD0;
    u8 padDC[0xC];
    s32 unkE8;
    s32 unkEC;
    s32 unkF0;
    s32 unkF4;
    u16 unkF8;
    u8 unkFA;
    u8 padFB[5];
    u16 unk100;
    u8 pad102[0x32];
    struct Other *unk134;
};

struct GlobalFA94 {
    u8 pad00[0xE0];
    s32 unkE0;
};

struct PlayerFA58 {
    u8 pad00[0x14];
    u16 unk14;
};

extern struct GlobalFA94 *D_8009FA94;
extern struct PlayerFA58 *D_8009FA58;
extern u16 D_8009F9FC;
extern void *jtbl_80011318[];

extern s32 func_80019668(void);
extern void func_80033608(struct Self *self, void *anim);
extern void func_800330E8(struct Self *self);
extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);
extern void func_800193F8(struct Vec3 *axis, struct Basis *basis);
extern s32 func_800331CC();
extern s32 func_8003532C();
extern s32 func_80032F3C();

s32 func_800357C0(s32 id, struct Self *a1, struct Other *a2) {
    u32 index;
        register struct OtherChild *p __asm__("$3");
    register struct PlayerFA58 *q __asm__("$2");
    register s32 lhs __asm__("$3");
    register s32 rhs __asm__("$2");
    static void *const label_table[] __attribute__((used)) = {
        &&case_1, &&case_2, &&case_default, &&case_4, &&case_default,
        &&case_6, &&case_default, &&case_default, &&case_9, &&case_default,
        &&case_11, &&case_default, &&case_default, &&case_14
    };

    index = id - 1;
    if (index >= 14) {
        goto case_default;
    }
    goto *jtbl_80011318[index];

case_1:
    return 0x138;

case_2:
    return func_800331CC();

case_6:
    if (a1->unkFA == 5) {
        return 0;
    }
    func_8003532C();
    return 0;

case_4:
    if (a1->unkFA != 5) {
        goto case_default;
    }
    {
        register s32 diff __asm__("$2");
        diff = a1->unk100 - D_8009F9FC;
        a1->unk100 = diff;
        if ((s16)diff > 0) {
            return 0;
        }
    }
    func_80035698(a1);
    return 0;

case_9:
    p = a2->unk08;
    if (p == 0) {
        goto case_default;
    }
    q = D_8009FA58;
    lhs = p->unk14;
    goto cmp_ptr;


case_14:
    if (a2 == 0) {
        goto case_default;
    }
    q = D_8009FA58;
    lhs = q->unk14;
    rhs = a2->unk00;
    goto cmp_val;

case_11:
    if (a2 == 0) {
        goto case_default;
    }
    q = D_8009FA58;
    lhs = a2->unk14;

cmp_ptr:
    rhs = q->unk14;

cmp_val:
    if (lhs != rhs) {
        goto case_default;
    }
    D_8009FA94->unkE0 = 5;
    goto case_default;

case_default:
    return func_80032F3C();
}
