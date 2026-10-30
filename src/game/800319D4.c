#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Obj {
    u8 *f0;
    u8 pad0[0x18];
    s32 f1C;
    s32 f20;
    s32 f24;
};

struct Data {
    s32 tag;
    s32 unk04;
    struct Vec3s32 pos;
    u16 f14;
    u16 f16;
    u16 f18;
    s8  f1A;
    u8  f1B;
    u16 f1C;
};

struct Node {
    s16 f0;
    s16 f2;
    s16 f4;
    u8  unk06[2];
    s32 f8;
    s16 fC;
    s16 fE;
    s16 f10;
    s16 f12;
    u8  f14;
    u8  f15;
    s16 f16;
    s32 f18;
    s32 f1C;
    s32 f20;
    struct Node *f24;
};

extern struct Node *D_8009F6F8;

extern u8 *func_80019C38(u8 *arg0, u32 arg1, u32 arg2, s32 arg3);
extern void *func_80017EBC(s32 size, s32 tag);
extern s32 func_80019668(void);
extern void func_800181D4(void *obj);
extern s32 func_80067684(struct Vec3s32 *position, s32 mode);

void func_800319D4(struct Obj *obj)
{
    u8 *r;
    struct Data *d;
    struct Node *node;
    s32 type;
    s32 mode;
    s32 value;

    r = func_80019C38(obj->f0, obj->f20, 0x74414354, obj->f1C);
    d = (struct Data *)(r - 4);
    node = (struct Node *)func_80017EBC(0x28, 3);

    node->f24 = D_8009F6F8;
    node->f18 = obj->f24;
    node->f8 = d->tag;
    node->f0 = d->pos.x >> 8;
    node->f4 = d->pos.z >> 8;
    D_8009F6F8 = node;

    type = d->f1A;
    if (type != 0) {
        if (type == 1) {
            mode = -2;
        } else {
            mode = type - 2;
        }
    } else {
        mode = -1;
    }
    value = func_80067684(&d->pos, mode);
    node->f2 = value >> 8;

    node->fC = (s16)d->f16 / 2;
    node->fE = (s16)d->f18 / 2;
    node->f10 = (s16)d->f14 / 2;
    node->f12 = d->f1C;
    node->f14 = d->f1B;
    node->f1C = 0;
    node->f20 = -1;
    node->f15 = 0;
    node->f16 = func_80019668() & 0xFF;
    func_800181D4(obj);
}
