#include "game/types.h"

typedef struct {
    u32 flags;
    u32 size;
} Block;

extern s32 D_8009EA3C;
extern s32 D_8009EA40;
extern s32 D_8009EECC;
extern s32 D_8009EED4;
extern s32 D_8009EF04;
extern s32 D_8009EF08;
extern Block *D_8009EF0C;

extern void func_80097DF8(void);

s32 func_80097B28(s32 size)
{
    s32 i = 0;
    s32 found = -1;
    s32 offset;
    register s32 count __asm__("$3");
    register s32 limit __asm__("$5");
    register Block *p __asm__("$4");

    if (D_8009EA3C == 0) {
        { register s32 zero __asm__("$0"); __asm__("" : "=r"(zero)); offset = zero; }
    } else {
        offset = (0x10000 - D_8009EA40) << D_8009EECC;
    }
    if (size & ~D_8009EED4) {
        size += D_8009EED4;
    }
    size = size >> D_8009EECC;
    size = size << D_8009EECC;
    if (D_8009EF0C[0].flags & 0x40000000) {
        found = 0;
        goto search_done;
    }
    func_80097DF8();
    count = D_8009EF04;
    if (i < count) {
        u32 alloc = 0x40000000;
        u32 used = 0x80000000;
        limit = count;
        { register u32 off __asm__("$2") = i * 8; register u32 base __asm__("$3") = (u32)D_8009EF0C; p = (Block *)(off + base); }
        do {
            if ((p->flags & alloc) ||
                ((p->flags & used) && p->size >= (u32)size)) {
                found = i;
                goto search_done;
            }
            i++;
            p++;
        } while (i < limit);
    }
search_done:
    if (found == -1) {
        return -1;
    }
    {
    register Block *table __asm__("$7") = D_8009EF0C;
    register Block *entry __asm__("$6");
    register u32 flags __asm__("$3");
    register u32 special __asm__("$8") = 0x40000000;
    entry = (Block *)((u32)(found * 8) + (u32)table);
    __asm__("" : "=r"(entry) : "0"(entry));
    flags = entry->flags;
    if (flags & special) {
        if (found < D_8009EF04 &&
            entry->size - (u32)offset >= (u32)size) {
            register u32 mask __asm__("$5") = 0x0FFFFFFF;
            register s32 next __asm__("$4");
            __asm__("" : "=r"(mask) : "0"(mask));
            next = found + 1;
            __asm__("" : "=r"(next) : "0"(next));
            table[next].flags =
                ((((volatile Block *)entry)->flags & mask) + (u32)size) | special;
            table[next].size = entry->size - (u32)size;
            D_8009EF08 = next;
            entry->size = size;
            entry->flags &= mask;
            func_80097DF8();
            return D_8009EF0C[found].flags;
        }
        return -1;
    }
    {
    u32 esize = entry->size;
    if ((u32)size < esize) {
        s32 j = D_8009EF08;
        if (j < D_8009EF04) {
            u32 nf = flags + (u32)size;
            u32 of = table[j].flags;
            u32 os = table[j].size;
            table[j].flags = nf | 0x80000000;
            table[j].size = esize - (u32)size;
            D_8009EF08 = j + 1;
            table[j + 1].flags = of;
            table[j + 1].size = os;
        }
    }
    }
    }
    D_8009EF0C[found].size = size;
    D_8009EF0C[found].flags &= 0x0FFFFFFF;
    func_80097DF8();
    return D_8009EF0C[found].flags;
}
