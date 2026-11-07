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
extern void func_800188D0(s32, s32, s32, void *);
extern s32 func_80067684(void *, s32);
extern void func_80010750(void *);
extern void func_80010234(void *, void *, s32);
s32 func_8002DAA4(struct Object *self, struct Data *data, s32 arg2)
{
    struct DataInner *inner;
    s32 mode;
    s32 value;
    func_8002AD58(2, self, data, arg2);
    if (self->model->anim != 0)
        func_80044988(self->model->anim, self->track, 1, 0);
    inner = data->inner;
    func_800188D0(inner->id, 0, 0, self->coord);
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
    func_80010750(self->coord);
    func_80010234(((self->frame << 4) + 8) + self->model->frames, self->mat, 1);
    return 0;
}
