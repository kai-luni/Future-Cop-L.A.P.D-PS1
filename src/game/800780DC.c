#include "game/types.h"

struct Unk780DC {
    u32 unk0;
    s32 unk4;
};

extern struct Unk780DC D_800A3588[2];
extern struct Unk780DC D_800A3598[2];
extern struct Unk780DC D_800A35A8[2];
extern struct Unk780DC D_800A35B8[2];
extern struct Unk780DC D_800A35C8[2];
extern struct Unk780DC D_800A35D8[2];
extern struct Unk780DC D_800A35E8[2];

extern u8 D_8009FAC4;
extern u8 D_8009F3F0;
extern s32 D_8009F3EC;
extern s32 D_8009F3F4;
extern s32 D_8009F8B0;
extern s32 D_8009F8B4;
extern s32 D_8009FA84;

extern void func_80077FFC(void);
extern void *func_80017EBC(s32 size, s32 tag);
extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern void func_800898C8(void);
extern void func_8007F9CC(void);
extern void func_80077AD4(s32 arg);

void func_800780DC(void)
{
    void *p;
    u32 i;

    D_8009FAC4 = 0;
    D_8009F3F0 = 1;
    D_8009F3EC = 0;
    func_80077FFC();
    p = func_80017EBC(0x78, 2);
    D_8009F3F4 = (s32)p;
    func_8001E4C0(p, 0, 0x78);
    func_800898C8();
    for (i = 0; i < 2; i++) {
        D_800A35D8[i].unk4 = 2;
        D_800A35C8[i].unk4 = 2;
        D_800A35B8[i].unk4 = 2;
        D_800A35A8[i].unk4 = 2;
        D_800A3598[i].unk4 = 2;
        D_800A3588[i].unk4 = 2;
        D_800A35E8[i].unk0 = 0x00080000;
        D_800A35E8[i].unk4 = 2;
        D_800A3588[i].unk0 = 0x00100000;
        D_800A3598[i].unk0 = 0x00400000;
        D_800A35A8[i].unk0 = 0x00800000;
        D_800A35B8[i].unk0 = 0x00200000;
        D_800A35C8[i].unk0 = 0x40000000;
        D_800A35D8[i].unk0 = 0x10000000;
    }
    func_8007F9CC();
    func_80077AD4(1);
    D_8009F8B0 = 0;
    D_8009F8B4 = 0;
    D_8009FA84 = 0;
}
