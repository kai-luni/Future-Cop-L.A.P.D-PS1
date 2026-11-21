// Categories: Resources > Memory Management | Platform > Memory
#include "game/types.h"
extern s32 gArenaCursor __asm__("D_8009EA40");
extern s32 gArenaOffsetFlag __asm__("D_8009EA3C");
extern s32 gSpuAlignMask __asm__("D_8009EED4");
extern s32 gSpuAddrShift __asm__("D_8009EECC");
extern s32 gSpuEntryTableCapacity __asm__("D_8009EF04");
extern s32 gTableCursor __asm__("D_8009EF08");
extern s32 *gSpuEntryTable __asm__("D_8009EF0C");
extern void gSpuCompactPool(void) __asm__("func_80097DF8");
extern s32 gSpuSplitPoolBlock(s32, s32, s32) __asm__("func_800980F8");
s32 gSpuAllocAt(s32 arg, s32 size) __asm__("func_80098458");


/**
 * @brief Allocates an aligned block of the requested size from the block
 *        record table, first fit, splitting the record it comes from.
 *
 * @param arg Lower bound for the block address; rounded up to the alignment and
 *        rejected when it ends up below 0x1010.
 * @param size Byte count to reserve; rounded up to the alignment.
 * @return Start address of the block, or -1 when the request does not fit, the
 *         address is out of range or the table has no room for the split.
 *
 * @par Confirmed
 * 1. Alignment uses D_8009EED4 as the mask and D_8009EECC as the shift: size and
 *    address are increased by the mask when a bit lies outside it and then
 *    truncated to a multiple of 1 << D_8009EECC, so both end up aligned.
 * 2. An aligned address below 0x1010 returns -1 (the first 0x1010 bytes are
 *    never handed out), and x + size above (0x10000 << D_8009EECC) minus the
 *    reserve returns -1.
 * 3. reserve is 0 while D_8009EA3C is 0, otherwise
 *    (0x10000 - D_8009EA40) << D_8009EECC.
 * 4. When the first record carries bit 0x40000000 the block is cut straight out
 *    of it: with a gap before x two records are written (a free 0x80001010
 *    record with the gap, then the tail at x + size) and with no gap one record
 *    replaces the header (x, size and the tail). The record count is stored back
 *    into D_8009EF08 and the capacity test is D_8009EF04 - 2 < D_8009EF08 for
 *    the two-record case and D_8009EF04 - 1 < D_8009EF08 otherwise.
 * 5. Otherwise gSpuCompactPool() coalesces the table first. Records whose address
 *    word has bit 0x80000000 are then examined: a record matches when x lies
 *    inside it and either the block extends past x + size or the next record is
 *    the tail, and the remaining space must be at least size. A record without
 *    bit 0x80000000 but with 0x40000000 is the tail: its size minus the reserve
 *    has to be at least size and it is used as is.
 * 6. The chosen record is split by gSpuSplitPoolBlock(record index, x, size) and that
 *    return value is the result; gSpuCompactPool() runs once more before returning.
 *
 * @par Likely role
 * The linear heap behind the sprite/asset subsystem's 0x11E80-byte work buffer:
 * the 0x1010 bytes at the bottom are a fixed header area, and the reserve keeps
 * a configurable region free at the top of the heap. The address bits read as
 * 0x80000000 = free record, 0x40000000 = last record (reaches the heap top) and
 * 0x0FFFFFFF = the address field.
 *
 * @par Unknown
 * What D_8009EA3C/D_8009EA40 describe (Likely a tail reservation for sound or
 * scratch data), whether the records are kept sorted by address, the unit of
 * D_8009EECC, and why the fast path overwrites the record count instead of
 * incrementing it (consistent with the first record being the only one).
 *
 * @par Callers
 * func_8002333C, the sprite/asset bring-up, which stores the result in
 * D_8009F27C from func_80098458(0x1010, 0x11E80).
 */
s32 gSpuAllocAt(s32 arg, s32 size)
{
    s32 reserve, mask, i;
    register s32 address __asm__("$6");
    register s32 remaining __asm__("$6");
    register s32 sum __asm__("$7");
    register s32 count __asm__("$2");
    register s32 gap __asm__("$5");
    s32 x;
    u32 start;
    s32 *p;
    if (gArenaOffsetFlag == 0) reserve = 0;
    else reserve = (0x10000 - gArenaCursor) << gSpuAddrShift;
    address = arg;
    mask = gSpuAlignMask;
    if (size & ~mask) size += mask;
    size >>= gSpuAddrShift;
    size <<= gSpuAddrShift;
    x = address;
    if (x & ~mask) x += mask;
    x >>= gSpuAddrShift;
    x <<= gSpuAddrShift;
    if (x < 0x1010) return -1;
    sum = x + size;
    if (sum > (0x10000 << gSpuAddrShift) - reserve) return -1;
    p = gSpuEntryTable;
    if (p[0] & 0x40000000) {
        register s32 *hi __asm__("$1");
        gap = x - 0x1010;
        { register s32 temp __asm__("$2");
          temp = p[1] + 0x1010;
          temp -= x;
          remaining = temp - size; }
        if (gap > 0) {
            if (gSpuEntryTableCapacity - 2 < gTableCursor) return -1;
            p[0] = 0x80001010;
            p[4] = sum | 0x40000000;
            __asm__ volatile ("" : : "r" (sum) : "memory");
            count = 2;
            __asm__ volatile ("" : "=r" (count) : "0" (count));
            p[1] = gap;
            p[2] = x;
            p[3] = size;
            p[5] = remaining;
        } else {
            if (gSpuEntryTableCapacity - 1 < gTableCursor) return -1;
            p[2] = sum | 0x40000000;
            __asm__ volatile ("" : : "r" (sum) : "memory");
            count = 1;
            __asm__ volatile ("" : "=r" (count) : "0" (count));
            p[0] = x;
            p[1] = size;
            p[3] = remaining;
        }
        __asm__ volatile ("lui %0,%%hi(D_8009EF08)" : "=r" (hi) : : "memory");
        *(s32 *)((char *)hi - 4344) = count;
    } else {
        gSpuCompactPool();
        i = 0;
        for (;;) {
            register s32 cond __asm__("$2");
            __asm__ volatile ("" : "=r" (i) : "0" (i) : "memory");
            start = gSpuEntryTable[i * 2];
            if ((s32)start < 0) {
                start &= 0x0fffffff;
                if ((u32)x >= start) {
                    if (!((u32)x < start + gSpuEntryTable[i * 2 + 1] ||
                        (gSpuEntryTable[i * 2 + 2] & 0x40000000))) goto NEXT;
                    {
                        register s32 *base __asm__("$2");
                        register s32 *q __asm__("$3");
                        __asm__ volatile ("" : "=r" (i) : "0" (i));
                        base = *(s32 * volatile *)&gSpuEntryTable;
                        q = (s32 *)((i * 8) + (s32)base);
                        __asm__ ("" : "=r" (q) : "0" (q));
                        {
                            register s32 value __asm__("$2");
                            register s32 length __asm__("$3");
                            value = q[0];
                            length = q[1];
                            __asm__("" : "=r" (value), "=r" (length) : "0" (value), "1" (length));
                            value &= 0x0fffffff;
                            value = x - value;
                            remaining = length - value;
                        }
                    }
                    __asm__("slt %0,%1,%2" : "=r" (cond) : "r" (remaining), "r" (size));
                    if (cond) goto NEXT;
                    start = x;
                    break;
                } else {
                    remaining = gSpuEntryTable[i * 2 + 1];
                    __asm__("slt %0,%1,%2" : "=r" (cond) : "r" (remaining), "r" (size));
                    if (!cond) break;
                    i++;
                    continue;
                }
            } else {
                if (start & 0x40000000) goto FINAL;
            NEXT:
                __asm__ volatile ("" : : "r" (i));
                i++;
                continue;
            FINAL:
                {
                    register s32 value __asm__("$2");
                    value = gSpuEntryTable[i * 2 + 1];
                    __asm__("" : "=r" (value) : "0" (value));
                    remaining = value - reserve;
                }
                __asm__("slt %0,%1,%2" : "=r" (cond) : "r" (remaining), "r" (size));
                if (cond) return -1;
                start &= 0x0fffffff;
                break;
            }
        }
        x = gSpuSplitPoolBlock(i, start, size);
    }
    gSpuCompactPool();
    return x;
}
