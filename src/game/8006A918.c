#include "game/types.h"

struct Resource;

struct Unk607B8 {
    u8 unk00[6];
    s16 unk06;
    s16 unk08;
    u8 unk0A[2];
    s16 unk0C;
};

struct Unk6A898 {
    u8 unk00[8];
    s16 unk08;
    s16 unk0A;
    struct Unk607B8 sub;
    u8 unk1A[6];
};

struct Unk60838Arg {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};

struct Unk6AAEC {
    u32 unk00;
    struct Unk6A898 obj1;
    struct Unk6A898 obj2;
    void *unk44[4];
};

extern s32 D_8009F848;
extern struct Unk6AAEC D_800A3340[];
extern u8 D_800A3344[];

extern void *func_80019C38(u32 a0, u32 a1, u32 a2, u32 a3);
extern void *func_80019CA0(u32 a0, u32 a1, u32 a2, u32 a3);
extern struct Resource *func_80019BF0(s32 group, s32 name);
extern void func_8006A898(struct Unk6A898 *value);
extern void func_800181D4(struct Unk60838Arg *arg0);
extern void *memcpy(void *dest, const void *src, u32 count);

void func_8006A918(struct Unk60838Arg *arg0)
{
    struct Unk6AAEC *entry;
    u8 *data;
    u8 *second;
    s32 i;
    s32 index;

    if (D_8009F848 < 6) {
        index = D_8009F848++;

        data = func_80019C38(arg0->unk00, arg0->unk20, 0x74414354, arg0->unk1C);

        memcpy(&D_800A3340[index], data + 0x10, 0x44);

        func_8006A898((struct Unk6A898 *)(D_800A3344 + (u32)(index * 84)));
        func_8006A898((struct Unk6A898 *)(D_800A3344 + (u32)(index * 84) + 0x20));

        second = func_80019CA0(arg0->unk00, arg0->unk20, 0x6152534C, arg0->unk1C);
        entry = (struct Unk6AAEC *)(D_800A3344 + (u32)(index * 84) - 4);
        __asm__("" : : "r"(entry));

        {
            register s32 *sp __asm__("$17");
            sp = (s32 *)(second + 8);
            for (i = 0; i < 4; i++, sp += 2) {
                if (sp[2] != 0) {
                    entry->unk44[i] = func_80019BF0(sp[1], sp[2]);
                } else {
                    entry->unk44[i] = 0;
                }
            }
        }
    }

    func_800181D4(arg0);
}
