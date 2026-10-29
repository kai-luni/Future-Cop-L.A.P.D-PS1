#include "game/types.h"

struct LinkState {
    s32 stride;
    u8 *end;
    s32 unk08;
    s32 count;
    s32 *link;
};

extern struct LinkState *D_8009FA4C;
extern struct LinkState *D_8009F9D0;
extern struct LinkState *D_8009FAAC;
extern s32 *D_8009F590;
extern s32 D_8009F594;
extern s32 D_8009F598;

extern void func_800195D0(struct LinkState *state, s32 *link);
extern s32 func_8001814C(void *block);
extern void func_8001E2C0(void *block);
extern u8 *func_8001E318(u8 *destination, u8 *source, s32 count);
extern u8 *func_8001E44C(u8 *destination, u8 *source, s32 count);

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

    if (((pool = D_8009FA4C),
         (s32)block >= (s32)pool && (s32)block < (s32)pool->end) ||
        ((pool = D_8009F9D0),
         (s32)block >= (s32)pool && (s32)block < (s32)pool->end) ||
        ((pool = D_8009FAAC),
         (s32)block >= (s32)pool && (s32)block < (s32)pool->end)) {
        func_800195D0(pool, (s32 *)block);
    } else if ((s32)block >= (s32)D_8009F590[0] &&
               (s32)block < (s32)D_8009F590[D_8009F594]) {
        block = (u8 *)block - 4;
        index = func_8001814C(block);
        end = (u8 *)block + *(s32 *)block;
        offset = (u32)index * 4;
        table = (u8 **)D_8009F590;
        slot = (u8 **)(offset + (u32)table);
        if (block == slot[0]) {
            if (end == slot[1]) {
                if (index != 0) {
                    if (index != D_8009F594 - 1) {
                        func_8001E318((u8 *)slot,
                                      (u8 *)(slot + 2),
                                      (D_8009F594 - index - 1) * 4);
                        D_8009F594 -= 2;
                        if (D_8009F598 >= index) {
                            D_8009F598 -= 2;
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
                              D_8009F594 * 4);
                D_8009F590[1] = (s32)block;
                D_8009F590[2] = (s32)end;
                D_8009F594 += 2;
                D_8009F598 += 2;
            }
        } else if (end == slot[1]) {
            total = D_8009F594;
            if (index + 1 < total) {
                slot[1] = block;
            } else {
                last = (u8 **)((u32)total * 4 + (u32)table);
                old = last[0];
                total += 2;
                last[0] = block;
                last[1] = end;
                D_8009F594 = total;
                last[2] = old;
            }
        } else {
            copy_source = slot + 1;
            slot += 3;
            func_8001E44C((u8 *)slot,
                          (u8 *)copy_source,
                          (D_8009F594 - index) * 4);
            D_8009F594 += 2;
            if (index < D_8009F598) {
                D_8009F598 += 2;
            }
            D_8009F590[index + 1] = (s32)block;
            D_8009F590[index + 2] = (s32)end;
        }
    } else {
        func_8001E2C0(block);
    }
}
