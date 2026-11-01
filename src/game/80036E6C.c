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

s32 func_80036E6C(s32 arg0, struct Entity *arg1, s32 arg2, s32 arg3)
{
    register s32 message __asm__("$18");
    register struct Entity *self __asm__("$16");
    register struct Entity *other __asm__("$17");
    register s32 value __asm__("$19");
    u32 index;
    static void *const label_table[] __attribute__((used)) = {
        &&message_1, &&message_2, &&message_3, &&message_4, &&message_5,
        &&message_6, &&message_forward, &&message_forward, &&message_forward,
        &&message_forward, &&message_forward
    };

    message = arg0;
    self = arg1;
    other = (struct Entity *)arg2;
    value = arg3;
    index = (u32)message - 1;
    if (index >= 11) {
        goto fallback;
    }
    goto *jtbl_80011380[index];

message_1:
    return 0xD4;
message_2:
    func_80036940(self, (s32)other, value);
zero:
    return 0;
message_3:
    {
        register s32 dx __asm__("$2");
        register s32 dy __asm__("$3");
        register s32 sign __asm__("$4");

        func_80036BD4(self, (s32)other, value);
        dx = self->unk78.x;
        dy = self->unk78.y;
        sign = dx >> 31;
        dx = (sign + dx) ^ sign;
        sign = dy >> 31;
        dy = (sign + dy) ^ sign;
        if (dx + dy < 0x10) {
            func_8001937C(&self->unk78, self->unk2C);
        } else {
            func_800193F8(&self->unk78, self->unk2C);
        }
        if (func_8001A750(&self->unk50, &self->unkC8) > 0x58000) {
            func_8004D958(self);
        }
        func_80010750(self->unk2C);
        {
            register s32 vertex_offset __asm__("$4");
            register u8 *vertices __asm__("$2");

            vertices = self->unk28->unk34;
            vertex_offset = self->unk70 << 4;
            func_80010234(vertices + 8 + vertex_offset, self->unk5C, 1);
        }
        return 0;
    }
message_4:
    {
        s32 different;

        different = 0;
        if (self->unk14 != other->unk14) {
            different = self->unkC0 != (struct EntityLink *)other;
        }
        return different;
    }
message_5:
    self->unk1C = -1;
    return 0;
message_6:
    func_80036AB4(self, value);
    goto fallback;
message_forward:
    if (other->unk08 != 0) {
        if (!(other->unk0C & 1) || other->unk08->unk18 != self->unk18) {
            func_8002AD58(message, self, (s32)other, value);
        }
    }
    return 0;
fallback:
    return func_8002AD58(message, self, (s32)other, value);
}
