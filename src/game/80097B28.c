// Categories: Resources > Memory Pools | Platform > Sound
#include "game/types.h"

typedef struct {
    u32 flags;
    u32 size;
} Block;

extern s32 gArenaOffsetFlag __asm__("D_8009EA3C");
extern s32 gArenaCursor __asm__("D_8009EA40");
extern s32 gSpuAddrShift __asm__("D_8009EECC");
extern s32 gSpuAlignMask __asm__("D_8009EED4");
extern s32 D_8009EF04;
extern s32 gTableCursor __asm__("D_8009EF08");
extern Block *gSpuEntryTable __asm__("D_8009EF0C");

extern void gSpuCompactPool(void) __asm__("func_80097DF8");

/**
 * @brief Allocates a block from the 8-byte-unit pool: rounds the
 *        request up, searches the block table for a free block, splits
 *        it, and returns the block address or -1.
 *
 * @param size Requested size in bytes. It is rounded up to the granule
 *        described by D_8009EECC (shift) and D_8009EED4 (mask), i.e.
 *        to the next multiple of 8, and is then carried in units.
 * @return The low 28 bits of the chosen entry's flags word - the block
 *         address in units - or -1 when the scan finds nothing or the
 *         tail block is too small.
 *
 * @par Confirmed
 * Table: D_8009EF0C holds entries of two words (address with flag bits,
 * then size in units), D_8009EF04 is the entry count and D_8009EF08
 * the insertion cursor. D_8009EA40 tracks the arena position:
 * offset = 0 while D_8009EA3C is 0, else
 * (0x10000 - D_8009EA40) << D_8009EECC, which is 0x10000 units or
 * 0x80000 bytes of pool.
 * Rounding: when size & ~D_8009EED4 is non-zero the mask is added, then
 * the value is shifted right and left by D_8009EECC, which rounds it up
 * to the granule.
 * D_8009EF0C[0].flags bit 0x40000000 means the whole pool is untouched,
 * in which case block 0 is taken without any search. Otherwise
 * gSpuCompactPool() runs: it merges adjacent free blocks, marks dead
 * entries 0x2fffffff and drops size-0 entries, then sorts by address
 * and compacts. The scan accepts a block whose flags has 0x40000000,
 * or has 0x80000000 with a size at least the request.
 * Tail split (flags 0x40000000): only when found < D_8009EF04 and
 * size - offset >= request. The follower entry becomes
 * ((address & 0x0FFFFFFF) + request) | 0x40000000 with the remaining
 * size, the cursor moves to found + 1, the block keeps the request as
 * its size and loses its flags, gSpuCompactPool() runs and the flags word
 * is returned.
 * Normal split: when the request is smaller than the block, a free
 * entry (address + request, size - request, flag 0x80000000) is
 * inserted at the cursor by shifting the entry that sits there one
 * slot up, and the cursor is incremented. The block then stores the
 * request and clears its top flag bits (flags &= 0x0FFFFFFF), which is
 * what marks it allocated.
 * Both paths end with gSpuCompactPool() and return the flags word after
 * that mask, i.e. the address field alone.
 *
 * @par Likely role
 * The sound-RAM allocator. 0x10000 units of 8 bytes is exactly 0x80000
 * bytes, the size of the SPU's sample memory, and the granule of 8
 * bytes is the granularity of the SPU transfer address; the callers
 * are all sound and stream code, one of them rounding a transfer size
 * up to 64 bytes and passing the result as a DMA handle. 0x40000000
 * marks the region past the last used block, 0x80000000 a block freed
 * or produced by a split, so a freed block is reused after
 * gSpuCompactPool has merged it with its neighbours.
 *
 * @par Unknown
 * Who resets the granule and arena globals besides the sound init, whether
 * the returned address is an absolute transfer address or an offset,
 * and how the sort order of gSpuCompactPool interacts with fragmentation
 * over a long session.
 *
 * @par Callers
 * func_80021898 (0x800218E0), func_8002333C (0x8002342C) and gOpenVideoLayer
 * (0x800880C0, 0x800880DC); all analysis/edges.json kind "direct".
 */
s32 func_80097B28(s32 size)
{
    s32 i = 0;
    s32 found = -1;
    s32 offset;
    register s32 count __asm__("$3");
    register s32 limit __asm__("$5");
    register Block *p __asm__("$4");

    if (gArenaOffsetFlag == 0) {
        { register s32 zero __asm__("$0"); __asm__("" : "=r"(zero)); offset = zero; }
    } else {
        offset = (0x10000 - gArenaCursor) << gSpuAddrShift;
    }
    if (size & ~gSpuAlignMask) {
        size += gSpuAlignMask;
    }
    size = size >> gSpuAddrShift;
    size = size << gSpuAddrShift;
    if (gSpuEntryTable[0].flags & 0x40000000) {
        found = 0;
        goto search_done;
    }
    gSpuCompactPool();
    count = D_8009EF04;
    if (i < count) {
        u32 alloc = 0x40000000;
        u32 used = 0x80000000;
        limit = count;
        { register u32 off __asm__("$2") = i * 8; register u32 base __asm__("$3") = (u32)gSpuEntryTable; p = (Block *)(off + base); }
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
    register Block *table __asm__("$7") = gSpuEntryTable;
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
            gTableCursor = next;
            entry->size = size;
            entry->flags &= mask;
            gSpuCompactPool();
            return gSpuEntryTable[found].flags;
        }
        return -1;
    }
    {
    u32 esize = entry->size;
    if ((u32)size < esize) {
        s32 j = gTableCursor;
        if (j < D_8009EF04) {
            u32 nf = flags + (u32)size;
            u32 of = table[j].flags;
            u32 os = table[j].size;
            table[j].flags = nf | 0x80000000;
            table[j].size = esize - (u32)size;
            gTableCursor = j + 1;
            table[j + 1].flags = of;
            table[j + 1].size = os;
        }
    }
    }
    }
    gSpuEntryTable[found].size = size;
    gSpuEntryTable[found].flags &= 0x0FFFFFFF;
    gSpuCompactPool();
    return gSpuEntryTable[found].flags;
}
