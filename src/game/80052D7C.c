// Categories: Graphics > Model Renderer > Animation | Graphics > GTE
#include "game/types.h"

struct Unk52D7CModel {
    u8 unk00[0x34];
    u8 *frames;
};

struct Unk52D7CPart {
    struct Unk52D7CModel *model;
    u8 matrix[0x30];
    u8 output[0x14];
    s16 frame;
    s16 unk4A;
};

struct Unk52D7CState {
    u8 unk00[0x178];
    struct Unk52D7CPart parts[2];
};

struct Unk52D7CInfo {
    u8 unk00[0xAC];
    struct Unk52D7CState *state;
};

struct Unk52D7C {
    u8 unk00[0x28];
    u8 coord[0x8C];
    struct Unk52D7CInfo *info;
};

extern void func_80052960(u8 *coord, u8 *first, u8 *second);
extern void gGteLoadMatrix(u8 *matrix) __asm__("func_80010750");
extern void gGteTransformVerticesWide(u8 *frame, u8 *output, s32 count) __asm__("func_80010234");

/**
 * @brief Transforms both parts of a two-part model for the current frame.
 *
 * @param object Object: coordinate block at +0x28 (0x8C bytes), info pointer at
 *        +0xB4, whose state block holds the two parts (each: model pointer, 0x30
 *        matrix, 0x14 output, frame index, half-word at +0x4A).
 * @return Nothing.
 *
 * @par Confirmed flow
 * func_80052960(object->coord, parts[0].matrix, parts[1].matrix) builds the two
 * matrices; for each part func_80010750 loads the matrix into the GTE and
 * func_80010234 transforms one vertex: the source is
 * `models->frames + (frame << 4) + 8` (a 16-byte record per frame, 8 bytes
 * header) and the destination is the part's 0x14-byte output block.
 *
 * @par Player model assembly
 * Called for the companion by func_80057E40, this uses the companion instance
 * as the hierarchy source and writes the matrices of the owner's instances at
 * state+0x178 and state+0x1C4. func_80052960 copies the fifth record's matrix
 * to the first instance and the third record's matrix to the second, provided
 * the hierarchy walk reaches those records before its termination flag. Both
 * instances use the same resource, bound in func_800579D4; their anatomy is open.
 *
 * @par Likely role
 * The two-part rig of an animated object (body plus turret, or the two halves of
 * a walk cycle) - the parts share the coordinate block but have their own frames.
 *
 * @par Environment path
 * Transforms both parts of a two-part model for the current frame:
 * func_80052960 builds the two part matrices from the object's coordinate
 * block and the two model records, then for each part gGteLoadMatrix loads
 * that matrix and gGteTransformVerticesWide transforms one vertex from
 * models->frames + (frame << 4) + 8 into the part's 0x14-byte output block.
 * It is the per-frame rig update of a compound actor, the body plus turret or
 * the two halves of a walk cycle, not a scene pass: it emits no polygon and
 * reads no global. Its caller is the player/companion step func_80057E40
 * (src/game/80057E40.c:186), which runs before the model is submitted, so the
 * unit only keeps the instance vertex data current for the actor passes that
 * draw it later.
 */
void func_80052D7C(struct Unk52D7C *object)
{
    struct Unk52D7CState *state;
    register u8 *pointer __asm__("$2");
    register s32 offset __asm__("$4");

    state = object->info->state;
    func_80052960(object->coord, state->parts[0].matrix, state->parts[1].matrix);
    gGteLoadMatrix(state->parts[0].matrix);
    offset = state->parts[0].frame;
    pointer = (u8 *)state->parts[0].model;
    offset <<= 4;
    __asm__("" : "=r"(offset) : "0"(offset));
    pointer = ((struct Unk52D7CModel *)pointer)->frames;
    offset += 8;
    gGteTransformVerticesWide(pointer + offset, state->parts[0].output, 1);
    gGteLoadMatrix(state->parts[1].matrix);
    offset = state->parts[1].frame;
    pointer = (u8 *)state->parts[1].model;
    offset <<= 4;
    __asm__("" : "=r"(offset) : "0"(offset));
    pointer = ((struct Unk52D7CModel *)pointer)->frames;
    offset += 8;
    gGteTransformVerticesWide(pointer + offset, state->parts[1].output, 1);
}
