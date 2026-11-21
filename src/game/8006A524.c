// Categories: Graphics > Effects > Ribbon Beam | Gameplay > Target Selection | Graphics > Effects > Object Pool
#include "game/types.h"

extern int gBeamPartUpdate();

#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Ctx {
    u8 pad00[0x14];
    u16 unk14;
    u8 pad16[2];
    u8 unk18;
};

struct Object;

struct Params {
    u8 pad00[0x36];
    u16 unk36;
    s16 unk38;
    u8 pad3A[1];
    u8 unk3B;
    u8 unk3C;
    u8 unk3D;
    s16 unk3E;
    s16 unk40;
    s16 unk42;
    s16 unk44;
    u8 pad46[2];
    s16 unk48;
    u16 unk4A;
};

struct CallArg {
    s32 unk00;
    s16 unk04;
    u8 unk06;
    u8 pad07;
    struct Ctx *ctx;
};

typedef void (*ObjectFunc)(s32, struct Object *, void *, s32);

struct Object {
    u8 pad00[4];
    ObjectFunc func4;
    s32 unk08;
    u8 pad0C[0x0C];
    u8 unk18;
    u8 pad19[0x0B];
    s32 flags24;
    u8 pad28[0x34];
    s32 x;
    s32 y;
    s32 z;
};

struct List {
    s32 count;
    s32 unk4;
    struct Object *items[1];
};

struct Unk73590;
struct Unk73590Owner;

extern struct List gCommandListHead __asm__("D_800A8F90");

extern s32 func_80019668(void);
extern struct Unk73590 *func_80073590(struct Unk73590Owner *owner, void *coord, s32 kind,
                                      s32 arg3, s32 arg4, s32 arg5);
extern void gCreateRibbonBeam(struct Unk73590 *a, struct Unk73590 *b, s32 arg2, s32 arg3,
                          s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 arg8, s32 arg9,
                          s32 arg10, s32 arg11, s32 arg12, s32 arg13) __asm__("func_8007377C");
void gSpawnChainBeamArcs(struct Ctx *ctx, struct Object *current, s32 arg2, u16 *arg3,
                   struct Params *params) __asm__("func_8006A524");


/**
 * @brief Chains a second beam from the current object to a random nearby
 *        object of another kind, then messages that object.
 *
 * @param ctx Source object; +0x14 a word copied into the message and +0x18 the
 *        kind byte that candidates must not share.
 * @param current Object the arc starts from; 0 returns immediately.
 * @param arg2 Unused - the caller passes the contact position, the body never
 *        reads it.
 * @param arg3 Pointer to a u16 copied into the message's +0x04 field.
 * @param params Optional parameter record; +0x36 segment count, +0x38 strip
 *        count, +0x3B/+0x3C/+0x3D the beam colour, +0x3E the gradient divisor,
 *        +0x40/+0x42/+0x44 three more beam words, and +0x48/+0x4A the widths.
 *        When 0 a fixed parameter set is used.
 * @return Nothing.
 *
 * @par Confirmed
 * The scan starts at func_80019668() modulo the D_800A8F90 count and wraps at
 * the end. Candidates are rejected when their +0x08 is not positive, when they
 * are the current object, when their +0x18 matches ctx +0x18, when their
 * +0x24 flags carry bit 0, or when any axis of the position delta exceeds
 * 0x7FFF. The attempt counter starts at 2 and the function leaves as soon as it
 * goes negative, so at most two candidates are ever accepted. For an accepted
 * candidate two func_80073590 nodes are allocated - current and candidate, kind
 * -1, the width taken from params +0x48, and 0 when params +0x4A has bit 8 - and
 * func_8007377C links them with (params +0x36) >> 1 segments, params +0x38
 * strips, the colour +0x3B/+0x3C/+0x3D, the divisor +0x3E, +0x40, +0x42, +0x44,
 * 0, 0 and 1. Without params the fixed set is 6 segments, 1 strip, colour
 * 0x7F/0x7F/0xFF, divisor 0x4B0, 0x7FFF, -0x1800, 0, 0, 0, 1. Either way the
 * candidate's handler at +0x04 is then called with message 0x0E and a block of
 * ctx +0x14, *arg3, byte 8 and ctx.
 *
 * @par Likely role
 * The branching half of a tether: after the main segment hits something,
 * gBeamPartUpdate calls this to arc a second beam from the tether to one random
 * object of a different class, so a struck target forks lightning or rope onto
 * a neighbour. The kind mismatch and the delta limit keep the arc from hitting
 * its own class or jumping across the level, and the 0x0E message hands the
 * effect to the candidate itself.
 *
 * @par Unknown
 * What message 0x0E means to the receivers, and why the attempt budget is two
 * rather than one.
 *
 * @par Callers
 * gBeamPartUpdate, on the type-0x0B beam branch when the swept test reported a
 * hit.
 */
void gSpawnChainBeamArcs(struct Ctx *ctx, struct Object *current, s32 arg2, u16 *arg3,
                   struct Params *params)
{
    struct Object *item;
    struct Unk73590 *nodeA;
    struct Unk73590 *nodeB;
    struct Vec3 delta;
    struct CallArg call;
    s32 index;
    s32 i;
    s32 attempts;
    u8 kind;

    if (current == 0) {
        return;
    }

    kind = ctx->unk18;
    attempts = 2;
    index = (s32)((u32)func_80019668() % (u32)gCommandListHead.count);
    for (i = 0; i < gCommandListHead.count; i++) {
        item = gCommandListHead.items[index];
        if (item->unk08 <= 0) {
            goto next;
        }
        if (item == current) {
            goto next;
        }
        if (item->unk18 == kind) {
            goto next;
        }
        if (item->flags24 & 1) {
            goto next;
        }

        delta.x = item->x - current->x;
        delta.y = item->y - current->y;
        delta.z = item->z - current->z;

        if (ABS(delta.x) > 0x7FFF) {
            goto next;
        }
        if (ABS(delta.y) > 0x7FFF) {
            goto next;
        }
        if (ABS(delta.z) > 0x7FFF) {
            goto next;
        }

        attempts--;
        if (attempts < 0) {
            return;
        }

        if (params != 0) {
            nodeA = func_80073590((struct Unk73590Owner *)current, 0, -1, 0,
                                  params->unk48, 0);
            nodeB = func_80073590((struct Unk73590Owner *)item, 0, -1, 0,
                                  (params->unk4A & 8) ? 0 : params->unk48, 0);
            gCreateRibbonBeam(nodeA, nodeB, (s16)params->unk36 >> 1, params->unk38,
                          params->unk3B, params->unk3C, params->unk3D, params->unk3E,
                          params->unk40, params->unk42, params->unk44, 0, 0, 1);
            goto L_join;
        } else {
            goto L_else;
        }

    L_join:
        call.unk00 = ctx->unk14;
        call.unk06 = 8;
        call.unk04 = *arg3;
        call.ctx = ctx;
        item->func4(0xE, item, &call, 0);
        goto L_cont;

    L_else:
        nodeA = func_80073590((struct Unk73590Owner *)current, 0, -1, 0, 0x30, 0);
        nodeB = func_80073590((struct Unk73590Owner *)item, 0, -1, 0, 0x30, 0);
        gCreateRibbonBeam(nodeA, nodeB, 6, 1, 0x7F, 0x7F, 0xFF, 0x4B0, 0x7FFF,
                      -0x1800, 0, 0, 0, 1);
        goto L_join;

    L_cont:
    next:
        index++;
        if (index >= gCommandListHead.count) {
            index = 0;
        }
    }
}
