#include "game/types.h"

struct Vec3s32 { s32 x; s32 y; s32 z; };
struct EntityLink { u8 pad[8]; s32 unk08; };
struct Entity28 { u8 pad[0x34]; u8 *unk34; };

struct Entity {
    s32 unk00;
    s32 unk04;
    struct Entity *unk08;
    u16 unk0C;
    u8 unk0E[6];
    u16 unk14;
    u8 unk16[2];
    u8 unk18;
    u8 unk19[3];
    s16 unk1C;
    u8 unk1E[4];
    s8 unk22;
    u8 unk23[5];
    struct Entity28 *unk28;
    u8 unk2C[0x24];
    struct Vec3s32 unk50;
    u8 unk5C[0x14];
    s16 unk70;
    u8 unk72;
    u8 unk73[5];
    struct Vec3s32 unk78;
    s32 unk84;
    u8 unk88[0x18];
    struct Vec3s32 unkA0;
    s32 unkAC;
    u8 unkB0[4];
    u8 unkB4;
    u8 unkB5;
    u8 unkB6[0xA];
    struct EntityLink *unkC0;
    s32 unkC4;
    s32 unkC8;
    u8 unkCC[4];
    s32 unkD0;
};

extern void func_8001ACA4(void *a, void *b, s32 c, s32 *d);
extern s32 func_800684AC(void *a, s32 *b, s32 c, s32 d, s32 e);
extern s32 func_8001A100(s32 *vector);
extern void func_8001A180(struct Vec3s32 *vector);
extern struct Entity *func_8004D3FC(s32 size);
extern s32 func_80019668(void);
extern s32 func_800402DC(void *owner, u8 column, s32 arg2, s32 row, s32 arg4);

extern void func_80036940(struct Entity *self, s32 arg2, s32 arg3);
extern void func_8001937C(struct Vec3s32 *axis, void *basis);
extern void func_800193F8(struct Vec3s32 *axis, void *basis);
extern s32 func_8001A750(struct Vec3s32 *a, void *b);
extern void func_8004D958(struct Entity *entity);
extern void func_80010750(void *p);
extern void func_80010234(void *a, void *b, s32 c);
extern void func_80036AB4(struct Entity *self, s32 arg3);
extern s32 func_8002AD58(s32 message, struct Entity *self, s32 arg2, s32 arg3);
extern void *jtbl_80011380[];

void func_80036BD4(struct Entity *self, s32 arg1, s32 arg2)
{
    s32 tmp[3];
    struct Entity *new;
    s32 i;
    s32 oldA4;
    s32 valA4;
    struct Entity *saved08;
    s32 saved00;

    if (self->unkC0 != 0 && self->unkC0->unk08 != self->unkC4) {
        self->unkC0 = 0;
    }

    i = 0;
    oldA4 = self->unkA0.y;
    valA4 = oldA4 - (self->unkAC * arg1) / 300;
    self->unkA0.y = valA4;
    if (oldA4 >= 0) {
        i = (u32)valA4 >> 31;
    }

    func_8001ACA4(&self->unk50, &self->unkA0, arg1 * 218, tmp);
    if (func_800684AC(&self->unk50, tmp, 0, 0, 1) != 0) {
        self->unk1C = -1;
        self->unk72 |= 8;
    } else {
        self->unk50 = *(struct Vec3s32 *)tmp;
    }

    self->unk78 = self->unkA0;
    self->unk84 = func_8001A100((s32 *)&self->unk78) / 300;
    func_8001A180(&self->unk78);

    if (self->unkB4 == 20 && (i & 0xFF) != 0) {
        for (i = 0; i < 4; i++) {
            new = func_8004D3FC(0xD4);
            if (new != 0) {
                saved08 = new->unk08;
                saved00 = new->unk00;
                *new = *self;
                new->unkB4 = 4;
                new->unk08 = saved08;
                new->unk00 = saved00;
                new->unk22 = -1;

                new->unkA0.x += ((i & 1) ? 0x2000 : -0x2000) +
                                ((func_80019668() & 0x3FFF) - 0x2000);
                new->unkA0.y -= (func_80019668() & 0x1FFF) + (i << 13);
                new->unkA0.z += ((i & 2) ? 0x2000 : -0x2000) +
                                ((func_80019668() & 0x3FFF) - 0x2000);

                if (self->unkB5 != 0) {
                    func_800402DC(new, self->unkB5, 0, 1, 0);
                }
            }
        }
    }
}
