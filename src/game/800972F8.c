#include "game/types.h"

typedef struct {
    u8 pad[0x1AA];
    volatile u16 flags;
} Unk9EEA4;

extern Unk9EEA4 *D_8009EEA4;
extern volatile u32 D_8009EEDC;
extern u32 D_8009EEF4;
extern void func_80097990(void);
extern void func_8008DFF0(u32 arg0, s32 arg1);

void func_800972F8(void)
{
    u32 i;

    if (D_8009EEF4 == 0) {
        func_80097990();
    }

    D_8009EEA4->flags &= 0xFFCF;

    if (D_8009EEA4->flags & 0x30) {
        i = 0;
        do {
            i++;
        } while (i < 0xF01 && (D_8009EEA4->flags & 0x30));
    }

    if (D_8009EEDC != 0) {
        ((void (*)(void))D_8009EEDC)();
    } else {
        func_8008DFF0(0xF0000009, 0x20);
    }
}
