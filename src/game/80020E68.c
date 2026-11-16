// Categories: Platform > Memory Card | Resources > Save Data
#include "game/types.h"

extern void func_800207CC(void);
extern s32 func_80020DF8(u32 slot);

/**
 * @brief Finds a contiguous run of free 8 KB blocks on the memory card.
 *
 * Refreshes the directory table via func_800207CC, rounds byte size up to
 * 8 KB (0x2000-byte) blocks k = (size + 0x1FFF) >> 13, and scans the 16 slots
 * for the first contiguous sequence of k slots whose state equals 2.
 *
 * @param size Requested save size in bytes.
 * @return Starting slot index of the first k-block free run, or 0 if none exists (note: matching at slot 0 also returns 0).
 *
 * @par Globals
 * None directly; callees refresh and read D_8009AB08->slots[0..15].
 *
 * @par Control flow
 * Slides a k-slot window i = 0..(16-k), breaking the inner loop when func_80020DF8(slot) != 2 and returning i on the first full run.
 *
 * @par Calls
 * func_800207CC - refreshes memory card directory state; func_80020DF8 - reads slot status word (2 = free block).
 *
 * @par Side effects
 * Refreshes D_8009AB08 directory state via func_800207CC.
 *
 * @par Callers
 * Confirmed: func_80020FDC (0x80021064, save file block allocation), func_8007FB04 (0x8007FD78, checks free space for 0x678-byte profile).
 */
u32 func_80020E68(u32 size)
{
    register u32 k asm("s0") = size;
    register u32 j asm("s1");
    register u32 i asm("s2");
    register u32 limit asm("s3");
    register u32 result asm("s4") = 0;
    register u32 two asm("s5");
    register u32 bound asm("s6");
    register u32 tmp asm("v0");

    asm("" : "+r"(k), "+r"(result));
    func_800207CC();
    i = result;
    two = 2;
    k = (k + 0x1FFF) >> 13;
    tmp = 16;
    asm("" : "+r"(tmp));
    bound = tmp - k;
    j = k;
    do {
        k = i;
        if (i < j) {
            limit = j;
            do {
                if (func_80020DF8(k) != two)
                    break;
                k++;
            } while (k < limit);
        }
        if (k == j) {
            result = i;
            break;
        }
        i++;
        j++;
    } while (i <= bound);
    return result;
}
