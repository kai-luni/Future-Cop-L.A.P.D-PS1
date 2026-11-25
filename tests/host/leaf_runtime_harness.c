/* leaf_runtime: 14 hardware-free runtime helpers, from packet builders and
 * geometry queries to the heap allocator and the map loaders:
 *
 *   800516A0  compound visual subpart reset
 *   80030498  octagonal-distance resupply trigger
 *   8006C080  priority sound voice allocator (two scenarios)
 *   80046B04  three-slot status bar
 *   8007C940  vibration toggle (fixed `$2` through a trampoline)
 *   8001484C  gouraud disc packet build
 *   80043430  particle emitter setup, 0x1C-stride pool
 *   800458E8  spatial attenuation curve
 *   80056634  player view placement from the focus block
 *   80060838  attack definition / aRSL load
 *   8006A918  vm record registry load
 *   80017EBC  heap allocator (linked for real, see below)
 *   80050B54  path mover vertical motion
 *   80067914  map cell ray test
 *
 * 80017EBC is part of the suite, so gAllocMemory resolves to the real unit and
 * the other tests that allocate drive it through a synthetic free-range table
 * (lr_free_ranges) instead of a stand-in: allocations are then observable as
 * range-end movement plus the four-byte size header, and a failure is staged
 * with gFreeRangeCount = 0 and a failing search.
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
/* @mock sound_mock */
/* func_80046C84 (SE trigger) comes from the shared mock library: the rumble
 * checks read fcm_sound.sfx_* back instead of a private stand-in. */
#include "sound_mock.h"

/* --------------------------------------------------------------- recorder */

#define LR_MAX 64

#define LR_CLIP      1   /* func_800448D8 select animation clip          */
#define LR_REGISTRY  2   /* func_8006AAEC registry find                  */
#define LR_HUDNUM    3   /* func_800491F8 formatted hud number           */
#define LR_ICON      4   /* func_8006BF44 status slot icon               */
#define LR_POP       5   /* func_8004665C pop free sound voice           */
#define LR_RELEASE   6   /* func_80046698 release sound voice            */
#define LR_LINK      7   /* func_80046870 link active sound voice        */
#define LR_PADTYPE   9   /* func_8001EF34 pad type                       */
#define LR_READY     10  /* func_8001EAEC pad rumble ready               */
#define LR_ENABLED   11  /* func_8001EB2C pad rumble enabled             */
#define LR_SETEN     12  /* func_8001EC04 set pad rumble enabled         */
#define LR_SMALL     13  /* func_8001E96C set port small rumble          */
#define LR_VISUAL    14  /* func_80013BD8 bind model slot                */
#define LR_INITMOTION 15 /* func_8003C14C spawn motion init              */
#define LR_DETAIL    16  /* func_80017B88 detail gate                    */
#define LR_ALLOC     17  /* func_80017EBC heap allocation                */
#define LR_MATRIX    18  /* func_800188D0 rotation matrix                */
#define LR_COLORRATE 19  /* func_80043250 emitter colour rates           */
#define LR_EYE       20  /* func_8005612C camera eye                     */
#define LR_VIEWRECORD 21 /* func_80017B60 view record select             */
#define LR_CAMPOS    22  /* func_800177AC camera position                */
#define LR_CAMROT    23  /* func_80017860 camera rotation                */
#define LR_FETCH     24  /* func_80019C38 tagged payload fetch           */
#define LR_FETCH2    25  /* func_80019CA0 tagged reference block         */
#define LR_RESOLVE   26  /* func_80019BF0 resource resolve               */
#define LR_SUBSCALE  27  /* func_800607B8 sub-record scaler              */
#define LR_FREE      28  /* func_800181D4 free block                     */
#define LR_VMSCALE   29  /* func_8006A898 vm record object scaler        */
#define LR_RAW       30  /* func_8001E240 raw heap allocator             */
#define LR_RSEARCH_FWD 31 /* func_80017D5C free-range search from front  */
#define LR_RSEARCH_BACK 32 /* func_80017E0C free-range search from back  */
#define LR_POOLPOP   33  /* func_800195A4 fixed pool pop                 */
#define LR_MEMCOPY   34  /* func_8001E318 word move                      */
#define LR_GROUND    35  /* func_80067684 ground height query            */

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

/* Calls that landed on this callee id. */
static int lr_count(int id)
{
    int i, n = 0;
    for (i = 0; i < lr_calls_n; i++)
        if (lr_calls[i].id == id) n++;
    return n;
}

/* The nth call to this callee id, or a zeroed record. */
static struct LrCall lr_call(int id, int nth)
{
    int i;
    for (i = 0; i < lr_calls_n; i++) {
        if (lr_calls[i].id == id && nth-- == 0) return lr_calls[i];
    }
    {
        struct LrCall none;
        none.id = 0;
        none.a0 = none.a1 = none.a2 = none.a3 = 0;
        return none;
    }
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
    void *parent;           /* +0xB4, stored into the other entity as-is */
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

static s32 lr_padtype_ret;         /* func_8001EF34 */
static u8 lr_ready_ret;            /* func_8001EAEC */
static u8 lr_enabled_ret;          /* func_8001EB2C */

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

/* ======================================================================
 * 8001484C gouraud disc packet emission
 * ====================================================================== */

struct LrVec {
    s16 x, y, z;
};
struct LrPolyG3 {
    u32 color;
    u32 code;
    s16 x0, y0;
    u32 color1;
    s16 x1, y1;
    u32 color2;
    s16 x2, y2;
};

u8 *gPacketCursor __asm__("D_8009FA90");
u8 *volatile gPacketLimit __asm__("D_8009F9C0");
static u8 lr_pkt[0x400];

/* Sine/cosine samples per call; the unit asks for the start angle first and
 * then once per quarter of the fan. */
static s32 lr_sin_tab[8];
static s32 lr_cos_tab[8];
static u32 lr_trig_angle[8];
static int lr_trig_n;

void lr_stub_1846C(u32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");
void lr_stub_1846C(u32 angle, s32 *sine, s32 *cosine)
{
    int i = lr_trig_n < 8 ? lr_trig_n : 7;
    if (lr_trig_n < 8) lr_trig_angle[lr_trig_n] = angle;
    lr_trig_n++;
    *sine = lr_sin_tab[i];
    *cosine = lr_cos_tab[i];
}

void gEmitGouraudDisc(struct LrVec *v, u32 color0, u32 color1, s32 count,
                      s32 scale, s32 offset_z, s32 angle) __asm__("func_8001484C");

/* ======================================================================
 * 80043430 particle emitter setup
 * ====================================================================== */

struct LrEmitter {
    u8 unk00[0x64];
    void *unk64;
    s32 unk68;
    void *unk6C;
    s16 unk70;
    s16 unk72;
    s16 unk74;
    s16 unk76;
    s16 unk78;
    s16 unk7A;
    u16 unk7C;
    u16 unk7E;
    u16 unk80;
    u16 unk82;
    u16 unk84;
    u16 unk86;
    s16 unk88;
    s16 unk8A;
    u8 unk8C;
    u8 unk8D;
    s8 unk8E;
    u8 unk8F;
    u8 unk90;
    u8 unk91;
    u8 unk92;
    u8 unk93;
    u8 unk94;
    u8 unk95;
    s16 unk96;
    s16 unk98;
    s16 unk9A;
    u16 unk9C;
    u16 unk9E;
    u16 unkA0;
    u8 unkA2;
    u8 unkA3;
    u16 unkA4;
    s16 unkA6;
    void *unkA8;
};
struct LrEmitterDef {
    u8 unk00[0x34];
    u16 unk34;
    u16 unk36;
    u16 unk38;
    u16 unk3A;
    u16 unk3C;
    s16 unk3E;
    s16 unk40;
    s16 unk42;
    u16 unk44;
    u16 unk46;
    u16 unk48;
    u16 unk4A;
    u16 unk4C;
    u16 unk4E;
    u8 unk50;
    u8 unk51;
    u8 unk52;
    u8 unk53;
    u8 unk54;
    u8 unk55;
    u8 unk56;
    u8 unk57;
    u16 unk58;
    u8 unk5A;
    u8 unk5B;
    u16 unk5C;
    u8 unk5E;
    u8 unk5F;
};

static s32 lr_detail_mode;                 /* func_80017B88 */
static s32 lr_matrix_out[9];                /* func_800188D0 writes 9 words */

void lr_stub_3C14C(struct LrEmitter *p, struct LrEmitterDef *src) __asm__("func_8003C14C");
void lr_stub_3C14C(struct LrEmitter *p, struct LrEmitterDef *src)
{
    lr_rec(LR_INITMOTION, (s32)p, (s32)src, 0, 0);
}

s32 lr_stub_17B88(void) __asm__("func_80017B88");
s32 lr_stub_17B88(void)
{
    lr_rec(LR_DETAIL, 0, 0, 0, 0);
    return lr_detail_mode;
}

void lr_stub_188D0(s32 x, s32 y, s32 z, s32 *out) __asm__("func_800188D0");
void lr_stub_188D0(s32 x, s32 y, s32 z, s32 *out)
{
    int i;
    lr_rec(LR_MATRIX, x, y, z, (s32)out);
    for (i = 0; i < 9; i++) out[i] = lr_matrix_out[i];
}

void lr_stub_43250(struct LrEmitter *p) __asm__("func_80043250");
void lr_stub_43250(struct LrEmitter *p)
{
    lr_rec(LR_COLORRATE, (s32)p, 0, 0, 0);
}

u8 gSetupParticleEmitter(struct LrEmitter *p, struct LrEmitterDef *src)
    __asm__("func_80043430");

/* ======================================================================
 * 800458E8 spatial sound attenuation
 * ====================================================================== */

struct LrMix {
    s8 volume;
    s8 pan;
    s8 flag;
    s8 pad;
    s32 dx0, dz0, dx1, dz1;
};
struct LrExtra {
    u8 pad[0xAC];
    u8 *data;
};
struct LrExtraData {
    u8 pad[0xC1];
    s8 mode;
};

s32 lr_music_volume __asm__("D_8009F330");
s32 lr_sound_mode_flag __asm__("D_8009F730");
s32 lr_sound_spatial_mode __asm__("D_8009F734");
s32 lr_listener_table[8] __asm__("D_800A1920");

static s32 lr_dist_ret[4];
static int lr_dist_n;
static s32 *lr_dist_ptr[4];
static s32 lr_dist_words[4][2];

s32 lr_stub_1A074(s32 *xz) __asm__("func_8001A074");
s32 lr_stub_1A074(s32 *xz)
{
    int i = lr_dist_n < 4 ? lr_dist_n : 3;
    lr_dist_ptr[i] = xz;
    lr_dist_words[i][0] = xz[0];
    lr_dist_words[i][1] = xz[1];
    lr_dist_n++;
    return lr_dist_ret[i];
}

s32 gComputeSpatialSoundAttenuation(s32 xin, s32 zin, struct LrMix *mix,
                                    struct LrExtra *extra) __asm__("func_800458E8");

/* ======================================================================
 * 80056634 player view placement
 * ====================================================================== */

struct LrVec32 {
    s32 x, y, z;
};
/* 0x14 bytes: f0, f2, f4, then the 12-byte position. */
struct LrEntry {
    s16 f0;
    s16 f2;
    s32 f4;
    struct LrVec32 pos;
};
struct LrInfo {
    u8 unk00[0xA6];
    u8 flagA6;
    u8 flagA7;
    s16 valA8;
    s16 valAA;
    s8 idAC;
    u8 unkAD[0x0F];
    s32 valBC;
};
struct LrObject {
    u8 unk00[0xAC];
    struct LrInfo *info;
};
struct LrPlayer {
    u8 unk00[0x50];
    struct LrVec32 pos;
    u8 unk5C[0x92];
    s16 angle;
    u8 unkF0[0x2E];
    s8 index;
};

static struct LrPlayer lr_focus_block;
/* D_8009F9A0 holds a pointer to the focus block, not the block itself: the
 * unit loads it with lw before reading +0xEE/+0x11E. */
struct LrPlayer *gFocusPositions __asm__("D_8009F9A0") = &lr_focus_block;
struct LrEntry gFormCodeTable[8] __asm__("D_8009ACF4");
struct LrObject *gPlayerEntities[2] __asm__("D_8009FA58");

static struct LrVec32 lr_eye_out;           /* what the eye mock writes */
static s32 lr_eye_args[9];

void lr_stub_5612C(struct LrVec32 *pos, s32 angle, s32 angle2, s32 pitch, s32 dist,
                   struct LrVec32 *src, struct LrVec32 *dst, s32 val, s32 val2)
    __asm__("func_8005612C");
void lr_stub_5612C(struct LrVec32 *pos, s32 angle, s32 angle2, s32 pitch, s32 dist,
                   struct LrVec32 *src, struct LrVec32 *dst, s32 val, s32 val2)
{
    lr_rec(LR_EYE, (s32)pos, angle, angle2, pitch);
    lr_eye_args[0] = (s32)pos;
    lr_eye_args[1] = angle;
    lr_eye_args[2] = angle2;
    lr_eye_args[3] = pitch;
    lr_eye_args[4] = dist;
    lr_eye_args[5] = (s32)src;
    lr_eye_args[6] = (s32)dst;
    lr_eye_args[7] = val;
    lr_eye_args[8] = val2;
    *dst = lr_eye_out;
}

void lr_stub_17B60(s32 id) __asm__("func_80017B60");
void lr_stub_17B60(s32 id)
{
    lr_rec(LR_VIEWRECORD, id, 0, 0, 0);
}

void lr_stub_177AC(struct LrVec32 *p) __asm__("func_800177AC");
void lr_stub_177AC(struct LrVec32 *p)
{
    lr_rec(LR_CAMPOS, (s32)p, p->x, p->y, p->z);
}

void lr_stub_17860(s32 yaw, s32 pitch, s32 roll) __asm__("func_80017860");
void lr_stub_17860(s32 yaw, s32 pitch, s32 roll)
{
    lr_rec(LR_CAMROT, yaw, pitch, roll, 0);
}

void gPlacePlayerViews(void) __asm__("func_80056634");

/* ======================================================================
 * 80060838 attack definition loader
 * ====================================================================== */

struct LrSub607B8 {
    u8 unk00[6];
    s16 unk06;
    s16 unk08;
    u8 unk0A[2];
    s16 unk0C;
};
struct LrDefArg {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};
struct LrDefEntry {
    u8 unk00[8];
    s16 unk08;
    s16 unk0A;
    struct LrSub607B8 sub;
    u8 unk1A[6];
    void *unk20[4];
};

s32 lr_def_count __asm__("D_8009F7F8");
struct LrDefEntry gEffectKeyTable[8] __asm__("D_800A2300");

static u8 lr_payload[0x80];
static u8 lr_refblock[0x40];
static void *lr_ref_returns[4];
static s32 lr_ref_args[4][2];
static int lr_ref_n;
static s32 lr_fetch_args[4][4];
static int lr_fetch_n;

void *lr_stub_19C38(u32 a0, u32 a1, u32 a2, u32 a3) __asm__("func_80019C38");
void *lr_stub_19C38(u32 a0, u32 a1, u32 a2, u32 a3)
{
    int i = lr_fetch_n < 4 ? lr_fetch_n : 3;
    if (lr_fetch_n < 4) {
        lr_fetch_args[i][0] = (s32)a0;
        lr_fetch_args[i][1] = (s32)a1;
        lr_fetch_args[i][2] = (s32)a2;
        lr_fetch_args[i][3] = (s32)a3;
    }
    lr_fetch_n++;
    lr_rec(LR_FETCH, (s32)a0, (s32)a1, (s32)a2, (s32)a3);
    return lr_payload;
}

void *lr_stub_19CA0(u32 a0, u32 a1, u32 a2, u32 a3) __asm__("func_80019CA0");
void *lr_stub_19CA0(u32 a0, u32 a1, u32 a2, u32 a3)
{
    lr_rec(LR_FETCH2, (s32)a0, (s32)a1, (s32)a2, (s32)a3);
    return lr_refblock;
}

void *lr_stub_19BF0(s32 group, s32 name) __asm__("func_80019BF0");
void *lr_stub_19BF0(s32 group, s32 name)
{
    int i = lr_ref_n < 4 ? lr_ref_n : 3;
    if (lr_ref_n < 4) {
        lr_ref_args[i][0] = group;
        lr_ref_args[i][1] = name;
    }
    lr_ref_n++;
    lr_rec(LR_RESOLVE, group, name, 0, 0);
    return lr_ref_returns[i];
}

void lr_stub_607B8(struct LrSub607B8 *sub) __asm__("func_800607B8");
void lr_stub_607B8(struct LrSub607B8 *sub)
{
    lr_rec(LR_SUBSCALE, (s32)sub, sub->unk06, sub->unk08, sub->unk0C);
    sub->unk06 += 1;                /* visible marker, not a re-implementation */
}

void lr_stub_181D4(struct LrDefArg *arg0) __asm__("func_800181D4");
void lr_stub_181D4(struct LrDefArg *arg0)
{
    lr_rec(LR_FREE, (s32)arg0, 0, 0, 0);
}

void gLoadAttackDefinition(struct LrDefArg *arg0) __asm__("func_80060838");

/* ======================================================================
 * 8006A918 vm record registry loader
 * ====================================================================== */

struct LrVmObject {
    u8 unk00[8];
    s16 unk08;
    s16 unk0A;
    struct LrSub607B8 sub;
    u8 unk1A[6];
};
struct LrVmRecord {
    u32 unk00;
    struct LrVmObject obj1;
    struct LrVmObject obj2;
    void *unk44[4];
};

/* One object area for all six records; D_800A3344 is the object base, four
 * bytes into the record table, exactly as the unit computes it. */
__asm__(".data\n"
        "\t.globl D_800A3340\n"
        "D_800A3340:\n"
        "\t.space 0x400\n"
        "\t.globl D_800A3344\n"
        "D_800A3344 = D_800A3340 + 4\n"
        "\t.text\n");

s32 lr_vm_count __asm__("D_8009F848");
extern u8 lr_vm_area[] __asm__("D_800A3340");

void lr_stub_6A898(struct LrVmObject *obj) __asm__("func_8006A898");
void lr_stub_6A898(struct LrVmObject *obj)
{
    lr_rec(LR_VMSCALE, (s32)obj, obj->unk08, obj->unk0A, 0);
    obj->unk08 += 1;
}

void gRegistryLoad(struct LrDefArg *arg0) __asm__("func_8006A918");


/* ======================================================================
 * 80017EBC heap allocator
 * ====================================================================== */

struct LrLinkState {
    s32 stride;
    u8 *end;
    s32 unk08;
    s32 count;
    s32 *link;
};

static s32 lr_free_ranges[8];
s32 *gFreeRangeTablePtr __asm__("D_8009F590") = lr_free_ranges;
s32 gFreeRangeCount __asm__("D_8009F594");
s32 gHeapSearchCursor __asm__("D_8009F598");
struct LrLinkState *gFixedPool3 __asm__("D_8009FA4C");
struct LrLinkState *gFixedPool2 __asm__("D_8009FAAC");
struct LrLinkState *gFixedPool1 __asm__("D_8009F9D0");

static u8 lr_heap[0x8000];
static void *lr_raw_result;
static s32 lr_fwd_ret;
static s32 lr_back_ret;
static s32 lr_search_size[4];
static int lr_search_n;
static void *lr_pop_result;
static s32 lr_memcopy_args[4];

void *lr_stub_1E240(s32 size, s32 mode) __asm__("func_8001E240");
void *lr_stub_1E240(s32 size, s32 mode)
{
    lr_rec(LR_RAW, size, mode, 0, 0);
    return lr_raw_result;
}

s32 lr_stub_17D5C(s32 size) __asm__("func_80017D5C");
s32 lr_stub_17D5C(s32 size)
{
    lr_rec(LR_RSEARCH_FWD, size, 0, 0, 0);
    if (lr_search_n < 4) lr_search_size[lr_search_n] = size;
    lr_search_n++;
    return lr_fwd_ret;
}

s32 lr_stub_17E0C(s32 size) __asm__("func_80017E0C");
s32 lr_stub_17E0C(s32 size)
{
    lr_rec(LR_RSEARCH_BACK, size, 0, 0, 0);
    if (lr_search_n < 4) lr_search_size[lr_search_n] = size;
    lr_search_n++;
    return lr_back_ret;
}

struct LinkNode;
void *lr_stub_195A4(struct LrLinkState *state) __asm__("func_800195A4");
void *lr_stub_195A4(struct LrLinkState *state)
{
    lr_rec(LR_POOLPOP, (s32)state, 0, 0, 0);
    return lr_pop_result;
}

u8 *lr_stub_1E318(u8 *dst, u8 *src, s32 count) __asm__("func_8001E318");
u8 *lr_stub_1E318(u8 *dst, u8 *src, s32 count)
{
    s32 i;
    lr_rec(LR_MEMCOPY, (s32)dst, (s32)src, count, 0);
    lr_memcopy_args[0] = (s32)dst;
    lr_memcopy_args[1] = (s32)src;
    lr_memcopy_args[2] = count;
    for (i = 0; i < count; i++) dst[i] = src[i];
    return dst;
}

void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");

/* ======================================================================
 * 80050B54 path mover vertical motion
 * ====================================================================== */

struct LrObj {
    u8 pad0[6];
    s16 y;
    u8 pad8[4];
    s16 scale;
    u8 pade[14];
    u32 value;
    s16 divisor;
    u8 pad22[4];
    s16 offset;
    u8 pad28[8];
    s16 speed;
    s16 current;
    u8 pad34[8];
    u32 flags;
    s16 timer;
};
struct LrStepEntry {
    u8 pad0[2];
    s16 a;
    u8 pad4[4];
    s16 b;
    u8 padA[6];
};
struct LrChild {
    u8 pad0[0x34];
    struct LrStepEntry *entries;
};
struct LrState {
    u8 pad0[0x28];
    struct LrChild *child;
    u8 pad2c[0x24];
    u8 pos[4];
    s32 height;
    u8 pad58[0x18];
    s16 index;
    u8 pad72[0x12];
    s32 changed;
};

static s32 lr_ground_height;
static s32 lr_ground_pos[1];
static s32 lr_ground_arg;

s32 lr_stub_67684(void *pos, s32 arg) __asm__("func_80067684");
s32 lr_stub_67684(void *pos, s32 arg)
{
    lr_rec(LR_GROUND, (s32)pos, arg, 0, 0);
    lr_ground_pos[0] = (s32)pos;
    lr_ground_arg = arg;
    return lr_ground_height;
}

s32 gStepPathMoverVerticalMotion(struct LrObj *obj, s32 delta, struct LrState *state)
    __asm__("func_80050B54");

/* ======================================================================
 * 80067914 map cell ray test
 * ====================================================================== */

struct LrCellVec {
    s32 x, y, z;
};

static s32 lr_map_header[0x40];
s32 *gMapGridHeader __asm__("D_8009F80C") = lr_map_header;
static u8 *lr_tile_table[4];
u8 **gTileBlockTable __asm__("D_8009F808") = lr_tile_table;
u8 gTileShapeTable[256] __asm__("D_8009B1A4");
u8 gTileCornerIndices[128][4] __asm__("D_8009AFE8");
struct LrCellVec gCellRayStart __asm__("D_800A2600");
struct LrCellVec gCellRayEnd __asm__("D_800A2610");
struct LrCellVec gCellRayWallHit __asm__("D_800A2620");
struct LrCellVec gWallHitPositions[8] __asm__("D_800A32A0");
u32 gWallHitPolys[8] __asm__("D_800A3260");
s32 gWallHitKinds[8] __asm__("D_800A3280");
s32 gWallHitTopHeights[8] __asm__("D_800A3300");

static u8 lr_tile[0x800] __attribute__((aligned(16)));

s32 gTestMapCellRay(struct LrCellVec *a, struct LrCellVec *b, s32 *outA, s32 *outB,
                    s32 flag) __asm__("func_80067914");

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
    fcm_sound_reset();
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
    FCK_EQ(lr_calls_n, 1, "only the pad-type query goes through the recorder");
    FCK_EQ(fcm_sound.sfx_calls, 1, "the press plays one sound effect");
    FCK_EQ(fcm_sound.sfx_last_id, 0x48, "the press plays sound 0x48");
    FCK_EQ(fcm_sound.sfx_last_arg1, 0, "the sound effect gets argument 0");
    FCK_CHECK(lr_calls[0].id == LR_PADTYPE && lr_calls[0].a0 == 0,
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
    FCK_EQ(lr_calls_n, 2, "type and ready - and nothing else");
    FCK_CHECK(lr_calls[1].id == LR_READY && lr_calls[1].a0 == 0,
              "readiness is queried for port 0");

    /* ready pad, toggle 0 -> 1: motor 1 is pulsed and the saved mode restored */
    vibration_reset();
    lr_padtype_ret = 1;
    lr_ready_ret = 1;
    lr_enabled_ret = 5;
    FCK_EQ(lr_call_v0((s32)gToggleVibrationPair, 0x80000000u, 0, 0),
           (s32)0x80000000u, "the buttons come back unchanged");
    FCK_EQ(lr_rumble_toggle[0], 1, "the toggle flips to 1");
    FCK_EQ(lr_calls_n, 6, "type, ready, read mode, set 1, apply, set saved");
    FCK_CHECK(lr_calls[2].id == LR_ENABLED && lr_calls[2].a0 == 0,
              "the current motor mode is read for port 0");
    FCK_CHECK(lr_calls[3].id == LR_SETEN && lr_calls[3].a0 == 0 &&
              lr_calls[3].a1 == 1, "motor mode 1 is enabled");
    FCK_CHECK(lr_calls[4].id == LR_SMALL && lr_calls[4].a0 == 0 &&
              lr_calls[4].a1 == 1, "small rumble is applied on port 0");
    FCK_CHECK(lr_calls[5].id == LR_SETEN && lr_calls[5].a0 == 0 &&
              lr_calls[5].a1 == 5, "the saved mode 5 is written back");
    FCK_EQ(fcm_sound.sfx_calls, 1, "the flip plays one sound effect");

    /* toggle 1 -> 0: the pulse block is skipped */
    vibration_reset();
    lr_padtype_ret = 1;
    lr_ready_ret = 1;
    lr_rumble_toggle[0] = 1;
    lr_call_v0((s32)gToggleVibrationPair, 0x80000000u, 0, 0);
    FCK_EQ(lr_rumble_toggle[0], 0, "the toggle flips back to 0");
    FCK_EQ(lr_calls_n, 2, "no motor access when switching off");

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
    FCK_CHECK(lr_calls[0].id == LR_PADTYPE && lr_calls[0].a0 == 1,
              "port 1 maps to pad port 1");
    FCK_CHECK(lr_calls[2].id == LR_ENABLED && lr_calls[2].a0 == 1,
              "the motor mode is read for port 1");

    /* argument 2 is not a port: port = (player == 1), so player 2 queries
     * port 0 and flips toggle[2] - the index uses the argument, not the port. */
    vibration_reset();
    lr_padtype_ret = 1;
    lr_ready_ret = 1;
    lr_call_v0((s32)gToggleVibrationPair, 0x80000000u, 2, 2);
    FCK_CHECK(lr_calls[0].id == LR_PADTYPE && lr_calls[0].a0 == 0,
              "player 2 maps to pad port 0");
    FCK_EQ(lr_rumble_toggle[2], 1, "the toggle index is the argument itself");
}

/* ======================================================================
 * 8001484C gouraud disc
 * ====================================================================== */

static void pkt_reset(void)
{
    lk_fill(lr_pkt, 0xEE, sizeof lr_pkt);
    gPacketCursor = lr_pkt;
    gPacketLimit = lr_pkt + sizeof lr_pkt;
    lr_trig_n = 0;
}

/* Three trig samples make two fan quarters distinguishable: the start pair is
 * (0, 1.0), then (0.5, 0.75) and (0.25, 0.875) in 16.16. */
static void trig_plan(void)
{
    lr_sin_tab[0] = 0x0000; lr_cos_tab[0] = 0x10000;
    lr_sin_tab[1] = 0x8000; lr_cos_tab[1] = 0xC000;
    lr_sin_tab[2] = 0x4000; lr_cos_tab[2] = 0xE000;
    lr_trig_n = 0;
}

#define PKT(i, field) (*(s16 *)(lr_pkt + (i) * 0x1C + (field)))

static void test_gouraud_disc(void)
{
    static struct LrVec v;

    /* @leaf 8001484C spec psx-spx GPU/DMA + Psy-Q libgpu POLY_G3 (0x1C bytes): tag high byte = data-word count (0x01 = 1 word for GP0(E1h) draw mode 0xE10006A0, 0x06 = 6 words for GP0(32h) semi-transparent shaded triangle with colour0/xy0/colour1/xy1/colour2/xy2) and low 24 bits = DMA linked-list address field (0xFFFFFF is end-of-list); source (src/game/8001484C.c): stores depth-dz in the low 24-bit tag field, plus the fan walk and segment count */
    LK_FUNC("func_8001484C");

    /* depth 0x100 with offset 0x10 -> 0xF0; scale 4 -> (4 << 9) / 0x100 = 8
     * segments, count 8 does not clamp, angle 0x1200 -> hi 0x12, step 0x4000 */
    pkt_reset();
    trig_plan();
    v.x = 0x100;
    v.y = 0x200;
    v.z = 0x100;
    gEmitGouraudDisc(&v, 0x00112233, 0x00445566, 8, 4, 0x10, 0x1200);

    FCK_EQ(*(u32 *)(lr_pkt + 0x00), 0x010000F0u, "texpage tag: 1 data word + depth-dz in low 24 bits");
    FCK_EQ(*(u32 *)(lr_pkt + 0x04), 0xE10006A0u, "texpage code");
    FCK_EQ(*(u32 *)(lr_pkt + 0x1C + 0x00), 0x060000F0u, "fan tag: 6 data words + depth-dz in low 24 bits");
    FCK_EQ(*(u32 *)(lr_pkt + 0x1C + 0x04), 0x32001122u, "fan code = 0x32000000 | color0 >> 8");
    FCK_EQ(*(u32 *)(lr_pkt + 0x1C + 0x0C), 0x00445566u >> 8, "first rim colour = color1 >> 8");
    FCK_EQ(*(u32 *)(lr_pkt + 0x1C + 0x14), 0x00445566u >> 8, "second rim colour = color1 >> 8");
    FCK_EQ(*(s16 *)(lr_pkt + 2 * 0x1C + 0x08), 0x100, "every fan packet repeats the centre x");
    FCK_EQ(*(s16 *)(lr_pkt + 8 * 0x1C + 0x0A), 0x200, "every fan packet repeats the centre y");
    FCK_EQ((s32)(gPacketCursor - lr_pkt), 9 * 0x1C, "nine packets came from the cursor");

    /* Start sample (0, 8 after the >>16 scale) and quarter sample (4, 6):
     * the four rim points of the first quarter are the four sign combinations
     * of (cos, sin) around the centre, each on its own packet. */
    FCK_EQ(*(s16 *)(lr_pkt + 1 * 0x1C + 0x10), 0x108, "pkt1 x1 = +cos");
    FCK_EQ(*(s16 *)(lr_pkt + 1 * 0x1C + 0x12), 0x200, "pkt1 y1 = +sin");
    FCK_EQ(*(s16 *)(lr_pkt + 3 * 0x1C + 0x10), 0x100, "pkt3 x1 = -sin");
    FCK_EQ(*(s16 *)(lr_pkt + 3 * 0x1C + 0x12), 0x208, "pkt3 y1 = +cos");
    FCK_EQ(*(s16 *)(lr_pkt + 5 * 0x1C + 0x10), 0x0F8, "pkt5 x1 = -cos");
    FCK_EQ(*(s16 *)(lr_pkt + 5 * 0x1C + 0x12), 0x200, "pkt5 y1 = -sin");
    FCK_EQ(*(s16 *)(lr_pkt + 7 * 0x1C + 0x10), 0x100, "pkt7 x1 = +sin");
    FCK_EQ(*(s16 *)(lr_pkt + 7 * 0x1C + 0x12), 0x1F8, "pkt7 y1 = -cos");
    FCK_EQ(*(s16 *)(lr_pkt + 1 * 0x1C + 0x18), 0x106, "pkt1 x2 uses the quarter sample cos");
    FCK_EQ(*(s16 *)(lr_pkt + 1 * 0x1C + 0x1A), 0x204, "pkt1 y2 uses the quarter sample sin");
    FCK_EQ(*(s16 *)(lr_pkt + 3 * 0x1C + 0x18), 0x0FC, "pkt3 x2 = -sin");
    FCK_EQ(*(s16 *)(lr_pkt + 5 * 0x1C + 0x18), 0x0FA, "pkt5 x2 = -cos");
    FCK_EQ(*(s16 *)(lr_pkt + 7 * 0x1C + 0x18), 0x104, "pkt7 x2 = +sin");

    /* Second quarter: the previous quarter sample becomes the new start. */
    FCK_EQ(*(s16 *)(lr_pkt + 2 * 0x1C + 0x10), 0x106, "pkt2 x1 = previous cos");
    FCK_EQ(*(s16 *)(lr_pkt + 4 * 0x1C + 0x10), 0x0FC, "pkt4 x1 = -previous sin");
    FCK_EQ(*(s16 *)(lr_pkt + 6 * 0x1C + 0x10), 0x0FA, "pkt6 x1 = -previous cos");
    FCK_EQ(*(s16 *)(lr_pkt + 8 * 0x1C + 0x10), 0x104, "pkt8 x1 = +previous sin");
    FCK_EQ(*(s16 *)(lr_pkt + 8 * 0x1C + 0x18), 0x102, "pkt8 x2 from the last sample");
    FCK_EQ(*(s16 *)(lr_pkt + 8 * 0x1C + 0x1A), 0x1F9, "pkt8 y2 from the last sample");

    FCK_EQ(lr_trig_n, 3, "one start sample and one per quarter");
    FCK_EQ(lr_trig_angle[0], 0x12, "start sample uses the angle high byte");
    FCK_EQ(lr_trig_angle[1], 0x12 + 0x200, "quarter 1 angle = hi + step/32");
    FCK_EQ(lr_trig_angle[2], 0x12 + 0x400, "quarter 2 angle = hi + 2*step/32");

    /* A count above the segment estimate is clamped down to the estimate. */
    pkt_reset();
    trig_plan();
    gEmitGouraudDisc(&v, 0, 0, 100, 4, 0x10, 0x1200);
    FCK_EQ((s32)(gPacketCursor - lr_pkt), 9 * 0x1C, "100 is clamped to the 8 segments");

    /* A count below the estimate wins: four segments and one quarter. */
    pkt_reset();
    trig_plan();
    gEmitGouraudDisc(&v, 0, 0, 4, 4, 0x10, 0x1200);
    FCK_EQ((s32)(gPacketCursor - lr_pkt), 5 * 0x1C, "four segments plus the texpage");
    FCK_EQ(*(s16 *)(lr_pkt + 2 * 0x1C + 0x10), 0x100, "pkt2 x1 = -sin for fp 4");
    FCK_EQ(*(s16 *)(lr_pkt + 2 * 0x1C + 0x12), 0x208, "pkt2 y1 = +cos for fp 4");
    FCK_EQ(*(s16 *)(lr_pkt + 4 * 0x1C + 0x10), 0x100, "pkt4 x1 = +sin for fp 4");
    FCK_EQ(*(s16 *)(lr_pkt + 4 * 0x1C + 0x1A), 0x1FA, "pkt4 y2 from the only quarter");
    FCK_EQ(lr_trig_n, 2, "fp 4 has a single quarter");

    /* depth - offset <= 0: nothing is emitted, not even the texpage. */
    pkt_reset();
    trig_plan();
    v.z = 0x10;
    gEmitGouraudDisc(&v, 0, 0, 8, 4, 0x10, 0x1200);
    FCK_EQ((s32)(gPacketCursor - lr_pkt), 0, "zero depth returns before any packet");
    FCK_EQ(lr_trig_n, 0, "zero depth samples no angle");
    FCK_CHECK(*(u32 *)lr_pkt == 0xEEEEEEEEu, "the packet buffer is untouched");
    v.z = 0x0F;
    gEmitGouraudDisc(&v, 0, 0, 8, 4, 0x10, 0x1200);
    FCK_EQ((s32)(gPacketCursor - lr_pkt), 0, "negative depth returns as well");

    /* Cursor at the limit: the packets are written in place and the cursor is
     * not advanced, so the last corner written is the one that survives. */
    pkt_reset();
    trig_plan();
    v.z = 0x100;
    gPacketLimit = lr_pkt;
    gEmitGouraudDisc(&v, 0, 0, 8, 4, 0x10, 0x1200);
    FCK_EQ((s32)(gPacketCursor - lr_pkt), 0, "a full buffer does not move the cursor");
    FCK_EQ(*(u32 *)(lr_pkt + 0x04), 0x32000000u, "the last code written wins in place");
    FCK_EQ(*(s16 *)(lr_pkt + 0x10), 0x104, "the last x1 written wins");
    FCK_EQ(*(s16 *)(lr_pkt + 0x18), 0x102, "the last x2 written wins");
}

/* ======================================================================
 * 80043430 particle emitter setup
 * ====================================================================== */

static void emitter_reset(struct LrEmitter *p, struct LrEmitterDef *src)
{
    int i;
    lk_fill((u8 *)p, 0xEE, sizeof *p);
    lk_fill((u8 *)src, 0x00, sizeof *src);
    lr_reset();
    /* one free range for the real allocator: mode 1 carves from its end */
    lk_fill(lr_heap, 0x00, sizeof lr_heap);
    lr_free_ranges[0] = (s32)lr_heap;
    lr_free_ranges[1] = (s32)(lr_heap + 0x1000);
    lr_free_ranges[2] = 0;
    lr_free_ranges[3] = 0;
    gFreeRangeCount = 2;
    gHeapSearchCursor = -1;
    lr_back_ret = 0;
    lr_raw_result = 0;
    gFixedPool1 = 0;
    gFixedPool2 = 0;
    gFixedPool3 = 0;
    lr_detail_mode = 0;
    for (i = 0; i < 9; i++) lr_matrix_out[i] = i + 1;
    lr_matrix_out[6] = 0x1000;
    lr_matrix_out[7] = 0x2000;
    lr_matrix_out[8] = -0x1000;
    src->unk34 = 20;
    src->unk36 = 10;
    src->unk38 = 0x1111;
    src->unk3A = 0x2222;
    src->unk3C = 50;
    src->unk3E = 0x40;
    src->unk40 = 0x10;
    src->unk42 = 0x100;
    src->unk44 = 0x3333;
    src->unk46 = 0x4444;
    src->unk48 = 0x5555;
    src->unk4A = 0x6666;
    src->unk4C = 0x7777;
    src->unk4E = 0x8888;
    src->unk50 = 0x08;
    src->unk51 = 0xA1;
    src->unk52 = 0xA2;
    src->unk53 = 0xA3;
    src->unk54 = 0xA4;
    src->unk55 = 0xA5;
    src->unk56 = 0xA6;
    src->unk58 = 0xFFF6;        /* -10 */
    src->unk5A = 5;
    src->unk5B = 6;
    src->unk5C = 0x9999;
    src->unk5E = 0x3F;
    src->unk5F = 0x2C;
}

static void test_particle_emitter(void)
{
    static struct LrEmitter p;
    static struct LrEmitterDef src;
    u8 *node;
    int i;

    /* @leaf 80043430 semantic clamping and pool layout from src/game/80043430.c: unk36 -1 maps to unk34 and caps at 700, unk34 never exceeds unk36, detail mode 1 halves both when unk34 >= 11; +0x8A/+0x88 copies of those counts, 0x1C-stride free list, unk72 = (-unk72 << 12)/100, matrix[6..8] drive +0x76..+0x7A, unk8E = ((s8)unk50) >> 1 limited to [-7,7] with 0 -> 1, unkA6 = |unk58| */
    LK_FUNC("func_80043430");

    emitter_reset(&p, &src);
    FCK_EQ(gSetupParticleEmitter(&p, &src), 1, "a valid definition succeeds");
    FCK_EQ(src.unk34, 10, "unk34 is pulled down to unk36");
    FCK_EQ(p.unk8A, 10, "+0x8A is the clamped count");
    FCK_EQ(p.unk88, 10, "+0x88 is the clamped definition count");
    FCK_EQ(p.unk7C, 0x3333, "+0x7C = src unk44");
    FCK_EQ(p.unk80, 0x1111, "+0x80 = src unk38");
    FCK_EQ(p.unk82, 0x2222, "+0x82 = src unk3A");
    FCK_EQ(p.unk7E, 0x4444, "+0x7E = src unk46");
    FCK_EQ(p.unk9C, 0x5555, "+0x9C = src unk48");
    FCK_EQ(p.unk9E, 0x6666, "+0x9E = src unk4A");
    FCK_EQ(p.unk84, 0x7777, "+0x84 = src unk4C");
    FCK_EQ(p.unk86, 0x8888, "+0x86 = src unk4E");
    FCK_EQ(p.unk8C, 5, "+0x8C = src unk5A");
    FCK_EQ(p.unk8D, 6, "+0x8D = src unk5B");
    FCK_EQ(p.unk8F, 0xA1, "+0x8F = src unk51");
    FCK_EQ(p.unk93, 0xA5, "+0x93 = src unk55");
    FCK_EQ(p.unkA0, 0x9999, "+0xA0 = src unk5C");
    FCK_EQ(p.unkA2, 0x0F, "+0xA2 = src unk5E & 0xF");
    FCK_EQ(p.unkA3, 0x0C, "+0xA3 = src unk5F & 0xF");
    FCK_EQ(p.unkA6, 10, "+0xA6 = |unk58| of -10");
    FCK_EQ(p.unk8E, 4, "+0x8E = (s8)unk50 >> 1");
    FCK_EQ(p.unk72, -2048, "+0x72 = (-50 << 12) / 100");
    FCK_EQ(p.unk76, 16, "+0x76 from matrix[6] >> 4");
    FCK_EQ(p.unk78, 32, "+0x78 from matrix[7] >> 4");
    FCK_EQ(p.unk7A, -16, "+0x7A from the sign of matrix[8] >> 4");
    FCK_EQ(p.unk68, 0, "+0x68 is cleared after the pool link");
    FCK_EQ(p.unkA4, 0, "+0xA4 is cleared");
    FCK_EQ(p.unk74, 0, "+0x74 is cleared");
    FCK_EQ(p.unk70, 0, "+0x70 is cleared");
    FCK_CHECK(p.unk6C == lr_heap + 0xEE8 && p.unk64 == lr_heap + 0xEE8,
              "the pool pointer and its free-list head are the allocation");
    FCK_EQ(*(s32 *)(lr_heap + 0xEE4), 0x11C, "the pool header holds 10 * 0x1C aligned");
    FCK_EQ(lr_free_ranges[1], (s32)(lr_heap + 0xEE4), "the range end moved down by the pool");
    FCK_CHECK(p.unkA8 == 0, "equal counts allocate no secondary buffer");

    node = (u8 *)p.unk6C;
    for (i = 0; i < 9; i++) {
        if (*(u8 **)(node + i * 0x1C) != node + (i + 1) * 0x1C) break;
    }
    FCK_EQ(i, 9, "the first nine nodes link to their successor");
    FCK_CHECK(*(u8 **)(node + 9 * 0x1C) == 0, "the last node terminates the list");

    FCK_EQ(lr_count(LR_POOLPOP), 0, "no fixed pool is used");
    /* order: motion init, detail gate, rotation matrix, allocation, colours */
    FCK_CHECK(lr_calls_n == 5, "five callees");
    FCK_CHECK(lr_calls[0].id == LR_INITMOTION && lr_calls[0].a0 == (s32)&p &&
              lr_calls[0].a1 == (s32)&src, "motion init runs first on both blocks");
    FCK_CHECK(lr_calls[1].id == LR_DETAIL, "the detail gate is asked once");
    FCK_CHECK(lr_calls[2].id == LR_MATRIX && lr_calls[2].a0 == 0x40 &&
              lr_calls[2].a1 == -0x10 && lr_calls[2].a2 == 0,
              "the rotation matrix uses (unk3E, -unk40, 0)");
    FCK_CHECK(lr_calls[3].id == LR_RSEARCH_BACK && lr_calls[3].a0 == 0x11C,
              "the pool is allocated from the range end");
    FCK_CHECK(lr_calls[4].id == LR_COLORRATE && lr_calls[4].a0 == (s32)&p,
              "the colour rates are computed after the link");

    /* -1 means "same as unk34" */
    emitter_reset(&p, &src);
    src.unk36 = 0xFFFF;
    src.unk34 = 33;
    FCK_EQ(gSetupParticleEmitter(&p, &src), 1, "unk36 -1 succeeds");
    FCK_EQ(src.unk36, 33, "unk36 takes unk34");
    FCK_EQ(p.unk8A, 33, "the pool takes the copied count");
    FCK_EQ(p.unk88, 33, "both counts match, no secondary pool");

    /* the 700 cap pulls unk36 down before unk34 is clamped to it; 700 nodes
     * at 0x1C need 0x4C90 bytes, so this case gets a wider free range. */
    emitter_reset(&p, &src);
    lr_free_ranges[1] = (s32)(lr_heap + 0x8000);
    src.unk36 = 0x1000;
    src.unk34 = 800;
    FCK_EQ(gSetupParticleEmitter(&p, &src), 1, "an oversized count still succeeds");
    FCK_EQ(src.unk36, 700, "unk36 is capped at 0x2BC");
    FCK_EQ(src.unk34, 700, "unk34 is pulled to the cap too");
    FCK_EQ(p.unk8A, 700, "the pool follows the cap");
    FCK_EQ(*(s32 *)(lr_heap + 0x336C), 0x4C94, "the capped pool header holds (700 * 0x1C + 7) & ~3");
    FCK_CHECK(p.unk6C == lr_heap + 0x3370, "the capped pool starts at the range end");
    FCK_CHECK(*(u8 **)((u8 *)p.unk6C + 699 * 0x1C) == 0, "the 700th node ends the list");

    /* zero/negative counts fail before any setup beyond motion init */
    emitter_reset(&p, &src);
    src.unk36 = 0;
    FCK_EQ(gSetupParticleEmitter(&p, &src), 0, "a zero count fails");
    FCK_EQ(lr_calls_n, 1, "only the motion init ran");
    FCK_CHECK(p.unk6C == (void *)0xEEEEEEEE, "a failure leaves the pool pointer");

    emitter_reset(&p, &src);
    src.unk36 = 0xFFFE;        /* -2 */
    FCK_EQ(gSetupParticleEmitter(&p, &src), 0, "a negative count fails");
    FCK_EQ(lr_free_ranges[1], (s32)(lr_heap + 0x1000), "a failure leaves the range alone");

    /* detail mode halves both counts once unk34 reaches 11 */
    emitter_reset(&p, &src);
    src.unk34 = 20;
    src.unk36 = 20;
    lr_detail_mode = 1;
    FCK_EQ(gSetupParticleEmitter(&p, &src), 1, "detail mode succeeds");
    FCK_EQ(src.unk34, 10, "unk34 is halved");
    FCK_EQ(src.unk36, 10, "unk36 is halved");
    FCK_EQ(p.unk8A, 10, "the pool takes the halved count");

    /* 10 is below the halving threshold */
    emitter_reset(&p, &src);
    src.unk34 = 10;
    src.unk36 = 10;
    lr_detail_mode = 1;
    FCK_EQ(gSetupParticleEmitter(&p, &src), 1, "10 is not halved");
    FCK_EQ(src.unk34, 10, "unk34 stays at 10");

    /* a failed pool allocation aborts before the colour rates */
    emitter_reset(&p, &src);
    gFreeRangeCount = 0;            /* no range and no raw fallback */
    lr_back_ret = -1;
    FCK_EQ(gSetupParticleEmitter(&p, &src), 0, "a failed allocation fails the setup");
    FCK_EQ(lr_calls_n, 5, "the colour rates are not reached");
    FCK_EQ(lr_calls[4].id, LR_RAW, "the failed search falls back to the raw allocator");
    FCK_CHECK(p.unk6C == 0, "the failed pool pointer is null");

    /* a smaller unk34 adds the optional secondary pool */
    emitter_reset(&p, &src);
    src.unk34 = 5;
    src.unk36 = 10;
    FCK_EQ(gSetupParticleEmitter(&p, &src), 1, "a smaller unk34 succeeds");
    FCK_EQ(*(s32 *)(lr_heap + 0xE54), 0x90, "the secondary pool header holds (5 * 0x1C + 7) & ~3");
    FCK_CHECK(p.unkA8 == lr_heap + 0xE58, "the secondary pool pointer is stored");
}

/* ======================================================================
 * 800458E8 spatial sound attenuation
 * ====================================================================== */

static void mix_reset(struct LrMix *mix)
{
    lk_fill((u8 *)mix, 0x55, sizeof *mix);
    lr_dist_n = 0;
    lr_dist_ret[0] = lr_dist_ret[1] = lr_dist_ret[2] = lr_dist_ret[3] = 0;
}

static void test_spatial_attenuation(void)
{
    static struct LrMix mix;
    static struct LrExtra extra;
    static struct LrExtraData data;
    s32 r;

    /* @leaf 800458E8 math linear attenuation volume - ((distance * volume) >> 17) over 0x20000 with a 0x1FFFF cut-off, the louder of the two listeners in the two-listener mode, centre pan 64 and the flag/pan split from D_8009F730, plus the low-byte volume read that src/game/800458E8.c performs through a u8 pointer */
    LK_FUNC("func_800458E8");

    lr_listener_table[0] = 0x1000;
    lr_listener_table[2] = 0x2000;
    lr_listener_table[4] = 0x3000;
    lr_listener_table[6] = 0x4000;
    lr_music_volume = 100;
    lr_sound_mode_flag = 0;
    lr_sound_spatial_mode = 1;

    /* one listener, half the range: 128 - (0x10000 * 128 >> 17) = 64 */
    mix_reset(&mix);
    lr_dist_ret[0] = 0x10000;
    r = gComputeSpatialSoundAttenuation(0x1500, 0x2800, &mix, 0);
    FCK_EQ(r, 1, "an in-range sound is audible");
    FCK_EQ(mix.volume, 50, "volume is linearly attenuated over 0x20000");
    FCK_EQ(mix.pan, 64, "centre pan with the mode flag clear");
    FCK_EQ(mix.flag, 1, "amplitude flag set with the mode flag clear");
    FCK_EQ(mix.dx0, 0x500, "dx0 = x - listener 0 x");
    FCK_EQ(mix.dz0, 0x800, "dz0 = z - listener 0 z");
    FCK_EQ(lr_dist_n, 1, "one distance query");
    FCK_CHECK(lr_dist_ptr[0] == &mix.dx0, "the distance query sees the delta pair");
    FCK_EQ(lr_dist_words[0][0], 0x500, "the query reads dx0");
    FCK_EQ(lr_dist_words[0][1], 0x800, "the query reads dz0");

    /* the mode flag moves the result from pan to flag */
    mix_reset(&mix);
    lr_dist_ret[0] = 0x10000;
    lr_sound_mode_flag = 1;
    gComputeSpatialSoundAttenuation(0x1500, 0x2800, &mix, 0);
    FCK_EQ(mix.flag, 0, "the mode flag clears the amplitude flag");
    FCK_EQ(mix.pan, 0x55, "the mode flag leaves the pan alone");
    FCK_EQ(mix.volume, 50, "the volume does not depend on the flag");
    lr_sound_mode_flag = 0;

    /* full attenuation is still audible; one unit further is not */
    mix_reset(&mix);
    lr_dist_ret[0] = 0x1FFFF;
    FCK_EQ(gComputeSpatialSoundAttenuation(0x1500, 0x2800, &mix, 0), 1,
           "distance 0x1FFFF is the last audible step");
    FCK_EQ(mix.volume, 1, "the last step leaves one volume unit");

    mix_reset(&mix);
    lr_dist_ret[0] = 0x20000;
    lr_sound_mode_flag = 1;
    FCK_EQ(gComputeSpatialSoundAttenuation(0x1500, 0x2800, &mix, 0), 0,
           "distance 0x20000 is out of range");
    FCK_EQ(mix.volume, 0, "an out-of-range sound is silent");
    FCK_EQ(mix.pan, 64, "the out-of-range branch forces centre pan");
    FCK_EQ(mix.flag, 1, "the out-of-range branch forces the flag");
    lr_sound_mode_flag = 0;

    /* an unsupported listener count is a full-volume centre mix */
    mix_reset(&mix);
    lr_sound_spatial_mode = 0;
    FCK_EQ(gComputeSpatialSoundAttenuation(0x1500, 0x2800, &mix, 0), 1,
           "mode 0 is always audible");
    FCK_EQ(mix.volume, 100, "mode 0 keeps the full volume");
    FCK_EQ(mix.pan, 64, "mode 0 stays centred");
    FCK_EQ(mix.flag, 1, "mode 0 sets the flag");
    FCK_EQ(lr_dist_n, 0, "mode 0 measures no distance");

    /* non-positional origin: centre mix, or hard left/right in stereo 2P */
    mix_reset(&mix);
    lr_sound_spatial_mode = 1;
    FCK_EQ(gComputeSpatialSoundAttenuation(0, 0, &mix, 0), 1,
           "the origin is audible");
    FCK_EQ(mix.volume, 100, "the origin plays at full volume");
    FCK_EQ(mix.pan, 64, "the origin is centred");
    FCK_EQ(mix.flag, 1, "the origin sets the flag");
    FCK_EQ(lr_dist_n, 0, "the origin measures no distance");

    lk_fill((u8 *)&extra, 0, sizeof extra);
    extra.data = (u8 *)&data;
    lk_fill((u8 *)&data, 0, sizeof data);
    lr_sound_spatial_mode = 2;
    lr_sound_mode_flag = 1;
    data.mode = 0;
    gComputeSpatialSoundAttenuation(0, 0, &mix, &extra);
    FCK_EQ(mix.pan, 0, "stereo mode 0 pans hard left");
    data.mode = 1;
    gComputeSpatialSoundAttenuation(0, 0, &mix, &extra);
    FCK_EQ(mix.pan, 127, "stereo mode 1 pans hard right");
    data.mode = 2;
    gComputeSpatialSoundAttenuation(0, 0, &mix, &extra);
    FCK_EQ(mix.pan, 64, "any other stereo mode stays centred");
    gComputeSpatialSoundAttenuation(0, 0, &mix, 0);
    FCK_EQ(mix.pan, 64, "no entity falls back to centre");
    lr_sound_mode_flag = 0;

    /* two listeners: the second pair comes from table[4]/table[6] and the
     * louder listener wins. */
    mix_reset(&mix);
    lr_dist_ret[0] = 0x10000;       /* 64 */
    lr_dist_ret[1] = 0;
    FCK_EQ(gComputeSpatialSoundAttenuation(0x1500, 0x2800, &mix, 0), 1,
           "the nearer listener keeps the sound audible");
    FCK_EQ(mix.dx0, 0x500, "listener 0 x delta");
    FCK_EQ(mix.dz0, 0x800, "listener 0 z delta");
    FCK_EQ(mix.dx1, 0x1500 - 0x3000, "listener 1 x delta");
    FCK_EQ(mix.dz1, 0x2800 - 0x4000, "listener 1 z delta");
    FCK_EQ(mix.volume, 100, "the louder of 50 and 100 wins");
    FCK_EQ(lr_dist_n, 2, "both listeners are measured");
    FCK_CHECK(lr_dist_ptr[0] == &mix.dx0 && lr_dist_ptr[1] == &mix.dx1,
              "each listener measures its own delta pair");

    mix_reset(&mix);
    lr_dist_ret[0] = 0;
    lr_dist_ret[1] = 0x10000;
    gComputeSpatialSoundAttenuation(0x1500, 0x2800, &mix, 0);
    FCK_EQ(mix.volume, 100, "the order of the listeners does not matter");

    mix_reset(&mix);
    lr_dist_ret[0] = 0x1FFFF;       /* 1 */
    lr_dist_ret[1] = 0x20000;       /* 0 */
    gComputeSpatialSoundAttenuation(0x1500, 0x2800, &mix, 0);
    FCK_EQ(mix.volume, 1, "the near listener's residual volume wins");

    mix_reset(&mix);
    lr_dist_ret[0] = 0x20001;
    lr_dist_ret[1] = 0x20002;
    FCK_EQ(gComputeSpatialSoundAttenuation(0x1500, 0x2800, &mix, 0), 0,
           "both listeners out of range is silence");
    FCK_EQ(mix.volume, 0, "both out of range silences the mix");
    FCK_EQ(mix.pan, 64, "both out of range forces centre pan");

    mix_reset(&mix);
    lr_dist_ret[0] = 0x20001;       /* one out of range is enough */
    lr_dist_ret[1] = 0;
    FCK_EQ(gComputeSpatialSoundAttenuation(0x1500, 0x2800, &mix, 0), 1,
           "one listener in range is audible");
    FCK_EQ(mix.volume, 100, "the in-range listener keeps the full volume");
    lr_sound_spatial_mode = 1;

    /* the volume field is read back as a byte: 0x1234 with distance 0 gives
     * 0x34, not 0x1234. */
    mix_reset(&mix);
    lr_music_volume = 0x1234;
    lr_dist_ret[0] = 0;
    gComputeSpatialSoundAttenuation(0x1500, 0x2800, &mix, 0);
    FCK_EQ(mix.volume, 0x34, "the attenuated volume keeps only the low byte");
    lr_music_volume = 100;
}

/* ======================================================================
 * 80056634 player view placement
 * ====================================================================== */

static void test_place_player_views(void)
{
    static struct LrObject obj0, obj1;
    static struct LrInfo info0, info1;

    /* @leaf 80056634 source src/game/80056634.c: entry = table[focus->index] with angle = focus->angle; for each live player the eye is placed from the focus position, (angle, angle + f0, f2, f4) and the entry position, the view record is selected by info +0xAC, the eye position/rotation are committed as (angle, f2, 0), and flags +0xA6/+0xA7 become 1 with valAA cleared */
    LK_FUNC("func_80056634");

    lk_fill((u8 *)&lr_focus_block, 0, sizeof lr_focus_block);
    lk_fill((u8 *)gFormCodeTable, 0xEE, sizeof gFormCodeTable);
    gFormCodeTable[3].f0 = 0x20;
    gFormCodeTable[3].f2 = 0x30;
    gFormCodeTable[3].f4 = 0x40;
    gFormCodeTable[3].pos.x = 1;
    gFormCodeTable[3].pos.y = 2;
    gFormCodeTable[3].pos.z = 3;
    lr_focus_block.pos.x = 0x700;
    lr_focus_block.pos.y = 0x800;
    lr_focus_block.pos.z = 0x900;
    lr_focus_block.angle = 0x400;
    lr_focus_block.index = 3;

    lk_fill((u8 *)&info0, 0, sizeof info0);
    info0.valA8 = 0x111;
    info0.valBC = 0x222;
    info0.idAC = 0x0D;
    info0.valAA = 7;
    obj0.info = &info0;
    lk_fill((u8 *)&info1, 0, sizeof info1);
    info1.valA8 = 0x333;
    info1.valBC = 0x444;
    info1.idAC = 0x0E;
    info1.valAA = 9;
    obj1.info = &info1;
    gPlayerEntities[0] = &obj0;
    gPlayerEntities[1] = 0;

    lr_eye_out.x = 0x11;
    lr_eye_out.y = 0x22;
    lr_eye_out.z = 0x33;
    lr_reset();
    gPlacePlayerViews();

    FCK_EQ(lr_eye_args[0], (s32)&lr_focus_block.pos, "the eye is based on the focus position");
    FCK_EQ(lr_eye_args[1], 0x400, "the eye keeps the focus angle");
    FCK_EQ(lr_eye_args[2], 0x420, "the second angle adds entry f0");
    FCK_EQ(lr_eye_args[3], 0x30, "the pitch is entry f2");
    FCK_EQ(lr_eye_args[4], 0x40, "the distance is entry f4");
    FCK_EQ(lr_eye_args[5], (s32)&gFormCodeTable[3].pos, "the offset is the entry position");
    FCK_EQ(lr_eye_args[7], 0x111, "info valA8 is forwarded");
    FCK_EQ(lr_eye_args[8], 0x222, "info valBC is forwarded");
    FCK_EQ(lr_count(LR_EYE), 1, "the null player is skipped");
    FCK_EQ(lr_calls[1].id, LR_VIEWRECORD, "the view record follows the eye");
    FCK_EQ(lr_calls[1].a0, 0x0D, "the view record id is info +0xAC");
    FCK_EQ(lr_calls[2].id, LR_CAMPOS, "the eye position is committed");
    FCK_EQ(lr_calls[2].a1, 0x11, "the committed x is the eye output");
    FCK_EQ(lr_calls[2].a2, 0x22, "the committed y is the eye output");
    FCK_EQ(lr_calls[2].a3, 0x33, "the committed z is the eye output");
    FCK_EQ(lr_calls[3].id, LR_CAMROT, "the rotation is committed last");
    FCK_EQ(lr_calls[3].a0, 0x400, "the yaw is the focus angle");
    FCK_EQ(lr_calls[3].a1, 0x30, "the pitch is entry f2");
    FCK_EQ(lr_calls[3].a2, 0, "the roll is zero");
    FCK_EQ(lr_calls_n, 4, "four callees for one live player");
    FCK_EQ(info0.flagA6, 1, "flag +0xA6 is set");
    FCK_EQ(info0.flagA7, 1, "flag +0xA7 is set");
    FCK_EQ(info0.valAA, 0, "valAA is cleared");
    FCK_EQ(info1.flagA6, 0, "the absent player's info is untouched");

    /* both players place their own eye with their own info */
    gPlayerEntities[1] = &obj1;
    gFormCodeTable[5].f0 = -0x10;
    gFormCodeTable[5].f2 = 0x50;
    gFormCodeTable[5].f4 = 0x60;
    gFormCodeTable[5].pos.z = 0x77;
    lr_focus_block.index = 5;
    lr_reset();
    gPlacePlayerViews();
    FCK_EQ(lr_count(LR_EYE), 2, "both players are placed");
    FCK_EQ(lr_eye_args[2], 0x400 - 0x10, "the second player uses entry 5 f0");
    FCK_EQ(lr_eye_args[7], 0x333, "the second player forwards its own valA8");
    FCK_EQ(lr_eye_args[8], 0x444, "the second player forwards its own valBC");
    FCK_EQ(lr_count(LR_VIEWRECORD), 2, "both view records are selected");
    FCK_EQ(lr_call(LR_VIEWRECORD, 1).a0, 0x0E, "the second player uses its own id");
    FCK_EQ(info1.flagA6, 1, "the second player's flag is set");
    FCK_EQ(info1.valAA, 0, "the second player's valAA is cleared");

    /* no live player: nothing happens, not even a table read side effect */
    gPlayerEntities[0] = 0;
    gPlayerEntities[1] = 0;
    lr_reset();
    gPlacePlayerViews();
    FCK_EQ(lr_calls_n, 0, "no live player calls nothing");
}

/* ======================================================================
 * 80060838 attack definition loader
 * ====================================================================== */

static void definition_reset(void)
{
    int i;
    lk_fill((u8 *)gEffectKeyTable, 0xEE, sizeof gEffectKeyTable);
    lk_fill(lr_payload, 0x5A, sizeof lr_payload);
    lk_fill(lr_refblock, 0x00, sizeof lr_refblock);
    lr_def_count = 0;
    lr_ref_n = 0;
    lr_fetch_n = 0;
    lr_reset();
    for (i = 0; i < 4; i++) lr_ref_returns[i] = (void *)(0x9000 + i);
    /* the copied 0x20 bytes and the two scaled words */
    lr_payload[0x18] = 0xDA;        /* unk08 = 218 */
    lr_payload[0x19] = 0x00;
    lr_payload[0x1A] = 0x6D;        /* unk0A = 109 */
    lr_payload[0x1B] = 0x00;
    lr_payload[0x22] = 0x05;        /* sub.unk06 = 5 */
    lr_payload[0x23] = 0x00;
    /* Four 8-byte slots from +8: sp[1] is the group, sp[2] the name. A zero
     * name means "no resource", so slot 0 and slot 3 stay empty. */
    ((s32 *)lr_refblock)[4] = 0;        /* slot 0 name */
    ((s32 *)lr_refblock)[5] = 3;        /* slot 1 group */
    ((s32 *)lr_refblock)[6] = 7;        /* slot 1 name */
    ((s32 *)lr_refblock)[7] = 4;        /* slot 2 group */
    ((s32 *)lr_refblock)[8] = 9;        /* slot 2 name */
    ((s32 *)lr_refblock)[10] = 0;       /* slot 3 name */
}

static void test_attack_definition(void)
{
    static struct LrDefArg arg;
    struct LrDefEntry *e;
    int i;

    /* @leaf 80060838 libc the 0x20-byte copy is the unit's own memcpy of the tACT payload at +0x10 into a 0x30-byte table slot, then src/game/80060838.c: index = count++, unk08/unk0A = (value << 12)/218, the sub-record scaler, four (group, name) pairs from the aRSL block at +8 with a zero name meaning null, and the descriptor is freed */
    LK_FUNC("func_80060838");

    definition_reset();
    arg.unk00 = 0x1234;
    arg.unk1C = 0x56;
    arg.unk20 = 0x78;
    gLoadAttackDefinition(&arg);

    e = &gEffectKeyTable[0];
    FCK_EQ(lr_def_count, 1, "the entry count advances");
    FCK_EQ(*(u8 *)((u8 *)e + 0x00), 0x5A, "the first copied byte");
    FCK_EQ(*(u8 *)((u8 *)e + 0x07), 0x5A, "the eighth copied byte");
    FCK_EQ(e->unk00[0], 0x5A, "the copy starts at the payload +0x10");
    FCK_EQ(e->unk08, 4096, "unk08 is scaled by 4096/218");
    FCK_EQ(e->unk0A, 2048, "unk0A is scaled by 4096/218");
    FCK_EQ(e->unk1A[0], 0x5A, "the tail of the copy is unk1A");
    FCK_EQ(e->unk20[0], 0, "a zero reference name yields null");
    FCK_EQ((s32)e->unk20[1], 0x9000, "the first resolved reference");
    FCK_EQ((s32)e->unk20[2], 0x9001, "the second resolved reference");
    FCK_EQ(e->unk20[3], 0, "the third zero name yields null");
    FCK_EQ(lr_ref_n, 2, "only named references are resolved");
    FCK_EQ(lr_ref_args[0][0], 3, "the first pair group");
    FCK_EQ(lr_ref_args[0][1], 7, "the first pair name");
    FCK_EQ(lr_ref_args[1][0], 4, "the second pair group");
    FCK_EQ(lr_ref_args[1][1], 9, "the second pair name");

    FCK_EQ(lr_fetch_n, 1, "one tagged payload fetch (the reference block is the other call)");
    FCK_EQ(lr_fetch_args[0][0], 0x1234, "the fetch gets the descriptor blob");
    FCK_EQ(lr_fetch_args[0][1], 0x78, "the fetch gets the descriptor length");
    FCK_EQ(lr_fetch_args[0][2], 0x74414354, "the fetch asks for the tACT tag");
    FCK_EQ(lr_fetch_args[0][3], 0x56, "the fetch gets the section id");

    FCK_EQ(lr_count(LR_FETCH), 1, "one tACT fetch");
    FCK_EQ(lr_count(LR_FETCH2), 1, "one aRSL fetch");
    FCK_EQ(lr_call(LR_FETCH2, 0).a2, 0x6152534C, "the aRSL tag");
    FCK_EQ(lr_count(LR_SUBSCALE), 1, "the sub-record is scaled once");
    FCK_EQ(lr_call(LR_SUBSCALE, 0).a0, (s32)&e->sub, "the scaler sees the sub-record");
    FCK_EQ(lr_call(LR_SUBSCALE, 0).a1, 5, "the scaler sees the copied unk06");
    FCK_EQ(e->sub.unk06, 6, "the scaler's write is visible in the entry");
    FCK_EQ(lr_count(LR_FREE), 1, "the descriptor is freed once");
    FCK_EQ(lr_call(LR_FREE, 0).a0, (s32)&arg, "the freed block is the argument");
    FCK_EQ(lr_calls_n, 6, "fetch, scaler, fetch, two resolves, free");

    /* the next load takes the next slot and leaves the previous one alone */
    definition_reset();
    gEffectKeyTable[2].unk08 = 0x1111;
    lr_def_count = 2;
    gLoadAttackDefinition(&arg);
    FCK_EQ(lr_def_count, 3, "the count keeps advancing");
    FCK_EQ(gEffectKeyTable[2].unk08, 4096, "slot 2 is overwritten");
    FCK_EQ(gEffectKeyTable[1].unk08, (s16)0xEEEE, "slot 1 is untouched");
    FCK_EQ(gEffectKeyTable[3].unk08, (s16)0xEEEE, "slot 3 is untouched");
    for (i = 0; i < 8; i++) {
        if (gEffectKeyTable[3].unk00[i] != 0xEE) break;
    }
    FCK_EQ(i, 8, "the following slot is untouched");
}


/* ======================================================================
 * 8006A918 vm record registry loader
 * ====================================================================== */

static void registry_reset(void)
{
    int i;
    lk_fill(lr_vm_area, 0x00, 0x400);
    lr_vm_count = 0;
    lr_ref_n = 0;
    lr_fetch_n = 0;
    lr_reset();
    for (i = 0; i < 0x60; i++) lr_payload[i] = (u8)(0x80 + i);
    lk_fill(lr_refblock, 0x00, sizeof lr_refblock);
    lr_ref_returns[0] = (void *)0xA000;
    lr_ref_returns[1] = (void *)0xA001;
    /* slot 0 empty, slots 1 and 2 named, slot 3 empty */
    ((s32 *)lr_refblock)[4] = 0;
    ((s32 *)lr_refblock)[5] = 3;
    ((s32 *)lr_refblock)[6] = 7;
    ((s32 *)lr_refblock)[7] = 4;
    ((s32 *)lr_refblock)[8] = 9;
    ((s32 *)lr_refblock)[10] = 0;
}

static void test_registry_load(void)
{
    static struct LrDefArg arg;

    /* @leaf 8006A918 libc the 0x44-byte copy is the unit's own memcpy of the tACT payload at +0x10 into a 0x54-byte record; from src/game/8006A918.c: the load only runs while the count is below six, the count advances, both 0x20-byte objects are scaled, the four aRSL reference pairs fill record +0x44..+0x53 right after the copied 0x44 bytes, and the descriptor is freed even when the gate refuses */
    LK_FUNC("func_8006A918");

    registry_reset();
    arg.unk00 = 0x11;
    arg.unk1C = 0x22;
    arg.unk20 = 0x33;
    gRegistryLoad(&arg);

    FCK_EQ(lr_vm_count, 1, "the count advances");
    FCK_EQ(*(u8 *)(lr_vm_area + 0x00), 0x90, "the copy starts at payload +0x10");
    FCK_EQ(*(u8 *)(lr_vm_area + 0x43), 0x90 + 0x43, "the copy is 0x44 bytes");
        FCK_EQ(*(u8 *)(lr_vm_area + 0x0C), 0x90 + 0x0C + 1, "the first object was scaled");
    FCK_EQ(*(u8 *)(lr_vm_area + 0x2C), 0x90 + 0x2C + 1, "the second object was scaled");
    FCK_EQ(lr_count(LR_VMSCALE), 2, "two object scalers run");
    FCK_EQ(lr_call(LR_VMSCALE, 0).a0, (s32)(lr_vm_area + 0x04),
           "the first object is four bytes into the table");
    FCK_EQ(lr_call(LR_VMSCALE, 1).a0, (s32)(lr_vm_area + 0x24),
           "the second object follows 0x20 bytes later");
    FCK_EQ((s32)*(void **)(lr_vm_area + 0x44), 0, "an empty slot writes null");
    FCK_EQ((s32)*(void **)(lr_vm_area + 0x48), 0xA000, "the first named reference");
    FCK_EQ((s32)*(void **)(lr_vm_area + 0x4C), 0xA001, "the second named reference");
    FCK_EQ((s32)*(void **)(lr_vm_area + 0x50), 0, "the last empty slot writes null");
    FCK_EQ(lr_ref_n, 2, "two references are resolved");
    FCK_EQ(lr_ref_args[0][0], 3, "the first pair group");
    FCK_EQ(lr_ref_args[0][1], 7, "the first pair name");
    FCK_EQ(lr_ref_args[1][0], 4, "the second pair group");
    FCK_EQ(lr_ref_args[1][1], 9, "the second pair name");
    FCK_EQ(lr_call(LR_FETCH, 0).a2, 0x74414354, "the record copy asks for tACT");
    FCK_EQ(lr_call(LR_FETCH2, 0).a2, 0x6152534C, "the references ask for aRSL");
    FCK_EQ(lr_count(LR_FREE), 1, "the descriptor is freed");
    FCK_EQ(lr_call(LR_FREE, 0).a0, (s32)&arg, "the freed block is the argument");

    /* the sixth load is the last one: 6 is refused but the block is freed */
    registry_reset();
    lr_vm_count = 6;
    gRegistryLoad(&arg);
    FCK_EQ(lr_vm_count, 6, "a full registry does not advance the count");
    FCK_EQ(lr_count(LR_FETCH), 0, "a full registry fetches nothing");
    FCK_EQ(lr_count(LR_VMSCALE), 0, "a full registry scales nothing");
    FCK_EQ(lr_count(LR_FREE), 1, "a full registry still frees the block");
    FCK_EQ(*(u8 *)lr_vm_area, 0x00, "a full registry writes nothing");

    /* the fifth load lands at index 5, a 0x54-byte stride away */
    registry_reset();
    lr_vm_count = 5;
    gRegistryLoad(&arg);
    FCK_EQ(lr_vm_count, 6, "the fifth load still runs");
    FCK_EQ(*(u8 *)(lr_vm_area + 5 * 0x54), 0x90, "index 5 starts at 5 * 0x54");
    FCK_EQ(lr_call(LR_VMSCALE, 0).a0, (s32)(lr_vm_area + 5 * 0x54 + 0x04),
           "index 5's first object");
    FCK_EQ((s32)*(void **)(lr_vm_area + 5 * 0x54 + 0x48), 0xA000,
           "index 5's second reference");
    FCK_EQ(*(u8 *)(lr_vm_area + 0x00), 0x00, "index 0 is untouched");
}

/* ======================================================================
 * 80017EBC heap allocator
 * ====================================================================== */

static void alloc_reset(void)
{
    int i;
    lk_fill(lr_heap, 0x00, sizeof lr_heap);
    for (i = 0; i < 8; i++) lr_free_ranges[i] = 0;
    gFreeRangeCount = 2;
    gHeapSearchCursor = -1;
    lr_raw_result = 0;
    lr_fwd_ret = -1;
    lr_back_ret = -1;
    lr_search_n = 0;
    lr_pop_result = 0;
    lr_reset();
}

static void test_heap_allocator(void)
{
    u8 *h = lr_heap;
    static struct LrLinkState pool1, pool2, pool3;

    /* @leaf 80017EBC semantic allocation contract from src/game/80017EBC.c: a zero size returns null; mode 0 goes to the raw allocator and falls back to mode 2; modes 2 and 1 round (size + 7) & ~3, keep a four-byte size header, carve from the front/back of a free range, drop a consumed range and move the mode-2 cursor; a failed range search retries the raw allocator with four bytes less; mode 3 pops from pool 3, then 2, then 1 and otherwise retries as mode 1 */
    LK_FUNC("func_80017EBC");

    alloc_reset();
    FCK_EQ((s32)gAllocMemory(0, 2), 0, "a zero size returns null");
    FCK_EQ(lr_calls_n, 0, "a zero size calls nothing");

    alloc_reset();
    lr_raw_result = (void *)0xD000;
    FCK_EQ((s32)gAllocMemory(0x20, 0), 0xD000, "mode 0 returns the raw block");
    FCK_EQ(lr_call(LR_RAW, 0).a0, 0x20, "mode 0 passes the size");
    FCK_EQ(lr_call(LR_RAW, 0).a1, 0, "mode 0 passes mode 0");
    FCK_EQ(lr_calls_n, 1, "mode 0 makes a single call");

    /* a failed raw allocation retries as a front-of-range allocation */
    alloc_reset();
    lr_free_ranges[0] = (s32)(h + 0x20);
    lr_free_ranges[1] = (s32)(h + 0x120);
    lr_fwd_ret = 0;
    FCK_EQ((s32)gAllocMemory(5, 0), (s32)(h + 0x24), "the fallback returns the range block");
    FCK_EQ(lr_call(LR_RSEARCH_FWD, 0).a0, 12, "the fallback searches the aligned size");
    FCK_EQ(*(s32 *)(h + 0x20), 12, "the fallback writes the size header");

    /* mode 2: (5 + 7) & ~3 = 12 bytes from the front of the range */
    alloc_reset();
    lr_free_ranges[0] = (s32)(h + 0x20);
    lr_free_ranges[1] = (s32)(h + 0x120);
    lr_fwd_ret = 0;
    FCK_EQ((s32)gAllocMemory(5, 2), (s32)(h + 0x24), "mode 2 returns past the header");
    FCK_EQ(*(s32 *)(h + 0x20), 12, "mode 2 stores the aligned size");
    FCK_EQ(lr_free_ranges[0], (s32)(h + 0x2C), "mode 2 moves the range start");
    FCK_EQ(gHeapSearchCursor, 0, "mode 2 records the search cursor");
    FCK_EQ(gFreeRangeCount, 2, "a partly used range stays in the table");
    FCK_EQ(lr_calls_n, 1, "only the range search runs");

    /* the alignment rounds up, never down */
    alloc_reset();
    lr_free_ranges[0] = (s32)(h + 0x20);
    lr_free_ranges[1] = (s32)(h + 0x320);
    lr_fwd_ret = 0;
    gAllocMemory(0x11, 2);
    FCK_EQ(*(s32 *)(h + 0x20), 0x18, "(0x11 + 7) & ~3 = 0x18");

    /* a consumed range is removed from the table */
    alloc_reset();
    lr_free_ranges[0] = (s32)h;
    lr_free_ranges[1] = (s32)(h + 0x100);
    lr_free_ranges[2] = (s32)(h + 0x100);
    lr_free_ranges[3] = (s32)(h + 0x200);
    gFreeRangeCount = 4;
    lr_fwd_ret = 0;
    FCK_EQ((s32)gAllocMemory(0xF9, 2), (s32)(h + 4), "0xF9 rounds to the whole first range");
    FCK_EQ(*(s32 *)h, 0x100, "the whole 0x100 bytes are taken");
    FCK_EQ(lr_count(LR_MEMCOPY), 1, "the consumed range is dropped");
    FCK_EQ(lr_memcopy_args[0], (s32)&lr_free_ranges[0], "the table is compacted in place");
    FCK_EQ(lr_memcopy_args[1], (s32)&lr_free_ranges[2], "the following pair moves up");
    FCK_EQ(lr_memcopy_args[2], 12, "(count - index - 1) * 4 bytes move");
    FCK_EQ(gFreeRangeCount, 2, "one range pair is removed");
    FCK_EQ(lr_free_ranges[0], (s32)(h + 0x100), "the second pair is now first");
    FCK_EQ(lr_free_ranges[1], (s32)(h + 0x200), "the second pair's end moved with it");

    /* a failed search retries the raw allocator four bytes smaller */
    alloc_reset();
    FCK_EQ((s32)gAllocMemory(5, 2), 0, "a failed search and raw allocation returns null");
    FCK_EQ(lr_search_size[0], 12, "the search saw the aligned size");
    FCK_EQ(lr_call(LR_RAW, 0).a0, 8, "the retry is four bytes smaller");
    FCK_EQ(lr_call(LR_RAW, 0).a1, 0, "the retry uses the raw allocator");

    /* mode 1 carves from the end of the range */
    alloc_reset();
    lr_free_ranges[0] = (s32)h;
    lr_free_ranges[1] = (s32)(h + 0x100);
    lr_back_ret = 0;
    FCK_EQ((s32)gAllocMemory(5, 1), (s32)(h + 0xF8), "mode 1 returns from the range end");
    FCK_EQ(*(s32 *)(h + 0xF4), 12, "mode 1 stores the size header");
    FCK_EQ(lr_free_ranges[1], (s32)(h + 0xF4), "mode 1 lowers the range end");
    FCK_EQ(lr_free_ranges[0], (s32)h, "mode 1 leaves the start alone");

    /* a consumed mode-1 range also pulls the cursor back */
    alloc_reset();
    lr_free_ranges[0] = (s32)h;
    lr_free_ranges[1] = (s32)(h + 0x100);
    lr_free_ranges[2] = (s32)(h + 0x100);
    lr_free_ranges[3] = (s32)(h + 0x200);
    gFreeRangeCount = 4;
    gHeapSearchCursor = 5;
    lr_back_ret = 0;
    gAllocMemory(0xF9, 1);
    FCK_EQ(gFreeRangeCount, 2, "the consumed mode-1 range is dropped");
    FCK_EQ(gHeapSearchCursor, 3, "a cursor past the removed pair moves back two");

    /* mode 3 pops from the third, then second, then first pool */
    alloc_reset();
    pool3.stride = 0x10; pool3.count = 2;
    pool2.stride = 0x10; pool2.count = 2;
    pool1.stride = 0x10; pool1.count = 2;
    gFixedPool3 = &pool3;
    gFixedPool2 = &pool2;
    gFixedPool1 = &pool1;
    lr_pop_result = (void *)0xC000;
    FCK_EQ((s32)gAllocMemory(8, 3), 0xC000, "mode 3 returns the pool node");
    FCK_EQ(lr_call(LR_POOLPOP, 0).a0, (s32)&pool3, "the third pool is tried first");

    alloc_reset();
    pool3.stride = 0x04; pool3.count = 2;
    pool2.stride = 0x08; pool2.count = 0;
    pool1.stride = 0x02; pool1.count = 1;
    gFixedPool3 = &pool3;
    gFixedPool2 = &pool2;
    gFixedPool1 = &pool1;
    lr_pop_result = (void *)0xC100;
    FCK_EQ((s32)gAllocMemory(8, 3), 0xC100, "a too small pool falls through");
    FCK_EQ(lr_call(LR_POOLPOP, 0).a0, (s32)&pool1, "the first pool has no size check");

    alloc_reset();
    pool3.stride = 0x20; pool3.count = 0;
    pool2.stride = 0x20; pool2.count = 0;
    pool1.stride = 0x20; pool1.count = 0;
    gFixedPool3 = &pool3;
    gFixedPool2 = &pool2;
    gFixedPool1 = &pool1;
    lr_free_ranges[0] = (s32)h;
    lr_free_ranges[1] = (s32)(h + 0x100);
    lr_back_ret = 0;
    FCK_EQ((s32)gAllocMemory(8, 3), (s32)(h + 0xF8), "empty pools fall back to a range");
    FCK_EQ(lr_call(LR_RSEARCH_BACK, 0).a0, 12, "the fallback searches the aligned size");
    FCK_EQ(lr_count(LR_POOLPOP), 0, "no pool is popped");

    /* an unknown mode is a null return without calls */
    alloc_reset();
    FCK_EQ((s32)gAllocMemory(0x10, 4), 0, "an unknown mode returns null");
    FCK_EQ(lr_calls_n, 0, "an unknown mode calls nothing");
}

/* ======================================================================
 * 80050B54 path mover vertical motion
 * ====================================================================== */

static void mover_reset(struct LrObj *obj, struct LrState *state)
{
    lk_fill((u8 *)obj, 0x00, sizeof *obj);
    lk_fill((u8 *)state, 0x77, sizeof *state);
    lr_reset();
    obj->y = 20;
    obj->scale = 0x100;
    obj->value = 0x100;
    obj->divisor = 0x100;
    obj->offset = 10;
    obj->speed = 100;
    obj->current = 0x5555;
    obj->timer = 0x7777;
    obj->flags = 0x11;
    state->height = 1000;
    state->changed = 0;
    state->child = 0;
    state->index = 0;
    lr_ground_height = 0;
}

static void test_path_mover_vertical(void)
{
    static struct LrObj obj;
    static struct LrState state;
    static struct LrChild child;
    static u8 entries[0x40];

    /* @leaf 80050B54 math the 0x10 mode computes target = (offset << 4) + (y << 8) + ((scale << 8) * ((value * (0x80000000 / (divisor << 7))) >> 16)) >> 16 and climbs (speed * delta) >> 1; the 0x40 mode uses the ground query plus (entry->b - entry->a) << 4 and descends the same step; only the sub-target path writes state->height, and a mode-0x10 arrival clears flag 0x10, the timer and records the speed */
    LK_FUNC("func_80050B54");

    /* divisor 0x100 -> reciprocal 0x10000; value 0x100 -> 0x100; scale 0x100
     * -> target 256; plus (10 << 4) and (20 << 8) -> 5536. */
    mover_reset(&obj, &state);
    FCK_EQ(gStepPathMoverVerticalMotion(&obj, 16, &state), 1, "a climb reports movement");
    FCK_EQ(state.height, 1800, "1000 + (100 * 16 >> 1)");
    FCK_EQ(obj.flags, 0x11, "a climb below the target keeps the flags");
    FCK_EQ(obj.timer, 0x7777, "a climb below the target keeps the timer");
    FCK_EQ(obj.current, 0x5555, "a climb below the target keeps the speed record");
    FCK_EQ(state.changed, 0, "a climb below the target leaves changed alone");

    /* the same step from 5000 arrives: 5800 >= 5536 */
    mover_reset(&obj, &state);
    state.height = 5000;
    FCK_EQ(gStepPathMoverVerticalMotion(&obj, 16, &state), 1, "an arrival reports movement");
    FCK_EQ(obj.flags, 0x01, "the arrival clears flag 0x10");
    FCK_EQ(obj.timer, 0, "the arrival clears the timer");
    FCK_EQ(obj.current, 100, "the arrival records the speed in current");
    FCK_EQ(state.changed, 1, "the arrival sets state->changed");
    FCK_EQ(state.height, 5000, "the arrival does not rewrite the state height");

    /* the step scales with the delta */
    mover_reset(&obj, &state);
    obj.speed = 200;
    gStepPathMoverVerticalMotion(&obj, 4, &state);
    FCK_EQ(state.height, 1400, "1000 + (200 * 4 >> 1)");

    /* a slower object still lands exactly on the target height */
    mover_reset(&obj, &state);
    state.height = 5530;
    gStepPathMoverVerticalMotion(&obj, 16, &state);
    FCK_EQ(obj.flags, 0x01, "a two unit step also counts as an arrival");

    /* mode 0x40: ground 4000 plus (0x20 - 0x10) << 4 = 4256 */
    mover_reset(&obj, &state);
    obj.flags = 0x40;
    state.height = 5000;
    child.entries = (struct LrStepEntry *)entries;
    lk_fill(entries, 0, sizeof entries);
    /* the unit reads the entry at base + index * 0x10 + 8, so record 0's
     * heights are the words at +0x0A and +0x10. */
    *(s16 *)(entries + 0x0A) = 0x10;
    *(s16 *)(entries + 0x10) = 0x20;
    state.child = &child;
    lr_ground_height = 4000;
    FCK_EQ(gStepPathMoverVerticalMotion(&obj, 16, &state), 1, "a descent reports movement");
    FCK_EQ(lr_ground_arg, -1, "the ground query asks for height -1");
    FCK_EQ(lr_ground_pos[0], (s32)state.pos, "the ground query uses the state position");
    FCK_EQ(state.height, 4256, "the descent lands on the ground target");
    FCK_EQ(obj.flags, 0x50, "the landing sets flag 0x10 again");
    FCK_EQ(lr_calls_n, 1, "one ground query");

    /* still above the target after the step: the state follows the step */
    mover_reset(&obj, &state);
    obj.flags = 0x40;
    state.height = 6000;
    child.entries = (struct LrStepEntry *)entries;
    state.child = &child;
    lr_ground_height = 4000;
    gStepPathMoverVerticalMotion(&obj, 16, &state);
    FCK_EQ(state.height, 5200, "6000 - (100 * 16 >> 1)");
    FCK_EQ(obj.flags, 0x40, "an unfinished descent keeps the flags");

    /* no child: the target is the bare ground height */
    mover_reset(&obj, &state);
    obj.flags = 0x40;
    state.height = 5000;
    state.child = 0;
    lr_ground_height = 4000;
    gStepPathMoverVerticalMotion(&obj, 16, &state);
    FCK_EQ(state.height, 4200, "4000 < 4200 keeps the stepped height");

    /* the entry comes from the state index, 0x10 bytes apart */
    mover_reset(&obj, &state);
    obj.flags = 0x40;
    state.height = 2000;
    child.entries = (struct LrStepEntry *)entries;
    lk_fill(entries, 0, sizeof entries);
    *(s16 *)(entries + 0x2A) = 0x100;
    *(s16 *)(entries + 0x30) = 0x80;   /* negative contribution: -0x800 */
    state.child = &child;
    state.index = 2;
    lr_ground_height = 4000;
    gStepPathMoverVerticalMotion(&obj, 16, &state);
    FCK_EQ(state.height, 1952, "4000 - 0x800 is the descendant target");
    FCK_EQ(obj.flags, 0x50, "the negative offset still lands on flag 0x10");

    /* a leaf with 0x10 already clear does not climb */
    mover_reset(&obj, &state);
    obj.flags = 0x01;
    state.height = 1000;
    FCK_EQ(gStepPathMoverVerticalMotion(&obj, 16, &state), 0, "flag 0x10 clear matches no mode");
    FCK_EQ(state.height, 1000, "an unmatched mode leaves the height alone");

    /* unsupported flag mixes */
    mover_reset(&obj, &state);
    obj.flags = 0x20;
    FCK_EQ(gStepPathMoverVerticalMotion(&obj, 16, &state), 0, "0x20 matches no mode");
    FCK_EQ(lr_calls_n, 0, "an unmatched mode calls nothing");
    FCK_EQ(state.height, 1000, "an unmatched mode leaves the height");

    mover_reset(&obj, &state);
    obj.flags = 0x50;
    FCK_EQ(gStepPathMoverVerticalMotion(&obj, 16, &state), 0, "0x50 matches no mode");
}

/* ======================================================================
 * 80067914 map cell ray test
 * ====================================================================== */

static void cell_reset(void)
{
    int i;
    lk_fill(lr_tile, 0x00, sizeof lr_tile);
    lk_fill((u8 *)gTileShapeTable, 0x00, sizeof gTileShapeTable);
    lk_fill((u8 *)gTileCornerIndices, 0x00, sizeof gTileCornerIndices);
    lk_fill((u8 *)lr_map_header, 0x00, sizeof lr_map_header);
    lk_fill((u8 *)gWallHitPositions, 0x00, sizeof gWallHitPositions);
    lk_fill((u8 *)gWallHitPolys, 0x00, sizeof gWallHitPolys);
    lk_fill((u8 *)gWallHitKinds, 0x00, sizeof gWallHitKinds);
    lk_fill((u8 *)gWallHitTopHeights, 0x00, sizeof gWallHitTopHeights);
    lk_fill((u8 *)&gCellRayStart, 0x00, sizeof gCellRayStart);
    lk_fill((u8 *)&gCellRayEnd, 0x00, sizeof gCellRayEnd);
    lk_fill((u8 *)&gCellRayWallHit, 0x00, sizeof gCellRayWallHit);
    for (i = 0; i < 4; i++) lr_tile_table[i] = lr_tile;
    /* map header: world[1] is the row stride, world[9] the cell descriptor
     * whose bits 2..15 select the tile block. */
    lr_map_header[1] = 8;
    lr_map_header[9] = 0x0004;
    /* one polygon at the cell's polygon list, 0x10 words in */
    *(u16 *)(lr_tile + 0x3CC) = (0x10 << 4) | 1;
    /* the four corner heights of the block */
    lr_tile[12] = 0x10;
    lr_tile[13] = 0x20;
    lr_tile[14] = 0x30;
    lr_tile[15] = 0x40;
    /* surface material of material slot 0 */
    lr_tile[0x370] = 0x2A;
    lr_reset();
}

static void test_map_cell_ray(void)
{
    static struct LrCellVec a, b;
    static s32 outA[3];
    static s32 outB;
    s32 i;
    u32 poly;

    /* @leaf 80067914 math the ray's midpoint selects cell (>>13) and the tile block (world[9] bits 2..15); cell-local endpoints drive height interpolation low = (h0<<8) + ((h3-h0)*dist>>5) and high = (h1<<8) + ((h2-h1)*dist>>5); a wall is recorded when its hit z lies between them, a type below 71 is only tested with flag set and then reports a fixed 5 with the surface material, and the diagonal kind solves p0 = (v*t0 + (8192-q)*delta)/den before interpolating */
    LK_FUNC("func_80067914");

    /* a wall hit: start (1000,-400,8000), end (3000,400,8000); the shape
     * table entry 11 means "top equals the third corner", the hit x 3000
     * enters the interpolation. */
    cell_reset();
    a.x = 1000; a.y = -400; a.z = 8000;
    b.x = 3000; b.y = 400; b.z = 8000;
    poly = 80u << 15;
    *(u32 *)(lr_tile + 0x5E0) = poly;
    gTileShapeTable[80] = (11 << 3) | 1;
    gTileCornerIndices[80][0] = 0;
    gTileCornerIndices[80][1] = 1;
    gTileCornerIndices[80][2] = 2;
    gTileCornerIndices[80][3] = 3;
    FCK_EQ(gTestMapCellRay(&a, &b, outA, &outB, 0), 0x2A01, "a wall hit returns kind 1 and material 0x2A");
    FCK_EQ(gCellRayStart.x, 1000, "the start stays in the cell");
    FCK_EQ(gCellRayStart.y, -400, "the cell offset is zero for this fixture");
    FCK_EQ(gCellRayEnd.x, 3000, "the end stays in the cell");
    FCK_EQ(gWallHitPolys[0], poly, "the polygon is recorded");
    FCK_EQ(gWallHitKinds[0], 1, "the surface kind is recorded");
    FCK_EQ(gWallHitTopHeights[0], 9692, "high = (0x20 << 8) + (0x10 * 3000 >> 5)");
    FCK_EQ(gWallHitPositions[0].x, 3000, "the hit x is the segment end");
    FCK_EQ(gWallHitPositions[0].y, 400, "the hit y is the segment end");
    FCK_EQ(gWallHitPositions[0].z, 8000, "the hit z is the segment end");
    FCK_EQ(outB, 9692, "outB carries the top height");
    FCK_EQ(outA[0], 400, "outA y");
    FCK_EQ(outA[1], 8000, "outA z");
    FCK_EQ(outA[2], 3000, "outA x");

    /* the same wall at a height above the interpolated top: low = 7096 */
    cell_reset();
    a.x = 1000; a.y = -400; a.z = 12000;
    b.x = 3000; b.y = 400; b.z = 12000;
    *(u32 *)(lr_tile + 0x5E0) = poly;
    gTileShapeTable[80] = (11 << 3) | 1;
    gTileCornerIndices[80][1] = 1;
    gTileCornerIndices[80][2] = 2;
    gTileCornerIndices[80][3] = 3;
    outA[0] = -1; outA[1] = -1; outA[2] = -1; outB = -1;
    FCK_EQ(gTestMapCellRay(&a, &b, outA, &outB, 0), 0, "a height above the top is no hit");
    FCK_EQ(outA[0], -1, "a rejected wall leaves outA alone");
    FCK_EQ(outB, -1, "a rejected wall leaves outB alone");
    FCK_EQ(gWallHitPolys[0], 0, "a rejected wall records nothing");

    /* a polygon word the loop must not reach when the count is zero */
    cell_reset();
    *(u16 *)(lr_tile + 0x3CC) = 0x10 << 4;      /* count 0 */
    *(u32 *)(lr_tile + 0x5E0) = 40u << 15;      /* a hit if it were read */
    gTileShapeTable[40] = 0;
    gTileCornerIndices[40][1] = 1;
    gTileCornerIndices[40][2] = 2;
    a.x = 1000; a.y = -400; a.z = 2000;
    b.x = 3000; b.y = 400; b.z = 8000;
    outA[0] = -1; outB = -1;
    FCK_EQ(gTestMapCellRay(&a, &b, outA, &outB, 1), 0, "a zero count skips the polygon list");
    FCK_EQ(outB, -1, "a zero count writes no output");
    FCK_EQ(gWallHitPolys[0], 0, "a zero count records no candidate");

    /* type 71 is already a wall, not a plane */
    cell_reset();
    *(u32 *)(lr_tile + 0x5E0) = 71u << 15;
    gTileShapeTable[71] = (11 << 3) | 1;
    gTileCornerIndices[71][0] = 0;
    gTileCornerIndices[71][1] = 1;
    gTileCornerIndices[71][2] = 2;
    gTileCornerIndices[71][3] = 3;
    a.x = 1000; a.y = -400; a.z = 8000;
    b.x = 3000; b.y = 400; b.z = 8000;
    FCK_EQ(gTestMapCellRay(&a, &b, outA, &outB, 1), 0x2A01, "type 71 takes the wall path");
    FCK_EQ(gWallHitKinds[0], 1, "type 71 is recorded as a wall");
    FCK_EQ(outB, 9692, "type 71 interpolates like the other walls");

    /* both endpoints exactly on the plane: the product t0^p0 is zero and the
     * surface still counts as hit */
    cell_reset();
    *(u32 *)(lr_tile + 0x5E0) = 40u << 15;
    gTileShapeTable[40] = 0;
    gTileCornerIndices[40][1] = 1;
    gTileCornerIndices[40][2] = 2;
    a.x = 0; a.y = 0; a.z = 4096;
    b.x = 0; b.y = 0; b.z = 4096;
    outA[0] = -1; outA[1] = -1; outA[2] = -1; outB = -1;
    FCK_EQ(gTestMapCellRay(&a, &b, outA, &outB, 1), 0x2A05,
           "a segment on the plane is a hit, not a miss");
    FCK_EQ(outB, 4096, "the on-plane hit reports the endpoint z");

    /* no polygons in the cell */
    cell_reset();
    *(u16 *)(lr_tile + 0x3CC) = 0x10 << 4;
    a.x = 1000; a.y = -400; a.z = 8000;
    b.x = 3000; b.y = 400; b.z = 8000;
    FCK_EQ(gTestMapCellRay(&a, &b, outA, &outB, 1), 0, "an empty cell returns zero");
    FCK_EQ(gWallHitPolys[0], 0, "an empty cell records nothing");

    /* a plane surface is skipped unless the fourth argument allows it */
    cell_reset();
    *(u32 *)(lr_tile + 0x5E0) = 40u << 15;
    gTileShapeTable[40] = 0;
    gTileCornerIndices[40][1] = 1;
    gTileCornerIndices[40][2] = 2;
    a.x = 1000; a.y = -400; a.z = 2000;
    b.x = 3000; b.y = 400; b.z = 8000;
    FCK_EQ(gTestMapCellRay(&a, &b, outA, &outB, 0), 0, "a plane below type 71 needs the flag");

    /* the same plane with the flag: the normal (-1024, -512, 1024) makes the
     * two endpoints straddle the triangle */
    cell_reset();
    *(u32 *)(lr_tile + 0x5E0) = 40u << 15;
    gTileShapeTable[40] = 0;
    gTileCornerIndices[40][1] = 1;
    gTileCornerIndices[40][2] = 2;
    a.x = 1000; a.y = -400; a.z = 2000;
    b.x = 3000; b.y = 400; b.z = 8000;
    outA[0] = -1; outA[1] = -1; outA[2] = -1; outB = -1;
    FCK_EQ(gTestMapCellRay(&a, &b, outA, &outB, 1), 0x2A05, "a plane hit returns kind 5 and material 0x2A");
    FCK_EQ(outB, 8000, "a plane hit reports the z of the endpoint that was kept");
    FCK_EQ(outA[0], 400, "the plane hit y");
    FCK_EQ(outA[1], 8000, "the plane hit z");
    FCK_EQ(outA[2], 3000, "the plane hit x");
    FCK_EQ(gWallHitPolys[0], 0, "a plane hit records no wall candidate");

    /* a diagonal wall: p0 = (1000 * 5000 + 10192 * 5000) / 10000 = 5596 */
    cell_reset();
    *(u32 *)(lr_tile + 0x5E0) = 81u << 15;
    gTileShapeTable[81] = (6 << 3) | 3;
    gTileCornerIndices[81][1] = 1;
    gTileCornerIndices[81][2] = 2;
    gTileCornerIndices[81][3] = 3;
    a.x = 1000; a.y = -2000; a.z = 8000;
    b.x = 6000; b.y = 3000; b.z = 8000;
    FCK_EQ(gTestMapCellRay(&a, &b, outA, &outB, 0), 0x2A03, "a diagonal hit returns kind 3 and the material");
    FCK_EQ(gCellRayWallHit.x, 5596, "the diagonal crossing is p0");
    FCK_EQ(gCellRayWallHit.y, 2596, "the crossing y is 8192 - p0");
    FCK_EQ(gCellRayWallHit.z, 8000, "the crossing keeps the segment z");
    FCK_EQ(gWallHitPositions[0].x, 5596, "the diagonal candidate x");
    FCK_EQ(gWallHitPositions[0].y, 2596, "the diagonal candidate y");
    FCK_EQ(gWallHitKinds[0], 3, "the diagonal candidate kind");
    FCK_EQ(gWallHitTopHeights[0], 9692, "shape 6 shifts the corners: high = (0x10 << 8) + (0x20 * 5596 >> 5)");
    FCK_EQ(outB, 9692, "the diagonal top feeds outB");
    FCK_EQ(outA[0], 2596, "the diagonal outA y");
    FCK_EQ(outA[2], 5596, "the diagonal outA x");

    /* two candidates: both are recorded, neither beats the corner baseline,
     * so the last one stays selected */
    cell_reset();
    *(u16 *)(lr_tile + 0x3CC) = (0x10 << 4) | 2;
    *(u32 *)(lr_tile + 0x5E0) = 80u << 15;
    *(u32 *)(lr_tile + 0x5E4) = 80u << 15;
    gTileShapeTable[80] = (11 << 3) | 1;
    gTileCornerIndices[80][1] = 1;
    gTileCornerIndices[80][2] = 2;
    gTileCornerIndices[80][3] = 3;
    a.x = 1000; a.y = -400; a.z = 8000;
    b.x = 3000; b.y = 400; b.z = 8000;
    FCK_EQ(gTestMapCellRay(&a, &b, outA, &outB, 0), 0x2A01, "two identical walls keep the last");
    FCK_EQ(gWallHitPolys[0], poly, "the first candidate is recorded");
    FCK_EQ(gWallHitPolys[1], poly, "the second candidate is recorded");
    FCK_EQ(gWallHitKinds[1], 1, "the second candidate kind");
    FCK_EQ(outB, 9692, "outB comes from the selected candidate");

    /* optional outputs may be null */
    cell_reset();
    *(u16 *)(lr_tile + 0x3CC) = (0x10 << 4) | 1;
    *(u32 *)(lr_tile + 0x5E0) = 80u << 15;
    gTileShapeTable[80] = (11 << 3) | 1;
    gTileCornerIndices[80][1] = 1;
    gTileCornerIndices[80][2] = 2;
    gTileCornerIndices[80][3] = 3;
    a.x = 1000; a.y = -400; a.z = 8000;
    b.x = 3000; b.y = 400; b.z = 8000;
    FCK_EQ(gTestMapCellRay(&a, &b, 0, 0, 0), 0x2A01, "null outputs still return the code");
    FCK_EQ(gWallHitPolys[0], poly, "null outputs still record the candidate");
    (void)i;
}

int main(void)
{
    test_compound_visual_reset();
    test_resupply_proximity();
    test_status_bar();
    test_priority_voice();
    test_vibration_toggle();
    test_gouraud_disc();
    test_particle_emitter();
    test_spatial_attenuation();
    test_place_player_views();
    test_attack_definition();
    test_registry_load();
    test_heap_allocator();
    test_path_mover_vertical();
    test_map_cell_ray();
    return lk_done("leaf_runtime");
}
