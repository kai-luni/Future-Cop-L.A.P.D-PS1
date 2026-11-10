#include "game/types.h"

typedef struct {
    u8 pad00[0xC];
    s32 unk0C;
    u8 pad10[0x4C];
    s32 unk5C;
} Mode;

extern Mode *D_8009FA94;
extern u8 D_8009FAC4;
extern u8 D_800A35FD[];
extern u8 D_800A35FB[];
extern u8 D_8009C1BC[];
extern u8 D_8009C1AC[];
extern u8 D_800A3948[];
extern u8 D_800A3938[];
extern volatile s32 D_8009F8F8;
extern u16 D_8009F92A;
extern void func_8007F02C(void);

void func_8007E7A8(void)
{
    s32 i;
    u8 *odd;
    u8 *even;
    u8 *dst;
    u8 *src;
    u8 *letter;
    Mode *p;

    D_8009FAC4 = 1;
    if (D_8009FA94->unk0C == 2) {
        D_800A35FD[0] = 0;
    } else {
        D_800A35FD[0] = 1;
    }
    func_8007F02C();
    i = 0;
    odd = D_8009C1BC;
    even = D_8009C1AC;
    dst = D_800A3948;
    src = D_800A3938;
    do {
        if (i & 1) {
            letter = (u8 *)(((*src + i) & 7) + (u32)odd);
        } else {
            letter = (u8 *)(((*src + i) & 0xF) + (u32)even);
        }
        *dst++ = *letter - 0x41;
        __asm__ volatile("");
        i++;
        src++;
    } while (i < 10);
    p = D_8009FA94;
    D_8009F8F8 = 0;
    __asm__ volatile("" ::: "memory");
    D_8009F92A = 0;
    if (p->unk5C == 1) {
        if (p->unk0C == 1) {
            D_800A35FB[0] = 9;
        } else {
            D_800A35FB[0] = 3;
        }
    } else if (p->unk0C == 1) {
        D_800A35FB[0] = 1;
    } else {
        D_800A35FB[0] = 8;
    }
}
