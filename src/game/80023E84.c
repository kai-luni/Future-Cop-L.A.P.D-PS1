#include "game/types.h"

struct Unk4749CResource {
    u8 unk00[0xD];
    s8 owner;
    u8 unk0E[2];
    s32 serial;
};

struct Unk4749CHandle {
    struct Unk4749CResource *resource;
    s32 serial;
    s32 state;
    u8 unk0C[4];
    s32 unk10;
    u8 unk14[4];
    u8 flag18;
};

extern struct Unk4749CHandle D_800A0A80;

extern void func_80022240(s32 owner);
extern void func_80029720(void);
extern void func_80045370(void);
extern s32 func_800474F0(struct Unk4749CHandle *handle);

void func_80023E84(void)
{
    if (D_800A0A80.state == 1) {
        if (D_800A0A80.resource != 0) {
            func_80022240(D_800A0A80.resource->owner);
        }
        D_800A0A80.state = 0;
        if (D_800A0A80.flag18 == 0) {
            func_80045370();
        }
    }
    func_800474F0(&D_800A0A80);
    D_800A0A80.unk10 = 0;
    func_80029720();
}
