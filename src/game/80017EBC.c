#include "game/types.h"

struct LinkState {
    s32 stride;
    u8 *end;
    s32 unk08;
    s32 count;
    s32 *link;
};

struct LinkNode;

extern s32 *D_8009F590;
extern s32 D_8009F594;
extern s32 D_8009F598;
extern struct LinkState *D_8009FA4C;
extern struct LinkState *D_8009FAAC;
extern struct LinkState *D_8009F9D0;

extern s32 func_80017D5C(s32 size);
extern s32 func_80017E0C(s32 size);
extern struct LinkNode *func_800195A4(struct LinkState *state);
extern void *func_8001E240(s32 size, s32 mode);
extern u8 *func_8001E318(u8 *destination, u8 *source, s32 count);

/**
 * Allocates memory using one of several allocation strategies.
 *
 * Supports a general-purpose allocator, bidirectional allocation
 * from a shared free-memory range table, and fixed-size memory pools.
 *
 * Allocation modes:
 *   0 - General allocator (func_8001E240).
 *       Falls back to mode 2 if allocation fails.
 *
 *   1 - Allocates from the end of a suitable free-memory range,
 *       using a reverse next-fit search.
 *
 *   2 - Allocates from the beginning of a suitable free-memory range,
 *       using a forward next-fit search.
 *
 *   3 - Allocates from one of three preallocated memory pools.
 *       Falls back to mode 1 if no pool is available.
 *
 * Modes 1 and 2 reserve an additional 4-byte header containing
 * the total aligned allocation size. The returned pointer refers
 * to the usable memory immediately after this header.
 *
 * Allocation sizes for modes 1 and 2 are rounded up to a
 * 4-byte boundary, including the header.
 *
 * Fully consumed free-memory ranges are removed from the range
 * table. The current search position is updated when allocating
 * from the beginning of a range (mode 2).
 *
 * If allocation through mode 1 or 2 fails, the function attempts
 * the general allocator (mode 0) without further fallback.
 *
 * @param size Number of usable bytes requested.
 * @param tag Allocation mode (0-3).
 * @return Pointer to allocated memory, or NULL on failure.
 *         Returns NULL immediately if size is zero.
 *
 * @note Mode 3 pool sizes and allocation semantics depend on
 *       the initialization of the LinkState structures.
 * @note The behavior for allocation modes outside 0-3 is undefined
 *       by this function.
 */
void *func_80017EBC(s32 size, s32 tag)
{
    s32 index;
    void *result;
    struct LinkState *pool;

    if (size == 0) {
        return 0;
    }
    result = 0;
    index = 1;

retry:
    switch (tag) {
    case 0:
        result = func_8001E240(size, 0);
        if (result != 0) {
            break;
        }
        if (index != 0) {
            tag = 2;
            index = 0;
            goto retry;
        }
        break;

    case 2:
        size += 7;
        size &= ~3;
        index = func_80017D5C(size);
        if (index < 0) {
            size -= 4;
            tag = 0;
            index = 0;
            goto retry;
        }
        result = (void *)D_8009F590[index];
        D_8009F598 = index;
        *(s32 *)result = size;
        result = (u8 *)result + 4;
        D_8009F590[index] += size;
        if (D_8009F590[index] == D_8009F590[index + 1] && D_8009F594 >= 4) {
            func_8001E318((u8 *)&D_8009F590[index], (u8 *)(&D_8009F590[index] + 2),
                         (D_8009F594 - index - 1) * 4);
            D_8009F594 -= 2;
        }
        break;

    case 1:
        size += 7;
        size &= ~3;
        index = func_80017E0C(size);
        if (index < 0) {
            size -= 4;
            tag = 0;
            index = 0;
            goto retry;
        }
        D_8009F590[index + 1] -= size;
        result = (void *)D_8009F590[index + 1];
        *(s32 *)result = size;
        result = (u8 *)result + 4;
        if (D_8009F590[index] == D_8009F590[index + 1] && D_8009F594 >= 4) {
            func_8001E318((u8 *)&D_8009F590[index], (u8 *)(&D_8009F590[index] + 2),
                         (D_8009F594 - index - 1) * 4);
            D_8009F594 -= 2;
            if (D_8009F598 >= index + 2) {
                D_8009F598 -= 2;
            }
        }
        break;

    case 3:
        pool = D_8009FA4C;
        if (pool->stride < size || pool->count <= 0) {
            pool = D_8009FAAC;
            if (pool->stride < size || pool->count <= 0) {
                pool = D_8009F9D0;
                if (pool->count <= 0) {
                    tag = 1;
                    goto retry;
                }
            }
        }
        result = func_800195A4((struct LinkState *)pool);
        break;
    }
    return result;
}
