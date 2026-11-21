// Categories: Graphics > Screen Regions > Viewports | Graphics > HUD
typedef unsigned int u32;
typedef unsigned char u8;
typedef struct { char pad[0x5c]; int mode; } Context;
typedef struct { char pad[0x18]; int count; } Other;
typedef struct { u32 x,y,w,h; int width,height; } Rect;
typedef struct { u8 flag; char pad[0x43]; } Entry;
extern volatile Context *gGameSession __asm__("D_8009FA94");
extern Other *gMapGridHeader __asm__("D_8009F80C");
extern int gPlayerCount __asm__("D_8009FA0C");
extern volatile int D_8009F82C;
extern volatile int gCellEntityCount __asm__("D_8009F828");
extern u8 D_8009F836, D_8009F837;
extern u8 gGridOriginY __asm__("D_8009F835");
extern u8 gGridOriginX __asm__("D_8009F834");
extern u32 *gCellMap __asm__("D_8009F820");
extern Rect gCellClipRect __asm__("D_800A2970");
extern Entry gPlayerCollisionCandidates[] __asm__("D_800A8DC0");
extern void gBuildSingleViewObjectGrid(void) __asm__("func_80062C3C");
extern void gBuildDualViewObjectListsAndGrid(void) __asm__("func_80062874");
extern void gSweepCollisionGridPairs(void) __asm__("func_80062614");
void gUpdateViewCollisionGrid(void) __asm__("func_80062FB4");

/**
 * @brief Computes the active screen region and marks its pixels in the region
 *        table.
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * 1. D_8009F82C and D_8009F828 are cleared (the two region counters).
 * 2. Region by game mode:
 *    - state block mode == 1 (split screen): 16 pixels are reserved at top and
 *      bottom (h = height - 16), the width shrinks by 8 per extra player
 *      (w = width - count * 8 + 8) and the origin is (count - 1) * 4;
 *    - D_8009FA0C == 1 (small view): a fixed 20x20 box at the current cursor;
 *    - otherwise: the bounding box of the two corner markers
 *      D_8009F834..D_8009F837 (each * 4, sorted with XOR swaps) plus 20.
 * 3. n = w * h, then the pixel loop walks the region in D_8009F820 and sets the
 *    flag bytes of D_800A8DC0, and D_800A2970 receives the rectangle.
 *
 * @par Likely role
 * The viewport/occlusion bookkeeping of the split-screen renderer: every pixel
 * that a view draws is marked, so the next view knows what it may overwrite.
 *
 * @par Callers
 * func_80062C3C, func_80062874 and func_80062614 (the three mode-specific
 * callers).
 */
void gUpdateViewCollisionGrid(void)
{
    u32 x, y, x2, y2, w, h, n, i;
    u32 *p;
    Entry *entry;
    D_8009F82C = 0;
    gCellEntityCount = 0;
    if (gGameSession->mode == 1) {
        h = gCellClipRect.height - 16;
        w = gCellClipRect.width - gMapGridHeader->count * 8 + 8;
        x = y = (gMapGridHeader->count - 1) * 4;
    } else if (gPlayerCount == 1) {
        h = w = 20;
        x = gGridOriginX * 4;
        y = gGridOriginY * 4;
    } else {
        x = gGridOriginX * 4;
        y = gGridOriginY * 4;
        x2 = D_8009F836 * 4;
        y2 = D_8009F837 * 4;
        if (x2 < x) { x2 ^= x; x ^= x2; x2 ^= x; }
        if (y2 < y) { y2 ^= y; y ^= y2; y2 ^= y; }
        w = x2 - x + 20;
        h = y2 - y + 20;
    }
    n = w * h;
    p = gCellMap;
    gCellClipRect.x = x;
    gCellClipRect.y = y;
    gCellClipRect.w = w;
    gCellClipRect.h = h;
    { register u32 count __asm__("$3");
    count = n >> 2;
    while (count) { *p++ = 0; count--; } }
    if (gPlayerCount == 1) gBuildSingleViewObjectGrid();
    else gBuildDualViewObjectListsAndGrid();
    i = 0;
    __asm__ volatile ("" : "+r" (i));
    entry = gPlayerCollisionCandidates;
    do { entry->flag = 0; i++; entry++; } while (i < 2);
    gSweepCollisionGridPairs();
}
