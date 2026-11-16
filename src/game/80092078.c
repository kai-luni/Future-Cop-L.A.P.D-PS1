// Categories: Graphics > VRAM Transfer | Platform > GPU
#include "game/types.h"

struct S {
    u16 a;
    u16 b;
    u16 c;
    u16 d;
};

extern void func_80092F38(void *rect, void *data);
extern u16 func_8009226C(s32 x, s32 y);

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
 */
u16 func_80092078(s32 x, s32 a, s32 b)
{
    struct S s;

    s.a = a;
    s.b = b;
    s.c = 0x100;
    s.d = 1;
    func_80092F38(&s, (void *)x);
    return func_8009226C(a, b);
}
