// Categories: Resources > Memory Management
#include "game/types.h"

struct LinkState {
    s32 stride;
    u8 *end;
    s32 unk08;
    s32 count;
    s32 *link;
};

extern struct LinkState *gPool3Size __asm__("D_8009FA4C");
extern struct LinkState *gPool1Size __asm__("D_8009F9D0");
extern struct LinkState *gPool2Size __asm__("D_8009FAAC");
extern s32 *gFreeRangeTablePtr __asm__("D_8009F590");
extern s32 gFreeRangeCount __asm__("D_8009F594");
extern s32 gHeapSearchCursor __asm__("D_8009F598");

extern void func_800195D0(struct LinkState *state, s32 *link);
extern s32 func_8001814C(void *block);
extern void func_8001E2C0(void *block);
extern u8 *func_8001E318(u8 *destination, u8 *source, s32 count);
extern u8 *func_8001E44C(u8 *destination, u8 *source, s32 count);

/**
 * @brief Frees a block obtained from func_80017EBC(): pool, free-range or
 *        general allocator.
 *
 * @param block Pointer returned by the allocator. Any other pointer is not
 *              validated beyond the range checks below.
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * 1. Pool case: if the block lies inside D_8009FA4C, D_8009F9D0 or D_8009FAAC
 *    (start <= block < D_8009xxxx->end) func_800195D0(pool, block) pushes the
 *    node back onto that pool's free list and the function returns.
 * 2. Free-range case: if the block lies inside the range table window
 *    (D_8009F590[0] .. D_8009F590[D_8009F594]) the 4-byte header is restored
 *    (block -= 4), the matching range index found with func_8001814C() and the
 *    range start/end pair merged with its neighbours via func_8001E318/
 *    func_8001E44C (word moves). Fully covered ranges are dropped and the
 *    search cursor D_8009F598 adjusted.
 * 3. Otherwise the block is handed to func_8001E2C0(), the general allocator's
 *    free.
 *
 * @par Globals
 * D_8009F590/D_8009F594/D_8009F598 (free-range table, count and cursor),
 * D_8009FA4C/D_8009F9D0/D_8009FAAC (the three fixed pools).
 *
 * @par Callers
 * The whole executable - it is the single deallocator (over 80 call sites,
 * from the front-end through the renderer to the game code).
 *
 * @par Matching note
 * The $2/$3/$5 pins and the comma-expression in the pool test reproduce the
 * retail register allocation and branch layout; the logic is a plain
 * three-way dispatch.
 */
void func_800181D4(void *block)
{
    struct LinkState *pool;
    u8 *end;
    u8 *old;
    register u8 **copy_source __asm__("$3");
    u8 **slot;
    register u8 **table __asm__("$5");
    register u8 **last __asm__("$2");
    s32 index;
    u32 offset;
    register s32 total __asm__("$3");

    if (((pool = gPool3Size),
         (s32)block >= (s32)pool && (s32)block < (s32)pool->end) ||
        ((pool = gPool1Size),
         (s32)block >= (s32)pool && (s32)block < (s32)pool->end) ||
        ((pool = gPool2Size),
         (s32)block >= (s32)pool && (s32)block < (s32)pool->end)) {
        func_800195D0(pool, (s32 *)block);
    } else if ((s32)block >= (s32)gFreeRangeTablePtr[0] &&
               (s32)block < (s32)gFreeRangeTablePtr[gFreeRangeCount]) {
        block = (u8 *)block - 4;
        index = func_8001814C(block);
        end = (u8 *)block + *(s32 *)block;
        offset = (u32)index * 4;
        table = (u8 **)gFreeRangeTablePtr;
        slot = (u8 **)(offset + (u32)table);
        if (block == slot[0]) {
            if (end == slot[1]) {
                if (index != 0) {
                    if (index != gFreeRangeCount - 1) {
                        func_8001E318((u8 *)slot,
                                      (u8 *)(slot + 2),
                                      (gFreeRangeCount - index - 1) * 4);
                        gFreeRangeCount -= 2;
                        if (gHeapSearchCursor >= index) {
                            gHeapSearchCursor -= 2;
                        }
                    } else {
                        slot[0] = end;
                    }
                } else {
                    table[1] = block;
                }
            } else if (index > 0) {
                slot[0] = end;
            } else {
                func_8001E44C((u8 *)(table + 3),
                              (u8 *)(table + 1),
                              gFreeRangeCount * 4);
                gFreeRangeTablePtr[1] = (s32)block;
                gFreeRangeTablePtr[2] = (s32)end;
                gFreeRangeCount += 2;
                gHeapSearchCursor += 2;
            }
        } else if (end == slot[1]) {
            total = gFreeRangeCount;
            if (index + 1 < total) {
                slot[1] = block;
            } else {
                last = (u8 **)((u32)total * 4 + (u32)table);
                old = last[0];
                total += 2;
                last[0] = block;
                last[1] = end;
                gFreeRangeCount = total;
                last[2] = old;
            }
        } else {
            copy_source = slot + 1;
            slot += 3;
            func_8001E44C((u8 *)slot,
                          (u8 *)copy_source,
                          (gFreeRangeCount - index) * 4);
            gFreeRangeCount += 2;
            if (index < gHeapSearchCursor) {
                gHeapSearchCursor += 2;
            }
            gFreeRangeTablePtr[index + 1] = (s32)block;
            gFreeRangeTablePtr[index + 2] = (s32)end;
        }
    } else {
        func_8001E2C0(block);
    }
}
