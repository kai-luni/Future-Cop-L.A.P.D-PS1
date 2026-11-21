// Categories: Resources > Asset Loader | Resources > Spawn Tables | Gameplay > Object Spawning
#include "game/types.h"

extern s32 gSpawnTableRow __asm__("D_8009F310");
extern void *jtbl_800115B8[];

extern void gSetSlotSpawnDescriptor(void *data) __asm__("func_8003BC7C");
extern void gRegisterActorDef5D(void *data) __asm__("func_80040DCC");
extern void gRegisterActorDef5E(void *data) __asm__("func_80044770");
extern void gRegisterActorDef5A(void *data) __asm__("func_8004C884");
extern void gRegisterActorDef5C(void *data) __asm__("func_8006EAF0");
extern void gRegisterActorDef59(void *data) __asm__("func_800703A8");
extern void gRegisterActorDef58(void *data) __asm__("func_80073F88");
extern void gCloneAndInstallSpawnRecord(void *data) __asm__("func_800767E4");
s32 gParseCsfxChunks(s32 arg0, s32 arg1) __asm__("func_8003BFD0");


/**
 * @brief Walks a Csfx chunk stream and dispatches type 0x56..0x5E spawn/snapshot records.
 *
 * Saves D_8009F310, resets it to 0, iterates over size-prefixed chunks in [arg0, arg0+arg1),
 * and dispatches byte type - 0x56 (0..8) through jtbl_800115B8 to update D_8009F310 or
 * register spawn records, restoring D_8009F310 upon consuming the full stream.
 *
 * @param arg0 Pointer to the start of the Csfx chunk buffer.
 * @param arg1 Total byte length of the chunk stream.
 * @return 1 if all chunks had valid types 0x56..0x5E, or 0 immediately on an unknown type.
 *
 * @par Globals
 * D_8009F310 - active spawn-table row (saved/restored, set by chunk type 0x56); jtbl_800115B8 - 9-entry dispatch table.
 *
 * @par Control flow
 * Loop reads size = ptr[1] and type = ((u8 *)ptr)[4]; type < 0x56 or > 0x5E returns 0 early; jtbl_800115B8 dispatches 0x56..0x5E, then steps by size - 8 until remaining <= 0.
 *
 * @par Calls
 * jtbl_800115B8 calls: 0x57 func_800767E4, 0x58 func_80073F88, 0x59 func_800703A8, 0x5A func_8004C884, 0x5B func_8003BC7C, 0x5C func_8006EAF0, 0x5D func_80040DCC, 0x5E func_80044770.
 *
 * @par Side effects
 * Temporarily modifies D_8009F310 and populates D_800A0C70/D_800A7040 via handlers.
 *
 * @par Callers
 * Confirmed: func_8003C110 (0x8003C128, processes 0x43736678 Csfx resource node).
 */
s32 gParseCsfxChunks(s32 arg0, s32 arg1)
{
    register s32 *ptr __asm__("$16");
    register s32 remaining __asm__("$17");
    register s32 size __asm__("$18");
    register s32 old __asm__("$19");
    void **table;
    static void *const label_table[] __attribute__((used)) = {
        &&case_56, &&case_57, &&case_58, &&case_59, &&case_5A,
        &&case_5B, &&case_5C, &&case_5D, &&case_5E
    };
    register u32 type __asm__("$2");
    register u32 dispatch_index __asm__("$3");

    ptr = (s32 *)arg0;
    remaining = arg1;
    old = gSpawnTableRow;
    table = (void **)jtbl_800115B8;
    gSpawnTableRow = 0;

loop:
    ptr++;
    size = *ptr;
    ptr++;
    type = ((u8 *)ptr)[4];
    dispatch_index = type - 0x56;
    if (dispatch_index < 9) {
        type = dispatch_index << 2;
        type += (u32)table;
        type = *(u32 *)type;
        goto *(void *)type;
    }
    goto default_case;

case_56:
    gSpawnTableRow = ptr[5];
    goto done;
case_57:
    gRegisterActorDef5E(ptr);
    goto done;
case_58:
    gRegisterActorDef5D(ptr);
    goto done;
case_59:
    gRegisterActorDef5C(ptr);
    goto done;
case_5A:
    gRegisterActorDef5A(ptr);
    goto done;
case_5B:
    gSetSlotSpawnDescriptor(ptr);
    goto done;
case_5C:
    gRegisterActorDef59(ptr);
    goto done;
case_5D:
    gRegisterActorDef58(ptr);
    goto done;
case_5E:
    gCloneAndInstallSpawnRecord(ptr);
    goto done;

default_case:
    return 0;

done:
    ptr = (s32 *)((u8 *)ptr + (size - 8));
    remaining -= size;
    if (remaining > 0) {
        goto loop;
    }
    gSpawnTableRow = old;
    return 1;
}
