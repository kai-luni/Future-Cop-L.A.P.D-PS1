#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Entity {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C;
    u8 unk0D[0x09];
    u8 unk16;
    u8 unk17[0x11];
    u8 unk28[0x04];
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    struct Vec3 unk50;
    struct Vec3 unk5C;
    u8 unk68[0x38];
    u8 unkA0;
    u8 unkA1;
    s16 unkA2;
    s32 unkA4;
    struct Entity *unkA8;
    s32 unkAC;
    s8 unkB0;
    s8 unkB1;
};

extern struct Entity *func_8004D69C(s32 tag);
extern void func_80017668(void *instance, s32 part, s32 *out);
extern s32 func_80030DE0(struct Entity *self, void *data, s32 arg);
extern u8 func_8002F8B8(u8 arg0, s16 arg1, void *arg2, s32 arg3);
extern s32 func_80031238(struct Entity *self, void *data);
extern s32 func_80031120(struct Entity *self, void *data, s32 arg);
extern s32 func_8002AD58(s32 message, struct Entity *self, void *data);
extern void func_80031004(struct Entity *self);
extern s32 D_8009FAA4;
extern void *jtbl_8001123C[];

s32 func_8003151C(s32 message, struct Entity *arg1, void *arg2, s32 arg3)
{
    register struct Entity *self __asm__("$16");
    register void *data __asm__("$17");
    register s32 a3v __asm__("$7");
    u32 index;
    void **table;
    s32 result;
    static void *const label_table[] __attribute__((used)) = {
        &&m1, &&m2, &&mzero, &&mzero, &&mzero, &&m6, &&mzero, &&mzero,
        &&mzero, &&m10, &&m11, &&mzero, &&mzero
    };

    a3v = arg3;
    self = arg1;
    data = arg2;
    __asm__("" : "=r"(self), "=r"(data) : "0"(self), "1"(data));
    index = (u32)message - 1;
    if (index >= 13) {
        goto fallback;
    }
    table = jtbl_8001123C;
    __asm__("" : "=r"(table) : "0"(table));
    goto *table[index];

m1:
    return 0xB8;
m2:
    return func_80030DE0(self, data, a3v);
m10:
    a3v = self->unkA0;
    if (a3v & 0x10) {
        return 0;
    }
    result = func_8002F8B8(self->unkA1, self->unkA2, data, ((u32)a3v >> 2) & 1);
    if (result != 0) {
        if (self->unk16 == 0x26) {
            result = func_80031238(self, data);
        }
        if (result != 0) {
            if (self->unkA4 == D_8009FAA4 && (self->unkA0 & 4)) {
                result = 0;
            }
        }
    }
    return result;
m11:
    return func_80031120(self, data, a3v);
m6:
    if (self->unk0C != 0) {
        func_80031004(self);
    }
    if (self->unk16 == 0x26) {
        func_800313F0(self);
    }
mzero:
    return 0;
fallback:
    return func_8002AD58(message, self, data);
}
