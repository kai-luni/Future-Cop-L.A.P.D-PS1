// Categories: Gameplay > Entity Animation | Graphics > Model Renderer
typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;
typedef unsigned char u8;
struct Model { u8 pad[0x10]; void *anim; u8 pad14[0x20]; u8 *frames; };
struct Object { u8 pad00[0x24]; u32 flags; struct Model *model; u8 coord[0x24]; s32 pos[3]; u8 mat[0x14]; s16 frame; u8 pad72[0x2e]; u8 track[1]; };
struct DataInner { u8 pad[0x24]; s32 height; s16 id; s16 mode; };
struct Data { u8 pad[8]; struct DataInner *inner; };
extern s32 func_8002AD58(s32, struct Object *, struct Data *, s32);
extern void func_80044988(void *, void *, s32, s32);
extern void gBuildRotationMatrix(s32, s32, s32, void *) __asm__("func_800188D0");
extern s32 func_80067684(void *, s32);
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, void *, s32) __asm__("func_80010234");
/**
 * @brief Advances one entity's animation and places its frame vertex.
 *
 * @param self Entity: flags at +0x24, model at +0x28, transform at +0x2C, position
 *        at +0x50, matrix at +0x60, frame index at +0x70, track data at +0xA2.
 * @param data Data block whose inner record holds height (+0x24), id and mode.
 * @param arg2 Forwarded to func_8002AD58.
 * @return 0.
 *
 * @par Confirmed
 * func_8002AD58(2, self, data, arg2) runs the behaviour step; when the model has
 * an animation record, func_80044988(model->anim, track, 1, 0) advances it;
 * func_800188D0(inner->id, 0, 0, coord) builds the rotation matrix, the vertical
 * offset is `func_80067684(pos, mode) + inner->height`, then func_80010750 loads
 * the matrix and func_80010234 transforms one vertex
 * (`model->frames + (frame << 4) + 8`) into the entity's 0x14 target.
 *
 * @par Likely role
 * The "walk animator" of a ground entity: height from the data record plus the
 * floor query func_80067684 keeps the frame on the terrain.
 */
s32 func_8002DAA4(struct Object *self, struct Data *data, s32 arg2)
{
    struct DataInner *inner;
    s32 mode;
    s32 value;
    func_8002AD58(2, self, data, arg2);
    if (self->model->anim != 0)
        func_80044988(self->model->anim, self->track, 1, 0);
    inner = data->inner;
    gBuildRotationMatrix(inner->id, 0, 0, self->coord);
    value = inner->mode;
    if (value == 0) goto zero;
    if (value == 1) { mode = -2; goto done; }
    mode = value - 2;
    goto done;
zero:
    mode = -1;
done:
    self->pos[1] = func_80067684(self->pos, mode) + inner->height;
    self->flags |= 0xA00000;
    gGteLoadMatrix(self->coord);
    gGteTransformVerticesWide(((self->frame << 4) + 8) + self->model->frames, self->mat, 1);
    return 0;
}
