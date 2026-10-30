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
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    u8 flag18;
};

extern u8 D_8009F26C;
extern struct Unk4749CHandle D_800A0A80;

extern int func_80024170(void);
extern void func_80023E84(void);
extern s32 func_80045028(s32 mode);
extern struct Unk4749CResource *func_80046B04(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
extern void func_8004749C(struct Unk4749CHandle *handle, struct Unk4749CResource *resource);

s32 func_80024064(s32 arg0, s32 arg1)
{
    struct Unk4749CResource *handle;

    handle = 0;
    if (D_8009F26C != 0) {
        if ((u8)func_80024170()) {
            func_80023E84();
        }
        D_800A0A80.unk10 = 0;
        handle = func_80046B04(0x7F, 0xA, 0, 0xFF);
        if (handle != 0) {
            func_8004749C(&D_800A0A80, handle);
            if ((u8)func_80045028(arg0)) {
                D_800A0A80.flag18 = 1;
                D_800A0A80.unk0C = arg0;
            } else {
                D_800A0A80.flag18 = 0;
                D_800A0A80.unk0C = -1;
            }
        }
        if (handle != 0) {
            D_800A0A80.unk14 = arg1 / 21;
            if (D_800A0A80.unk14 >= 0xBB9) {
                D_800A0A80.unk14 = 0x834;
            }
        }
    }
    return handle != 0;
}
