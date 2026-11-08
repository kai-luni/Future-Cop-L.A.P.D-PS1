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
