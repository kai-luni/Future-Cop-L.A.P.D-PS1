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

void func_80035698(struct Self *self) {
    register u32 mask __asm__("$7");
    register struct Other *other __asm__("$3");
    register u16 val20 __asm__("$6");
    register s32 trig __asm__("$2");
    s32 sine;
    s32 cosine;

    self->unk1C = self->unk1E;
    self->unk134->unk1E = ((s16)self->unk1E >> 1) - (func_80019668() & 0x3F);
    self->unk19 &= 0x0F;
    other = self->unk134;
    self->unkF4 = other->unk24;
    mask = 0xFDFFFFCA;
    val20 = other->unk20;
    self->unk24 &= mask;
    self->unkFA = 4;
    self->unkF8 = val20;
    other->unk18 = 0;
    func_80033608(self, 0);
    {
        register struct Other *other2 __asm__("$2") = self->unk134;
        other2->unk1A = 0;
        func_800330E8(self);
    }
    func_8001846C(func_80019668() & 0xFFF, &sine, &cosine);
    trig = sine;
    self->unk78.x = trig;
    trig = cosine;
    self->unk78.y = 0;
    self->unkEC = 0;
    self->unkF0 = 0;
    self->unkE8 = 0;
    self->unk78.z = trig;
    self->unk50 = self->unkD0;
    func_800193F8(&self->unk78, &self->unk2C);
    self->unkFA = self->unk134->unk1D;
    {
        register s32 nine __asm__("$2") = 9;
        register struct GlobalFA94 *global __asm__("$3") = D_8009FA94;
        global->unkE0 = nine;
    }
}
