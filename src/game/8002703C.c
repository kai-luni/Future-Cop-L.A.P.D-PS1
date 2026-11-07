#include "game/types.h"

struct Unk9F9A0 {
    u8 unk00[0x11C];
    u8 unk11C;
    u8 unk11D;
};

extern volatile s32 D_8009FA60;
extern volatile s32 D_8009FA30;
extern s32 D_8009F9FC;
extern s32 D_8009FA00;
extern s32 D_8009FA84;
extern s32 D_8009FAA4;
extern u8 D_8009F690;
extern u8 D_8009F691;
extern u8 D_8009F692;
extern u8 D_8009F693;
extern u8 *D_8009F9A0[];

extern void func_80057864(void);
extern s32 func_80059DB8(void);
extern void func_8004DB74(s32 elapsed);
extern void func_80063568(s32 arg);
extern void func_800136C8(s32 arg);
extern void func_8004CC68(void);

void func_8002703C(void)
{
    s32 diff;
    register s32 sum __asm__("$2");
    s32 value;
    s32 nsum;
    struct Unk9F9A0 *obj;

    do {
        s32 cur = D_8009FA60;
        s32 prev = D_8009FA30;
        diff = cur - prev;
        D_8009FAA4 = cur;
    } while (diff < 5);

    __asm__ volatile("");
    sum = diff + (s8)D_8009F690 + (s8)D_8009F691;
    D_8009F9FC = diff;
    sum = sum + (s8)D_8009F692 + (s8)D_8009F693;
    D_8009F9FC = sum;
    D_8009F693 = *(u8 *)&D_8009F9FC & 3;
    if (sum < 0) {
        sum += 3;
    }
    __asm__ volatile("");
    obj = D_8009F9A0[0];
    D_8009F9FC = sum >> 2;
    D_8009F692 = D_8009F691;
    D_8009F691 = D_8009F690;
    D_8009F690 = diff;
    if (obj != 0 && obj->unk11C != 0 && obj->unk11D != 0) {
        D_8009F9FC = obj->unk11D * 5;
    }

    if (D_8009F9FC >= 0x3C) {
        D_8009F9FC = 0x3C;
    }

    func_80057864();
    if (D_8009FA84 > 0) {
        value = func_80059DB8();
        D_8009F9FC = value;
        if (value < 0) {
            D_8009FA00 = 5;
            D_8009F9FC = 5;
        }
        nsum = D_8009FA30 + D_8009F9FC;
        D_8009FAA4 = nsum;
        D_8009FA60 = nsum;
    }

    D_8009FA30 = D_8009FAA4;
    func_8004DB74(D_8009F9FC);
    func_80063568(D_8009F9FC);
    func_800136C8(D_8009F9FC);
    func_8004CC68();
}
