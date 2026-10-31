#include "game/types.h"

struct Resource;

struct Unk607B8 {
    u8 unk00[6];
    s16 unk06;
    s16 unk08;
    u8 unk0A[2];
    s16 unk0C;
};

struct Unk60838Arg {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};

struct Unk60838Entry {
    u8 unk00[8];
    s16 unk08;
    s16 unk0A;
    struct Unk607B8 sub;
    u8 unk1A[6];
    void *unk20[4];
};

extern s32 D_8009F7F8;
extern struct Unk60838Entry D_800A2300[];

extern void *func_80019C38(u32 a0, u32 a1, u32 a2, u32 a3);
extern void *func_80019CA0(u32 a0, u32 a1, u32 a2, u32 a3);
extern struct Resource *func_80019BF0(s32 group, s32 name);
extern void func_800607B8(struct Unk607B8 *value);
extern void func_800181D4(struct Unk60838Arg *arg0);
extern void *memcpy(void *dest, const void *src, u32 count);

void func_80060838(struct Unk60838Arg *arg0)
{
    struct Unk60838Entry *entry;
    u8 *data;
    u8 *second;
    u8 *p;
    register s32 *sp __asm__("$17");
    s32 i;
    s32 index;

    index = D_8009F7F8++;

    data = func_80019C38(arg0->unk00, arg0->unk20, 0x74414354, arg0->unk1C);
    entry = &D_800A2300[index];

    memcpy(entry, data + 0x10, 0x20);

    func_800607B8(&entry->sub);

    entry->unk08 = (entry->unk08 << 12) / 218;
    entry->unk0A = (entry->unk0A << 12) / 218;

    second = func_80019CA0(arg0->unk00, arg0->unk20, 0x6152534C, arg0->unk1C);

    sp = (s32 *)(second + 8);
    for (i = 0; i < 4; i++, sp += 2) {
        if (sp[2] != 0) {
            entry->unk20[i] = func_80019BF0(sp[1], sp[2]);
        } else {
            entry->unk20[i] = 0;
        }
    }

    func_800181D4(arg0);
}
