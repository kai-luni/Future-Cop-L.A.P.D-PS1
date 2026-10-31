#include "game/types.h"

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RectA414;

typedef struct {
    u8 unk00[8];
    s32 count;
} HeaderA414;

typedef struct {
    u8 *unk00;
} ArgA414;

extern void **D_8009F7D8;
extern s32 D_8009F7DC;

extern void *func_80017EBC(s32 size, s32 tag);
extern void func_800181D4(void *block);
extern void func_8005A220(void);
extern void func_8008CD10(void *dest, void *src, s32 size);
extern void func_80092F38(RectA414 *rect, void *data);
extern void func_80092C8C(s32 arg);
extern s32 func_8005A2BC(void **array, s32 count);

/**
 * Consumes a freshly loaded, packed asset blob: unpacks it into per-entry blocks,
 * uploads the pixel data into a fixed VRAM window and builds the descriptor table.
 *
 * The header behind arg0->unk00 carries count and a length-prefixed list from +0xC.
 * Each entry is copied as 8 bytes of head plus count * 8 bytes of payload; the copy
 * is func_8008CD10, which is a BIOS stub for A0:0x2A (memcpy) - argument in the
 * call shape only, no C body exists for it.
 *
 * The pixel data goes up 16 pixels at a time (0x20 source bytes per call, i.e.
 * 16 bpp) into a 64 pixel wide field anchored at VRAM (320, 480); the x/y cursor is
 * shared across all entries, so they are stacked top to bottom and each entry's
 * start coordinate is stored in its bytes +4 and +5. The trailing region is uploaded
 * as 0x40 x 0x1E0 before func_8005A2BC stamps the tile coordinates. The transfer
 * itself is func_80092F38: a wrapper that forwards the rectangle and the data pointer
 * to the callback in D_8009DDE8, so the VRAM write happens in library code.
 *
 * Both error paths (empty header, failed allocation) run func_8005A220, which frees
 * the blocks allocated so far in reverse order - a partial load leaves no leak. The
 * input buffer is freed at the end, so the function takes ownership of it.
 */
void func_8005A414(ArgA414 *arg0)
{
    register HeaderA414 *header __asm__("$18");
    s32 count;
    s32 i;
    s32 j;
    s32 x;
    s32 y;
    s32 size;
    u8 *p;
    RectA414 rect[2];

    header = (HeaderA414 *)arg0->unk00;
    count = header->count;

    if (count <= 0) {
        goto cleanup;
    }

    D_8009F7DC = count;
    D_8009F7D8 = func_80017EBC(count * 4, 2);
    p = (u8 *)header + 0xC;
    if (D_8009F7D8 == 0) {
        goto cleanup;
    }

    for (i = 0; i < count; i++) {
        size = p[2] * 8;
        D_8009F7D8[i] = func_80017EBC(size + 8, 2);
        *(s32 *)D_8009F7D8[i] = *(s32 *)p;
        func_8008CD10((u8 *)D_8009F7D8[i] + 8, p + 4, size);
        p += size + 4;
    }

    p += 8;
    x = 0;
    y = 0;
    for (i = 0; i < count; i++) {
        ((u8 *)D_8009F7D8[i])[4] = x;
        ((u8 *)D_8009F7D8[i])[5] = y;
        for (j = 0; j < ((u8 *)D_8009F7D8[i])[2]; j++) {
            rect[0].x = x + 0x140;
            rect[0].y = y + 0x1E0;
            rect[0].w = 0x10;
            rect[0].h = 1;
            func_80092F38(&rect[0], p);
            func_80092C8C(0);
            x += 0x10;
            p += 0x20;
            if (x == 0x40) {
                x = 0;
                y++;
            }
        }
    }

    rect[1].x = 0x140;
    rect[1].y = 0;
    rect[1].w = 0x40;
    rect[1].h = 0x1E0;
    func_80092F38(&rect[1], p + 8);
    func_80092C8C(0);

    if ((func_8005A2BC(D_8009F7D8, count) & 0xFF) != 0) {
        goto done;
    }

cleanup:
    func_8005A220();

done:
    func_800181D4(arg0);
}
