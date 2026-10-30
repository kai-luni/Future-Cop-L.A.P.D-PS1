#include "game/types.h"

extern u8 D_8009F26C;
extern s8 D_800A0A00[];
extern void *D_8009F640[2];
extern volatile s32 D_8009F650;

extern void func_80023AB4(s32 arg0);
extern void func_80023A24(void);
extern void func_800181D4(void *block);
extern void func_80044EF8(void);

void func_80023BF8(void)
{
    register u32 i __asm__("$17");
    s8 *entry;
    s32 value;
    register void **slot __asm__("$16");
    void *block;

    if (D_8009F26C != 0) {
        i = 0;
        entry = D_800A0A00;
        do {
            value = entry[1];
            entry += 0x10;
            i++;
            func_80023AB4(value);
        } while (i < 8);

        i = 0;
        func_80023A24();
        slot = D_8009F640;
        do {
            block = *slot;
            if (block != 0) {
                func_800181D4(block);
                *slot = 0;
            }
            i++;
            slot++;
        } while (i < 2);

        func_80044EF8();
    }
}

void D_80023C94(void)
{
    D_8009F650 = 1;
}
