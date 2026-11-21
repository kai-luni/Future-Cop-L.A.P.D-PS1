// Categories: Graphics > Effects | Gameplay > Object Draw Handlers
#include "game/types.h"

struct Unk596C4Part {
    u8 unk00[0x1C];
    s32 x;
    s32 y;
    s32 z;
    u8 unk28[0x24];
};

struct Unk596C4State {
    u8 unk00[0x178];
    struct Unk596C4Part parts[2];
    u8 unk210[0xEC];
    s8 part;
};

struct Unk596C4 {
    u8 unk00[0xAC];
    struct Unk596C4State *state;
};

struct Unk596C4Effect {
    u8 unk00[0x10];
    s32 color;
};

extern s32 gDescriptorBlock __asm__("D_8009AF88");
extern s32 gBeamSparkColorTable[] __asm__("D_8009AF94");

extern void gTransformPartVertex(struct Unk596C4Part *part, s32 arg1, s32 *out) __asm__("func_80017668");
extern u32 func_80019668(void);
extern void gDrawBeamQuad(s32 *from, s32 *to, struct Unk596C4Effect *effect) __asm__("func_80014C64");
void gEmitObjectBeamSegment(struct Unk596C4 *object) __asm__("func_800596C4");


/**
 * @brief Emits one short effect segment from the active part of an object.
 *
 * @param object Object with a state block; state->part (s8) selects the
 *               active part, a negative index skips the effect.
 * @return Nothing.
 *
 * @par Confirmed
 * 1. index = object->state->part; return when index < 0.
 * 2. func_80017668(part, 0, from) fetches the part world position.
 * 3. to = from + part->x/y/z >> 4 per axis (written as (v << 4) >> 8).
 * 4. D_8009AF88 = D_8009AF94[func_80019668() % 3] - a random entry of a
 *    three-word color table.
 * 5. func_80014C64(from, to, effect) spawns the segment; the effect
 *    descriptor sits at &D_8009AF88 - 0x10, so the random word lands in
 *    its color field at +0x10.
 *
 * @par Likely role
 * Spark or tracer emission attached to a moving part (the caller is the
 * glow-disc draw handler family).
 *
 * @par Callers
 * func_800597C0.
 *
 * @par Environment path
 * Transforms an active object part position via gTransformPartVertex, picks a
 * random descriptor using func_80019668, and emits one short effect beam
 * segment via gDrawBeamQuad (called by func_800597C0:147,307).
 */
void gEmitObjectBeamSegment(struct Unk596C4 *object)
{
    struct Unk596C4Part *part;
    s32 from[3];
    s32 to[3];
    s32 index;
    s32 z;

    index = object->state->part;
    if (index < 0) {
        return;
    }
    part = &object->state->parts[index];
    gTransformPartVertex(part, 0, from);
    to[0] = from[0] + ((part->x << 4) >> 8);
    to[1] = from[1] + ((part->y << 4) >> 8);
    z = from[2] + ((part->z << 4) >> 8);
    __asm__("" : : "r"(z));
    to[2] = z;
    gDescriptorBlock = gBeamSparkColorTable[func_80019668() % 3];
    gDrawBeamQuad(from, to,
                  (struct Unk596C4Effect *)((u8 *)&gDescriptorBlock - 0x10));
}
