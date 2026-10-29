#include "game/types.h"

struct Resource;

struct Unk60E74Entry {
    void *unk00;
    void *unk04;
};

struct Unk60E74List {
    u32 count;
    struct Unk60E74Entry entries[1];
};

struct Unk60E74First {
    u8 unk00[4];
    u32 unk04;
    struct Unk60E74List list;
};

struct Unk60E74Arg {
    s32 unk00;
    void *unk04;
    s32 unk08;
    u8 unk0C[0x10];
    s32 unk1C;
    s32 unk20;
    u8 unk24[0x0C];
    s32 unk30;
    u8 unk34[4];
    u8 unk38[1];
};

struct Unk60E74Data {
    u8 unk00[0x0C];
    s16 unk0C;
    u8 unk0E[6];
    s16 unk14;
    s16 unk16;
    u8 *unk18;
    u8 *unk1C;
    s32 unk20;
    void *unk24;
    u8 *unk28;
    struct Unk60E74Arg *unk2C;
};

struct Unk60E74Global {
    s32 count;
    struct Unk60E74Arg *entries[1];
};

extern struct Unk60E74Global *D_8009F800;

extern void func_80019A74(struct Resource *resource);
extern void *func_80019CA0(u32 a0, u32 a1, u32 a2, u32 a3);
extern s32 func_80019668(void);
extern void *func_80019C38(u32 a0, u32 a1, u32 a2, u32 a3);
extern struct Resource *func_80019BF0(s32 group, s32 name);
extern s32 func_8002AE54(u8 arg0);

void func_80060E74(struct Unk60E74Arg *arg0)
{
    struct Unk60E74Data *data;
    struct Unk60E74First *first;
    struct Unk60E74List *list;
    void *second;
    u32 i;

    func_80019A74((struct Resource *)arg0);

    *(D_8009F800->entries + D_8009F800->count++) = arg0;

    data = func_80019CA0(arg0->unk00, arg0->unk20, 0x74534143, arg0->unk1C);
    arg0->unk04 = data;
    arg0->unk08 = 0;

    data->unk16 = 0;
    data->unk0C = data->unk0C * 5;
    if (data->unk0C < 0) {
        data->unk0C = -data->unk0C;
        data->unk14 = data->unk0C;
    } else {
        /* Das Orakel verwendet hier eine vorzeichenlose Division. */
        data->unk14 = (u32)func_80019668() % data->unk0C;
    }

    data->unk18 = arg0->unk38;
    data->unk1C = data->unk18 + arg0->unk30;

    first = func_80019CA0(arg0->unk00, arg0->unk20, 0x6152534C, arg0->unk1C);
    second = func_80019C38(arg0->unk00, arg0->unk20, 0x74414354, arg0->unk1C);

    data->unk28 = (u8 *)second - 4;
    i = 0;
    data->unk24 = &first->list;
    list = &first->list;
    list->count = (first->unk04 - 0xC) >> 3;

    for (; i < list->count; i++) {
        if (list->entries[i].unk04 != 0) {
            list->entries[i].unk04 = func_80019BF0(
                (s32)list->entries[i].unk00, (s32)list->entries[i].unk04);
        }
    }

    data->unk20 = func_8002AE54(*(u8 *)(data->unk28 + 4));
    data->unk2C = arg0;
}
