// Categories: Graphics > VRAM Transfer | Platform > GPU
#include "game/types.h"

struct S {
    u16 a;
    u16 b;
    u16 c;
    u16 d;
};

extern void gLoadImage(void *rect, void *data) __asm__("func_80092F38");
extern u16 gGetClutId(s32 x, s32 y) __asm__("func_8009226C");
u16 gLoadClut256(s32 x, s32 a, s32 b) __asm__("func_80092078");


/**
 * @brief Uploads a 256-entry (0x100x1) CLUT palette to VRAM and returns its GPU CLUT handle.
 *
 * Confirmed: builds an 8-byte stack RECT {a, b, 0x100, 1}, transfers main-memory palette data x to VRAM via func_80092F38, and packs (a, b) into a 16-bit CLUT handle via func_8009226C. Likely: PsyQ libgpu LoadClut (8-bit 256-color CLUT upload).
 *
 * @param x Pointer to 256 halfword (512-byte) palette data in main RAM.
 * @param a Destination VRAM X coordinate (in halfwords).
 * @param b Destination VRAM Y coordinate (scanline).
 * @return Packed 16-bit GPU CLUT handle ((b << 6) | ((a >> 4) & 0x3F)).
 *
 * @par Globals
 * None directly (func_80092F38 reads D_8009DDE8 and D_80012A94).
 *
 * @par Control flow
 * Straight-line stack RECT initialization, VRAM upload dispatch, and packed CLUT handle return; no branches or loops.
 *
 * @par Calls
 * func_80092F38 - executes LoadImage VRAM transfer for the 0x100x1 RECT; func_8009226C - packs (a, b) into a CLUT id.
 *
 * @par Side effects
 * Transfers 0x100 halfwords from x into GPU VRAM at (a, b) via D_8009DDE8->callback.
 *
 * @par Callers
 * Confirmed: none in edges.json or asm/us/main.s / Likely: unreferenced libgpu LoadClut helper / Unknown: indirect or external callers.
 *
 * @par Environment path
 * CLUT upload helper for 8-bit textures: sends 256 palette entries to VRAM at
 * a,b through func_80092F38 and returns the CLUT id (b shifted left 6) or (a
 * shifted right 4 and 0x3F). No caller reaches it in the sources.
 */
u16 gLoadClut256(s32 x, s32 a, s32 b)
{
    struct S s;

    s.a = a;
    s.b = b;
    s.c = 0x100;
    s.d = 1;
    gLoadImage(&s, (void *)x);
    return gGetClutId(a, b);
}
