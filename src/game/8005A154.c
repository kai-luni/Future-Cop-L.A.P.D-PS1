// Categories: Platform > Sound > SE | Graphics > HUD > Gauge
#include "game/types.h"

struct Unk5A154 {
    u8 unk00[0xC1];
    s8 side;
};

struct Unk5A154Gauge {
    u8 unk00[0x13];
    u8 sound;
};

extern s32 D_8009FA58[];

extern s32 func_80046C84(s32 id, s32 arg1);

/**
 * @brief Plays the gauge's edge sound with a per-slot parameter.
 *
 * func_8005A190 calls this on both edges of the gauge fill machine (full to filling, filling to empty). The effect id is the byte at +0x13 of the gauge record, so each gauge carries its own sound.
 *
 * @param object Gauge owner; the signed side byte sits at +0xC1.
 * @param gauge Gauge record; the effect id byte sits at +0x13.
 * @return Nothing.
 *
 * @par Globals
 * D_8009FA58 - per-slot table, indexed by the side byte and read only.
 *
 * @par Control flow
 * Two field loads, a sign-extended index scaled by 4, one call.
 *
 * @par Calls
 * func_80046C84 - resolves the id in the effect table and starts it, with the D_8009FA58 entry as its second argument so the same id sounds different per player slot.
 *
 * @par Side effects
 * One sound voice starts; the gauge and object fields are not modified, which is why func_8005A190 can pass a third argument that this body never reads.
 *
 * @par Callers
 * Confirmed: func_8005A190 (0x8005A208), the gauge update.
 */
void func_8005A154(struct Unk5A154 *object, struct Unk5A154Gauge *gauge)
{
    func_80046C84(gauge->sound, D_8009FA58[object->side]);
}
