/* Source evidence on synthetic fixtures, not a retail parity claim.
 * Hardware-free helpers are linked unchanged, including the Cobj chunk parser.
 */
#include "leaf_kit.h"

/* @link 80012E48 */
/* @link 8001306C */

struct AnimEntry {
    u8 max, cur, type, flags;
    s16 rate, timer;
    u8 *src, *dst;
};

struct AnimNode {
    struct AnimNode *next;
    s32 pad, count;
    struct AnimEntry entries[1];
};

struct ModelHeader {
    u32 tag, length;
    struct AnimNode *anim;
    u32 flags, x10, x14, x18;
    u8 *p1c, *p20, *p24, *p28, *p2c;
    u32 p30, *p34, p38;
};

struct Chunk {
    u32 tag, length, index;
};

struct ModelFixture {
    struct ModelHeader geometry;
    struct { struct Chunk header; u32 words[3]; } refs[3];
    struct { struct Chunk header; u32 data; } vertices, bounds, polygons,
        hierarchy, mesh, anim_data, hs;
    struct { struct Chunk header; u32 records[10]; } triangles;
    struct AnimNode track;
    u32 sprites[2];
    struct Chunk wave;
};

u32 triangle_bytes __asm__("D_8009F528");
u8 *geometry __asm__("D_8009F52C");
u8 *polygons __asm__("D_8009F530");
u8 *triangles __asm__("D_8009F534");
u8 *hierarchy __asm__("D_8009F538");
u8 *mesh __asm__("D_8009F53C");
u8 *bounds __asm__("D_8009F540");
u8 *anim_data __asm__("D_8009F544");
u32 *hs __asm__("D_8009F548");
struct AnimNode *track __asm__("D_8009F54C");
u32 *wave __asm__("D_8009F550");
u8 **vertex_table __asm__("D_8009F554");
s32 *normal_table __asm__("D_8009F558");
s32 *reloc_table __asm__("D_8009F55C");
u8 **ref_table __asm__("D_8009F560");
struct AnimNode *track_list __asm__("D_8009F564");
u32 *wave_list __asm__("D_8009F568");
u32 texpage[2] __asm__("D_800A8E80");

extern void gRelocateModelResource(struct ModelHeader **out, void *data,
                                   u32 size, u32 arg3) __asm__("func_800131A0");

static void chunk(struct Chunk *header, u32 tag, u32 length, u32 index)
{
    header->tag = tag;
    header->length = length;
    header->index = index;
}

static void init_model(struct ModelFixture *model)
{
    static u8 *refs[3], *vertices[1];
    int i;

    lk_fill(model, 0, sizeof(*model));
    ref_table = refs;
    vertex_table = vertices;
    model->geometry.tag = 0x34444749; /* 4DGI: header starts at the tag. */
    model->geometry.length = sizeof(model->geometry);
    model->geometry.flags = 0x02000123;
    model->geometry.p20 = (u8 *)1;
    model->geometry.p2c = (u8 *)2;
    for (i = 0; i < 3; i++) {
        chunk(&model->refs[i].header, 0x33445246, sizeof(model->refs[i]), i);
    }
    model->refs[1].words[0] = 0x3444564C; /* Resolve one 4DVL reference. */
    model->refs[1].words[1] = 1;
    chunk(&model->vertices.header, 0x3444564C, sizeof(model->vertices), 0);
    chunk(&model->bounds.header, 0x33444242, sizeof(model->bounds), 0);
    chunk(&model->polygons.header, 0x3344514C, sizeof(model->polygons), 0);
    chunk(&model->hierarchy.header, 0x33444859, sizeof(model->hierarchy), 0);
    chunk(&model->mesh.header, 0x33444D49, sizeof(model->mesh), 0);
    chunk(&model->anim_data.header, 0x416E6D44, sizeof(model->anim_data), 0);
    chunk(&model->hs.header, 0x33444853, sizeof(model->hs), 0);
    chunk(&model->triangles.header, 0x3344544C, sizeof(model->triangles), 0);
    model->triangles.records[0] = 1;
    model->triangles.records[2] = 2;
    model->triangles.records[6] = 3;
    model->triangles.records[9] = 1;
    texpage[0] = 0x145;
    texpage[1] = 0x28A;
}

static void test_800131A0(void)
{
    struct ModelFixture model;
    struct ModelHeader *out = 0;
    struct AnimNode prior_track;
    u32 prior_wave[2];

    /* @leaf 800131A0 source real chunk indexing precedes reference, bounds and triangle relocation; track and wave insertion, offsets, rates and absent-track flag clearing */
    LK_FUNC("func_800131A0");
    init_model(&model);
    model.track.next = (struct AnimNode *)0x33445441; /* 3DTA tag before linking */
    model.track.pad = sizeof(model.track) + sizeof(model.sprites);
    model.track.count = 1;
    model.track.entries[0].max = 4;
    model.track.entries[0].type = 16;
    model.track.entries[0].rate = 8;
    model.track.entries[0].src = (u8 *)4;
    model.track.entries[0].dst = (u8 *)8;
    chunk(&model.wave, 0x3344414C, sizeof(model.wave), 0);
    track_list = &prior_track;
    wave_list = prior_wave;

    gRelocateModelResource(&out, &model, sizeof(model), 0x55);
    FCK_EQ(out, &model.geometry, "publishes indexed geometry header");
    FCK_EQ(model.geometry.p34, &model.bounds.header.index, "bounds pointer");
    FCK_EQ(model.bounds.data, 0x1230, "bounds field uses low flags halfword shifted by four");
    FCK_EQ(model.geometry.p1c, &model.refs[0].words[1], "reference at +1c points past tag");
    FCK_EQ(model.geometry.p20, &model.refs[1].words[1], "reference at +20 points past tag");
    FCK_EQ(model.geometry.p2c, &model.refs[2].words[1], "reference at +2c points past tag");
    FCK_EQ(model.refs[1].words[2], &model.vertices.data, "vertex index resolved");
    FCK_EQ(model.geometry.p24, &model.polygons.data, "polygon payload assigned");
    FCK_EQ(model.geometry.p28, model.triangles.records, "triangle payload assigned");
    FCK_EQ(model.triangles.records[5], 0x145, "type 2 uses page table");
    FCK_EQ(model.triangles.records[6], 2, "type 3 becomes type 2");
    FCK_EQ(model.triangles.records[9], 0x2AA, "type 3 adds page flag");
    FCK_EQ(model.geometry.x14, &model.hierarchy.data, "hierarchy installed with bit 25");
    FCK_EQ(model.geometry.x18, &model.mesh.data, "mesh installed with bit 25");
    FCK_EQ(model.geometry.x10, &model.anim_data.header.index, "animation data pointer");
    FCK_EQ(model.geometry.p38, &model.hs.header.index, "3DHS payload pointer");

    FCK_EQ(model.geometry.flags, 0x03000123, "track sets flag bit 24 only");
    FCK_EQ(model.geometry.anim, &model.track, "header references track");
    FCK_EQ(track_list, &model.track, "track prepended");
    FCK_EQ(model.track.next, &prior_track, "track links to old head");
    FCK_EQ(model.track.entries[0].src, &model.sprites[1], "source relative to entry table end");
    FCK_EQ(model.track.entries[0].dst, &model.triangles.records[2], "destination relative to triangles");
    FCK_EQ(model.track.entries[0].rate, 4096, "type 16 uses 0x20000 / (8 * 4)");
    FCK_EQ(wave_list, &model.wave, "wave prepended");
    FCK_EQ(model.wave.tag, prior_wave, "wave links to old head");
    FCK_EQ(model.wave.length, &model.hierarchy.header, "wave points four bytes past polygon count");

    init_model(&model);
    model.geometry.flags = 0x01000123;
    model.geometry.anim = &prior_track;
    model.geometry.x14 = 0x1234;
    model.geometry.x18 = 0x5678;
    track_list = &prior_track;
    wave_list = prior_wave;
    gRelocateModelResource(&out, &model,
                           sizeof(model) - sizeof(model.track) - sizeof(model.sprites) - sizeof(model.wave), 0);
    FCK_EQ(model.geometry.flags, 0x123, "missing track clears bit 24 only");
    FCK_EQ(model.geometry.anim, 0, "missing track clears header pointer");
    FCK_EQ(track_list, &prior_track, "missing track preserves list");
    FCK_EQ(wave_list, prior_wave, "missing wave preserves list");
    FCK_EQ(model.geometry.x14, 0x1234, "no bit 25: hierarchy field preserved");
    FCK_EQ(model.geometry.x18, 0x5678, "no bit 25: mesh field preserved");
}

int main(void)
{
    test_800131A0();
    return lk_done("leaf_res_r4");
}
