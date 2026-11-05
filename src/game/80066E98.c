#include "game/types.h"

struct Info66E98 {
    u8 pad[0xAC];
    s8 id;
};
struct Object66E98 {
    u8 pad[0xAC];
    struct Info66E98 *info;
};
struct Region66E98 {
    u8 data[400];
};
struct Coord66E98 {
    u8 x;
    u8 y;
};
extern u8 *D_8009F9E4;
extern struct Region66E98 D_800AB3B8[];
extern struct Coord66E98 D_8009F834[];
extern void func_80017B60(s32);
extern void func_80010820(void *);
extern void func_8006531C(s32, s32, void *);
extern void func_8004A924(s32, s32, void *);
extern void func_80084398(s32, s32, void *);
extern void func_80062EB8(s32);
extern void func_8003F6E4(s32);

void func_80066E98(s32 index, struct Object66E98 *object)
{
    struct Region66E98 *region;
    s32 x;
    s32 y;

    func_80017B60(object->info->id);
    func_80010820(D_8009F9E4 + 0x28);
    region = &D_800AB3B8[index];
    x = D_8009F834[index].x;
    y = D_8009F834[index].y;
    func_8006531C(x, y, region);
    func_80010820(D_8009F9E4 + 0x28);
    func_8004A924(x, y, region);
    func_80084398(x, y, region);
    func_80062EB8(index);
    func_8003F6E4(index);
}
