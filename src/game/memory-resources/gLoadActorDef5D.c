// Categories: Resources > Asset Loader | Gameplay > Object Spawning
#include "game/types.h"

struct Unk41070 {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};

extern u8 *func_80019C38(s32 arg0, s32 arg1, u32 tag, s32 arg3);
extern void gRegisterActorDef5D(u8 *data) __asm__("func_80040DCC");
extern void gFreeBlock(void *block) __asm__("func_800181D4");
void gLoadActorDef5D(struct Unk41070 *resource) __asm__("func_80041070");


/**
 * @brief Extracts a type-0x5D `0x74414354` spawn record, registers it via func_80040DCC, and frees the request block.
 *
 * Looks up the `0x74414354` chunk matching `resource->unk1C`, backs up 4 bytes to its header start for the 0x58-byte registrar func_80040DCC, and frees `resource`. Category Resources > Asset Loader | Gameplay > Object Spawning follows chunk lookup, D_800A7040 registration, and dispatch from func_8004D6D4.
 *
 * @param resource Request block (`+0x00` blob base, `+0x1C` section id, `+0x20` byte length), freed before return.
 *
 * @par Globals
 * None directly (func_80040DCC writes D_800A7040 via func_8003C404).
 *
 * @par Control flow
 * Unconditional linear sequence of three calls; no branches or loops.
 *
 * @par Calls
 * func_80019C38 - locates the `0x74414354` sub-chunk in `resource->unk00` (length `unk20`, id `unk1C`).
 * func_80040DCC - registers the 0x58-byte spawn record (`chunk - 4`) into D_800A7040.
 * func_800181D4 - frees the `resource` request block.
 *
 * @par Side effects
 * Updates D_800A7040 / deferred spawn queue via func_80040DCC and frees `resource`.
 *
 * @par Callers
 * Confirmed: func_8004D6D4 (0x8004D7BC in asm/us/main.s, switch case `0x5D`) / Likely: none / Unknown: none.
 *
 * @par Environment path
 * Chunk loader for 'Cact' actor type 0x5D: it finds the 'tACT' (0x74414354)
 * record of the request's section id (+0x1C) inside the request data
 * (+0x0/+0x20) with func_80019C38, passes the record base (result - 4) to the
 * registrar func_80040DCC (0x58-byte record) and frees the request node. Its
 * only caller is the 'Cact' dispatcher func_8004D6D4 (8004D6D4.c:123), itself
 * called from gLoadQueuedAssets (func_80024EC8) for each 'Cact' request
 * during level loading. It feeds the spawn table D_800A7040 through
 * gInstallSpawnRecord.
 */
void gLoadActorDef5D(struct Unk41070 *resource)
{
    gRegisterActorDef5D(func_80019C38(resource->unk00, resource->unk20, 0x74414354,
                                resource->unk1C) - 4);
    gFreeBlock(resource);
}
