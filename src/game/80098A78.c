#include "game/types.h"

typedef struct {
    u8 pad[0x1AA];
    volatile u16 flags;
} Unk9EEA4;

extern Unk9EEA4 *D_8009EEA4;
extern void func_8008E000(char *a, char *b);

s32 func_80098A78(s32 arg)
{
    u32 i;
    register char *a0 __asm__("$4");
    register char *a1 __asm__("$5");

    if (arg == 0 || arg == 3) {
        D_8009EEA4->flags &= 0xFFBF;
        i = 0;
        while (D_8009EEA4->flags & 0x40) {
            if (++i > 0xF00) {
                a0 = (char *)0x80010000;
                __asm__("" : "=r"(a0) : "0"(a0));
                a0 += 0x2D2C;
                a1 = (char *)0x80010000;
                __asm__("" : "=r"(a1) : "0"(a1));
                a1 += 0x2D3C;
                func_8008E000(a0, a1);
                return -1;
            }
        }
    }
    if (arg == 1 || arg == 3) {
        D_8009EEA4->flags |= 0x40;
        i = 0;
        while (!(D_8009EEA4->flags & 0x40)) {
            if (++i > 0xF00) {
                a0 = (char *)0x80010000;
                __asm__("" : "=r"(a0) : "0"(a0));
                a0 += 0x2D2C;
                a1 = (char *)0x80010000;
                __asm__("" : "=r"(a1) : "0"(a1));
                a1 += 0x2D4C;
                func_8008E000(a0, a1);
                return -1;
            }
        }
    }
    return arg;
}
