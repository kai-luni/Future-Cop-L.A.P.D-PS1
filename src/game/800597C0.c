// Categories: Graphics > Effects > Glow Discs | Gameplay > Object Draw Handlers | Graphics > Renderer > Primitive Submission | Graphics > Clipping
#include "game/types.h"

struct Unk17508 {
    void *model;
};

struct State597C0;
struct Info597C0;

struct Object597C0 {
    u8 unk00[0x24];
    u32 flags;
    struct Unk17508 instance;
    u8 unk2C[0x88];
    struct State597C0 *state;
};

struct State597C0 {
    u8 unk00[0x24];
    u32 flags;
    struct Unk17508 instance;
    u8 unk2C[0x66];
    u8 unk92;
    u8 unk93;
    u8 unk94[0x18];
    struct Info597C0 *info;
};

struct Model597C0Sub {
    u8 unk00[0xC];
    u8 unk0C;
    u8 unk0D;
};

struct Model597C0 {
    u8 unk00[8];
    struct Model597C0Sub *unk08;
};

struct Info597C0 {
    u8 unk00[0xBC];
    s32 unkBC;
    u8 unkC0[0xB8];
    struct Unk17508 unk178;
    u8 unk17C[0x48];
    struct Unk17508 unk1C4;
    u8 unk1C8[0x48];
    struct Unk17508 unk210;
    u8 unk214[0x104];
    s16 unk318;
    u8 unk31A[0xA];
    s16 unk324;
};

struct Camera597C0 {
    u8 unk00[0x28];
    u8 unk28[0x54];
    s32 unk7C;
    s32 unk80;
    s32 unk84;
};

struct Global597C0 {
    u8 unk00[0x5C];
    s32 unk5C;
};

extern u8 gFrameTime[] __asm__("D_8009FAA4");
extern u8 gDiscPartIndexTable[] __asm__("D_8009AF3C");
u8 D_8009F36D;
extern u8 D_8009F36E;
extern u8 D_8009F36F;
extern s32 gDiscColourWordSecondary __asm__("D_8009F370");
extern struct Camera597C0 *gCamera __asm__("D_8009F9E4");
extern struct Global597C0 *gGameSession[] __asm__("D_8009FA94");

extern void gSetModelUvOffset(s32 low, s32 high) __asm__("func_80013AA0");
extern u8 gSubmitModelInstance(struct Unk17508 *instance, u32 *flags, s32 arg2) __asm__("func_80017508");
extern void gTransformPartVertex(struct Unk17508 *instance, s32 part, s32 *out) __asm__("func_80017668");
extern s32 gTestFrustumFixedNear(s16 *point, s32 margin) __asm__("func_80019D90");
extern s32 func_800584BC(s32 message, struct State597C0 *object, void *hit, s32 arg3, s32 arg4);
extern void func_8005920C(struct Object597C0 *object, void *spawn, s32 arg2);
extern void gDrawOtherPlayerArrow(struct State597C0 *state) __asm__("func_800592D0");
extern void func_800596C4(struct State597C0 *state);
extern void gGteTransformVertices(s16 *a, s16 *b, s32 c) __asm__("func_8001010C");
extern void gProjectVertexBatch(s16 *a, s16 *b, s16 *c, s32 d) __asm__("func_8001047C");
extern void gGteLoadRotation(u8 *a) __asm__("func_80010820");
extern void gEmitGouraudDisc(s16 *a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g) __asm__("func_8001484C");
extern s32 gEntityBaseHandler(s32 kind, struct Object597C0 *object, void *arg2, s32 arg3) __asm__("func_8002AD58");

/**
 * @brief Draw handler of one effect object: re-arms its model instances, then
 *        submits two overlapping translucent discs with a colour ramp that
 *        advances by half the part count per disc.
 *
 * @param object Object block. Fields used: the child/state pointer at +0xB4
 *               (also the second instance slot at +0x24..+0x2C), flags at
 *               +0x24, instance at +0x28.
 *
 * @return Nothing.
 *
 * @par Globals
 * - D_8009FAA4 (0x8009FAA4): a byte read when the info block asks for it; its
 *   value >> 2 becomes the high colour of the pair.
 * - D_8009AF3C (0x8009AF3C): byte table of model part indices, two entries used.
 * - D_8009F36D, D_8009F36E, D_8009F36F (0x8009F36D..0x8009F36F, reached through
 *   $gp at 0x159/0x15A/0x15B): the ramp byte and two bytes cleared before every
 *   disc.
 * - D_8009F370 (0x8009F370, $gp 0x15C): second colour word passed to
 *   func_8001484C.
 * - D_8009F9E4 (0x8009F9E4): camera block; eye at +0x7C..0x84, matrix at +0x28.
 *
 * @par Control flow
 * Confirmed:
 * 1. state = object->0xB4; info = state->0xAC. Return immediately when the s16
 *    at info+0x318 is positive (a "busy/disabled" counter).
 * 2. func_80013AA0(state->0x92 as low byte, high byte) where the high byte is
 *    D_8009FAA4 >> 2 when the s16 at info+0x324 is positive and state->0x93
 *    otherwise. func_80013AA0 packs the pair into the s16 D_8009F580.
 * 3. Five func_80017508 instance calls, each with its own flag word:
 *    - (&state->0x28, &state->0x24, arg) with arg = 1, or 5 when info+0xBC != 0,
 *      and arg |= 2 when info+0xBC != 0x10000;
 *    - (&object->0x28, &object->0x24, arg) with arg = 1, or 0xB on the same
 *      test, and arg |= 4 when info+0xBC != 0x10000;
 *    - (&info->0x178, &object->0x24, 1), (&info->0x1C4, ..., 1),
 *      (&info->0x210, ..., 1).
 * 4. func_80013AA0(0, 0) clears the packed colour before the last instance and
 *    func_800592D0(state) advances the object's own animation.
 * 5. count = ((Model *)info->0x210)->0x08->0x0C, a byte. For i = 0 and 1:
 *    rem = (model->0x08->0x0D + offset) % count (hardware div with the break 7
 *    and break 6 guards), then the ramp is (rem * 255) / count, computed as
 *    rem << 8 minus rem before a second div.
 * 6. The ramp is written to D_8009F36D while D_8009F36E and D_8009F36F are
 *    cleared, then func_80017668(&info->0x210, D_8009AF3C[i], from) fetches the
 *    part's position.
 * 7. Camera relative position, >> 4 into three s16 screen coordinates,
 *    func_80010820(camera + 0x28), func_8001010C(coords, point, 1).
 * 8. The disc is drawn only when func_80019D90(point, 0x700) returns 2, i.e.
 *    when the point is on screen with a 0x700 margin. Then
 *    func_8001047C(coords, point, hit, 1) and
 *    func_8001484C(point, word at D_8009F36C, D_8009F370, 4, 0x70, 0x200, 0):
 *    four triangles, scale 0x70, depth offset 0x200, no extra angle.
 * 9. On a successful draw offset += count >> 1, so the second disc reads the
 *    ramp half a model period later; otherwise the offset is unchanged and the
 *    second disc repeats the same colour.
 * 10. func_800596C4(object->0xB4) runs after the loop and finishes the object's
 *     per-frame work.
 *
 * @par Player model assembly and visibility
 * state is the owning player object; object is its companion. The five calls
 * submit the player's main instance, the companion instance and three extra
 * instances in the player's info block. Each submission may still be culled
 * or suppressed by func_80017508; five calls do not guarantee five visible
 * parts. func_80016A64 interprets arg as a polygon-group bit mask:
 * - info+0xBC == 0: main 0x03, companion 0x05;
 * - info+0xBC == 0x10000: main 0x05, companion 0x0B;
 * - other values: main 0x07, companion 0x0F.
 * The three extra instances each use 0x01. func_80055988 updates +0xBC during
 * the form transition. The actual resource IDs and anatomical assignments,
 * including which instance contains the legs, are not established here.
 * The separate .NET FcModelViewer currently loads one Cobj and uses mask
 * 0xFF; it does not assemble these instances or reproduce these masks.
 *
 * @par Calls
 * func_80013AA0 (three times), func_80017508 (five times), func_800592D0,
 * func_80017668, func_80010820, func_8001010C, func_80019D90, func_8001047C,
 * func_8001484C, func_800596C4.
 *
 * @par Side effects
 * Writes the three shared bytes at D_8009F36D..F and the packed s16 D_8009F580,
 * mutates the flags words of three model instances through func_80017508, and
 * appends two disc primitive sets to the packet area (through func_8001484C,
 * not directly).
 *
 * @par Likely role
 * A pulsing glow pair - engine flame, muzzle flash or energy trail - on an
 * effect object: the two discs sit on model parts 0 and 1 of the same instance,
 * the colour ramp cycles with the model's frame counter, and the 0x700 margin
 * reads as an aggressive on-screen test so that the glow survives just off the
 * screen edge. The handler is reached from message 3, which in the same family
 * of dispatchers is the draw message; the geometry and the ramp are confirmed,
 * the effect name is not.
 *
 * @par Unknown
 * - What D_8009AF3C holds beyond "two byte part indices".
 * - What D_8009F370 is used for; func_8001484C stores its own argument in the
 *   +0xC and +0x14 fields of the disc packets, so the two colours of the ramp
 *   are the packed word at D_8009F36C and this global.
 * - Why the ramp is computed from model metadata (0x0C/0x0D) rather than from
 *   the animation frame.
 *
 * @par Callers
 * func_80059ABC, recorded at 0x80059ABC (direct call at 0x80059B40), from its
 * message 3 branch; the handler returns 5 afterwards. That dispatcher has no
 * direct caller in analysis/edges.json.
 */
void func_800597C0(struct Object597C0 *object)
{
    struct State597C0 *state;
    struct Info597C0 *info;
    register struct Camera597C0 *cam __asm__("$6");
    s32 arg2;
    register s32 i __asm__("$18");
    s32 offset;
    s32 count;
    u8 *color;
    u8 *parts;
    s32 dx;
    s32 dy;
    s32 dz;
    struct {
        s32 from[3];
        u8 pad0[4];
        s16 point[4];
        s16 coords[4];
        s16 hit[4];
    } loc;

    state = object->state;
    if (state->info->unk318 > 0) {
        return;
    }
    info = state->info;

    {
        register s32 low __asm__("$4");
        register s32 high __asm__("$5");
        s32 condition = info->unk324;
        __asm__ volatile("" : "+r"(condition));
        low = state->unk92;
        __asm__ volatile("" : "+r"(low));
        if (condition > 0) {
            register s32 value __asm__("$2");
            __asm__ volatile("" : : : "memory");
            value = *(volatile u8 *)gFrameTime;
            __asm__ volatile("" : "+r"(value));
            high = value >> 2;
        } else {
            high = state->unk93;
        }
        gSetModelUvOffset(low, high);
    }

    arg2 = 1;
    if (info->unkBC != 0) {
        arg2 = 5;
    }
    if (info->unkBC != 0x10000) {
        arg2 |= 2;
    }
    gSubmitModelInstance(&state->instance, &state->flags, arg2);

    arg2 = 1;
    if (info->unkBC != 0) {
        arg2 = 0xB;
    }
    if (info->unkBC != 0x10000) {
        arg2 |= 4;
    }
    gSubmitModelInstance(&object->instance, &object->flags, arg2);

    gSubmitModelInstance(&info->unk178, &object->flags, 1);
    gSubmitModelInstance(&info->unk1C4, &object->flags, 1);
    gSetModelUvOffset(0, 0);
    gSubmitModelInstance(&info->unk210, &object->flags, 1);
    gDrawOtherPlayerArrow(state);

    offset = 0;
    i = offset;
    __asm__ volatile("" : "+r"(i));
    color = &D_8009F36D;
    parts = gDiscPartIndexTable;
    count = ((struct Model597C0 *)info->unk210.model)->unk08->unk0C;

    do {
        struct Model597C0 *model = (struct Model597C0 *)info->unk210.model;
        s32 temp = model->unk08->unk0D + offset;
        s32 rem = temp % count;
        s32 quotient = (rem * 255) / count;

        D_8009F36E = 0;
        D_8009F36F = 0;
        D_8009F36D = quotient;

        gTransformPartVertex(&info->unk210, *(u8 *)(i + (u32)parts), loc.from);

        cam = gCamera;
        loc.from[0] -= cam->unk7C;
        loc.from[1] -= cam->unk80;
        loc.from[2] -= cam->unk84;
        loc.coords[0] = loc.from[0] >> 4;
        loc.coords[1] = loc.from[1] >> 4;
        loc.coords[2] = loc.from[2] >> 4;

        gGteLoadRotation((u8 *)cam + 0x28);
        gGteTransformVertices(loc.coords, loc.point, 1);

        if ((u8)gTestFrustumFixedNear(loc.point, 0x700) == 2) {
            gProjectVertexBatch(loc.coords, loc.point, loc.hit, 1);
            gEmitGouraudDisc(loc.point, *(s32 *)(color - 1), gDiscColourWordSecondary, 4, 0x70, 0x200, 0);
            offset += count >> 1;
        }
        i++;
    } while (i < 2);

    func_800596C4(object->state);
}
