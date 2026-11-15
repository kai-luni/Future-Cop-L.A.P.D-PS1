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
extern void func_80010750(u8 *matrix);
extern void func_80010234(u8 *frame, u8 *output, s32 count);

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
 * @par Likely role
 * The two-part rig of an animated object (body plus turret, or the two halves of
 * a walk cycle) - the parts share the coordinate block but have their own frames.
 */
void func_80052D7C(struct Unk52D7C *object)
{
    struct Unk52D7CState *state;
    register u8 *pointer __asm__("$2");
    register s32 offset __asm__("$4");

    state = object->info->state;
    func_80052960(object->coord, state->parts[0].matrix, state->parts[1].matrix);
    func_80010750(state->parts[0].matrix);
    offset = state->parts[0].frame;
    pointer = (u8 *)state->parts[0].model;
    offset <<= 4;
    __asm__("" : "=r"(offset) : "0"(offset));
    pointer = ((struct Unk52D7CModel *)pointer)->frames;
    offset += 8;
    func_80010234(pointer + offset, state->parts[0].output, 1);
    func_80010750(state->parts[1].matrix);
    offset = state->parts[1].frame;
    pointer = (u8 *)state->parts[1].model;
    offset <<= 4;
    __asm__("" : "=r"(offset) : "0"(offset));
    pointer = ((struct Unk52D7CModel *)pointer)->frames;
    offset += 8;
    func_80010234(pointer + offset, state->parts[1].output, 1);
}
