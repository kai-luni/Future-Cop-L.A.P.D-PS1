#include "game/types.h"

struct Unk2C174Local {
    s32 unk00;
    s32 x;
    s32 y;
    s32 z;
    u8 pad10[0x40 - 0x10];
    s16 unk40;
    u8 pad42[0x49 - 0x42];
    u8 unk49;
};

struct Unk2C174 {
    u8 pad00[0x16];
    u8 unk16;
    u8 pad17[2];
    u8 unk19;
    u8 pad1A[2];
    s16 unk1C;
    s16 unk1E;
    u8 unk20;
    u8 pad21[3];
    u32 unk24;
    u8 pad28[0x5C - 0x28];
    s32 x;
    s32 y;
    s32 z;
    u8 pad68[0x8C - 0x68];
    u32 unk8C;
    u8 pad90[0x15F - 0x90];
    u8 unk15F;
};

extern struct Unk2C174 *func_8004D664(u32 id);
extern s32 func_80019668(void);
extern void func_8001A180(void *vec);
extern u8 func_8003B7D8(struct Unk2C174 *obj);
extern void func_800402DC(struct Unk2C174 *obj, u8 column, s32 arg2, s32 row, s32 arg4);
extern void func_8002A810(struct Unk2C174 *obj, s32 a, s32 b);
extern void func_8002B0EC(struct Unk2C174 *obj, s32 a);
extern void func_8002B1DC(struct Unk2C174 *obj, s32 a, s32 b);

void func_8002C174(struct Unk2C174 *obj)
{
    register struct Unk2C174 *target __asm__("$7");
    struct Unk2C174Local local;
    u8 sound;
    s32 type;
    s32 sel;
    s32 r;
    s32 dx;

    if (obj->unk8C != 0) {
        target = func_8004D664(obj->unk8C & 0x3FFFFFFF);
    } else {
        target = 0;
    }

    if (target != 0 && (target->unk16 == 1 || target->unk16 == 0x12) &&
        (obj->unk8C & 0x40000000) == 0) {
        local.unk49 = 2;
        dx = obj->x - target->x;
        local.y = obj->y - target->y;
        local.z = obj->z - target->z;
        local.x = dx;
        func_8001A180(&local.x);
        local.unk40 = 0x32;
        sound = func_8003B7D8(obj);
        if (sound == 0) {
            sound = 0x5B;
        }
        func_800402DC(obj, sound, (s32)&local, 0, 0);
        obj->unk20 = 0;
        func_8002A810(obj, 1, 1);
    } else {
        type = (obj->unk19 >> 3) & 0x1E;
        if (type >= 2 || -(obj->unk1E << 2) < obj->unk1C) {
            switch (type) {
            case 4:
                sel = 0x41;
                switch (func_80019668() & 3) {
                case 0:
                    sel = 0x42;
                    break;
                case 1:
                    sel = 0x44;
                    break;
                }
                func_8002B0EC(obj, sel);
                func_8002B1DC(obj, 7, 0);
                break;
            case 8:
                obj->unk15F = 7;
                obj->unk24 |= 1;
                func_8002B0EC(obj, 0x2A);
                func_800402DC(obj, 0xD2, 0, 0, 0);
                break;
            default:
                func_8002B1DC(obj, 7, 0);
                break;
            }
        } else {
            sound = func_8003B7D8(obj);
            if (sound == 0) {
                sound = 0x5B;
            }
            obj->unk20 = 0;
            func_800402DC(obj, sound, 0, 0, 0);
            func_8002A810(obj, 1, 1);
        }
    }
}
