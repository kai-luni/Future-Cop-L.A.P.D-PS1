// Categories: Resources > Graphics Assets > Texture Upload | Graphics > VRAM Transfer
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

extern void **gTextureDescriptorTable __asm__("D_8009F7D8");
extern s32 gTexDescriptorCount __asm__("D_8009F7DC");

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void gFreeBlock(void *block) __asm__("func_800181D4");
extern void gFreeTextureDescriptors(void) __asm__("func_8005A220");
extern void gBiosMemcpy(void *dest, void *src, s32 size) __asm__("func_8008CD10");
extern void gLoadImage(RectA414 *rect, void *data) __asm__("func_80092F38");
extern void gDrawSync(s32 arg) __asm__("func_80092C8C");
extern s32 gBuildSpriteDescTable(void **array, s32 count) __asm__("func_8005A2BC");
void gLoadTextureChunksToVram(ArgA414 *arg0) __asm__("func_8005A414");


/**
 * @brief Unpacks a "Cpyr" sprite/tile atlas blob, uploads its pixels to VRAM,
 *        and builds the descriptor table.
 *
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
 *
 * @param arg0 Packed asset blob; the header behind unk00 carries the entry count
 *             and a length-prefixed list from +0xC. The buffer is freed at the
 *             end, so the caller must not use it afterwards.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F7D8 (descriptor table), D_8009F7DC (entry cursor into VRAM).
 *
 * @par Side effects
 * Allocates the per-entry blocks with func_80017EBC, writes into VRAM through
 * func_80092F38 and syncs with func_80092C8C. On failure func_8005A220 releases
 * the blocks allocated so far.
 *
 * @par Callers
 * func_80024EC8, when a graphics asset of a loaded level becomes visible.
 *
 * @par Texture load path
 * Called by func_80024EC8 on tag `0x43707972` ("Cpyr"). Unpacks
 * `count` entries of `8 + p[2] * 8` bytes into D_8009F7D8 (count in D_8009F7DC),
 * uploads 0x20 bytes (0x10 x 1) per tile anchored at VRAM (0x140, 0x1E0) in a
 * 0x40-wide column and a trailing 0x40 x 0x1E0 region at (0x140, 0) via
 * func_80092F38 / func_80092C8C, then calls func_8005A2BC to stamp tile
 * coordinates and frees `arg0` via func_800181D4 (or rolls back via func_8005A220).
 *
 * @par Environment path
 * Consumes a 'Cpyr' texture/tile atlas blob from func_80024EC8:221, allocates
 * gTextureDescriptorTable and per-entry blocks via gAllocMemory, uploads 16x1
 * tiles at VRAM (0x140, 0x1E0) and a 0x40x0x1E0 block at (0x140, 0) via
 * func_80092F38/func_80092C8C, stamps coordinates via func_8005A2BC, and
 * frees arg0.
 */
void gLoadTextureChunksToVram(ArgA414 *arg0)
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

    gTexDescriptorCount = count;
    gTextureDescriptorTable = gAllocMemory(count * 4, 2);
    p = (u8 *)header + 0xC;
    if (gTextureDescriptorTable == 0) {
        goto cleanup;
    }

    for (i = 0; i < count; i++) {
        size = p[2] * 8;
        gTextureDescriptorTable[i] = gAllocMemory(size + 8, 2);
        *(s32 *)gTextureDescriptorTable[i] = *(s32 *)p;
        gBiosMemcpy((u8 *)gTextureDescriptorTable[i] + 8, p + 4, size);
        p += size + 4;
    }

    p += 8;
    x = 0;
    y = 0;
    for (i = 0; i < count; i++) {
        ((u8 *)gTextureDescriptorTable[i])[4] = x;
        ((u8 *)gTextureDescriptorTable[i])[5] = y;
        for (j = 0; j < ((u8 *)gTextureDescriptorTable[i])[2]; j++) {
            rect[0].x = x + 0x140;
            rect[0].y = y + 0x1E0;
            rect[0].w = 0x10;
            rect[0].h = 1;
            gLoadImage(&rect[0], p);
            gDrawSync(0);
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
    gLoadImage(&rect[1], p + 8);
    gDrawSync(0);

    if ((gBuildSpriteDescTable(gTextureDescriptorTable, count) & 0xFF) != 0) {
        goto done;
    }

cleanup:
    gFreeTextureDescriptors();

done:
    gFreeBlock(arg0);
}
