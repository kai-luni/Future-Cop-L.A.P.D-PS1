/* Source characterizations of entity commands, motion, and RAM packets.
 * All fixtures are synthetic. Included helpers are real original C/assembly
 * and are not attributed as tested units in this suite. */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

#include "../../src/game/utilities/gApproximateDistance3.c"
#include "../../src/game/gameplay-ai/gRemoveEntityCommand.c"
#include "../../src/game/renderer-scene/gInitModelInstance.c"
/* These original translation units use different C declarations for the
 * same linked symbol. Keep their declarations separate within the harness. */
#define gSelectAnimationClip select_clip_from_actor
#include "../../src/game/gameplay-ai/gSetActorMode.c"
#undef gSelectAnimationClip
#include "../../src/game/utilities/gAdvancePrimaryPrng.c"
#include "../../src/game/gpu-2d-hud/gSelectAnimationClip.c"
#include "../../src/game/boot-frontend/gOrGridCellFlags.c"
#include "../../src/game/gameplay-ai/gGetSpawnTypeFlagPtr.c"
#include "../../src/game/utilities/gAdvanceSecondaryPrng.c"
#define Model AttachmentModel
#include "../../src/game/renderer-scene/gGetAnimatedModelAttachmentPoint.c"
#undef Model
#include "../../src/game/other/gIsQueuedCommandKind.c"
#define Entity PendingTarget
#include "../../src/game/boot-frontend/gEnqueuePendingMessage.c"
#undef Entity
#define Entity CommandTarget
#define gIsQueuedCommandKind filter_kind_from_command
#define gEnqueuePendingMessage enqueue_from_command
#include "../../src/game/gameplay-ai/gSendEntityCommand.c"
#undef gEnqueuePendingMessage
#undef gIsQueuedCommandKind
#undef Entity
#include "../../src/game/memory-resources/gFindRecordPayloadById.c"
#include "../../src/game/utilities/gVectorScaleInPlace.c"
#include "../../src/game/platform/gPlayGaugeEdgeSound.c"

#define B8(p, offset) (*(u8 *)((u8 *)(p) + (offset)))
#define H16(p, offset) (*(u16 *)((u8 *)(p) + (offset)))

struct Unk2AB8C {
    u8 unk00[0x17], unk17, unk18, unk19[3];
    s16 unk1C;
    u8 unk1E[0x32];
    s32 unk50, unk54, unk58, unk5C[3], unk68;
    u8 unk6C[0x18];
    s32 unk84;
};

s32 gPlayerEntities[2];
s32 gQueryBaseEntityField(struct Unk2AB8C *object, s32 query) __asm__("func_8002AB8C");

static void test_entity_query(void)
{
    struct Unk2AB8C object, players[2];

    /* @leaf 8002AB8C source src/game/gameplay-ai/gQueryBaseEntityField.c: field dispatch, signed fields and clamped minimum distance net of both radii */
    LK_FUNC("func_8002AB8C");
    lk_fill(&object, 0, sizeof(object));
    lk_fill(players, 0, sizeof(players));
    object.unk17 = 250;
    object.unk18 = 131;
    object.unk1C = -312;
    object.unk50 = 11;
    object.unk54 = 22;
    object.unk58 = -33;
    object.unk84 = -44;
    FCK_EQ(gQueryBaseEntityField(&object, 0), -312, "query zero sign-extends the halfword");
    FCK_EQ(gQueryBaseEntityField(&object, 1), 11, "query one reads field 50");
    FCK_EQ(gQueryBaseEntityField(&object, 2), 22, "query two reads field 54");
    FCK_EQ(gQueryBaseEntityField(&object, 3), -33, "query three reads field 58");
    FCK_EQ(gQueryBaseEntityField(&object, 4), -44, "query four reads field 84");
    FCK_EQ(gQueryBaseEntityField(&object, 7), 250, "query seven reads unsigned byte 17");
    FCK_EQ(gQueryBaseEntityField(&object, 8), 131, "query eight reads unsigned byte 18");
    FCK_EQ(gQueryBaseEntityField(&object, 9), 0, "unsupported query returns zero");

    gPlayerEntities[0] = (s32)&players[0];
    gPlayerEntities[1] = (s32)&players[1];
    object.unk68 = 7;
    players[0].unk5C[0] = -40;
    players[0].unk68 = 2;
    players[1].unk5C[2] = 30;
    players[1].unk68 = 6;
    FCK_EQ(gQueryBaseEntityField(&object, 5), 17, "nearest of two players supplies surface distance");
    players[0].unk5C[0] = -8;
    FCK_EQ(gQueryBaseEntityField(&object, 5), 0, "overlap clamps closest distance to zero");
    FCK_EQ(object.unk68, 7, "query preserves object's radius");
    FCK_EQ(players[0].unk5C[0], -8, "query preserves closest player's position");
    FCK_EQ(players[1].unk68, 6, "query preserves other player's radius");
    FCK_EQ(gPlayerEntities[0], &players[0], "query preserves player table");
}

struct QueueCommand { u8 op, arg; };
struct Unk2CB20 {
    u8 unk00[0x0C];
    struct QueueCommand commands[4];
    u8 unk14[2], unk16, unk17[0x0D];
    u32 options;
    u8 unk28[0xB4];
    s16 unkDC;
    u8 unkDE, unkDF, unkE0[0x28];
    struct Model *models[4];
};
struct Unk2E528 {
    u8 unk00[0x0C];
    struct QueueCommand commands[4];
    u8 unk14[0xC8], state, unkDD[9], unkE6;
};
static union {
    struct Unk2E528 queue;
    struct Unk2E2BC actor;
} actor_fixture;

void gDispatchModelVariantCommands(struct Unk2CB20 *self) __asm__("func_8002CB20");
void gProcessActorCommandQueue(struct Unk2E528 *self) __asm__("func_8002E528");
s32 gPrimaryPrngCursor __asm__("D_8009F5A8");
s32 gStateTable[20] __asm__("D_800A0480");

static void test_model_and_actor_queues(void)
{
    struct Unk2CB20 variant;
    struct Model model;
    struct ModelFrames frames;
    struct ModelInstance *instance;
    struct Unk2E528 *queue = &actor_fixture.queue;
    struct Unk2E2BC *actor = &actor_fixture.actor;
    struct Unk2E2BC28 metadata;
    int i;

    /* @leaf 8002CB20 source src/game/renderer-scene/gDispatchModelVariantCommands.c: consumes variant commands, preserves foreign commands and gates instance initialization */
    LK_FUNC("func_8002CB20");
    lk_fill(&variant, 0, sizeof(variant));
    model.flags = 0x40000000;
    model.frames = &frames;
    frames.frames[0].radius = 27;
    frames.frames[0].unk00 = 0x1234;
    frames.frames[0].unk06 = 0x3456;
    variant.models[3] = &model;
    variant.unk16 = 0x1E;
    variant.options = 0x88000;
    instance = (struct ModelInstance *)variant.unk28;
    variant.commands[0].op = 0x79;
    variant.commands[1].op = 0x2B;
    variant.commands[1].arg = 71;
    variant.commands[2].op = 0x2D;
    variant.commands[3].op = 0x2E;
    variant.commands[3].arg = 3;

    gDispatchModelVariantCommands(&variant);
    FCK_EQ(variant.unkDE, 3, "variant kind comes from opcode");
    FCK_EQ(variant.unkDC, 71, "variant argument stored");
    FCK_EQ(variant.unkDF, 4, "zero toggle argument sets flag four");
    FCK_EQ(instance->model, &model, "eligible variant chooses model slot three");
    FCK_EQ(instance->radius, 432, "real initializer scales the frame radius");
    FCK_EQ(instance->unk44, 0x1234, "real initializer copies first frame attribute");
    FCK_EQ(instance->unk46, 0x3456, "real initializer copies second frame attribute");
    FCK_EQ(instance->flags, 0x58, "real initializer applies option and model flags");
    FCK_EQ(variant.commands[0].op, 0x79, "foreign opcode remains queued");
    FCK_EQ(variant.commands[1].op, 0, "processed commands compacted away");

    variant.commands[1].op = 0x2D;
    variant.commands[1].arg = 1;
    variant.commands[2].op = 0x2E;
    variant.commands[2].arg = 4;
    gDispatchModelVariantCommands(&variant);
    FCK_EQ(variant.unkDF, 0, "nonzero toggle clears flag four");
    FCK_EQ(instance->model, &model, "slot four rejected without changing instance");
    variant.commands[1].op = 0x2E;
    variant.commands[1].arg = 0;
    variant.unk16 = 0x1D;
    gDispatchModelVariantCommands(&variant);
    FCK_EQ(instance->model, &model, "ineligible type does not select null model");
    lk_fill(&variant, 0, sizeof(variant));
    for (i = 0; i < 4; i++)
        variant.commands[i].op = 0x70 + i;
    variant.unk14[0] = 0x29;
    variant.unk14[1] = 9;
    gDispatchModelVariantCommands(&variant);
    FCK_EQ(variant.unkDE, 0, "byte after the fourth slot is not a variant command");
    FCK_EQ(variant.unk14[0], 0x29, "byte after the fourth slot is not consumed");
    FCK_EQ(variant.commands[3].op, 0x73, "full foreign queue stays intact");

    /* @leaf 8002E528 source src/game/gameplay-ai/gProcessActorCommandQueue.c: command 55 selects mode, 56 stores byte, unknown command stays */
    LK_FUNC("func_8002E528");
    lk_fill(&actor_fixture, 0, sizeof(actor_fixture));
    metadata.unk10 = 0; /* No animation installed; use the real no-clip path. */
    actor->unk28 = &metadata;
    actor->unkE8 = 12345;
    queue->commands[0].op = 0x77;
    queue->commands[0].arg = 0xE1;
    queue->commands[1].op = 0x55;
    queue->commands[1].arg = 1;
    queue->commands[2].op = 0x56;
    queue->commands[2].arg = 222;
    gProcessActorCommandQueue(queue);
    FCK_EQ(queue->state, 1, "enable command moves dormant actor to mode one");
    FCK_EQ(actor->unkE2, 0x4650, "mode transition sets timer");
    FCK_EQ(actor->unk84, 12345, "mode transition restores positive actor value");
    FCK_EQ(queue->unkE6, 222, "parameter command preserves unsigned byte");
    FCK_EQ(queue->commands[0].op, 0x77, "unknown command retained");
    FCK_EQ(queue->commands[0].arg, 0xE1, "unknown argument retained");
    FCK_EQ(queue->commands[1].op, 0, "known commands removed");

    queue->commands[1].op = 0x55;
    queue->commands[1].arg = 0;
    actor->unkE8 = -5;
    gProcessActorCommandQueue(queue);
    FCK_EQ(queue->state, 3, "disable command selects mode three for active actor");
    FCK_EQ(actor->unk84, 0, "disable transition clamps negative actor value");
    actor->unkE2 = 13;
    queue->commands[1].op = 0x55;
    queue->commands[1].arg = 0;
    gProcessActorCommandQueue(queue);
    FCK_EQ(actor->unkE2, 13, "repeated disable leaves transition timer untouched");
    FCK_EQ(queue->commands[1].op, 0, "ignored repeat is still consumed");
    for (i = 0; i < 4; i++)
        queue->commands[i].op = 0x70 + i;
    queue->unk14[0] = 0x56;
    queue->unk14[1] = 99;
    gProcessActorCommandQueue(queue);
    FCK_EQ(queue->unkE6, 222, "byte after the fourth slot is not a parameter command");
    FCK_EQ(queue->unk14[0], 0x56, "byte after the fourth slot is not consumed");
    FCK_EQ(queue->commands[3].op, 0x73, "full foreign actor queue stays intact");
}

struct Unk33608Anim { u8 unk00[8]; s32 value08; };
struct Unk33608Mirror {
    u8 unk00[0x1B], base1B, unk1C[0x3A], mode56, unk57;
    u16 value58;
    u8 unk5A[0x0C];
    u16 value66;
    struct Unk33608Anim *anim68;
    s32 value6C;
    u8 unk70[0x0E], mode7E, unk7F;
    u16 value80;
    u8 unk82[0x0C];
    u16 value8E;
    struct Unk33608Anim *anim90;
    s32 value94;
};
struct Unk33608Owner {
    u8 unk00[0xA6], modeA6, unkA7;
    u16 valueA8;
    u8 unkAA[0x0A];
    u16 valueB4, valueB6;
    struct Unk33608Anim *animB8;
    s32 valueBC;
    u8 unkC0[0x74];
    struct Unk33608Mirror *mirror;
};
void gSetDroneAttackTarget(struct Unk33608Owner *owner, struct Unk33608Anim *anim)
    __asm__("func_80033608");

struct Unk3C500Cell { u8 unk00[0x14], id, next; };
struct Unk3C500Cell *gSpawnRecordMap[2][0x100] __asm__("D_800A7040");
u32 gSpawnTypeFlags[2 * 0x100];
void gMarkSpawnCycleFlags(s32 column, s32 stop, s32 row) __asm__("func_8003C500");

static void test_drone_and_spawn_cycle(void)
{
    struct Unk33608Owner owner;
    struct Unk33608Mirror mirror;
    struct Unk33608Anim anim;
    struct Unk3C500Cell cells[3];

    /* @leaf 80033608 source src/game/gameplay-ai/gSetDroneAttackTarget.c: changed animation mirrored twice and null target applies fallback mode */
    LK_FUNC("func_80033608");
    lk_fill(&owner, 0, sizeof(owner));
    lk_fill(&mirror, 0, sizeof(mirror));
    owner.mirror = &mirror;
    anim.value08 = -123;
    gSetDroneAttackTarget(&owner, &anim);
    FCK_EQ(owner.animB8, &anim, "owner holds animation pointer");
    FCK_EQ(mirror.anim68, &anim, "first mirror holds animation pointer");
    FCK_EQ(mirror.anim90, &anim, "second mirror holds animation pointer");
    FCK_EQ(owner.valueBC, -123, "owner copies signed animation value");
    FCK_EQ(mirror.value6C, -123, "first mirror copies animation value");
    FCK_EQ(mirror.value94, -123, "second mirror copies animation value");
    anim.value08 = 999;
    gSetDroneAttackTarget(&owner, &anim);
    FCK_EQ(owner.valueBC, -123, "same pointer does not refresh owner value");
    FCK_EQ(mirror.value94, -123, "same pointer does not refresh mirror value");

    owner.valueB4 = 0xFE91;
    mirror.base1B = 228;
    gSetDroneAttackTarget(&owner, 0);
    FCK_EQ(owner.valueB6, 0xFE91, "fallback copies owner halfword");
    FCK_EQ(mirror.value66, 0xFE91, "fallback copies halfword to first mirror");
    FCK_EQ(mirror.value8E, 0xFE91, "fallback copies halfword to second mirror");
    FCK_EQ(owner.modeA6, 4, "fallback sets owner mode four");
    FCK_EQ(mirror.mode56, 4, "fallback sets first mirror mode four");
    FCK_EQ(mirror.mode7E, 4, "fallback sets second mirror mode four");
    FCK_EQ(owner.valueA8, 228, "fallback zero-extends base byte");
    FCK_EQ(mirror.value58, 228, "first mirror gets base byte");
    FCK_EQ(mirror.value80, 228, "second mirror gets base byte");
    mirror.base1B = 77;
    owner.valueB4 = 9;
    gSetDroneAttackTarget(&owner, 0);
    FCK_EQ(owner.valueA8, 228, "mode four suppresses reinitialization");
    FCK_EQ(mirror.value8E, 9, "mode four still refreshes fallback halfword");
    FCK_EQ(owner.animB8, &anim, "null branch does not clear animation pointer");

    /* @leaf 8003C500 source src/game/boot-frontend/gMarkSpawnCycleFlags.c: walk next columns in one row and mark the successor even on the stopping record */
    LK_FUNC("func_8003C500");
    lk_fill(gSpawnTypeFlags, 0xA1, sizeof(gSpawnTypeFlags));
    cells[0].id = 9;
    cells[0].next = 250;
    cells[1].id = 42;
    cells[1].next = 3;
    cells[2].id = 42;
    cells[2].next = 3;
    gSpawnRecordMap[1][6] = &cells[0];
    gSpawnRecordMap[1][250] = &cells[1];
    gSpawnRecordMap[1][3] = &cells[2];
    gMarkSpawnCycleFlags(0x106, 0x12A, 1);
    FCK_EQ(gSpawnTypeFlags[0x100 + 6], 0xA1A1A1A1, "start column itself untouched");
    FCK_EQ(gSpawnTypeFlags[0x100 + 250], 0xA1A1A1A5, "first successor ORs low byte only");
    FCK_EQ(gSpawnTypeFlags[0x100 + 3], 0xA1A1A1A5, "stopping cell successor also marked");
    FCK_EQ(gSpawnTypeFlags[250], 0xA1A1A1A1, "same column in other row untouched");
    FCK_EQ(cells[1].next, 3, "record link not modified");
}

struct Unk3D0F0Model {
    u8 unk00[0x0C];
    u32 flags;
    u8 unk10[0x0C];
    u32 unk1C;
    u8 unk20[0x14];
    u32 unk34;
};
struct Unk3D0F0Instance {
    struct Unk3D0F0Model *model;
    u8 unk04[0x30];
    struct Vec3 unk34;
    u8 unk40[8];
    s16 frame;
};
void gResolveModelEffectPoint(struct Unk3D0F0Instance *instance, s32 arg1, s32 arg2, struct Vec3 *out)
    __asm__("func_8003D0F0");
s32 gFreeRangeCursor __asm__("D_8009F5AC");
s32 gStateEntryTable[20] __asm__("D_800A04D0");

struct Unk51580 { s32 unk00; u8 unk04[0x1C]; s32 unk20; };
struct Unk51580Out { s32 unk00, unk04, unk08; };
s32 gGetLoopingPathSegmentDelta(struct Unk51580 *arg0, s32 arg1, struct Unk51580Out *arg2)
    __asm__("func_80051580");

static void test_points_and_path(void)
{
    struct Unk3D0F0Instance instance;
    struct Unk3D0F0Model model;
    union {
        u32 aligned[8];
        s16 coordinates[16];
    } frame, random_frame;
    s32 *frame_table[3];
    struct { struct Vec3 point; s32 guard; } out;
    struct Unk51580 path;
    struct { struct Unk51580Out delta; s32 guard; } segment;
    /* Two records: header with count two, then two 12-byte path entries. */
    s32 records[] = { 0x74415048, 24, 0, 0, 0, 2,
                      0x61415048, 36, 0, 12, 18, 0x10000, -16, 30, 0x8000 };

    /* @leaf 8003D0F0 source src/game/renderer-scene/gResolveModelEffectPoint.c: origin fallback, deterministic random point, attachment slot and direct frame position */
    LK_FUNC("func_8003D0F0");
    lk_fill(&instance, 0, sizeof(instance));
    lk_fill(&model, 0, sizeof(model));
    instance.unk34.x = -4;
    instance.unk34.y = 7;
    instance.unk34.z = 9;
    out.guard = 0x12345678;
    gResolveModelEffectPoint(&instance, 0, 0, &out.point);
    FCK_EQ(out.point.x, -4, "no-model x from origin");
    FCK_EQ(out.point.y, 7, "no-model y from origin");
    FCK_EQ(out.point.z, 9, "no-model z from origin");

    instance.model = &model;
    instance.frame = 1;
    model.unk34 = (u32)&frame;
    frame.coordinates[12] = 15;
    frame.coordinates[13] = -20;
    frame.coordinates[14] = 28;
    gResolveModelEffectPoint(&instance, 4, 0x100, &out.point);
    FCK_EQ(out.point.x, 240, "non-attachment x uses second frame");
    FCK_EQ(out.point.y, -320, "non-attachment y sign-extends and scales");
    FCK_EQ(out.point.z, 448, "non-attachment z scales by sixteen");

    model.unk1C = (u32)frame_table;
    frame_table[2] = (s32 *)random_frame.aligned;
    random_frame.aligned[0] = 2;
    random_frame.coordinates[2] = 7;
    random_frame.coordinates[3] = -8;
    random_frame.coordinates[4] = 9;
    random_frame.coordinates[6] = -12;
    random_frame.coordinates[7] = 23;
    random_frame.coordinates[8] = 34;
    gFreeRangeCursor = 3;
    gStateEntryTable[2] = 0x500;
    gStateEntryTable[5] = 0x300;
    gResolveModelEffectPoint(&instance, 0, 1, &out.point);
    FCK_EQ(gFreeRangeCursor, 2, "random branch consumes secondary PRNG value");
    FCK_EQ(out.point.x, -192, "half-scale PRNG chooses second coordinate x");
    FCK_EQ(out.point.y, 368, "half-scale PRNG chooses second coordinate y");
    FCK_EQ(out.point.z, 544, "half-scale PRNG chooses second coordinate z");
    B8(&model, 0x33) = 0;
    gResolveModelEffectPoint(&instance, 3, 0, &out.point);
    FCK_EQ(out.point.x, 112, "attachment slot zero x via real helper");
    FCK_EQ(out.point.y, -128, "attachment slot zero y via real helper");
    FCK_EQ(out.point.z, 144, "attachment slot zero z via real helper");
    FCK_EQ(out.guard, 0x12345678, "only three destination words written");

    /* @leaf 80051580 source src/game/memory-resources/gGetLoopingPathSegmentDelta.c: one-based bounded path index, Q16 scaling and zeroed failure output */
    LK_FUNC("func_80051580");
    path.unk00 = (s32)records;
    path.unk20 = sizeof(records);
    segment.guard = 0x12345678;
    FCK_EQ(gGetLoopingPathSegmentDelta(&path, 1, &segment.delta), 1, "first segment returns success");
    FCK_EQ(segment.delta.unk00, 18, "first entry x from its second word");
    FCK_EQ(segment.delta.unk04, 0, "first entry always has zero y");
    FCK_EQ(segment.delta.unk08, 12, "first entry z from its first word");
    FCK_EQ(gGetLoopingPathSegmentDelta(&path, 2, &segment.delta), 1, "index equal to count succeeds");
    FCK_EQ(segment.delta.unk00, 15, "second entry half-scales x");
    FCK_EQ(segment.delta.unk04, 0, "second entry always has zero y");
    FCK_EQ(segment.delta.unk08, -8, "second entry half-scales signed z");
    FCK_EQ(gGetLoopingPathSegmentDelta(&path, 3, &segment.delta), 0, "index past count fails");
    FCK_CHECK(lk_all(&segment.delta, 0, sizeof(segment.delta), 0), "past-count failure clears output");
    lk_fill(&segment.delta, 0xA5, sizeof(segment.delta));
    FCK_EQ(gGetLoopingPathSegmentDelta(&path, 0, &segment.delta), 0, "zero index fails");
    FCK_CHECK(lk_all(&segment.delta, 0, sizeof(segment.delta), 0), "zero index failure clears output");
    lk_fill(&segment.delta, 0xA5, sizeof(segment.delta));
    FCK_EQ(gGetLoopingPathSegmentDelta(0, 1, &segment.delta), 0, "null path fails");
    FCK_CHECK(lk_all(&segment.delta, 0, sizeof(segment.delta), 0), "null path failure clears output");
    FCK_EQ(segment.guard, 0x12345678, "path writes only its destination vector");
}

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    u8 unk0C[0x0B], group;
};
struct Entity *gUnitListHead __asm__("D_8009FA40");
struct EntityMessage gPendingMessages[16];
s32 gPendingMessageMask;
void gStepPendingMessages(s32 elapsed) __asm__("func_8004DB74");
void gSendCommandByGroup(s32 group, s32 kind, s32 value) __asm__("func_8004DD68");

static void test_entity_messages(void)
{
    struct CommandTarget targets[4];
    int i;

    /* @leaf 8004DD68 source src/game/boot-frontend/gSendCommandByGroup.c: route to live group members, retrying through the real pending queue */
    LK_FUNC("func_8004DD68");
    lk_fill(targets, 0, sizeof(targets));
    lk_fill(gPendingMessages, 0xA5, sizeof(gPendingMessages));
    gPendingMessageMask = 0;
    gUnitListHead = (struct Entity *)targets;
    for (i = 0; i < 4; i++) {
        targets[i].next = i == 3 ? 0 : &targets[i + 1];
        targets[i].id = 100 + i;
        ((struct Entity *)&targets[i])->group = 9;
        targets[i].type = 1;
        targets[3].slots[i].kind = 1 + i;
    }
    targets[1].id = 0;
    ((struct Entity *)&targets[2])->group = 8;
    gSendCommandByGroup(9, 0x3D, 0x71);
    FCK_EQ(targets[0].slots[0].kind, 0x3D, "live matching entity receives command");
    FCK_EQ(targets[0].slots[0].value, 0x71, "kind and value routed together");
    FCK_EQ(targets[0].slots[1].kind, 0, "only first free slot filled");
    FCK_EQ(targets[1].slots[0].kind, 0, "matching entity with zero ID skipped");
    FCK_EQ(targets[2].slots[0].kind, 0, "other group skipped");
    FCK_EQ(targets[3].slots[3].kind, 4, "full command buffer preserved");
    FCK_EQ(gPendingMessageMask, 1, "full matching entity queued for retry");
    FCK_EQ(gPendingMessages[0].target, &targets[3], "retry record remembers overflowing entity");
    FCK_EQ(gPendingMessages[0].id, 103, "retry record remembers entity identity");
    FCK_EQ(gPendingMessages[0].timer, 600, "retry timer initialized");
    FCK_CHECK(lk_all(&gPendingMessages[1], 0, sizeof(gPendingMessages[1]), 0xA5), "only retry slot zero populated");

    /* @leaf 8004DB74 source src/game/gameplay-ai/gStepPendingMessages.c: sparse mask, exact expiration, retry on full target and stale ID removal */
    LK_FUNC("func_8004DB74");
    lk_fill(gPendingMessages, 0xA5, sizeof(gPendingMessages));
    gPendingMessageMask = 0;
    FCK_EQ(gEnqueuePendingMessage((struct PendingTarget *)&targets[3], 0x3D, 0x71),
           1, "synthetic full-target retry installed");
    gStepPendingMessages(599);
    FCK_EQ(gPendingMessages[0].timer, 1, "non-expired timer decremented");
    FCK_EQ(gPendingMessageMask, 1, "non-expired slot still pending");
    gStepPendingMessages(1);
    FCK_EQ(gPendingMessages[0].timer, 600, "equal-zero expiration schedules next retry");
    FCK_EQ(gPendingMessageMask, 1, "full target remains pending");

    gPendingMessageMask |= 4;
    gPendingMessages[2].timer = 1;
    gPendingMessages[2].target = (struct PendingTarget *)&targets[2];
    gPendingMessages[2].id = 18; /* Reused target, different live ID. */
    targets[3].slots[2].kind = 0;
    gStepPendingMessages(605);
    FCK_EQ(gPendingMessages[0].timer, 595, "overshoot is preserved across the period");
    FCK_EQ(gPendingMessages[2].timer, -4, "stale slot timer goes negative across expiration (no re-clamp)");
    FCK_EQ(gPendingMessageMask, 0, "successful and stale entries clear their bits");
    FCK_EQ(targets[3].slots[2].kind, 0x3D, "retry delivers into newly available slot");
    FCK_EQ(targets[3].slots[2].value, 0x71, "retry preserves original value");
    FCK_EQ(targets[2].slots[0].kind, 0, "stale entry does not deliver");
    FCK_CHECK(lk_all(&gPendingMessages[1], 0, sizeof(gPendingMessages[1]), 0xA5), "mask hole never inspected or written");
    gStepPendingMessages(19);
    FCK_EQ(gPendingMessages[0].timer, 595, "empty mask leaves stored timers intact");
}

struct Sub58 {
    u8 unk00[0x6C];
    s32 unk6C;
    u8 unk70[0x34], unkA4, unkA5, unkA6, unkA7, pad[2];
    u16 unkAA, padAC, unkAE;
    u8 unkB0[0x10];
    s8 unkC0;
    u8 unkC1[0x20F], unk2D0[0x2D], unk2FD;
};
struct Obj58 {
    u8 unk00[0x0C];
    struct QueueCommand commands[4];
    u8 unk14[0x98];
    struct Sub58 *unkAC;
};
void gDispatchQueuedPlayerCommands(struct Obj58 *self) __asm__("func_800585B8");

struct Unk5A190Gauge { s32 unk00, value, unk08, limit; u8 state, unk11[3]; };
struct Unk5A190 { u8 unk00[0x54]; struct Unk5A190Gauge gauges[1]; };
void gSetEntityGaugeValue(struct Unk5A190 *object, s32 index, s32 value) __asm__("func_8005A190");

/* The indirect audio boundary records the real gauge helper's sound/owner.
 * The caller ignores the result, so return the unavailable-voice sentinel. */
static s32 sound_id, sound_owner, sound_calls;
s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1)
{
    sound_id = id;
    sound_owner = arg1;
    sound_calls++;
    return -1;
}

static void test_player_queue_and_gauge(void)
{
    struct Obj58 player;
    struct Sub58 sub;
    union {
        struct Unk5A190 gauge;
        struct Unk5A154 sound;
    } fixture;
    int i;

    /* @leaf 800585B8 source src/game/gameplay-ai/gDispatchQueuedPlayerCommands.c: four player commands modify different fields and compact the queue */
    LK_FUNC("func_800585B8");
    lk_fill(&player, 0, sizeof(player));
    lk_fill(&sub, 0xA5, sizeof(sub));
    player.unkAC = &sub;
    H16(&sub, 0xD0) = 0xFF80;
    sub.unkC0 = 0;
    player.commands[0].op = 0x5F;
    player.commands[1].op = 0x60;
    player.commands[1].arg = 2;
    player.commands[2].op = 0x61;
    player.commands[2].arg = 173;
    player.commands[3].op = 0x62;
    player.commands[3].arg = 225;
    gDispatchQueuedPlayerCommands(&player);
    FCK_EQ(sub.unk6C, 0, "reset command clears field 6C");
    FCK_EQ(H16(&sub, 0xD0), 128, "increment adds shifted argument modulo halfword");
    FCK_EQ(sub.unkA4, 173, "effect command copies argument");
    FCK_EQ(sub.unkA6, 1, "effect command enables first flag");
    FCK_EQ(sub.unkA7, 1, "effect command enables second flag");
    FCK_EQ(sub.unkAA, 0, "effect command resets counter");
    FCK_EQ(sub.unkAE, 900, "effect command starts timer");
    FCK_EQ(sub.unk2FD, 225, "unlocked player receives command byte");
    FCK_EQ(player.commands[0].op, 0, "all known commands consumed");
    FCK_EQ(sub.unkA5, 0xA5, "neighboring effect field untouched");

    sub.unkC0 = 1;
    player.commands[0].op = 0x75;
    player.commands[1].op = 0x62;
    player.commands[1].arg = 10;
    gDispatchQueuedPlayerCommands(&player);
    FCK_EQ(sub.unk2FD, 225, "locked player keeps previous command byte");
    FCK_EQ(player.commands[0].op, 0x75, "foreign operation remains queued");
    FCK_EQ(player.commands[1].op, 0, "blocked known operation is still consumed");
    for (i = 0; i < 4; i++)
        player.commands[i].op = 0x70 + i;
    player.unk14[0] = 0x5F;
    sub.unk6C = 77;
    gDispatchQueuedPlayerCommands(&player);
    FCK_EQ(sub.unk6C, 77, "byte after the fourth slot is not a player command");
    FCK_EQ(player.unk14[0], 0x5F, "byte after the fourth player slot is not consumed");
    FCK_EQ(player.commands[3].op, 0x73, "full foreign player queue stays intact");

    /* @leaf 8005A190 source src/game/gpu-2d-hud/gSetEntityGaugeValue.c: limit and zero thresholds drive state and real sound helper only on the two falling edges */
    LK_FUNC("func_8005A190");
    lk_fill(&fixture, 0, sizeof(fixture));
    fixture.sound.side = 1;
    fixture.gauge.gauges[0].limit = 80;
    fixture.gauge.gauges[0].unk08 = 123;
    B8(&fixture.gauge.gauges[0], 0x13) = 47;
    gPlayerEntities[0] = 19;
    gPlayerEntities[1] = 27;
    sound_calls = 0;
    gSetEntityGaugeValue(&fixture.gauge, 0, 80);
    FCK_EQ(fixture.gauge.gauges[0].value, 80, "limit is included in full gauge state");
    FCK_EQ(fixture.gauge.gauges[0].state, 2, "full threshold sets state two");
    FCK_EQ(sound_calls, 0, "becoming full does not play transition sound");
    gSetEntityGaugeValue(&fixture.gauge, 0, 25);
    FCK_EQ(fixture.gauge.gauges[0].value, 25, "lower value stored");
    FCK_EQ(fixture.gauge.gauges[0].state, 1, "leaving full sets state one");
    FCK_EQ(sound_calls, 1, "leaving full calls audio once");
    FCK_EQ(sound_id, 47, "real helper selects synthetic gauge sound");
    FCK_EQ(sound_owner, 27, "real helper selects matching player");
    gSetEntityGaugeValue(&fixture.gauge, 0, 5);
    FCK_EQ(sound_calls, 1, "positive state-one update plays no sound");
    gSetEntityGaugeValue(&fixture.gauge, 0, 0);
    FCK_EQ(fixture.gauge.gauges[0].state, 0, "exact zero clears state one");
    FCK_EQ(sound_calls, 2, "reaching zero plays one more sound");
    gSetEntityGaugeValue(&fixture.gauge, 0, -7);
    FCK_EQ(fixture.gauge.gauges[0].value, 0, "negative value clamped to zero");
    FCK_EQ(sound_calls, 2, "already-empty gauge does not replay sound");
    FCK_EQ(fixture.gauge.gauges[0].unk08, 123, "unrelated gauge word preserved");
}

u8 gTileCornerIndices[61][4] __asm__("D_8009AFE8");
u8 gTileShapeTable[60] __asm__("D_8009B1A4");
int gSampleTileHeight(s8 *data, unsigned int index, int x, int y, int *out) __asm__("func_80067480");

static void test_tile_height(void)
{
    s8 heights[4] = { 40, 8, 24, -8 };
    struct { int height, guard; } out;
    int i;

    out.guard = 0x2468ACE;
    for (i = 0; i < 5; i++) {
        int index = i == 4 ? 60 : i;
        gTileCornerIndices[index][0] = 1;
        gTileCornerIndices[index][1] = 2;
        gTileCornerIndices[index][2] = 0;
        gTileCornerIndices[index][3] = 3;
        if (i < 4)
            gTileShapeTable[index] = i * 8 + 3;
    }

    /* @leaf 80067480 source src/game/gameplay-ai/gSampleTileHeight.c: four triangular clipping modes and bilerp at the four-corner threshold */
    LK_FUNC("func_80067480");
    FCK_EQ(gSampleTileHeight(heights, 60, 32768, 32768, &out.height), 1, "index sixty uses four corners");
    FCK_EQ(out.height, 4096, "quad center averages signed corners");
    FCK_EQ(gSampleTileHeight(heights, 0, 32768, 32768, &out.height), 1, "mode zero includes the sum boundary");
    FCK_EQ(out.height, 6144, "mode zero interpolates its triangle");
    FCK_EQ(gSampleTileHeight(heights, 0, 32768, 32769, &out.height), 0, "mode zero rejects above boundary");
    FCK_EQ(out.height, 6144, "rejected point leaves prior height alone");
    FCK_EQ(gSampleTileHeight(heights, 1, 32768, 32767, &out.height), 0, "mode one rejects below sum boundary");
    FCK_EQ(out.height, 6144, "mode one rejection leaves height alone");
    FCK_EQ(gSampleTileHeight(heights, 1, 32768, 32768, &out.height), 1, "mode one includes the sum boundary");
    FCK_EQ(out.height, 7168, "mode one uses alternate left edge");
    FCK_EQ(gSampleTileHeight(heights, 2, 32768, 32767, &out.height), 0, "mode two rejects y below x");
    FCK_EQ(out.height, 7168, "mode two rejection leaves height alone");
    FCK_EQ(gSampleTileHeight(heights, 2, 32768, 32768, &out.height), 1, "mode two includes the diagonal");
    FCK_EQ(out.height, 6144, "mode two height on diagonal");
    FCK_EQ(gSampleTileHeight(heights, 3, 32767, 32768, &out.height), 0, "mode three rejects x below y");
    FCK_EQ(out.height, 6144, "mode three rejection leaves height alone");
    FCK_EQ(gSampleTileHeight(heights, 3, 32768, 32768, &out.height), 1, "mode three includes the diagonal");
    FCK_EQ(out.height, 5120, "mode three height on diagonal");
    FCK_EQ(heights[3], -8, "read-only source height preserved");
    FCK_EQ(out.guard, 0x2468ACE, "height output uses just one word");
}

typedef struct { u16 vx, vy; } DVECTOR;
typedef struct {
    union { u32 word; u8 bytes[4]; } hdr;
    u32 unk04;
    s16 x0, y0;
    u32 unk0C;
    s16 x1, y1;
    u32 unk14;
    s16 x2, y2;
    u32 unk1C;
    s16 x3, y3;
    u8 pad24[0x34 - 0x24];
} Prim;
Prim *gPrimPoolNext __asm__("D_8009F9F0");
Prim *gPrimPoolEnd __asm__("D_8009FA28");
void gEmitTracerGouraudQuad(DVECTOR v0, DVECTOR v1, DVECTOR v2, DVECTOR v3,
                           s32 dx, s32 dy_in, u32 unk04, u32 unk1C, u16 tag_in)
    __asm__("func_8006F21C");

static void test_tracer_packet(void)
{
    Prim packets[3];
    DVECTOR v0 = { 100, 25 }, v1 = { 31, 62 }, v2 = { 90, 105 }, v3 = { 19, 38 };

    /* @leaf 8006F21C source src/game/renderer-scene/gEmitTracerGouraudQuad.c: packet coordinates, color words, signed tag and non-advancing end cursor */
    LK_FUNC("func_8006F21C");
    lk_fill(packets, 0xA5, sizeof(packets));
    gPrimPoolNext = &packets[0];
    gPrimPoolEnd = &packets[1];
    gEmitTracerGouraudQuad(v0, v1, v2, v3, -9, 12, 0x12345678, 0x12FEDCBA, 0x8001);
    FCK_EQ(gPrimPoolNext, &packets[1], "room in pool advances cursor by one packet");
    FCK_EQ(packets[0].hdr.word, 0x08FF8001, "signed halfword tag extended to 24 bits under command length");
    FCK_EQ(packets[0].unk04, 0x12345678, "leading color word stored");
    FCK_EQ(packets[0].unk1C, 0x12FEDCBA, "trailing color word stored");
    FCK_EQ(packets[0].unk0C, 0, "first unused color cleared");
    FCK_EQ(packets[0].unk14, 0, "second unused color cleared");
    FCK_EQ(packets[0].x0, 91, "vertex zero x translated");
    FCK_EQ(packets[0].y0, 37, "vertex zero y translated");
    FCK_EQ(packets[0].x1, 81, "vertex two x translated");
    FCK_EQ(packets[0].y1, 117, "vertex two y translated");
    FCK_EQ(packets[0].x2, 100, "vertex zero x copied unshifted");
    FCK_EQ(packets[0].y2, 25, "vertex zero y copied unshifted");
    FCK_EQ(packets[0].x3, 90, "vertex two x copied unshifted");
    FCK_EQ(packets[0].y3, 105, "vertex two y copied unshifted");
    FCK_CHECK(lk_all(packets[0].pad24, 0, sizeof(packets[0].pad24), 0xA5), "packet padding remains untouched");
    FCK_CHECK(lk_all(&packets[1], 0, sizeof(Prim), 0xA5), "future packet not yet populated");

    /* The original writes at the end cursor without advancing it; give
     * that path a backed scratch packet, not an out-of-bounds pointer. */
    gEmitTracerGouraudQuad(v0, v1, v2, v3, 0, 0, 1, 2, 7);
    FCK_EQ(gPrimPoolNext, &packets[1], "end cursor does not advance");
    FCK_EQ(packets[1].hdr.word, 0x08000007, "end cursor still receives a packet");
    FCK_EQ(packets[0].unk04, 0x12345678, "completed packet untouched");
    FCK_CHECK(lk_all(&packets[2], 0, sizeof(Prim), 0xA5), "after-end packet untouched");
}

struct ByteChannels {
    u8 pad[0x6C], bounds[12], pad78, value[3], pad7C, value2[3];
    s16 speed[6];
    u16 flags;
};
struct Actor59 {
    struct Actor59 *next;
    u8 pad04[0x0B], flags, pad10[2];
    s16 angle;
    u8 pad14[0x44];
    s32 x, y, z;
    u8 pad64[2];
    s16 rotation;
    u8 pad68[0x2C];
    s16 vx, vy, vz, acceleration;
};
void gStepBoundedByteChannels(struct ByteChannels *e, int dt) __asm__("func_8006FB7C");
void gIntegrateActor59List(struct Actor59 *entity, s32 dt) __asm__("func_80070048");

static void test_motion(void)
{
    struct ByteChannels channels;
    union { struct Actor59 actor; struct ByteChannels channels; } objects[3];

    /* @leaf 8006FB7C source src/game/boot-frontend/gStepBoundedByteChannels.c: all six channels advance or clamp with optional speed reversal */
    LK_FUNC("func_8006FB7C");
    lk_fill(&channels, 0, sizeof(channels));
    channels.bounds[0] = 5;
    channels.bounds[1] = 6;
    channels.bounds[2] = 7;
    channels.bounds[3] = 50;
    channels.bounds[4] = 60;
    channels.bounds[5] = 70;
    channels.bounds[6] = 8;
    channels.bounds[7] = 9;
    channels.bounds[8] = 10;
    channels.bounds[9] = 80;
    channels.bounds[10] = 90;
    channels.bounds[11] = 100;
    channels.value[0] = 42;
    channels.value[1] = 9;
    channels.value[2] = 20;
    channels.value2[0] = 40;
    channels.value2[1] = 9;
    channels.value2[2] = 50;
    channels.speed[0] = 8;
    channels.speed[1] = -6;
    channels.speed[2] = 3;
    channels.speed[3] = -4;
    channels.speed[4] = -1;
    channels.speed[5] = 0;
    channels.flags = 0x101;
    channels.pad78 = 0xA5;
    channels.pad7C = 0xA5;
    gStepBoundedByteChannels(&channels, 300);
    FCK_EQ(channels.value[0], 50, "exact upper bound clamps first channel");
    FCK_EQ(channels.speed[0], -8, "exact upper bound reverses first speed");
    FCK_EQ(channels.value[1], 6, "crossed lower bound clamps second channel");
    FCK_EQ(channels.speed[1], 6, "lower bound reverses second speed");
    FCK_EQ(channels.value[2], 23, "interior positive delta adds without clamping");
    FCK_EQ(channels.value2[0], 36, "interior negative delta subtracts without clamping");
    FCK_EQ(channels.value2[1], 9, "second bank uses its own low bound");
    FCK_EQ(channels.speed[4], 1, "second bank reverses its own speed");
    FCK_EQ(channels.value2[2], 50, "zero delta is stationary");
    FCK_EQ(channels.flags, 0x101, "step preserves flags");
    FCK_EQ(channels.pad78, 0xA5, "first pad byte untouched");
    FCK_EQ(channels.pad7C, 0xA5, "second pad byte untouched");
    channels.flags = 0x100;
    channels.value[0] = 49;
    channels.speed[0] = 8;
    gStepBoundedByteChannels(&channels, 300);
    FCK_EQ(channels.value[0], 50, "disabled bounce still clamps");
    FCK_EQ(channels.speed[0], 8, "disabled bounce leaves speed unchanged");
    gStepBoundedByteChannels(&channels, 1);
    FCK_EQ(channels.value2[0], 32, "fraction below one truncates toward zero");

    /* @leaf 80070048 source src/game/gameplay-ai/gIntegrateActor59List.c: linked integration skips flag eight and steps channels on moving nodes */
    LK_FUNC("func_80070048");
    lk_fill(objects, 0, sizeof(objects));
    objects[0].actor.next = &objects[1].actor;
    objects[1].actor.next = &objects[2].actor;
    objects[0].actor.x = 100;
    objects[0].actor.y = 150;
    objects[0].actor.z = 300;
    objects[0].actor.angle = 70;
    objects[0].actor.rotation = -64;
    objects[0].actor.vx = -32;
    objects[0].actor.vy = 16;
    objects[0].actor.vz = 48;
    objects[0].actor.acceleration = 24;
    objects[0].channels.bounds[3] = 100;
    objects[0].channels.value[0] = 10;
    objects[0].channels.speed[0] = 75;
    objects[1].actor.flags = 8;
    objects[1].actor.x = 77;
    objects[1].actor.vx = 32;
    objects[1].channels.bounds[3] = 100;
    objects[1].channels.value[0] = 20;
    objects[1].channels.speed[0] = 75;
    objects[2].actor.flags = 4;
    objects[2].actor.z = 99;
    objects[2].actor.vz = -16;
    gIntegrateActor59List(&objects[0].actor, 64);
    FCK_EQ(objects[0].actor.vy, 22, "vertical acceleration changes velocity first");
    FCK_EQ(objects[0].actor.x, -28, "first actor integrates signed x displacement");
    FCK_EQ(objects[0].actor.y, 238, "position uses newly accelerated vertical velocity");
    FCK_EQ(objects[0].actor.z, 492, "first actor integrates z displacement");
    FCK_EQ(objects[0].actor.angle, 54, "signed angular displacement");
    FCK_EQ(objects[0].channels.value[0], 26, "real channel step also runs");
    FCK_EQ(objects[1].actor.x, 77, "flag eight prevents movement");
    FCK_EQ(objects[1].channels.value[0], 20, "flag eight prevents channel updates");
    FCK_EQ(objects[2].actor.z, 35, "later actor moves despite skipped predecessor");
    FCK_EQ(objects[1].actor.next, &objects[2].actor, "list links preserved");
    gIntegrateActor59List(0, 64);
    FCK_EQ(objects[2].actor.z, 35, "null list leaves prior fixture unchanged");
}

int main(void)
{
    test_entity_query();
    test_model_and_actor_queues();
    test_drone_and_spawn_cycle();
    test_points_and_path();
    test_entity_messages();
    test_player_queue_and_gauge();
    test_tile_height();
    test_tracer_packet();
    test_motion();
    return lk_done("leaf_fewmore_b");
}
