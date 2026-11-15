// Categories: Resources > Memory Pools | Platform > Memory
#include "game/types.h"

extern s32 *D_8009F590;
extern s32 D_8009F594;
extern s32 D_8009F598;
extern void *D_8009F9D0;
extern s32 D_8009F9E8;
extern void *D_8009FA4C;
extern s32 D_8009FA9C;
extern void *D_8009FAAC;

extern void *func_800194DC(s32 count, s32 size, s32 tag);

/**
 * @brief Installs the three fixed-size pools from three (size, count) pairs and
 *        resets the free-range table to one empty span.
 *
 * @param a0, a1 First pool: size a0, count a1 (stored in D_8009F9D0).
 * @param a2, a3 Second pool: size a2, count a3 (stored in D_8009FAAC).
 * @param a4, a5 Third pool: size a4, count a5 (stored in D_8009FA4C).
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * D_8009F594 = 3 (three valid words in the range table), D_8009F598 = 1 (the
 * search cursor, so range 0 is skipped), and the table is rewritten to
 * { base, base, base + size, base + size } where base = D_8009FA9C and size =
 * D_8009F9E8 - i.e. one free span covering the whole heap.
 * The three pools are then created with func_800194DC(count, size, 2).
 *
 * @par Globals
 * D_8009F590/D_8009F594/D_8009F598 (range table and cursor),
 * D_8009FA9C/D_8009F9E8 (heap base and size), D_8009F9D0, D_8009FAAC,
 * D_8009FA4C (pools).
 *
 * @par Callers
 * func_80077050 (all three pools with size 8, count 0), func_80024904
 * (0x168/0, 0x118/0, 0xFC/0) and func_80024A24 - i.e. every screen start
 * rebuilds the pool layer for its own size mix.
 */
void func_80017C68(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5)
{
    s32 off = D_8009F9E8;
    s32 *ptr = D_8009F590;
    s32 base;

    D_8009F594 = 3;
    base = D_8009FA9C;
    D_8009F598 = 1;
    ptr[0] = base;
    ptr[1] = base;
    ptr[2] = base + off;
    ptr[3] = base + off;
    D_8009F9D0 = func_800194DC(a1, a0, 2);
    D_8009FA4C = func_800194DC(a5, a4, 2);
    D_8009FAAC = func_800194DC(a3, a2, 2);
}
