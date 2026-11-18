// Categories: Gameplay > AI > Target Selection | Gameplay > Object Lookup | Physics > Collision
#include "game/types.h"

extern int gGuidedProjectileUpdate();

struct Unk4E80C {
    u8 pad00[8];
    s32 unk08;
    u8 pad0C[8];
    u16 unk14;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19;
    u8 pad1A[0xA];
    u32 unk24;
    u8 pad28[0x34];
    u8 unk5C[1];
};

struct Unk4E80CList {
    s32 count;
    s32 pad04;
    struct Unk4E80C *items[1];
};

extern struct Unk4E80CList gCommandListHead __asm__("D_800A8F90");

extern s32 func_8004E5F0(struct Unk4E80C *a, struct Unk4E80C *b, s32 c, s32 d);
extern s32 func_800684AC(void *a, void *b, s32 c, s32 d, s32 e);
extern s32 func_80019668(void);

/**
 * @brief Picks an object out of the global candidate list by class filter,
 *        range and facing, with four search strategies.
 *
 * @param ctx Seeker: id at +0x08, mode byte at +0x16 (the value 1 adds a
 *        candidate filter), visibility sub-block at +0x5C.
 * @param type Class filter: 2 compares +0x14, 1 compares +0x16, 3 compares
 *        +0x17, 4 compares +0x18, 5 tests the bit (1 << +0x18) against the mask,
 *        7 requires bit 0x4000 of +0x14. Any other value aborts with 0.
 * @param val Value or bit mask for the class filter.
 * @param mode 6 returns ctx without searching; 0 returns the first candidate;
 *        1 returns the nearest; 3 returns the nearest with clear line of sight;
 *        2 returns a random candidate; any other value returns 0.
 * @param arg4 Maximum distance, also the initial best score.
 * @param arg5 Facing threshold handed to func_8004E5F0; 0xFFFF0000 disables the
 *        facing test, which is what every game call site passes.
 * @param flag Priority mode: non-zero makes the low nibble of candidate +0x19 a
 *        priority that outranks distance ties and skips lower-priority objects.
 * @return The chosen object, 0 when nothing matched, or ctx for mode 6.
 *
 * @par Confirmed
 * The candidate list is the global table D_800A8F90 - count at +0x00, object
 * pointers from +0x08.
 * Every strategy skips candidates with +0x08 <= 0, candidates whose +0x08 equals
 * the seeker's, the seeker itself, bit 0 of +0x24, and - when the seeker's +0x16
 * is 1 - bit 0x20000 of +0x24.
 * Mode 0 returns the first survivor of the class filter.
 * Modes 1 and 3 score with func_8004E5F0(ctx, candidate, arg4, arg5), which
 * returns -1 outside the range box or the facing cone and the distance
 * otherwise; the smallest score wins and, with flag set, an equal score only
 * replaces the current best when the candidate's priority nibble is strictly
 * higher, and candidates below the current priority are skipped up front. Mode 3
 * additionally drops candidates for which func_800684AC(ctx +0x5C, candidate
 * +0x5C, 0, 0, 0) reports a hit, i.e. it keeps only unobstructed ones.
 * Mode 2 counts the survivors in the first pass, draws
 * func_80019668() % count and counts down in a second pass. That second pass
 * re-tests only the class filter - the id, self, +0x24 and team tests of the
 * counting pass are gone - so the drawn index can land on an object the first
 * pass had excluded. The quirk is in the original and is preserved.
 *
 * @par Likely role
 * The shared "acquire a target" service of the AI: callers pass a class byte, a
 * range already shifted into 1/16 units, the 0xFFFF0000 facing constant and 0/1
 * for the priority flag, and cache the result together with its +0x08 id to
 * notice later that the object was recycled (func_800324E0, func_8004F330,
 * gGuidedProjectileUpdate all do that).
 *
 * @par Unknown
 * The class numbering, whether the +0x19 nibble really is a threat priority, and
 * the meaning of the +0x24 bits used as exclusion flags.
 *
 * @par Callers
 * func_8002CC30, func_800324E0, gGuidedProjectileUpdate (two sites), func_8004F330,
 * func_800600E8.
 */
struct Unk4E80C *func_8004E80C(struct Unk4E80C *ctx, s32 type, s32 val, s32 mode,
                               s32 arg4, s32 arg5, u8 flag)
{
    struct Unk4E80C *best;
    struct Unk4E80C *o;
    s32 ctxId;
    s32 bestVal;
    s32 prio;
    s32 cnt;
    s32 r;
    s32 i;

    if (mode == 6) {
        return ctx;
    }
    cnt = 0;
    best = 0;
    ctxId = ctx->unk08;
    bestVal = arg4;
    if (flag) {
        prio = -1;
    }
    for (i = 0; i < gCommandListHead.count; i++) {
        o = gCommandListHead.items[i];
        if (o->unk08 <= 0) continue;
        if (o->unk08 == ctxId) continue;
        if (o == ctx) continue;
        if (o->unk24 & 1) continue;
        if (ctx->unk16 == 1 && (o->unk24 & 0x20000)) continue;
        switch (type) {
        case 2:
            if (o->unk14 != val) continue;
            break;
        case 1:
            if (o->unk16 != val) continue;
            break;
        case 3:
            if (o->unk17 != val) continue;
            break;
        case 4:
            if (o->unk18 != val) continue;
            break;
        case 5:
            if (!((1 << o->unk18) & val)) continue;
            break;
        case 7:
            if (!(o->unk14 & 0x4000)) continue;
            break;
        default:
            return 0;
        }
        switch (mode) {
        case 0:
            return o;
        case 1:
        case 3:
            if (flag) {
                if ((o->unk19 & 0xF) < prio) continue;
            }
            r = func_8004E5F0(ctx, o, arg4, arg5);
            if (r < 0) continue;
            if (r >= bestVal) {
                if (!flag) continue;
                if (!(prio < (o->unk19 & 0xF))) continue;
            }
            if (mode != 1) {
                if (func_800684AC(ctx->unk5C, o->unk5C, 0, 0, 0) != 0) continue;
            }
            bestVal = r;
            best = o;
            if (flag) {
                prio = o->unk19 & 0xF;
            }
            break;
        case 2:
            cnt++;
            break;
        default:
            return 0;
        }
    }
    if (mode != 2) {
        return best;
    }
    if (cnt > 0) {
    cnt = (u32)func_80019668() % (u32)cnt;
    for (i = 0; i < gCommandListHead.count; i++) {
        o = gCommandListHead.items[i];
        if (o->unk08 <= 0) continue;
        if (o->unk08 == ctxId) continue;
        switch (type) {
        case 2:
            if (o->unk14 != val) continue;
            break;
        case 1:
            if (o->unk16 != val) continue;
            break;
        case 3:
            if (o->unk17 != val) continue;
            break;
        case 4:
            if (o->unk18 != val) continue;
            break;
        case 5:
            if (!((1 << o->unk18) & val)) continue;
            break;
        case 7:
            if (!(o->unk14 & 0x4000)) continue;
            break;
        default:
            break;
        }
        cnt--;
        if (cnt < 0) {
            return o;
        }
    }
    }
    return 0;
}
