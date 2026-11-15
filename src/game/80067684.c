// Categories: Physics > Collision > Ground Query | Gameplay > Map | Graphics > Geometry
#include "game/types.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec67684;

typedef struct {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 arr[16];
} Res67684;

extern Res67684 *D_8009F80C;
extern u8 **D_8009F808;

extern s32 func_80067480(u8 *arg0, s32 arg1, s32 arg2, s32 arg3, s32 *arg4);

/**
 * @brief Enumerates the surfaces of the map cell under a position and returns one
 *        of them by rank: the lowest, the highest or the nth.
 *
 * @param pos Position; only +0x00 (x) and +0x08 (z) are read, y is ignored.
 * @param mode -3 is rejected explicitly; -2 returns the lowest hit, -1 the
 *        highest, 0, 1 and 2 the entry of that index in the ascending list. Any
 *        other value falls through to the failure return.
 * @return The selected surface height, or 0 when the position is outside the map,
 *         the sub-cell has no descriptors or no sample matched.
 *
 * @par Confirmed
 * Rejects z < 0, (z >> 17) >= map +0x04 - 1, x < 0 and (x >> 17) >= map +0x08 -
 * 1, with map = D_8009F80C; the two header words are therefore consumed
 * transposed against x and z, exactly as func_80066F78 consumes them.
 * Cell word: index = (z >> 17) + (map +0x04) * (x >> 17), slot = map->+0x24
 * [index], block = D_8009F808[(slot >> 2) & 0x3FFF].
 * Sub-cell: tx = (x >> 13) & 0xF, tz = (z >> 13) & 0xF, tile = tx * 16 + tz,
 * hw = the u16 at block + 2 * tile + 0x3CC, descriptor list at block + 0x5D0 +
 * (hw >> 4), sample count = hw & 0xF, and the per-descriptor byte table handed to
 * the sampler is block + 3 * (tx + tile) + 0xC.
 * Sampling: for each descriptor, type = (word >> 15) & 0x7F, the loop stops at
 * type >= 0x47, and func_80067480(table, type, (z << 3) & 0xFFFF,
 * (x << 3) & 0xFFFF, out) writes a height and returns non-zero on a hit; only
 * then does the output cursor advance, so the collected array is compact.
 * Sorting: a bubble pass with xor swaps orders the hits ascending. The switch
 * then returns results[0] for -2, *(results + (count << 2) - 4) for -1, and
 * results[mode] for 0, 1 and 2.
 * Bound: the local array holds 8 s32 while the sample loop may accept up to 15
 * hits (hw & 0xF). Nothing in this function enforces the smaller bound, so the
 * original relies on a cell never yielding more than eight surfaces.
 *
 * @par Likely role
 * The terrain selector of the placement code: 17 callers, all of which set a
 * position's y and then lift it by a per-class offset, and several of which
 * derive the selector from a terrain byte with the mapping 0 -> -1, 1 -> -2,
 * else value - 2 (func_8002DC94, func_8002C584). With the list ascending, -2 is
 * the bottom surface of the cell and -1 the top one - the floor and the roof -
 * while 0..2 pick intermediate ledges. It is the y-agnostic sibling of
 * func_80066F78, which instead takes a ceiling reference, applies the sub-cell
 * quadrant test, can return the surface normal and always answers with a single
 * height.
 *
 * @par Unknown
 * Why the two variants exist side by side and whether the 8-entry array is an
 * intentional bound; also whether the +0x04/+0x08 transposition is a property of
 * the map format or an artefact of the original code.
 *
 * @par Callers
 * func_8002C584, func_8002DAA4, func_8002DC94, func_8002DFC4, func_8002EED8,
 * func_8002FA20, func_800302E8, func_80030AD4 (two sites), func_80030DE0,
 * func_800319D4, func_80039E8C (two sites), func_80049C38, func_80050B54,
 * func_800511E4, func_800579D4, func_80070730 (three sites), func_80083A10.
 */
s32 func_80067684(Vec67684 *pos, s32 mode)
{
    s32 results[8];
    register s32 count __asm__("$18");
    register s32 *out __asm__("$17");
    register s32 z __asm__("$7");
    register s32 x __asm__("$8");
    register Res67684 *res __asm__("$6");
    register s32 z17 __asm__("$9");
    register s32 unk04 __asm__("$4");
    register s32 x17 __asm__("$3");
    register s32 tile __asm__("$5");
    s32 tx;
    register s32 tz __asm__("$2");
    register s32 shifted __asm__("$4");
    register s32 tile2 __asm__("$3");
    register s32 product __asm__("$10");
    register s32 index __asm__("$2");
    s32 slot;
    s32 idx;
    register u8 *base __asm__("$4");
    register s32 entryoffset __asm__("$2");
    register u8 *entryaddr __asm__("$2");
    s32 val;
    s32 n;
    s32 i;
    s32 j;
    s32 *cursor;
    u8 *arg0;
    s32 arg2;
    s32 arg3;
    register s32 shiftedpos __asm__("$2");
    s32 r;

    if (mode == -3) {
        goto fail;
    }

    z = pos->z;
    x = pos->x;

    if (z < 0) {
        goto fail;
    }

    res = D_8009F80C;
    unk04 = res->unk04;
    z17 = z >> 17;

    if (!(z17 < unk04 - 1)) {
        goto fail;
    }

    if (x < 0) {
        goto fail;
    }

    x17 = x >> 17;

    if (!(x17 < res->unk08 - 1)) {
        goto fail;
    }

    product = unk04 * x17;
    __asm__("" : "+r"(product));
    tx = (x >> 13) & 0xF;
    shifted = tx * 16;
    __asm__("" : "+r"(shifted));
    tz = (z >> 13) & 0xF;
    tile = shifted + tz;
    tile2 = tx + tile;

    __asm__("" : "+r"(tile2), "+r"(tile));
    index = z17 + product;
    __asm__("" : "+r"(index));
    slot = res->arr[index];
    idx = (slot >> 2) & 0x3FFF;
    __asm__("" : "+r"(idx));
    base = D_8009F808[idx];

    __asm__("" : "+r"(tile) : "r"(base));
    entryoffset = 2 * tile;
    __asm__("" : "+r"(entryoffset));
    entryaddr = (u8 *)(entryoffset + (s32)base);
    __asm__("" : "+r"(entryaddr));
    val = *(u16 *)(entryaddr + 0x3CC);
    cursor = (s32 *)(base + 0x5D0 + (val >> 4));
    arg0 = base + (3 * tile2 + 0xC);

    n = val & 0xF;

    if (n == 0) {
        goto fail;
    }

    count = 0;
    i = n;
    shiftedpos = z << 3;
    __asm__("" : "+r"(shiftedpos));
    arg2 = shiftedpos & 0xFFFF;
    shiftedpos = x << 3;
    __asm__("" : "+r"(shiftedpos));
    arg3 = shiftedpos & 0xFFFF;

    if (i <= 0) goto after_loop;
    out = results;
loop:
        val = (*(u32 *)cursor >> 15) & 0x7F;
        cursor++;

        if (val >= 0x47) {
            goto after_loop;
        }

        if (func_80067480(arg0, val, arg2, arg3, out) != 0) {
            out++;
            count++;
        }

        i--;
        if (i > 0) goto loop;
after_loop:

    if (count <= 0) {
        goto fail;
    }

    for (i = 0; i < count - 1; i++) {
        for (j = i + 1; j < count; j++) {
            if (results[i] > results[j]) {
                results[i] ^= results[j];
                results[j] ^= results[i];
                results[i] ^= results[j];
            }
        }
    }

    {
    register s32 offset __asm__("$2");
    switch (mode) {
    case -2: return results[0];
    case -1:
        offset = count << 2;
        __asm__("" : "+r"(offset));
        return *(s32 *)((u8 *)results + offset - 4);
    case 0: case 1: case 2: return results[mode];
    }
    }
fail:
    return 0;
}
