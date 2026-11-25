/* leaf_runtime: runtime helpers that other systems call into - a compound
 * visual reset, a proximity (octagonal-distance) trigger, the priority sound
 * voice allocator, the three-slot status bar and the vibration toggle.
 *
 * Fixture style: every D_ label a unit reads is defined here and written by the
 * test before the call; nothing relies on zero-init or on retail memory.
 * Callees outside the suite are stand-ins bound to their retail label (a wrong
 * binding is a link error); they only record their arguments and return a value
 * the test configured. The one exception is 8007C940, whose decompiled body
 * compares its port argument against `$2`, a register the decompiler left
 * uninitialised; `lr_call_v0` below is a four-instruction trampoline that sets
 * `$2` for that call so the comparison is a fixture and not a leftover of the
 * previous call. Every check says what it pins.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* --------------------------------------------------------------- recorder */

#define LR_MAX 64

#define LR_CLIP      1   /* func_800448D8 select animation clip          */
#define LR_REGISTRY  2   /* func_8006AAEC registry find                  */
#define LR_HUDNUM    3   /* func_800491F8 formatted hud number           */
#define LR_ICON      4   /* func_8006BF44 status slot icon               */
#define LR_POP       5   /* func_8004665C pop free sound voice           */
#define LR_RELEASE   6   /* func_80046698 release sound voice            */
#define LR_LINK      7   /* func_80046870 link active sound voice        */
#define LR_SFX       8   /* func_80046C84 non-positional sound effect    */
#define LR_PADTYPE   9   /* func_8001EF34 pad type                       */
#define LR_READY     10  /* func_8001EAEC pad rumble ready               */
#define LR_ENABLED   11  /* func_8001EB2C pad rumble enabled             */
#define LR_SETEN     12  /* func_8001EC04 set pad rumble enabled         */
#define LR_SMALL     13  /* func_8001E96C set port small rumble          */
#define LR_VISUAL    14  /* func_80013BD8 bind model slot                */

struct LrCall {
    int id;
    s32 a0, a1, a2, a3;
};

static struct LrCall lr_calls[LR_MAX];
static int lr_calls_n;

static void lr_rec(int id, s32 a0, s32 a1, s32 a2, s32 a3)
{
    if (lr_calls_n >= LR_MAX) return;          /* overflow is a test bug */
    lr_calls[lr_calls_n].id = id;
    lr_calls[lr_calls_n].a0 = a0;
    lr_calls[lr_calls_n].a1 = a1;
    lr_calls[lr_calls_n].a2 = a2;
    lr_calls[lr_calls_n].a3 = a3;
    lr_calls_n++;
}

static void lr_reset(void)
{
    int i;
    lr_calls_n = 0;
    for (i = 0; i < LR_MAX; i++) {
        lr_calls[i].id = 0;
        lr_calls[i].a0 = lr_calls[i].a1 = 0;
        lr_calls[i].a2 = lr_calls[i].a3 = 0;
    }
}

/* Index of the nth (0-based) call with this id, or -1. */
static int lr_idx(int id, int nth)
{
    int i;
    for (i = 0; i < lr_calls_n; i++) {
        if (lr_calls[i].id == id) {
            if (nth-- == 0) return i;
        }
    }
    return -1;
}

/* ======================================================================
 * 800516A0 compound visual subpart reset
 * ====================================================================== */

/* Owner: +0x2C fifteen transform words, +0xAC the state block. */
struct LrOwner {
    u8 pad00[0x2C];
    s32 transform[15];
    u8 pad68[0x44];
    u8 *state;
};

/* ------------------------------------------------------------- 80030498 */

void lr_stub_448D8(void *track, s32 index, s32 direction, s32 flags)
    __asm__("func_800448D8");
void lr_stub_448D8(void *track, s32 index, s32 direction, s32 flags)
{
    lr_rec(LR_CLIP, (s32)track, index, direction, flags);
}

struct LrCoord {
    u8 pad00[0x34];
    s32 x;              /* +0x34 */
    s32 y;              /* +0x38 */
    s32 z;              /* +0x3C */
};
struct LrSelf {
    u8 pad00[0x16];
    u8 type;            /* +0x16 */
    u8 pad17[0x11];
    struct LrCoord coord;
    s32 radius;         /* +0x68 */
    u8 pad6C[0x34];
    u8 track[0x14];     /* +0xA0 animation track block */
    struct LrSelf *parent;  /* +0xB4 */
    u8 padB8[5];
    u8 state;           /* +0xBD */
    u8 padBE[8];
    s16 sizec6;         /* +0xC6 */
    u8 padC8[4];
    s16 timer;          /* +0xCC */
    u8 padCE[2];
    struct LrSelf *target;  /* +0xD0 */
};
struct LrOther {
    u8 pad00[0x16];
    u8 type;            /* +0x16 */
    u8 pad17[0x11];
    struct LrCoord coord;
    s32 radius;         /* +0x40 of the coordinate block, i.e. +0x68 here */
    u8 pad6C[0x48];
    struct LrOther *parent; /* +0xB4 */
};

/* ------------------------------------------------------------- 8006C080 */

u8 lr_player_invuln[0x100] __asm__("D_8009FA8C");
s32 lr_frame_time[2] __asm__("D_8009FAA4");
s32 lr_single_player[2] __asm__("D_8009F9EC");

void lr_stub_6AAEC(u8 id) __asm__("func_8006AAEC");
void lr_stub_6AAEC(u8 id)
{
    lr_rec(LR_REGISTRY, id, 0, 0, 0);
}

void lr_stub_491F8(s32 value, s32 x, s32 y, s32 color) __asm__("func_800491F8");
void lr_stub_491F8(s32 value, s32 x, s32 y, s32 color)
{
    lr_rec(LR_HUDNUM, value, x, y, color);
}

void lr_stub_6BF44(s32 a0, s32 a1, s32 a2, s32 a3) __asm__("func_8006BF44");
void lr_stub_6BF44(s32 a0, s32 a1, s32 a2, s32 a3)
{
    lr_rec(LR_ICON, a0, a1, a2, a3);
}

/* Status block at arg0+0xAC (the unit's struct Unk6C080). */
struct LrStatus {
    u8 pad00[0x84];
    s16 field84[3];     /* +0x84 counters */
    s16 field8A[3];     /* +0x8A counters */
    u8 field90[3];      /* +0x90 registry ids */
    u8 pad93[0x2E];
    s8 fieldC1;         /* +0xC1 index into the invulnerability bytes */
};
struct LrStatusArg {
    u8 pad00[0xAC];
    struct LrStatus *ptr;
};

/* ------------------------------------------------------------- 80046B04 */

struct LrVoice {
    u32 slot;                     /* +0x00 */
    u8 volume;                    /* +0x04 */
    s8 unk5;                      /* +0x05 */
    u8 unk6;                      /* +0x06 */
    u8 unk7;                      /* +0x07 */
    s32 time;                     /* +0x08 priority */
    s8 pan;                       /* +0x0C class */
    u8 unkD, unkE, unkF;
    s32 seq;                      /* +0x10 generation */
    s32 unk14;                    /* +0x14 */
    struct LrVoice *next;         /* +0x18 */
    struct LrVoice *freeNext;     /* +0x1C */
};
struct LrBank {
    s32 samples[4];               /* +0x00 */
    u8 count;                     /* +0x10 */
    u8 cursor;                    /* +0x11 */
    u8 voices;                    /* +0x12 */
    u8 unk13;
    u8 unk14;                     /* +0x14 capacity */
    u8 unk15;
    u8 key;                       /* +0x16 */
    u8 unk17;
    struct LrVoice *head;         /* +0x18 */
};
struct LrBank lr_banks[0x100] __asm__("D_800A7938");
struct LrVoice *lr_priority_list __asm__("D_8009F744");
s32 lr_voice_generation __asm__("D_8009F33C");

static struct LrVoice *lr_pop_queue[4];
static int lr_pop_queue_n;

struct LrVoice *lr_stub_4665C(void) __asm__("func_8004665C");
struct LrVoice *lr_stub_4665C(void)
{
    struct LrVoice *v = 0;
    lr_rec(LR_POP, 0, 0, 0, 0);
    if (lr_pop_queue_n < 4) v = lr_pop_queue[lr_pop_queue_n];
    lr_pop_queue_n++;
    return v;
}

void lr_stub_46698(struct LrVoice *voice) __asm__("func_80046698");
void lr_stub_46698(struct LrVoice *voice)
{
    lr_rec(LR_RELEASE, (s32)voice, 0, 0, 0);
}

void lr_stub_46870(struct LrVoice *voice) __asm__("func_80046870");
void lr_stub_46870(struct LrVoice *voice)
{
    lr_rec(LR_LINK, (s32)voice, 0, 0, 0);
}

/* ------------------------------------------------------------- 8007C940 */

s32 lr_prev_buttons[2] __asm__("D_8009F8B0");
/* Four bytes: the unit indexes the toggle with its port argument and does not
 * bound it, so a port of 2 must still land inside the fixture. */
u8 lr_rumble_toggle[4] __asm__("D_8009F8D8");

static s32 lr_sfx_ret;             /* func_80046C84 */
static s32 lr_padtype_ret;         /* func_8001EF34 */
static u8 lr_ready_ret;            /* func_8001EAEC */
static u8 lr_enabled_ret;          /* func_8001EB2C */

s32 lr_stub_46C84(s32 a, s32 b) __asm__("func_80046C84");
s32 lr_stub_46C84(s32 a, s32 b)
{
    lr_rec(LR_SFX, a, b, 0, 0);
    return lr_sfx_ret;
}

s32 lr_stub_1EF34(s32 port) __asm__("func_8001EF34");
s32 lr_stub_1EF34(s32 port)
{
    lr_rec(LR_PADTYPE, port, 0, 0, 0);
    return lr_padtype_ret;
}

u8 lr_stub_1EAEC(s32 port) __asm__("func_8001EAEC");
u8 lr_stub_1EAEC(s32 port)
{
    lr_rec(LR_READY, port, 0, 0, 0);
    return lr_ready_ret;
}

u8 lr_stub_1EB2C(s32 port) __asm__("func_8001EB2C");
u8 lr_stub_1EB2C(s32 port)
{
    lr_rec(LR_ENABLED, port, 0, 0, 0);
    return lr_enabled_ret;
}

void lr_stub_1EC04(s32 port, s32 on) __asm__("func_8001EC04");
void lr_stub_1EC04(s32 port, s32 on)
{
    lr_rec(LR_SETEN, port, on, 0, 0);
}

void lr_stub_1E96C(s32 port, s32 on) __asm__("func_8001E96C");
void lr_stub_1E96C(s32 port, s32 on)
{
    lr_rec(LR_SMALL, port, on, 0, 0);
}

void lr_stub_13BD8(void *list, void *entry, s32 mode) __asm__("func_80013BD8");
void lr_stub_13BD8(void *list, void *entry, s32 mode)
{
    lr_rec(LR_VISUAL, (s32)list, (s32)entry, mode, 0);
}

/* Call `target(a0, a1)` with $v0 preset to `v0`.
 *
 * 8007C940 tests its port argument against `$2` at function entry - the
 * decompiler kept that compare but no assignment to $2 survived, so the value
 * is whatever the caller left there. This trampoline makes it a fixture: it
 * keeps $ra on the stack (the callee may clobber every t-register) and returns
 * the callee's $v0 unchanged, because the unit's return value is its input. */
s32 lr_call_v0(s32 target, s32 a0, s32 a1, s32 v0);
__asm__(".text\n"
        "\t.globl lr_call_v0\n"
        "lr_call_v0:\n"
        "\tsubu $sp,$sp,24\n"
        "\tsw $31,20($sp)\n"
        "\taddu $9,$4,$0\n"
        "\taddu $2,$7,$0\n"
        "\taddu $4,$5,$0\n"
        "\taddu $5,$6,$0\n"
        "\tjalr $9\n"
        "\tnop\n"
        "\tlw $31,20($sp)\n"
        "\taddu $sp,$sp,24\n"
        "\tjr $31\n"
        "\tnop\n");

/* ============================================================ tested units */

void gResetCompoundVisualSubpart(struct LrOwner *owner) __asm__("func_800516A0");
void gCheckResupplyPadProximity(struct LrSelf *self, struct LrOther *other)
    __asm__("func_80030498");
void gDrawThreeSlotStatusBar(struct LrStatusArg *arg0) __asm__("func_8006C080");
s32 gAllocPrioritySoundVoice(s32 arg0, s32 arg1, s32 arg2, s32 slot)
    __asm__("func_80046B04");
s32 gToggleVibrationPair(s32 arg0, s32 arg1) __asm__("func_8007C940");

/* ==================================================================== tests */

static u8 lr_state[0x600];

static void test_compound_visual_reset(void)
{
    static struct LrOwner owner;
    u8 *state;
    u8 *entry;
    int i;

    /* @leaf 800516A0 source src/game/800516A0.c: entry at state+0x2A8, +0x0C word keeps the low half 1 and bit 27, every other high bit is masked off; self link at +0x34/+0x3C, +0x40 = 0x10, four bytes at +0x30, the model slot bound, transform[0..14] copied to +0x260 */
    LK_FUNC("func_800516A0");

    state = lr_state;
    lk_fill(state, 0xFF, sizeof lr_state);
    for (i = 0; i < 15; i++) owner.transform[i] = 0x1000 + i * 0x11;
    owner.state = state;
    lr_reset();
    gResetCompoundVisualSubpart(&owner);
    entry = state + 0x2A8;

    FCK_EQ(*(s32 *)(entry + 0x00), 0, "+0x00 cleared");
    FCK_EQ(*(s32 *)(entry + 0x04), 0, "+0x04 cleared");
    FCK_EQ(*(s32 *)(entry + 0x08), 0, "+0x08 cleared");
    /* all-ones canary: the mask chain keeps only bit 27 of the old high half
     * and then sets bits 16 and 27, so the word is 0x08010001. */
    FCK_EQ(*(u32 *)(entry + 0x0C), 0x08010001u, "flag word from the mask chain");
    FCK_EQ(*(s32 *)(entry + 0x10), 0, "+0x10 cleared");
    FCK_EQ(*(s32 *)(entry + 0x14), 0, "+0x14 cleared");
    FCK_EQ(*(s32 *)(entry + 0x18), 0, "+0x18 cleared");
    FCK_EQ(*(s32 *)(entry + 0x1C), 0, "+0x1C cleared");
    FCK_EQ(*(s32 *)(entry + 0x20), 0, "+0x20 cleared");
    FCK_EQ(*(s32 *)(entry + 0x24), 0, "+0x24 cleared");
    FCK_EQ(*(s32 *)(entry + 0x28), 0, "+0x28 cleared");
    FCK_EQ(*(s32 *)(entry + 0x2C), 0, "+0x2C cleared");
    FCK_EQ(*(s32 *)(entry + 0x38), 0, "+0x38 cleared");
    FCK_CHECK(lk_all(entry, 0x30, 0x34, 0), "four bytes at +0x30 cleared");
    FCK_EQ(*(s32 *)(entry + 0x3C), 1, "+0x3C holds 1");
    FCK_CHECK(*(u8 **)(entry + 0x34) == entry + 0x3C,
              "+0x34 is a self link to +0x3C");
    FCK_EQ(*(s32 *)(entry + 0x40), 0x10, "+0x40 holds 0x10");
    FCK_CHECK(*(s32 *)(entry + 0x44) == (s32)0xFFFFFFFF &&
              *(s32 *)(entry + 0x50) == (s32)0xFFFFFFFF,
              "the reset does not run past +0x40");
    FCK_EQ(lr_calls_n, 1, "one callee");
    FCK_CHECK(lr_calls[0].id == LR_VISUAL &&
              lr_calls[0].a0 == (s32)(state + 0x25C) &&
              lr_calls[0].a1 == (s32)entry && lr_calls[0].a2 == 0,
              "the model slot at state+0x25C is bound to the entry, mode 0");
    for (i = 0; i < 15; i++) {
        if (*(s32 *)(state + 0x260 + i * 4) != owner.transform[i]) break;
    }
    FCK_EQ(i, 15, "transform[0..14] copied to state+0x260");
    FCK_CHECK(*(s32 *)(state + 0x29C) == (s32)0xFFFFFFFF,
              "the copy stops after fifteen words");

    /* A zero canary gives the same flag word: the chain clears every high bit
     * except 27, so nothing of the previous content survives. */
    lk_fill(state, 0x00, sizeof lr_state);
    owner.state = state;
    gResetCompoundVisualSubpart(&owner);
    FCK_EQ(*(u32 *)(entry + 0x0C), 0x08010001u,
           "zero canary: the same flag word (no old bits survive)");
    FCK_EQ(lr_calls[1].a1, (s32)entry, "the second call binds the same entry");

    /* A third canary with only bit 31 set checks that bit 31 itself is
     * cleared rather than preserved. */
    lk_fill(state, 0x00, sizeof lr_state);
    *(u32 *)(entry + 0x0C) = 0x80000000u;
    gResetCompoundVisualSubpart(&owner);
    FCK_EQ(*(u32 *)(entry + 0x0C), 0x08010001u, "bit 31 of the old word cleared");
}

/* Build a pair whose horizontal distance is exactly the octagonal estimate
 * for (dx, dz); keeps the intent of the cases readable. */
static void proximity_pair(struct LrSelf *self, struct LrOther *other,
                           s32 dx, s32 dy, s32 dz)
{
    lk_fill((u8 *)self, 0xA5, sizeof *self);
    lk_fill((u8 *)other, 0xA5, sizeof *other);
    self->coord.x = 0x4000;
    self->coord.y = 0x2200;
    self->coord.z = 0x8000;
    other->coord.x = 0x4000 + dx;
    other->coord.y = 0x2200 + dy;
    other->coord.z = 0x8000 + dz;
    self->state = 0;
    self->sizec6 = 0;
    self->radius = 0;
    self->timer = 0x1111;
    self->target = 0;
    self->type = 0x20;
    other->type = 0x11;
    other->radius = 0;
    other->parent = 0;
}

static void test_resupply_proximity(void)
{
    static struct LrSelf self;
    static struct LrOther other;

    /* @leaf 80030498 math octagonal 2D distance (max + min refined to sqrt2/128 units, >>2 / >>6 correction) against (unkC6<<4)+b->unk40+0x4000 in state 0 and unk68+b->unk40 in state 2, then the same estimate built from (horizontal distance, |dy|); state 0 selects clip 1 and latches state 1, state 2 latches state 5 with 0x190 and the parent of a type-0x12 target */
    LK_FUNC("func_80030498");

    /* state guard */
    proximity_pair(&self, &other, 0, 0, 0);
    self.state = 1;
    lr_reset();
    gCheckResupplyPadProximity(&self, &other);
    FCK_EQ(lr_calls_n, 0, "state 1 returns without a check");
    self.state = 3;
    gCheckResupplyPadProximity(&self, &other);
    FCK_EQ(lr_calls_n, 0, "state 3 returns without a check");
    FCK_EQ(self.timer, 0x1111, "guarded states keep the timer");

    /* state 0, coincident points: distance 0 < 0x4000 + radius */
    proximity_pair(&self, &other, 0, 0, 0);
    self.state = 0;
    self.sizec6 = 0x20;                 /* 0x200 */
    other.radius = 0x30;                /* limit 0x4000 + 0x200 + 0x30 */
    lr_reset();
    gCheckResupplyPadProximity(&self, &other);
    FCK_EQ(self.state, 1, "state 0 hit latches state 1");
    FCK_EQ(lr_calls_n, 1, "one clip selection");
    FCK_CHECK(lr_calls[0].id == LR_CLIP &&
              lr_calls[0].a0 == (s32)((u8 *)&self + 0xA0) &&
              lr_calls[0].a1 == 1 && lr_calls[0].a2 == 1 && lr_calls[0].a3 == 1,
              "clip 1 is selected forward with flags 1 on track self+0xA0");
    FCK_EQ(self.timer, 0x1111, "state 0 keeps the countdown");
    FCK_EQ((s32)self.target, 0, "state 0 does not set a target");

    /* dx 5000, dz 0: the estimate is the larger component, 5000; the limit
     * 0x4000 + 0x200 + 0x30 = 16944 still accepts it. */
    proximity_pair(&self, &other, 5000, 0, 0);
    self.state = 0;
    self.sizec6 = 0x20;
    other.radius = 0x30;
    lr_reset();
    gCheckResupplyPadProximity(&self, &other);
    FCK_EQ(self.state, 1, "5000 < 16944 still triggers");

    /* dx 20000 > limit: no hit, no clip, nothing written. */
    proximity_pair(&self, &other, 20000, 0, 0);
    self.state = 0;
    self.sizec6 = 0x20;
    other.radius = 0x30;
    lr_reset();
    gCheckResupplyPadProximity(&self, &other);
    FCK_EQ(self.state, 0, "20000 > limit keeps state 0");
    FCK_EQ(lr_calls_n, 0, "out of range selects no clip");

    /* Diagonal 2000/2000 -> 362000 >> 7 = 2828. Limit 4000 via unk68 in
     * state 2, so the diagonal is inside; |dy| 500 then keeps the second pass
     * at 3328 + 125 - 7 = 3446 < 4000. */
    proximity_pair(&self, &other, 2000, 500, 2000);
    self.state = 2;
    self.radius = 4000;
    other.radius = 0;
    other.type = 0x11;
    lr_reset();
    gCheckResupplyPadProximity(&self, &other);
    FCK_EQ(self.state, 5, "state 2 hit latches state 5");
    FCK_EQ(self.timer, 0x190, "latch arms the 0x190 countdown");
    FCK_CHECK(self.target == (struct LrSelf *)&other,
              "a non-0x12 target is stored directly");
    FCK_EQ(lr_calls_n, 0, "state 2 selects no clip");

    /* The same diagonal with |dy| 4000: the second pass starts from the 2828
     * horizontal estimate and |dy| 4000, and the octagonal refinement turns it
     * into 4707 + 293 - 18 = 4982 > 4000, so the latch is refused even though
     * the horizontal test passed. */
    proximity_pair(&self, &other, 2000, 4000, 2000);
    self.state = 2;
    self.radius = 4000;
    lr_reset();
    gCheckResupplyPadProximity(&self, &other);
    FCK_EQ(self.state, 2, "4982 > limit refuses the latch");
    FCK_EQ(self.timer, 0x1111, "refused latch keeps the timer");
    FCK_EQ(lr_calls_n, 0, "refused latch selects no clip");

    /* |dy| 1000 only adds the >>2/>>6 correction to the larger component:
     * 2828 + 250 - 15 = 3063 < 4000, so this one latches. */
    proximity_pair(&self, &other, 2000, 1000, 2000);
    self.state = 2;
    self.radius = 4000;
    lr_reset();
    gCheckResupplyPadProximity(&self, &other);
    FCK_EQ(self.state, 5, "3063 < limit latches state 5");
    FCK_EQ(self.timer, 0x190, "the latch arms the countdown");

    /* Diagonal 3000/3000 -> 546000 >> 7 = 4265 > 4000: the horizontal test
     * alone refuses, before the vertical pass. */
    proximity_pair(&self, &other, 3000, 0, 3000);
    self.state = 2;
    self.radius = 4000;
    lr_reset();
    gCheckResupplyPadProximity(&self, &other);
    FCK_EQ(self.state, 2, "4265 > limit keeps state 2");

    /* A type-0x12 target is replaced by its parent. */
    proximity_pair(&self, &other, 0, 0, 0);
    self.state = 2;
    self.radius = 0x100;
    other.type = 0x12;
    other.parent = &self;
    lr_reset();
    gCheckResupplyPadProximity(&self, &other);
    FCK_EQ(self.state, 5, "type 0x12 still latches");
    FCK_CHECK(self.target == &self, "type 0x12 stores the parent instead");

    /* Exactly at the limit: dist == limit is not a hit (strict <). With
     * dx = 0x4100 and a limit of 0x4100 in state 2. */
    proximity_pair(&self, &other, 0x4100, 0, 0);
    self.state = 2;
    self.radius = 0x4100;
    lr_reset();
    gCheckResupplyPadProximity(&self, &other);
    FCK_EQ(self.state, 2, "distance equal to the limit is not a hit");
    self.radius = 0x4101;
    gCheckResupplyPadProximity(&self, &other);
    FCK_EQ(self.state, 5, "one unit above the limit is a hit");
}

static void test_status_bar(void)
{
    static struct LrStatus d;
    static struct LrStatusArg arg;
    int i;
    int c;

    /* @leaf 8006C080 source src/game/8006C080.c: three slots at x 0x33/0x5C/0x85 and y 0x22/0x4B/0x74 (step 0x29): registry id, the +0x8A counter at 0xCE in colour 9/0xC from frame bit 0x40 when the invulnerability byte is clear, icon highlight from that byte, and the +0x84 counter at 0xD8 in colour 8; a closing icon (-1, byte, 0xC) and, with a live single-player entity, one more at 0x124 */
    LK_FUNC("func_8006C080");

    lk_fill((u8 *)&d, 0, sizeof d);
    lk_fill(lr_player_invuln, 0, sizeof lr_player_invuln);
    d.field84[0] = 11; d.field84[1] = 22; d.field84[2] = 33;
    d.field8A[0] = 44; d.field8A[1] = 55; d.field8A[2] = 66;
    d.field90[0] = 0x71; d.field90[1] = 0x72; d.field90[2] = 0x73;
    d.fieldC1 = 3;
    lr_player_invuln[3] = 0;
    arg.ptr = &d;
    lr_frame_time[0] = 0x40;
    lr_single_player[0] = 0;
    lr_reset();
    gDrawThreeSlotStatusBar(&arg);

    FCK_EQ(lr_calls_n, 13, "three slots of four calls plus the closing icon");
    for (i = 0; i < 3; i++) {
        c = i * 4;
        FCK_CHECK(lr_calls[c].id == LR_REGISTRY &&
                  lr_calls[c].a0 == 0x71 + i, "slot registry lookup");
        FCK_CHECK(lr_calls[c + 1].id == LR_HUDNUM &&
                  lr_calls[c + 1].a0 == 44 + i * 11 &&
                  lr_calls[c + 1].a1 == 0x33 + i * 0x29 &&
                  lr_calls[c + 1].a2 == 0xCE && lr_calls[c + 1].a3 == 9,
                  "highlighted counter at 0xCE, colour 9 for frame bit 0x40");
        FCK_CHECK(lr_calls[c + 2].id == LR_ICON &&
                  lr_calls[c + 2].a0 == i && lr_calls[c + 2].a1 == 1 &&
                  lr_calls[c + 2].a2 == 0x22 + i * 0x29 &&
                  lr_calls[c + 2].a3 == 0xE0, "slot icon is highlighted");
        FCK_CHECK(lr_calls[c + 3].id == LR_HUDNUM &&
                  lr_calls[c + 3].a0 == 11 + i * 11 &&
                  lr_calls[c + 3].a1 == 0x33 + i * 0x29 &&
                  lr_calls[c + 3].a2 == 0xD8 && lr_calls[c + 3].a3 == 8,
                  "total counter at 0xD8 takes the +0x84 word");
    }
    FCK_CHECK(lr_calls[12].id == LR_ICON && lr_calls[12].a0 == -1 &&
              lr_calls[12].a1 == 0 && lr_calls[12].a2 == 0xC &&
              lr_calls[12].a3 == 0xE0, "closing icon at y 0xC");

    /* Frame bit 0x40 clear -> colour 0xC. With the invulnerability byte clear
     * a zero counter skips its 0xCE number and leaves the slot unhighlighted,
     * while the 0xD8 number still takes the +0x84 word. */
    lk_fill((u8 *)&d, 0, sizeof d);
    d.field84[0] = 11; d.field84[1] = 22; d.field84[2] = 33;
    d.field8A[0] = 0; d.field8A[1] = 55; d.field8A[2] = 0;
    d.field90[0] = 0x71; d.field90[1] = 0x72; d.field90[2] = 0x73;
    d.fieldC1 = 5;
    lr_player_invuln[5] = 0;
    lr_frame_time[0] = 0;
    lr_single_player[0] = 0;
    lr_reset();
    gDrawThreeSlotStatusBar(&arg);
    /* slot 0: registry, icon(0), 0xD8  -> 3
     * slot 1: registry, 0xCE, icon(1), 0xD8 -> 4
     * slot 2: registry, icon(0), 0xD8  -> 3
     * plus the closing icon -> 11 */
    FCK_EQ(lr_calls_n, 11, "zero counters skip their 0xCE number");
    FCK_CHECK(lr_calls[0].id == LR_REGISTRY && lr_calls[0].a0 == 0x71,
              "slot 0 registry first");
    FCK_CHECK(lr_calls[1].id == LR_ICON && lr_calls[1].a1 == 0 &&
              lr_calls[1].a0 == 0, "a zero counter without the byte is dark");
    FCK_CHECK(lr_calls[2].id == LR_HUDNUM && lr_calls[2].a0 == 11 &&
              lr_calls[2].a2 == 0xD8 && lr_calls[2].a3 == 8,
              "slot 0's 0xD8 number takes the +0x84 word");
    FCK_CHECK(lr_calls[3].id == LR_REGISTRY && lr_calls[3].a0 == 0x72,
              "slot 1 registry");
    FCK_CHECK(lr_calls[4].id == LR_HUDNUM && lr_calls[4].a0 == 55 &&
              lr_calls[4].a1 == 0x5C && lr_calls[4].a2 == 0xCE &&
              lr_calls[4].a3 == 0xC,
              "slot 1 counter uses colour 0xC without frame bit 0x40");
    FCK_CHECK(lr_calls[5].id == LR_ICON && lr_calls[5].a0 == 1 &&
              lr_calls[5].a1 == 1, "slot 1 icon highlighted by its counter");
    FCK_CHECK(lr_calls[6].id == LR_HUDNUM && lr_calls[6].a0 == 22 &&
              lr_calls[6].a2 == 0xD8, "slot 1 0xD8 number takes the +0x84 word");
    FCK_CHECK(lr_calls[7].id == LR_REGISTRY && lr_calls[7].a0 == 0x73,
              "slot 2 registry");
    FCK_CHECK(lr_calls[8].id == LR_ICON && lr_calls[8].a2 == 0x74 &&
              lr_calls[8].a1 == 0, "slot 2 icon at y 0x74 is dark");
    FCK_CHECK(lr_calls[9].id == LR_HUDNUM && lr_calls[9].a0 == 33 &&
              lr_calls[9].a2 == 0xD8, "slot 2 0xD8 number");
    FCK_CHECK(lr_calls[10].id == LR_ICON && lr_calls[10].a0 == -1 &&
              lr_calls[10].a1 == 0 && lr_calls[10].a2 == 0xC,
              "closing icon with the byte value 0");

    /* Invulnerability byte set: no 0xCE numbers at all, every slot is
     * highlighted, the 0xD8 numbers take +0x8A, and the closing icon carries
     * the raw byte. A live single-player entity adds the 0x124 icon. */
    lk_fill((u8 *)&d, 0, sizeof d);
    d.field84[0] = 11; d.field84[1] = 22; d.field84[2] = 33;
    d.field8A[0] = 44; d.field8A[1] = 55; d.field8A[2] = 66;
    d.field90[0] = 0x71; d.field90[1] = 0x72; d.field90[2] = 0x73;
    d.fieldC1 = 7;
    lr_player_invuln[7] = 2;
    lr_frame_time[0] = 0x40;
    lr_single_player[0] = 1;
    lr_reset();
    gDrawThreeSlotStatusBar(&arg);
    FCK_EQ(lr_calls_n, 11, "three slots of three calls, two closers");
    for (i = 0; i < 3; i++) {
        c = i * 3;
        FCK_CHECK(lr_calls[c].id == LR_REGISTRY &&
                  lr_calls[c].a0 == 0x71 + i, "slot registry lookup");
        FCK_CHECK(lr_calls[c + 1].id == LR_ICON && lr_calls[c + 1].a1 == 1,
                  "the byte highlights every slot");
        FCK_CHECK(lr_calls[c + 2].id == LR_HUDNUM &&
                  lr_calls[c + 2].a0 == 44 + i * 11 &&
                  lr_calls[c + 2].a2 == 0xD8,
                  "the 0xD8 number takes the +0x8A word with the byte set");
    }
    FCK_CHECK(lr_calls[9].id == LR_ICON && lr_calls[9].a0 == -1 &&
              lr_calls[9].a1 == 2, "the closing icon carries the raw byte");
    FCK_CHECK(lr_calls[10].id == LR_ICON && lr_calls[10].a0 == -1 &&
              lr_calls[10].a1 == 0 && lr_calls[10].a2 == 0x124 &&
              lr_calls[10].a3 == 0xE0, "the live entity adds the 0x124 icon");
}

static void voice_reset(void)
{
    int i;
    lk_fill((u8 *)lr_banks, 0, sizeof lr_banks);
    for (i = 0; i < 4; i++) lr_pop_queue[i] = 0;
    lr_pop_queue_n = 0;
    lr_priority_list = 0;
    lr_voice_generation = 1;
    lr_reset();
}

static void test_priority_voice(void)
{
    static struct LrVoice v;
    static struct LrVoice w;
    static struct LrVoice node;
    s32 r;

    /* @leaf 80046B04 source src/game/80046B04.c: priority is arg0 + (arg1<<4); a free voice is taken when the slot is 0xFF or the bank has room (voices < unk14), otherwise the bank head (or the global list for 0xFF) is stolen when its priority is lower and, for a bank, it is panned hard or has a successor; the voice carries slot/volume/pan(=arg2)/priority and a fresh generation that wraps 1.., linked in by 80046870 */
    LK_FUNC("func_80046B04");

    /* 0xFF with a free voice: bank fields are not consulted */
    voice_reset();
    lk_fill((u8 *)&v, 0xAA, sizeof v);
    lr_pop_queue[0] = &v;
    lr_pop_queue_n = 0;
    r = gAllocPrioritySoundVoice(0x40, 3, 0, 0xFF);
    FCK_CHECK(r == (s32)&v, "0xFF returns the popped voice");
    FCK_EQ(v.slot, 0xFF, "bank slot 0xFF stored");
    FCK_EQ(v.volume, 0x40, "volume is arg0");
    FCK_EQ(v.pan, 0, "class is arg2");
    FCK_EQ(v.time, 0x40 + 0x30, "priority arg0 + (arg1 << 4)");
    FCK_EQ(v.seq, 1, "generation stored");
    FCK_EQ(v.unk5, -1, "unk5 set to -1");
    FCK_EQ(v.unk14, 0, "unk14 cleared");
    FCK_CHECK(v.next == 0, "next cleared");
    FCK_EQ(lr_voice_generation, 2, "generation advances");
    FCK_EQ(lr_calls_n, 2, "one pop and one link");
    FCK_CHECK(lr_calls[0].id == LR_POP && lr_calls[1].id == LR_LINK &&
              lr_calls[1].a0 == (s32)&v, "the voice is linked in");

    /* arg2 == 1 adds 0x64 to the priority */
    voice_reset();
    lr_pop_queue[0] = &v;
    lr_pop_queue_n = 0;
    r = gAllocPrioritySoundVoice(0x10, 1, 1, 0xFF);
    FCK_CHECK(r == (s32)&v, "class 1 still returns the voice");
    FCK_EQ(v.time, 0x10 + 0x10 + 0x64, "class 1 priority gets the 0x64 bonus");
    FCK_EQ(v.pan, 1, "class 1 stored in pan");

    /* generation wraps to 1, never 0 */
    voice_reset();
    lr_voice_generation = -1;
    lr_pop_queue[0] = &v;
    lr_pop_queue_n = 0;
    gAllocPrioritySoundVoice(0, 0, 0, 0xFF);
    FCK_EQ(v.seq, -1, "the current generation is stored as-is");
    FCK_EQ(lr_voice_generation, 1, "generation 0 wraps to 1");

    /* A real bank with room takes a free voice. */
    voice_reset();
    lr_banks[2].voices = 1;
    lr_banks[2].unk14 = 2;
    lr_pop_queue[0] = &v;
    lr_pop_queue_n = 0;
    r = gAllocPrioritySoundVoice(0x20, 0, 0, 2);
    FCK_CHECK(r == (s32)&v, "a bank with room allocates a free voice");
    FCK_EQ(v.slot, 2, "the real slot is stored");

    /* A full bank steals its head only when the priority is lower and the
     * head is panned hard or has a successor. */
    voice_reset();
    lr_banks[2].voices = 2;
    lr_banks[2].unk14 = 2;
    node.time = 0x30;
    node.pan = 1;
    node.next = 0;
    lr_banks[2].head = &node;
    lr_pop_queue[0] = &v;
    lr_pop_queue_n = 0;
    r = gAllocPrioritySoundVoice(0x40, 0, 0, 2);
    FCK_CHECK(r == (s32)&v, "a full bank steals its hard-panned head");
    FCK_EQ(lr_calls_n, 3, "release, pop, link");
    FCK_CHECK(lr_calls[0].id == LR_RELEASE && lr_calls[0].a0 == (s32)&node,
              "the stolen head is released");
    FCK_EQ(v.time, 0x40, "the steal takes the new priority");

    /* Head not panned hard but with a successor: still stealable. */
    voice_reset();
    lr_banks[2].voices = 2; lr_banks[2].unk14 = 2;
    node.time = 0x30; node.pan = 0; node.next = &w;
    lr_banks[2].head = &node;
    lr_pop_queue[0] = &v; lr_pop_queue_n = 0;
    FCK_CHECK(gAllocPrioritySoundVoice(0x40, 0, 0, 2) == (s32)&v,
              "a head with a successor is stealable");

    /* Head with equal priority or without successor and centre pan: nothing
     * is stolen. */
    voice_reset();
    lr_banks[2].voices = 2; lr_banks[2].unk14 = 2;
    node.time = 0x40; node.pan = 1; node.next = 0;
    lr_banks[2].head = &node;
    lr_pop_queue[0] = &v; lr_pop_queue_n = 0;
    FCK_EQ(gAllocPrioritySoundVoice(0x40, 0, 0, 2), 0,
           "equal priority is not stolen (strict <)");
    FCK_EQ(lr_calls_n, 0, "no release and no pop");

    node.time = 0x10; node.pan = 0; node.next = 0;
    lr_banks[2].head = &node;
    FCK_EQ(gAllocPrioritySoundVoice(0x40, 0, 0, 2), 0,
           "a centre-panned head without successor is not stolen");

    node.next = 0;
    lr_banks[2].head = 0;
    FCK_EQ(gAllocPrioritySoundVoice(0x40, 0, 0, 2), 0,
           "an empty full bank yields nothing");

    /* 0xFF with an empty free list falls back to the global priority list. */
    voice_reset();
    node.time = 0x20; node.pan = 0; node.next = 0;
    lr_priority_list = &node;
    lr_pop_queue[0] = 0;
    lr_pop_queue[1] = &v;
    lr_pop_queue_n = 0;
    r = gAllocPrioritySoundVoice(0x40, 0, 0, 0xFF);
    FCK_CHECK(r == (s32)&v, "the second pop supplies the voice");
    FCK_EQ(lr_calls_n, 4, "pop, release, pop, link");
    FCK_CHECK(lr_calls[0].id == LR_POP && lr_calls[1].id == LR_RELEASE &&
              lr_calls[1].a0 == (s32)&node && lr_calls[2].id == LR_POP &&
              lr_calls[3].id == LR_LINK, "steal order");

    /* Global list with a priority that is not lower: no steal at all. */
    voice_reset();
    node.time = 0x40;
    lr_priority_list = &node;
    lr_pop_queue[0] = 0; lr_pop_queue_n = 0;
    FCK_EQ(gAllocPrioritySoundVoice(0x40, 0, 0, 0xFF), 0,
           "equal global priority is not stolen");
    FCK_EQ(lr_calls_n, 1, "only the first pop ran");

    /* Null global list: failure with no link. */
    voice_reset();
    lr_priority_list = 0;
    lr_pop_queue[0] = 0; lr_pop_queue_n = 0;
    FCK_EQ(gAllocPrioritySoundVoice(0x40, 0, 0, 0xFF), 0, "no voice at all");
    FCK_EQ(lr_calls_n, 1, "no release and no link");
}

static void vibration_reset(void)
{
    lr_prev_buttons[0] = lr_prev_buttons[1] = 0;
    lr_rumble_toggle[0] = lr_rumble_toggle[1] = 0;
    lr_padtype_ret = 0;
    lr_ready_ret = 0;
    lr_enabled_ret = 0;
    lr_sfx_ret = 0;
    lr_reset();
}

static void test_vibration_toggle(void)
{
    /* @leaf 8007C940 source src/game/8007C940.c: the body runs only when $2 (unset by the decompiler, set here through lr_call_v0) equals the port; a new press is `buttons & ~prev[port] < 0` (sign bit), which plays 0x48 and, with a pad present and ready, flips toggle[port] and pulses motor mode 1 for one frame; the return value is arg0 */
    LK_FUNC("func_8007C940");

    /* port mismatch: $2 differs from the port argument */
    vibration_reset();
    FCK_EQ(lr_call_v0((s32)gToggleVibrationPair, 0x80000000u, 0, 1),
           (s32)0x80000000u, "the input buttons are returned");
    FCK_EQ(lr_calls_n, 0, "port mismatch touches nothing");
    FCK_EQ(lr_rumble_toggle[0], 0, "port mismatch leaves the toggle");

    /* new press but no pad */
    vibration_reset();
    FCK_EQ(lr_call_v0((s32)gToggleVibrationPair, 0x80000000u, 0, 0),
           (s32)0x80000000u, "buttons returned with no pad");
    FCK_EQ(lr_calls_n, 2, "sound and pad-type query");
    FCK_CHECK(lr_calls[0].id == LR_SFX && lr_calls[0].a0 == 0x48 &&
              lr_calls[0].a1 == 0, "the press plays sound 0x48 with argument 0");
    FCK_CHECK(lr_calls[1].id == LR_PADTYPE && lr_calls[1].a0 == 0,
              "pad type is queried for port 0");
    FCK_EQ(lr_rumble_toggle[0], 0, "no pad leaves the toggle");

    /* pad present but rumble not ready: the early return skips the toggle */
    vibration_reset();
    lr_padtype_ret = 1;
    lr_ready_ret = 0;
    lr_rumble_toggle[0] = 0;
    FCK_EQ(lr_call_v0((s32)gToggleVibrationPair, 0x80000000u, 0, 0),
           (s32)0x80000000u, "not-ready pad returns the buttons");
    FCK_EQ(lr_rumble_toggle[0], 0, "not-ready pad does not flip the toggle");
    FCK_EQ(lr_calls_n, 3, "sound, type, ready - and nothing else");
    FCK_CHECK(lr_calls[2].id == LR_READY && lr_calls[2].a0 == 0,
              "readiness is queried for port 0");

    /* ready pad, toggle 0 -> 1: motor 1 is pulsed and the saved mode restored */
    vibration_reset();
    lr_padtype_ret = 1;
    lr_ready_ret = 1;
    lr_enabled_ret = 5;
    FCK_EQ(lr_call_v0((s32)gToggleVibrationPair, 0x80000000u, 0, 0),
           (s32)0x80000000u, "the buttons come back unchanged");
    FCK_EQ(lr_rumble_toggle[0], 1, "the toggle flips to 1");
    FCK_EQ(lr_calls_n, 7, "sound, type, ready, read mode, set 1, apply, set saved");
    FCK_CHECK(lr_calls[3].id == LR_ENABLED && lr_calls[3].a0 == 0,
              "the current motor mode is read for port 0");
    FCK_CHECK(lr_calls[4].id == LR_SETEN && lr_calls[4].a0 == 0 &&
              lr_calls[4].a1 == 1, "motor mode 1 is enabled");
    FCK_CHECK(lr_calls[5].id == LR_SMALL && lr_calls[5].a0 == 0 &&
              lr_calls[5].a1 == 1, "small rumble is applied on port 0");
    FCK_CHECK(lr_calls[6].id == LR_SETEN && lr_calls[6].a0 == 0 &&
              lr_calls[6].a1 == 5, "the saved mode 5 is written back");

    /* toggle 1 -> 0: the pulse block is skipped */
    vibration_reset();
    lr_padtype_ret = 1;
    lr_ready_ret = 1;
    lr_rumble_toggle[0] = 1;
    lr_call_v0((s32)gToggleVibrationPair, 0x80000000u, 0, 0);
    FCK_EQ(lr_rumble_toggle[0], 0, "the toggle flips back to 0");
    FCK_EQ(lr_calls_n, 3, "no motor access when switching off");

    /* no new press: bit 31 is already in the previous mask */
    vibration_reset();
    lr_prev_buttons[0] = 0x80000000u;
    lr_padtype_ret = 1;
    lr_ready_ret = 1;
    lr_call_v0((s32)gToggleVibrationPair, 0x80000000u, 0, 0);
    FCK_EQ(lr_calls_n, 0, "a held button is not a new press");
    FCK_EQ(lr_rumble_toggle[0], 0, "a held button leaves the toggle");

    /* a press without the sign bit is not new either (the test is signed) */
    vibration_reset();
    lr_prev_buttons[0] = 0;
    lr_padtype_ret = 1;
    lr_ready_ret = 1;
    lr_call_v0((s32)gToggleVibrationPair, 0x4000, 0, 0);
    FCK_EQ(lr_calls_n, 0, "a press below the sign bit is not seen");

    /* port 1: the toggle and rumble calls use port 1 */
    vibration_reset();
    lr_padtype_ret = 1;
    lr_ready_ret = 1;
    lr_enabled_ret = 0;
    lr_call_v0((s32)gToggleVibrationPair, 0x80000000u, 1, 1);
    FCK_EQ(lr_rumble_toggle[1], 1, "port 1 flips its own toggle");
    FCK_EQ(lr_rumble_toggle[0], 0, "port 0 is untouched");
    FCK_CHECK(lr_calls[1].id == LR_PADTYPE && lr_calls[1].a0 == 1,
              "port 1 maps to pad port 1");
    FCK_CHECK(lr_calls[3].id == LR_ENABLED && lr_calls[3].a0 == 1,
              "the motor mode is read for port 1");

    /* argument 2 is not a port: port = (player == 1), so player 2 queries
     * port 0 and flips toggle[2] - the index uses the argument, not the port. */
    vibration_reset();
    lr_padtype_ret = 1;
    lr_ready_ret = 1;
    lr_call_v0((s32)gToggleVibrationPair, 0x80000000u, 2, 2);
    FCK_CHECK(lr_calls[1].id == LR_PADTYPE && lr_calls[1].a0 == 0,
              "player 2 maps to pad port 0");
    FCK_EQ(lr_rumble_toggle[2], 1, "the toggle index is the argument itself");
}

int main(void)
{
    test_compound_visual_reset();
    test_resupply_proximity();
    test_status_bar();
    test_priority_voice();
    test_vibration_toggle();
    return lk_done("leaf_runtime");
}
