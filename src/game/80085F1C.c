#include "game/types.h"

typedef struct { s32 v[4]; } Matrix;
extern Matrix D_8001220C;
extern u8 D_8009F4D4;
extern u8 D_800A3958[];
extern u8 D_800A3A58[];

void func_80085F1C(void)
{
    Matrix matrix;
    register s32 size __asm__("$7");
    register s32 level __asm__("$19");
    register s32 y __asm__("$15");
    register s32 row __asm__("$24");
    register s32 x __asm__("$10");
    register s32 col __asm__("$13");
    register s32 base __asm__("$20");
    register s32 shift __asm__("$18");
    register s32 rowcopy __asm__("$16");
    u8 *buffer;
    register s32 i __asm__("$6");
    register s32 j __asm__("$5");
    register s32 y2 __asm__("$25");
    register s32 colcopy __asm__("$14");
    register s32 dstrow __asm__("$12");
    register s32 srcrow __asm__("$11");
    register s32 *value __asm__("$9");
    register s32 amount __asm__("$8");
    register s32 neg __asm__("$2");
    register s32 dest __asm__("$4");
    register s32 src __asm__("$3");
    register u8 *out __asm__("$3");
    register s32 test __asm__("$2");
    u8 *table;
    register s32 max __asm__("$5");
    s32 k;
    if (!D_8009F4D4) {
        size = 1;
        level = 1;
        matrix = D_8001220C;
        D_800A3958[0] = 0;
        buffer = D_800A3958;
        base = 8;
        for (; size < 16; size *= 2, level++) {
            y = 1;
            shift = level * 2;
            for (row = size; y >= 0; y--, row += neg) {
                x = 1;
                rowcopy = row;
                y2 = y * 2;
                for (col = size; x >= 0; x--, col += neg) {
                    i = 0;
                    if (i < size) {
                    colcopy = col;
                    do {
                        j = 0;
                        if (j < size) {
                        neg = rowcopy + i;
                        __asm__("" : "+r"(neg));
                        dstrow = neg * 16;
                        __asm__("" : "+r"(dstrow));
                        srcrow = i * 16;
                        neg = y2 + x;
                        __asm__("" : "+r"(neg));
                        value = (s32 *)((neg * 4) + (u32)&matrix);
                        amount = base - shift;
                        do {
                            dest = colcopy + j;
                            src = srcrow + j;
                            j++;
                            dest = dstrow + dest;
                            dest = dest + (u32)buffer;
                            src = src + (u32)buffer;
                            *(u8 *)dest = *(u8 *)src + (*value << amount);
                        } while (j < size);
                        }
                        i++;
                    } while (i < size);
                    }
                    neg = -size;
                }
            }
        }
        for (k = 0; k < 256; k++) D_800A3958[k] >>= 5;
        k = 0;
        table = D_800A3A58;
        __asm__("" : "+r"(table) : : "$3");
        max = 31;
        __asm__("" : "+r"(k) : "r"(table), "r"(max));
        test = k < 256;
loop:
            out = (u8 *)(k + (u32)table);
            if (test) *out = k >> 3;
            else *out = max;
            k++;
            if (k >= 264) goto endloop;
            test = k < 256;
            goto loop;
endloop:
        test = k < 256;
        __asm__("" : : "r"(test));
        D_8009F4D4 = 1;
    }
}
