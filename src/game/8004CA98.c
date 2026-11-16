// Categories: Resources > Asset Loader | Gameplay > Object Spawning
#include "game/types.h"

struct Unk4CA98 {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};

extern u8 *func_80019C38(s32 arg0, s32 arg1, u32 tag, s32 arg3);
extern void func_8004C884(u8 *data);
extern void func_800181D4(void *block);

/**
 * @brief Extracts a type-0x5A `0x74414354` spawn record, registers it via func_8004C884, and frees the request block.
 *
 * Looks up the `0x74414354` chunk matching `resource->unk1C`, backs up 4 bytes to its record base for the 0x4C-byte registrar func_8004C884, and frees `resource`. Category Resources > Asset Loader | Gameplay > Object Spawning follows chunk lookup, D_800A7040 registration, and dispatch from func_8004D6D4.
 *
 * @param resource Request block (`+0x00` blob base, `+0x1C` section id, `+0x20` byte length), freed before return.
 *
 * @par Globals
 * None directly (func_8004C884 writes D_800A7040 via func_8003C404).
 *
 * @par Control flow
 * Unconditional linear sequence of three calls; no branches or loops.
 *
 * @par Calls
 * func_80019C38 - locates the `0x74414354` sub-chunk in `resource->unk00` (length `unk20`, id `unk1C`).
 * func_8004C884 - registers the 0x4C-byte spawn record (`chunk - 4`) into D_800A7040.
 * func_800181D4 - frees the `resource` request block.
 *
 * @par Side effects
 * Updates D_800A7040 / deferred spawn queue via func_8004C884 and frees `resource`.
 *
 * @par Callers
 * Confirmed: func_8004D6D4 (0x8004D7DC in asm/us/main.s, switch case `0x5A`) / Likely: none / Unknown: none.
 */
void func_8004CA98(struct Unk4CA98 *resource)
{
    func_8004C884(func_80019C38(resource->unk00, resource->unk20, 0x74414354,
                                resource->unk1C) - 4);
    func_800181D4(resource);
}
