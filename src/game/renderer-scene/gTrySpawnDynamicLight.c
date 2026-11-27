// Categories: Graphics > Lighting | Graphics > Effects
#include "game/types.h"

extern s32 gSpawnDynamicLightNode(u8 *data, s32 arg1) __asm__("func_80049C38");
s32 gTrySpawnDynamicLight(u8 *data, s32 arg1) __asm__("func_80049E88");


/**
 * @brief Boolean wrapper around func_80049C38 that spawns a map light node and returns 1 on success or 0 on failure.
 *
 * Confirmed: passes data and arg1 directly to func_80049C38(data, arg1) and returns (result != 0).
 * Category follows callee func_80049C38 (dynamic map light node spawner) and caller func_80049EA8 (0x74414354 resource lookup).
 * Likely: converts func_80049C38 node pointer / zero return into a 0/1 predicate for callers that do not keep the node pointer; Unknown: why func_80049EA8 discards the return value.
 *
 * @param data Pointer to Unk49C38 light descriptor block forwarded to func_80049C38.
 * @param arg1 Lifetime/duration parameter forwarded to func_80049C38 (-1 in func_80049EA8).
 * @return 1 if func_80049C38 returns nonzero (light node allocated), else 0.
 *
 * @par Globals
 * None directly (func_80049C38 touches D_8009F778, D_8009F780, D_8009F784, D_8009FAA4).
 * @par Control flow
 * Single call to func_80049C38 followed by != 0 normalization (sltu $v0, $zero, $v0).
 * @par Calls
 * func_80049C38 - resolves surface height, allocates a node from D_8009F784, and initializes the map light.
 * @par Side effects
 * Allocates and registers a light node in D_8009F784/D_8009F778 via func_80049C38 when successful.
 * @par Callers
 * Confirmed: func_80049EA8 (0x80049ED4 in asm/us/main.s, passing func_80019C38(..., 0x74414354, ...) - 4 and -1) / Likely: none / Unknown: none.
 */
s32 gTrySpawnDynamicLight(u8 *data, s32 arg1)
{
    return gSpawnDynamicLightNode(data, arg1) != 0;
}
