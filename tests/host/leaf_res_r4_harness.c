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

/* @link 8001E4C0 */
/* @link 80020610 */
/* @link 80020618 */

struct Profile {
    u8 pad0[0x200];
    u8 active, pad201[7];
    u32 result;
    u8 pad20C[0x6F8];
    void *volatile manager;
    u8 pad908[0x3D];
    u8 page, pad946[2];
};

struct Profile *profile __asm__("D_8009F92C");
u8 *mc_state __asm__("D_8009AB08");

static struct { struct Profile image; u32 guard; } profile_storage;
static u32 mc_storage[0x1BB8 / 4 + 1];
static const char alloc_call[] = "func_80017EBC";
static int alloc_count;

void *mock_alloc(s32 size, s32 tag) __asm__("func_80017EBC");
void *mock_alloc(s32 size, s32 tag)
{
    fck_record(alloc_call, size, tag, (s32)profile);
    if (alloc_count++ == 0)
        return &profile_storage.image;

    /* The defaults must be visible before allocating the second block. */
    FCK_EQ(profile->result, 3, "result is set before manager allocation");
    FCK_EQ(profile->page, 2, "page is set before manager allocation");
    FCK_EQ(profile->active, 0, "active byte still zero at manager allocation");
    FCK_EQ(profile->manager, 0, "manager slot still zero at allocation");
    return mc_storage;
}

extern void gOpenProfileSaveSubsystem(void) __asm__("func_8007F9CC");

static void test_8007F9CC(void)
{
    static const char *order[] = {alloc_call, alloc_call};
    struct FckCall first, second;
    int i;

    /* @leaf 8007F9CC source allocate and publish 0x948-byte profile, zero and set defaults before manager allocation, then initialize channel zero in the second block */
    LK_FUNC("func_8007F9CC");
    profile = 0;
    mc_state = 0;
    alloc_count = 0;
    fck_log_reset();
    lk_fill(&profile_storage, 0xA5, sizeof(profile_storage));
    lk_fill(mc_storage, 0x5A, sizeof(mc_storage));

    gOpenProfileSaveSubsystem();

    fck_expect_order(order, 2, "exactly two allocations");
    first = fck_call_n(alloc_call, 0);
    second = fck_call_n(alloc_call, 1);
    FCK_EQ(first.a0, 0x948, "profile allocation size");
    FCK_EQ(first.a1, 2, "profile allocation tag");
    FCK_EQ(first.a2, 0, "profile initially unpublished");
    FCK_EQ(second.a0, 0x1BB8, "manager allocation size from original size helper");
    FCK_EQ(second.a1, 2, "manager allocation tag");
    FCK_EQ(second.a2, &profile_storage.image, "profile published before manager allocation");
    FCK_EQ(profile, &profile_storage.image, "profile global holds first allocation");
    FCK_EQ(profile->active, 1, "profile active after manager allocation");
    FCK_EQ(profile->result, 3, "result defaults to three");
    FCK_EQ(profile->manager, mc_storage, "second allocation saved at +904");
    FCK_EQ(profile->page, 2, "page defaults to two");
    FCK_EQ(lk_all(profile, 0, 0x200, 0), 1, "profile zero before +200");
    FCK_EQ(lk_all(profile, 0x201, 0x208, 0), 1, "profile zero between active and result");
    FCK_EQ(lk_all(profile, 0x20C, 0x904, 0), 1, "profile image zero");
    FCK_EQ(lk_all(profile, 0x908, 0x945, 0), 1, "profile zero after manager pointer");
    FCK_EQ(lk_all(profile, 0x946, 0x948, 0), 1, "last profile bytes zero");
    FCK_EQ(profile_storage.guard, 0xA5A5A5A5, "profile guard untouched");

    FCK_EQ(mc_state, mc_storage, "original initializer installs second block");
    FCK_EQ(mc_storage[0], 0, "manager channel is zero");
    for (i = 0; i < 16; i++) {
        FCK_EQ(mc_storage[0x44 / 4 + i], mc_state + 0xC4 + i * 0x80,
               "manager slot frame pointer");
        FCK_EQ(mc_storage[0x84 / 4 + i], mc_state + 0x104 + i * 0x80,
               "manager directory frame pointer");
    }
    FCK_EQ(mc_storage[0x1BB8 / 4 - 1], 0, "manager last word zeroed");
    FCK_EQ(mc_storage[0x1BB8 / 4], 0x5A5A5A5A, "manager guard untouched");
}

/* @link 80019470 */
/* @link 8001A81C */

extern void gBuildViewMatrix(u8 *pose, u8 *view) __asm__("func_80018860");

static void test_80018860(void)
{
    static const s32 pose[12] = {
        0, -65536, 0,
        32768, 0, 0,
        -16384, 32768, 65536,
        101, -103, 107
    };
    static const s32 expected[12] = {
        0, 32768, -16384,
        -65536, 0, 32768,
        0, 0, 65536,
        -103, -50, -29
    };
    s32 source[12], guarded[14];
    int i;

    /* @leaf 80018860 source writes a stored transpose, then negates each component from the original column-strided multiply, which shifts each signed Q16 product before adding */
    LK_FUNC("func_80018860");
    for (i = 0; i < 12; i++)
        source[i] = pose[i];
    lk_fill(guarded, 0xA5, sizeof(guarded));

    /* The helper reads indices 2, 5, 8 for Z: floor(-101/4) +
     * floor(-103/2) + 107 = -26 - 52 + 107 before negation. */
    gBuildViewMatrix((u8 *)source, (u8 *)(guarded + 1));
    for (i = 0; i < 9; i++)
        FCK_EQ(guarded[i + 1], expected[i], "transposed rotation word");
    FCK_EQ(guarded[10], expected[9], "negated rotated X");
    FCK_EQ(guarded[11], expected[10], "negated rotated Y");
    FCK_EQ(guarded[12], expected[11], "negated rotated Z with per-product rounding");
    for (i = 0; i < 12; i++)
        FCK_EQ(source[i], pose[i], "source pose word untouched");
    FCK_EQ(guarded[0], 0xA5A5A5A5, "word before output untouched");
    FCK_EQ(guarded[13], 0xA5A5A5A5, "word after output untouched");
}

/* @link 80019AA8 */
/* @link 80019A74 */
/* @link 80019D00 */

struct ModelResource {
    void *data;
    s32 unk04, unk08;
    struct ModelResource *next, *prev;
    s32 unk14, group, name, size;
};

struct ModelResource *resource_head __asm__("D_8009F5B8");
struct ModelResource *resource_tail __asm__("D_8009F5BC");

static const char free_call[] = "func_800181D4";

void mock_free(void *resource) __asm__("func_800181D4");
void mock_free(void *resource)
{
    fck_record(free_call, (s32)resource, (s32)resource_head, (s32)resource_tail);
}

extern void gPrepareModelResource(struct ModelResource *resource) __asm__("func_80013550");

static void test_80013550(void)
{
    static const char *duplicate_order[] = {free_call};
    struct { struct Chunk prefix; struct ModelFixture model; } input;
    struct ModelFixture *model = &input.model;
    struct ModelFixture unused;
    struct ModelResource prior, request, duplicate;
    struct FckCall released;

    /* @leaf 80013550 source new key relocates +0 using length at +20, installs output and clears +8 before registry append; duplicate key frees only the request */
    LK_FUNC("func_80013550");
    chunk(&input.prefix, 0x54455354, sizeof(input.prefix), 0);
    init_model(model);
    init_model(&unused);
    lk_fill(&prior, 0, sizeof(prior));
    lk_fill(&request, 0, sizeof(request));
    request.data = &input;
    request.unk04 = 0x12345678;
    request.unk08 = 0x76543210;
    request.group = 7;
    request.name = -9;
    request.size = (u8 *)&model->track - (u8 *)&input;
    prior.group = 7;
    prior.name = -8;
    resource_head = resource_tail = &prior;
    model->track.next = (struct AnimNode *)0x33445441;
    model->track.pad = sizeof(model->track) + sizeof(model->sprites);
    fck_log_reset();

    gPrepareModelResource(&request);

    FCK_EQ(fck_log_count, 0, "new resource not freed");
    FCK_EQ(request.data, &model->geometry, "new resource publishes relocated header");
    FCK_EQ(request.unk08, 0, "new resource clears descriptor word +8");
    FCK_EQ(request.unk04, 0x12345678, "new resource preserves word +4");
    FCK_EQ(request.group, 7, "new resource preserves group");
    FCK_EQ(request.name, -9, "new resource preserves signed name");
    FCK_EQ(request.size, (u8 *)&model->track - (u8 *)&input, "new resource preserves byte length");
    FCK_EQ(model->geometry.p28, model->triangles.records, "length includes final triangle chunk");
    FCK_EQ(model->geometry.anim, 0, "length excludes trailing track chunk");
    FCK_EQ(resource_head, &prior, "registry head preserved");
    FCK_EQ(resource_tail, &request, "new resource becomes registry tail");
    FCK_EQ(prior.next, &request, "prior tail linked to new resource");
    FCK_EQ(request.prev, &prior, "new resource links back to prior tail");
    FCK_EQ(request.next, 0, "new resource terminates registry");

    duplicate = request;
    duplicate.data = &unused;
    duplicate.unk08 = 0x24681357;
    gPrepareModelResource(&duplicate);

    fck_expect_order(duplicate_order, 1, "duplicate goes to heap free once");
    released = fck_call_n(free_call, 0);
    FCK_EQ(released.a0, &duplicate, "free receives duplicate descriptor, not payload or prior entry");
    FCK_EQ(released.a1, &prior, "registry head preserved at release");
    FCK_EQ(released.a2, &request, "registry tail preserved at release");
    FCK_EQ(duplicate.data, &unused, "duplicate retains original data pointer");
    FCK_EQ(duplicate.unk08, 0x24681357, "duplicate does not clear word +8");
    FCK_EQ(unused.geometry.p28, 0, "duplicate payload not relocated");
    FCK_EQ(resource_head, &prior, "duplicate does not replace head");
    FCK_EQ(resource_tail, &request, "duplicate does not become tail");
    FCK_EQ(request.next, 0, "duplicate not linked");
}

int main(void)
{
    test_800131A0();
    test_8007F9CC();
    test_80018860();
    test_80013550();
    return lk_done("leaf_res_r4");
}
