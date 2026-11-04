#include "game/types.h"

/* Second half of the original 8005C344.c: the catalogue treats this as its own
   function (0x8005CA04, 380 bytes), the answer delivered both in one translation unit. */

struct Tgt;
struct Seg;

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Tag {
    u8 unk00[4];
    u8 type;
};

struct Owner {
    u8 unk00[0x20];
    struct Tag *unk20;
    u8 unk24[0x44];
    struct Seg *segs;
    u8 unk6C[0x24];
    s16 step;
};

struct Target {
    u8 unk00[8];
    struct Tgt *unk08;
    u8 unk0C[0xC];
    s32 unk18;
    u16 unk1C;
    u8 unk1E;
    u8 unk1F[9];
    u8 flags;
    u8 unk29;
    u8 unk2A;
    u8 unk2B;
    struct Vec3 pos;
};

struct Mid {
    u8 unk00[0x48];
    struct Target **unk48;
};

struct Obj2 {
    u8 unk00[0xF];
    u8 unkF;
    u8 unk10;
    u8 unk11[0xB];
    struct Mid *unk1C;
    struct Tag *unk20;
    s32 (*handler)(s32 id, struct Obj2 *self, s32 a, s32 b);
    u8 unk28[0x66];
    u16 unk8E;
};

extern s32 func_8005C344(struct Owner *owner, struct Target *t);

s32 func_8005CA04(u32 msg, struct Obj2 *obj, s32 arg2, s32 arg3)
{
    struct Target *t;
    s32 v;

    switch (msg) {
    case 3:
        if (obj->unk20->type == 0x57) {
            obj->unk8E = (obj->unk8E & 0xFFF7) | 1;
        }
        obj->unk10 |= 1;
        break;
    case 1:
        if (obj->unk20->type == 0x57) {
            t = *obj->unk1C->unk48;
            if (t->unk18 >= 0) {
                v = t->unk18 - arg2;
                t->unk18 = v;
                if (v < 0) {
                    t->flags |= 2;
                    if (obj != 0) {
                        obj->handler(8, obj, 0, 0);
                    }
                }
            }
            if (t->unk08 == 0) {
                t->flags |= 2;
                obj->unkF |= 1;
            }
            func_8005C344((struct Owner *)obj, t);
        }
        return 0;
    case 2:
        (*obj->unk1C->unk48)->flags |= 1;
        break;
    }
    return 0;
}
