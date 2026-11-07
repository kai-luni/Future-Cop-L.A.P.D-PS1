#include "game/types.h"

struct Unk6CAAC {
    s16 x;
    s16 y;
    s16 z;
    s8 i0;
    s8 i1;
};

struct Out6CAAC {
    s16 a;
    s16 b;
    s16 c;
    s16 pad;
};

void func_8006CAAC(struct Out6CAAC *dst, s16 *tbl, s16 count, struct Unk6CAAC *src, s16 scale)
{
    s32 n = count;
    register s32 i __asm__("$8") = 0;
    register s32 r __asm__("$2") = scale;
    s32 off;
    register s32 addr __asm__("$2");

    __asm__ volatile ("" : "+r"(i), "+r"(r));
    if (n > 0) {
        off = r * n;
        addr = off << 3;
        __asm__ volatile ("addu %0,%1,%2" : "=r"(dst) : "r"(addr), "r"(dst));
        do {
            dst->a = (src->x * tbl[src->i0]) >> 12;
            dst->b = (src->y * tbl[1]) >> 12;
            dst->c = (src->z * tbl[src->i1]) >> 12;
            i++;
            tbl += 4;
            dst++;
        } while (i < n);
    }
}
