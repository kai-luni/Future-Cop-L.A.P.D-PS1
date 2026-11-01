#include "game/types.h"

union Product36AB4 { unsigned long long value; u32 word[2]; };

struct Unk36AB4Child {
    u8 unk00[4];
    s32 (*callback)(s32, void *, void *, s32);
    s32 unk08;
    u8 unk0C[0x50];
    u8 unk5C[4];
};

struct Unk36AB4 {
    u8 unk00[0x1A];
    s16 unk1A;
    u8 unk1C[0x34];
    u8 unk50[0x60];
    s32 unkB0;
    s32 unkB4;
    s32 unkB8;
    struct Unk36AB4Child *unkBC;
    s32 unkC0;
};

extern void func_800410BC(void *arg0, s32 arg1, s32 arg2, s32 arg3);
extern s32 func_8001A4E8(void *arg0, void *arg1);

void func_80036AB4(struct Unk36AB4 *arg0, s32 arg1)
{
    register struct Unk36AB4 *self __asm__("$16") = arg0;
    register struct Unk36AB4Child *child __asm__("$17");
    register s32 temp __asm__("$5");
    register s32 limit __asm__("$4");
    register u32 delta __asm__("$3");
    register u32 recip __asm__("$2");
    register u32 product __asm__("$2");
    register union Product36AB4 wide __asm__("$8");
    register s32 scale __asm__("$4");
    register s32 scaled __asm__("$10");

    if (self->unkB0 > 0) {
        func_800410BC(self->unk50, self->unk1A, self->unkB0, self->unkC0);
        return;
    }

    child = self->unkBC;
    if (child == 0) {
        return;
    }
    if (child->unk08 != self->unkB8) {
        return;
    }

    temp = func_8001A4E8(child->unk5C, self->unk50);
    limit = self->unkB0;
    if (temp >= limit) {
        return;
    }

    recip = 0x80000000u / (limit >> 1);
    delta = (u32)limit - (u32)temp;
    scale = self->unk1A;
    __asm__ volatile("" ::: "memory");
    __asm__ volatile("multu %2, %3\n\t"
                     "mflo %0\n\t"
                     "mfhi %1"
                     : "=r"(wide.word[0]), "=r"(wide.word[1])
                     : "r"(delta), "r"(recip));
    wide.word[1] <<= 16;
    wide.word[0] >>= 16;
    product = wide.word[1] | wide.word[0];
    scaled = (s32)(product * (u32)scale);
    product = (u32)(scaled >> 16);
    self->unk1A = (s16)product;

    if (child->callback(0xA, child, self, arg1) != 0) {
        child->callback(0xB, child, self, arg1);
    }
}
