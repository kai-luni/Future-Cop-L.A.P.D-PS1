// Categories: Resources > Memory Pools | Platform > Sound
typedef unsigned int u32;
typedef int s32;
struct Entry { u32 addr; u32 size; };
extern s32 gTableCursor __asm__("D_8009EF08");
extern struct Entry *gSpuEntryTable __asm__("D_8009EF0C");

/**
 * @brief Compacts the SPU pool block table D_8009EF0C.
 *
 * Maintenance pass over the two-word block records (address with flag bits,
 * size in 8-byte units) used by the pool allocator func_80097B28. Flag
 * semantics: 0x80000000 free, 0x40000000 untouched tail space, no top bits
 * allocated, 0x2FFFFFFF dead tombstone, low 28 bits the address. Five passes:
 * coalesce each free entry with the next non-dead entry when it is also free
 * and physically contiguous; tombstone zero-size entries; selection-sort the
 * records by address, stopping at the first untouched-tail entry; compact the
 * first tombstone by moving the last record into its slot and lowering the
 * cursor D_8009EF08; finally fold the trailing run of free entries back into
 * the tail, converting them to 0x40000000 and accumulating their sizes.
 *
 * @par Globals
 * D_8009EF0C - block record table; D_8009EF08 - insertion cursor / last
 * record index, lowered by the compaction passes.
 *
 * @par Control flow
 * Five sequential loops over the table; the coalescing scan skips tombstones
 * and merges only adjacent free records; the sort and the tail fold both
 * stop at the untouched-tail marker.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Rewrites record addresses, sizes and flags in place and shrinks the
 * cursor; no allocation or hardware access.
 *
 * @par Callers
 * Confirmed: func_80097B28 (after every alloc/split, sites 0x80097BD0,
 * 0x80097D00, 0x80097DB4), func_800980F8 (site 0x8009843C) and
 * func_80098458 (site 0x800985E0); func_80098708 per its documentation.
 */
void func_80097DF8(void)
{
    s32 i, j;
    u32 a, b, size;
    {
        register s32 limit __asm__("$13");
        register struct Entry *base __asm__("$8");
        register struct Entry *cur __asm__("$7");
        register u32 deleted __asm__("$10");
        register u32 mask __asm__("$11");
        register u32 flag __asm__("$12");
        register s32 initial __asm__("$2");
        initial = gTableCursor;
        i = 0;
        if (initial >= 0) {
            flag = 0x80000000; deleted = 0x2fffffff; mask = 0x0fffffff;
            base = gSpuEntryTable; limit = initial; cur = base;
            do {
                if (cur->addr & flag) {
                    register struct Entry *scan __asm__("$3");
                    struct Entry *next;
                    register u32 offset __asm__("$2");
                    j = i + 1;
                    offset = j << 3;
                    __asm__("" : "+r" (offset));
                    scan = (struct Entry *)(offset + (u32)base);
                again:
                    if (scan->addr == deleted) { scan++; j++; goto again; }
                    __asm__("" : "+r" (j));
                    next = (struct Entry *)((j << 3) + (u32)base);
                    if ((next->addr & flag) && (next->addr & mask) == (cur->addr & mask) + cur->size) {
                        next->addr = deleted;
                        cur->size += next->size;
                        continue;
                    }
                }
                cur++; i++;
            } while (i <= limit);
        }
    }
    {
        register s32 count __asm__("$2");
        register s32 count2 __asm__("$4");
        register u32 deleted __asm__("$5");
        count = gTableCursor;
        __asm__ volatile (".set reorder\n.set noreorder" : "+r" (count));
        i = 0;
        if (count >= 0) {
            deleted = 0x2fffffff;
            __asm__("" : "+r" (deleted));
            count2 = count;
            do {
                __asm__ volatile (".set reorder\n.set noreorder");
                if (gSpuEntryTable[i].size == 0) gSpuEntryTable[i].addr = deleted;
                i++;
            } while (i <= count2);
        }
    }
    for (i = 0; i <= gTableCursor; i++) {
        if (gSpuEntryTable[i].addr & 0x40000000) break;
        for (j = i + 1; j <= gTableCursor; j++) {
            if (gSpuEntryTable[j].addr & 0x40000000) break;
            if ((gSpuEntryTable[j].addr & 0x0fffffff) < (gSpuEntryTable[i].addr & 0x0fffffff)) {
                a = gSpuEntryTable[i].addr;
                b = gSpuEntryTable[i].size;
                gSpuEntryTable[i].addr = gSpuEntryTable[j].addr;
                gSpuEntryTable[i].size = gSpuEntryTable[j].size;
                gSpuEntryTable[j].addr = a;
                gSpuEntryTable[j].size = b;
            }
        }
    }
    for (i = 0; i <= gTableCursor; i++) {
        if (gSpuEntryTable[i].addr & 0x40000000) break;
        if (gSpuEntryTable[i].addr == 0x2fffffff) {
            gSpuEntryTable[i].addr = gSpuEntryTable[gTableCursor].addr;
            gSpuEntryTable[i].size = gSpuEntryTable[gTableCursor].size;
            gTableCursor = i;
            break;
        }
    }
    for (i = gTableCursor - 1; i >= 0; i--) {
        if (!(gSpuEntryTable[i].addr & 0x80000000)) break;
        gSpuEntryTable[i].addr = (gSpuEntryTable[i].addr & 0x0fffffff) | 0x40000000;
        size = gSpuEntryTable[gTableCursor].size;
        gTableCursor = i;
        gSpuEntryTable[i].size += size;
    }
}
