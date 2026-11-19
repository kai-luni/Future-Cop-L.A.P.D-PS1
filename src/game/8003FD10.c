// Categories: Game Engine > Command Queue | Gameplay > Object Spawning
#include "game/types.h"

extern void gSpawnTypeAtOwner(void *object, u8 kind, s32 arg2, s32 arg3, s32 arg4) __asm__("func_800402DC");
extern void func_8003FB90(void *object, u8 kind);

/**
 * @brief Executes entity queue commands 0x64 (spawn/place) and 0x65 (retire/unlink) for an entity and kind byte.
 *
 * Dispatches command opcode `message` from func_8004CECC to either map-cell spawning via func_800402DC or active/deferred node removal via func_8003FB90. Category Game Engine > Command Queue | Gameplay > Object Spawning follows opcode dispatch from func_8004CECC and grid spawn/cleanup callees.
 *
 * @param message Command opcode (`0x64` to spawn/queue, `0x65` to retire/remove; others ignored).
 * @param object Entity pointer passed as owner/filter to the spawn or cleanup helper.
 * @param kind Command argument truncated to `u8` selecting the spawn-table column/kind.
 *
 * @par Globals
 * None directly (callees access D_800A7040, D_800A1070, and D_8009F30C).
 *
 * @par Control flow
 * Two-arm `switch (message)` (`0x64` and `0x65`) with an empty implicit default.
 *
 * @par Calls
 * func_800402DC - case `0x64`: attempts immediate or deferred placement with `(object, (u8)kind, 0, 0, 0)`.
 * func_8003FB90 - case `0x65`: retires/frees D_800A1070 objects and unlinks D_8009F30C nodes matching `(object, (u8)kind)`.
 *
 * @par Side effects
 * Allocates/spawns or retires/frees grid objects and deferred spawn entries via callees.
 *
 * @par Callers
 * Confirmed: func_8004CECC (0x8004D220 in asm/us/main.s, command opcodes `0x64` and `0x65`) / Likely: none / Unknown: none.
 */
void func_8003FD10(s32 message, void *object, s32 kind)
{
    switch (message) {
    case 0x64:
        gSpawnTypeAtOwner(object, kind, 0, 0, 0);
        break;
    case 0x65:
        func_8003FB90(object, kind);
        break;
    }
}
