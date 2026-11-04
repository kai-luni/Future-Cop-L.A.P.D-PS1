#include "game/types.h"

struct Cmd {
    u8 op;
    u8 arg;
};

struct Entry5FFB8 {
    u8 unk00[2];
    u16 unk02;
    u8 unk04[0x2C];
};

struct Obj5FFB8 {
    struct Entry5FFB8 *entry;
    u16 flags;
    u8 unk06;
    u8 pad07;
    u16 unk08;
    u8 pad0A[0x16];
    u16 unk20;
};

struct Script5FFB8 {
    u8 unk00[0x0C];
    struct Cmd commands[4];
};

extern struct Entry5FFB8 D_800A2300[];
extern s32 func_8005FF70(s32 key);
extern void func_8002A5B4(struct Script5FFB8 *script, s32 index);

void func_8005FFB8(struct Obj5FFB8 *obj, struct Script5FFB8 *script)
{
    register u8 *p __asm__("$17");
    s32 i;
    struct Entry5FFB8 *tbl;
    s32 op;
    s32 arg;
    s32 idx;

    i = 0;
    tbl = D_800A2300;
    p = (u8 *)script;
    while (i < 4) {
        op = p[0xC];
        if (op == 0) {
            break;
        }
        arg = p[0xD];
        switch (op) {
        case 0x1F:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
            obj->unk06 = op - 0x1E;
            obj->unk08 = arg;
            break;
        case 0x1E:
            if (arg == 0) {
                obj->flags |= 0x100;
            } else {
                obj->flags &= 0xFEFF;
            }
            break;
        case 0x26:
            idx = func_8005FF70(arg);
            if (idx >= 0) {
                obj->entry = (struct Entry5FFB8 *)(idx * 0x30 + (s32)tbl);
                obj->unk20 = obj->entry->unk02;
            }
            break;
        default:
            p += 2;
            i++;
            continue;
        }
        func_8002A5B4(script, i);
    }
}
