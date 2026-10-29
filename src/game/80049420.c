#include "game/types.h"

struct Entry {
    u32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
};

struct Data {
    u32 f0;
    u32 f4;
    u32 f8;
    struct Entry entries[1];
};

struct Obj {
    u32 f0;
    u8 pad[0x18];
    u32 f1C;
    u32 f20;
};

extern u32 *D_8009F770;
extern s32 D_8009F774;

struct Resource;
extern void func_80019A74(struct Resource *resource);
extern u32 func_80019CA0(u32 a, u32 b, u32 c, u32 d);
extern u32 func_80019C38(u32 a, u32 b, u32 c, u32 d);
extern s32 func_80019668(void);

void func_80049420(struct Obj *obj)
{
    struct Data *data;
    s32 *p;
    s32 i;
    u32 count;
    u32 r2;

    func_80019A74((struct Resource *)obj);

    data = (struct Data *)func_80019CA0(obj->f0, obj->f20, 0x7446554E, obj->f1C);
    r2 = func_80019C38(obj->f0, obj->f20, 0x74455854, obj->f1C);

    count = (data->f4 - 0xC) / 20;
    D_8009F770 = (u32 *)((u8 *)data + 0xC);
    D_8009F774 = (s32)count;

    p = (s32 *)((u8 *)data + 0x20);
    for (i = (s32)count; i > 0; i--) {
        s32 t = p[-4] * 5;
        p[-4] = t;
        if (t < 0) {
            p[-4] = -t;
            p[-3] = -t;
        } else {
            p[-3] = (s32)((u32)func_80019668() % (u32)p[-4]);
        }
        p[-2] = (s32)r2 + p[-2];
        p[-1] = (s32)r2 + p[-1];
        p += 5;
    }
}
