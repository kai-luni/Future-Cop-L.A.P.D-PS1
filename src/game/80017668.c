// Categories: Graphics > Model Renderer > Vertex Fetch | Graphics > Animation
#include "game/types.h"

struct ModelTable {
    s32 count;
};

struct ModelVertex {
    u8 unk00[4];
    u8 position[4];
};

struct ModelAnim {
    s32 count;
    struct ModelVertex *frames[1];
};

struct Model {
    u8 unk00[0x0C];
    u32 flags;
    u8 unk10[0x0C];
    struct ModelAnim *anim;
    u8 unk20[0x10];
    u8 parts[4];
    u8 unk34[4];
    struct ModelTable *table;
};

struct ModelInstance {
    struct Model *model;
    u8 coord[0x44];
    s16 frame;
};

extern void gGteLoadMatrix(u8 *coord) __asm__("func_80010750");
extern void gGteTransformVerticesWide(u8 *vertices, s32 *out, s32 count) __asm__("func_80010234");

/**
 * @brief Fetches and transforms one animated vertex of a model part.
 *
 * @param instance Model instance: coord matrix at +0, model at +0x44, frame at
 *        +0x48? (see the structs at the top of the file).
 * @param part Part index.
 * @param out Destination for the transformed vertex.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80010750(instance->coord) loads the matrix; the vertex source depends on
 * bit 25 of the model's flag word: with the bit set the animated frame record
 * (`frame->frames[0] + index`) is used, otherwise the static part data at
 * `vertices + part * 8`; func_80010234 transforms exactly one vertex into @c out
 * (0xC bytes per vertex).
 *
 * @par Likely role
 * The single-vertex accessor used for attachment points and effect origins,
 * where the whole model does not need to be transformed.
 *
 * @par Environment path
 * Fetches a single animated vertex of a model part and transforms it:
 * gGteLoadMatrix (func_80010750) loads the instance coordinate block and
 * gGteTransformVerticesWide (func_80010234) transforms exactly one vertex
 * into the caller's buffer. The source record depends on bit 25 of the model
 * flag word - the per-frame table at model->table when the bit is set,
 * otherwise the static part array indexed through model->parts[part] and the
 * animation record. It is the attachment-point accessor of the renderer: the
 * polygon pass gRunPolygonPass (func_80075A84, src/game/80075A84.c:186) and
 * the compound-object reset func_80058714 (src/game/80058714.c:129) call it,
 * so it is a leaf helper of the render path. It never emits geometry, never
 * touches a global and only produces one transformed point.
 */
void func_80017668(struct ModelInstance *instance, s32 part, s32 *out)
{
    struct Model *model;
    u8 *vertices;
    register struct ModelAnim *frame __asm__("$2");
    register u32 index __asm__("$16");

    model = instance->model;
    if ((model->flags >> 25) & 1) {
        vertices = (u8 *)(model->table + 1);
        vertices += instance->frame * model->table->count * 8;
        gGteLoadMatrix(instance->coord);
        gGteTransformVerticesWide(vertices + part * 8, out, 1);
    } else {
        index = model->parts[part];
        gGteLoadMatrix(instance->coord);
        frame = (struct ModelAnim *)((u32)(instance->frame * 4) +
                                     (u32)model->anim);
        __asm__("" : : "r"(frame));
        index *= 8;
        __asm__("" : "=r"(index) : "0"(index));
        index += 4;
        gGteTransformVerticesWide((u8 *)frame->frames[0] + index, out, 1);
    }
}
