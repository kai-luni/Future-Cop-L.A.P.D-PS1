/* Leaf-function suite leaf_follow_b: 20 further original single-object
 * functions, host-run under qemu-mipsel against synthetic state.
 *
 * Conventions are those of leaf_a1/a2 and leaf_hundred_*: every D_ symbol
 * the units reference is defined here as plain storage bound to the retail
 * label (or as a pointer slot aimed at synthetic storage), every test
 * explicitly writes the state it depends on before calling the unit, and no
 * check relies on a slot that just happens to be zero. Expected values never
 * come from running the unit: fixed-point results were recomputed
 * independently from the operation semantics (16.16 middle-product
 * arithmetic, the 4.12 octave recurrence), linear structures follow from
 * the list/array logic confirmed in the units' own descriptions, and no
 * retail table content is used - the sine table below is a hand-crafted
 * synthetic quarter wave, not retail data.
 */
#include "leaf_kit.h"

/* ------------------------------------------------- globals the units use */
#define DGLOBAL(label, words) u32 label##_[words] __asm__(#label) = {0}
DGLOBAL(D_800A7938, 0x1C * 3);  /* sound bank array, 0x1C-byte entries */
DGLOBAL(D_800A0B70, 0x18);      /* stream buffer list: pad[0x10], Node* head */
DGLOBAL(D_800A0C70, 8);          /* gSlotSpawnDescs pointer slots + item scratch */
DGLOBAL(D_8009F9B0, 1);         /* gStreamPreemptFlag */
DGLOBAL(D_800A1920, 0x20);      /* gPositionAngleTable, 16-byte entries */
DGLOBAL(D_8009FA40, 1);         /* gUnitListHead */
DGLOBAL(D_800A2290, 8);         /* gDemoCameraStates, 8-byte rows */
DGLOBAL(D_8009F9F0, 1);         /* gPrimPoolNext */
DGLOBAL(D_8009FA28, 1);         /* gPrimPoolEnd */
DGLOBAL(D_8009FA0C, 1);         /* gPlayerCount */
DGLOBAL(D_8009EEA4, 2);         /* gSpuRegBasePtr slot */
DGLOBAL(D_8009EECC, 1);         /* gSpuAddrShift */
DGLOBAL(D_8009DD90, 1);         /* gGpuDrawOffset */
DGLOBAL(D_8009C808, 2);         /* interrupt status state ptr slot */
DGLOBAL(D_8009F360, 1);         /* gRampFillCounter (ramp reject bound) */
DGLOBAL(D_800A1E10, 0x200);     /* gTextColorRampTable rows: 0x10 u16 per row */
DGLOBAL(D_8009FAC8, 1);         /* gFrameSwitch (portrait input) */
DGLOBAL(D_8009F9B4, 1);         /* gPortraitStreamId (portrait input) */

/* storage the pointer globals are aimed at */
static u16 spu_regs[64];
static s32 intr_state[4];
static s16 sio_ctrl_state[8];   /* { char pad[10]; volatile short value; } */

/* u16 ramp rows live inside D_800A1E10_ itself (unit treats it as an array) */

/* ------------------------------------------------------------ prototypes */
void gSinCos(u32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");
void gMulMatrixVector(volatile s32 *matrix, volatile s32 *input, s32 *output) __asm__("func_8001A81C");
void gVectorAddScaled(s32 *a, s32 *b, s32 scale, s32 *out) __asm__("func_8001ACA4");
u16 gConvertSpuNoteToPitch(u16 a0, u16 a1, u16 a2, u16 a3) __asm__("func_800999F4");
u32 gConvertSpuPitchToNote(u16 arg0, u16 arg1, u16 arg2) __asm__("func_80099B2C");
s32 gRemoveSoundProgramSample(s32 bankIndex, u8 key) __asm__("func_800242B4");
void gRecycleModuleReadBuffer(void *node) __asm__("func_800288BC");
u8 gGetSlotSpawnByte(u8 index, u8 slot, s32 mode) __asm__("func_8003B5AC");
void gSetType34StreamState(void *object, s16 value) __asm__("func_80041DC4");
void gStepPortraitScreen(void *obj) __asm__("func_8004228C");
void gEmitTexturedQuadPacket(s32 x, u8 *points, u8 *flags) __asm__("func_80042004");
int gProjectWorldPointAndExtent(s16 *p) __asm__("func_80049F4C");
void gBuildColorRampRow(s32 index, s32 blue, s32 green, s32 red) __asm__("func_800488A4");
void gSetPositionAngleEntry(s32 *position, s32 arg1, s32 angle, s32 index) __asm__("func_80045884");
struct Entity;
struct Entity *gFindUnitById(s32 id) __asm__("func_8004D664");
u8 gStepDemoCameraTable(void *state, s32 elapsed) __asm__("func_80056994");
void gSpuWriteRegister(s32 index, u32 value, s32 flag) __asm__("func_800977C4");
int gReadSpuRegister(int index, int mode) __asm__("func_800978AC");
int gGetGpuDrawOffset(void) __asm__("func_80091F74");
int gCheckIrqBit0Persistent(void) __asm__("func_8008DA7C");

/* u16 sine array bound to D_8009A234 (unit 8001846C reads it as u16[]). */
u16 gSineTableFollowB[0x401] __asm__("D_8009A234");

int main(void)
{
    s32 i;
    u32 *w;

    /* point pointer-typed globals at their synthetic targets */
    w = (u32 *)D_8009EEA4_;  w[0] = (u32)spu_regs;
    w = (u32 *)D_8009C808_;  w[0] = (u32)intr_state;
    D_8009EECC_[0] = 5;                 /* gSpuAddrShift */

    /* ================================================== func_8001846C */
    /* @leaf 8001846C source sine lookup with quadrant dispatch */
    LK_FUNC("func_8001846C");
    {
        s32 sine, cosine;
        for (i = 0; i <= 0x400; i++) gSineTableFollowB[i] = 0;
        gSineTableFollowB[0x155] = 2045;
        gSineTableFollowB[0x2AB] = 3547;   /* 0x400 - 0x155 */
        gSineTableFollowB[0x400] = 4095;
        gSinCos(0x155, &sine, &cosine);   /* q0: sine=s, cosine=c */
        FCK_EQ(sine, 4090, "sincos q0 sine");
        FCK_EQ(cosine, 7094, "sincos q0 cosine");
        gSinCos(0x000, &sine, &cosine);
        FCK_EQ(sine, 0, "sincos zero sine");
        FCK_EQ(cosine, 8190, "sincos zero cosine");
        gSinCos(0x555, &sine, &cosine);   /* q1: sine=c, cosine=-s */
        FCK_EQ(sine, 7094, "sincos q1 sine");
        FCK_EQ(cosine, -4090, "sincos q1 cosine");
        /* q2: idx 0x2AA, complementary index 0x156 */
        gSineTableFollowB[0x2AA] = 1700;
        gSineTableFollowB[0x156] = 2398;
        gSinCos(0xAAA, &sine, &cosine);   /* q2: sine=-s, cosine=-c */
        FCK_EQ(sine, -3400, "sincos q2 sine");
        FCK_EQ(cosine, -4796, "sincos q2 cosine");
        gSineTableFollowB[0x3FF] = 4094;
        gSinCos(0xFFF, &sine, &cosine);   /* q3: sine=-c, cosine=s */
        FCK_EQ(sine, 0, "sincos q3 sine");
        FCK_EQ(cosine, 8188, "sincos q3 cosine");
    }

    /* ================================================== func_8001A81C */
    /* @leaf 8001A81C source column-major 16 */
    LK_FUNC("func_8001A81C");
    {
        static s32 m[9] = { 0x10000, 0x20000, 0x30000,
                           0x40000, 0x50000, 0x60000,
                           0x70000, 0x80000, 0x90000 };
        static s32 v[3] = { 2, 3, 5 };
        static s32 m2[9] = { -0x10000, -0x20000, 0x30000,
                             0x40000, 10, 0,
                             1, 2, 0x10000 };
        static s32 v2[3] = { -5, 7, 100 };
        s32 out[3];
        lk_fill(out, 0xCC, sizeof(out));
        gMulMatrixVector(m, v, out);
        FCK_EQ(out[0], 0x31, "matvec out0");
        FCK_EQ(out[1], 0x3B, "matvec out1");
        FCK_EQ(out[2], 0x45, "matvec out2");
        gMulMatrixVector(m2, v2, out);
        FCK_EQ(out[0], 0x21, "matvec neg out0");
        FCK_EQ(out[1], 0x0A, "matvec neg out1");
        FCK_EQ(out[2], 0x55, "matvec neg out2");
    }

    /* ================================================== func_8001ACA4 */
    /* @leaf 8001ACA4 source scaled vector addition out = a + mid32(b*scale), aliasing... */
    LK_FUNC("func_8001ACA4");
    {
        static s32 a[3] = { 100, -200, 300 };
        static s32 b[3] = { 1 << 8, -(2 << 8), 7 << 12 };
        s32 out[3];
        gVectorAddScaled(a, b, 0x18000, out);
        FCK_EQ(out[0], 0x1E4, "vadd out0");
        FCK_EQ(out[1], -0x3C8, "vadd out1");
        FCK_EQ(out[2], 0xA92C, "vadd out2");
        gVectorAddScaled(a, b, 0, out);
        FCK_EQ(out[0], 100, "vadd scale0 out0");
        FCK_EQ(out[1], -200, "vadd scale0 out1");
        FCK_EQ(out[2], 300, "vadd scale0 out2");
        {
            s32 av[3] = { 1, 2, 3 };
            static s32 bv[3] = { 0x10000, 0x10000, 0x10000 };
            /* mid32(0x10000 * 1) = 1: each step adds exactly one */
            gVectorAddScaled(av, bv, 1, av);        /* a == out aliasing */
            FCK_EQ(av[0], 2, "vadd alias out0");
            FCK_EQ(av[1], 3, "vadd alias out1");
            FCK_EQ(av[2], 4, "vadd alias out2");
        }
    }

    /* ================================================== func_800999F4 */
    /* @leaf 800999F4 source bounded fixed-point note->pitch ratio */
    LK_FUNC("func_800999F4");
    {
        FCK_EQ((s32)gConvertSpuNoteToPitch(10, 0, 10, 0), 0x1000, "n2p same note");
        FCK_EQ((s32)gConvertSpuNoteToPitch(8, 0, 10, 0), 0x11ED, "n2p two up");
        FCK_EQ((s32)gConvertSpuNoteToPitch(10, 0, 8, 0), 0x0E20, "n2p two down");
        FCK_EQ((s32)gConvertSpuNoteToPitch(10, 3, 10, 0x25), 0x103E, "n2p fine up");
        FCK_EQ((s32)gConvertSpuNoteToPitch(2, 40, 3, 0), 0x10A3, "n2p cross fine");
        FCK_EQ((s32)gConvertSpuNoteToPitch(0, 0, 40, 0), 0x3FFF, "n2p capped");
    }

    /* ================================================== func_80099B2C */
    /* @leaf 80099B2C source inverse interval search over 48 groups of 32 steps of the 4 */
    LK_FUNC("func_80099B2C");
    {
        FCK_EQ((s32)gConvertSpuPitchToNote(0, 0, 0x1000), 0x0, "p2n unity");
        FCK_EQ((s32)gConvertSpuPitchToNote(0, 0, 0x11ED), 0x200, "p2n two up");
        FCK_EQ((s32)gConvertSpuPitchToNote(0, 0, 0x103E), 0x22, "p2n fine");
        FCK_EQ((s32)gConvertSpuPitchToNote(0, 0, 0x10A3), 0x58, "p2n cross");
        /* (6,9,0x0AA1): top bit 11 -> octave -1, index 640 -> q=5, r=0,
           return (6+5-12)<<8 | 9 = 0xFFFFFF09 = -247 */
        FCK_EQ((s32)gConvertSpuPitchToNote(6, 9, 0x0AA1), -247, "p2n octave -1");
        FCK_EQ((s32)gConvertSpuPitchToNote(0, 0, 3), (s32)-30720, "p2n tiny");
        FCK_EQ((s32)gConvertSpuPitchToNote(2, 3, 0x2A00), 0x1267, "p2n octave +1");
        /* top-bit-0 input (arg2 = 1): no interval matches (all lower bounds
           equal arg2), so the default index 0x600 applies: q=12, r=0,
           return (3+12-144)<<8 | 4 = -33020 = 0xFFFF7F04 */
        FCK_EQ((s32)gConvertSpuPitchToNote(3, 4, 1), -33020, "p2n smallest input hits default");
        /* first-interval case where the lower bound equals arg2: index 1
           with q=0, r=1 -> (0<<8)|1 = 1 */
        FCK_EQ((s32)gConvertSpuPitchToNote(0, 0, 0x1001), 1, "p2n exact lower bound");
    }

    /* ================================================== func_800242B4 */
    /* @leaf 800242B4 source swap-with-last sound-bank removal */
    LK_FUNC("func_800242B4");
    {
        u8 *banks = (u8 *)D_800A7938_;
        s32 r;
        banks[0x00] = 0x11; banks[0x04] = 0x22;
        banks[0x08] = 0x33; banks[0x0C] = 0x44;
        banks[0x10] = 4; banks[0x11] = 0;
        r = gRemoveSoundProgramSample(0, 0x22);
        FCK_EQ(r, 1, "bank remove hit");
        FCK_EQ((s32)banks[0x10], 3, "bank count decremented");
        FCK_EQ((s32)banks[0x04], 0x44, "last entry swapped into hole");
        r = gRemoveSoundProgramSample(0, 0x99);
        FCK_EQ(r, 0, "bank remove miss");
        FCK_EQ((s32)banks[0x10], 3, "bank count unchanged on miss");
        banks[0x00] = 0x11; banks[0x04] = 0x22; banks[0x08] = 0x33;
        banks[0x10] = 3;
        r = gRemoveSoundProgramSample(0, 0x33);        /* already last place */
        FCK_EQ(r, 1, "bank remove self-tail hit");
        FCK_EQ((s32)banks[0x10], 2, "bank count after self-tail remove");
        FCK_EQ((s32)banks[0x08], 0x33, "self-tail remove leaves slot");
        /* key only present beyond count: no match may happen (the read-only
           loop bound is exclusive) */
        banks[0x00] = 0x11; banks[0x04] = 0x22; banks[0x08] = 0x99;
        banks[0x10] = 2;
        r = gRemoveSoundProgramSample(0, 0x99);
        FCK_EQ(r, 0, "bank key beyond count is invisible");
        FCK_EQ((s32)banks[0x10], 2, "bank count kept when key beyond count");
        banks[0x1C] = 0x77;              /* second bank marker */
        r = gRemoveSoundProgramSample(0, 0x11);
        FCK_EQ((s32)banks[0x1C], 0x77, "bank 1 untouched");
    }

    /* ================================================== func_800288BC */
    /* @leaf 800288BC source buffer recycle */
    LK_FUNC("func_800288BC");
    {
        struct { u32 next; s32 unk4; s32 unk8; s32 count; } n1, n2, h, t;
        u32 *head = (u32 *)(D_800A0B70_ + 4);
        /* callers hand in detached nodes (next == 0): the unit only clears
           the count and appends; it never touches node->next itself */
        n1.next = 0; n1.count = 12;
        n2.next = 0; n2.count = 9;
        head[0] = 0;
        gRecycleModuleReadBuffer(&n1);
        FCK_EQ((s32)head[0], (s32)(u32)&n1, "recycle first becomes head");
        FCK_EQ((s32)n1.count, 0, "recycle first count cleared");
        FCK_EQ((s32)n1.next, 0, "recycle first stays detached");
        gRecycleModuleReadBuffer(&n2);
        FCK_EQ((s32)n1.next, (s32)(u32)&n2, "recycle second appended");
        FCK_EQ((s32)n2.count, 0, "recycle second count cleared");
        FCK_EQ((s32)n2.next, 0, "recycle second stays detached");
        h.next = (u32)&t; h.count = 1;
        t.next = 0; t.count = 2;
        head[0] = (u32)&h;
        gRecycleModuleReadBuffer(&n1);
        FCK_EQ((s32)t.next, (s32)(u32)&n1, "recycle appended after tail");
        FCK_EQ((s32)head[0], (s32)(u32)&h, "recycle head unchanged");
    }

    /* ================================================== func_8003B5AC */
    /* @leaf 8003B5AC source mode-dispatched spawn byte via the D_800A0C70 descriptor table */
    LK_FUNC("func_8003B5AC");
    {
        u8 *items = (u8 *)D_800A0C70_ + 8;
        u32 *slots = (u32 *)D_800A0C70_;
        items[0] = 0x11; items[1] = 0x22; items[2] = 0x33; items[3] = 0x44;
        items[4] = 0x55; items[5] = 0x66; items[6] = 0x77; items[7] = 0x88;
        slots[0] = (u32)(items - 4);
        slots[1] = 0;                       /* null descriptor */
        FCK_EQ((s32)gGetSlotSpawnByte(0, 0, 0), 0x44, "spawn mode0 unk3");
        FCK_EQ((s32)gGetSlotSpawnByte(0, 1, 0), 0x88, "spawn mode0 slot1");
        FCK_EQ((s32)gGetSlotSpawnByte(0, 0, 1), 0x33, "spawn mode1 unk2");
        FCK_EQ((s32)gGetSlotSpawnByte(0, 1, 1), 0x77, "spawn mode1 slot1");
        FCK_EQ((s32)gGetSlotSpawnByte(0, 0, 2), 0x22, "spawn mode2 unk1");
        FCK_EQ((s32)gGetSlotSpawnByte(0, 0, 3), 0, "spawn mode3 zero");
        FCK_EQ((s32)gGetSlotSpawnByte(0, 0, 9), 0x33, "spawn default unk2");
        FCK_EQ((s32)gGetSlotSpawnByte(1, 0, 0), 0, "spawn null descriptor");
    }

    /* ================================================== func_80041DC4 */
    /* @leaf 80041DC4 source conditionally updates a kind-0x22 object */
    LK_FUNC("func_80041DC4");
    {
        struct { u8 pad1[0x16]; u8 kind; u8 pad2[0x0D]; u32 flags;
                 u8 pad3[0x78]; u8 stateA0; u8 pad4[3]; s16 valueA4; } o;
        lk_fill(&o, 0, sizeof(o));
        o.kind = 0x99; o.flags = 0xF0F0; o.stateA0 = 0x40;
        D_8009F9B0_[0] = 5;
        gSetType34StreamState(&o, 1234);
        FCK_EQ((s32)D_8009F9B0_[0], 5, "kind mismatch preempt unchanged");
        FCK_EQ((s32)o.flags, (s32)0xF0F0, "kind mismatch flags unchanged");
        FCK_EQ((s32)o.stateA0, 0x40, "kind mismatch stateA0 unchanged");
        FCK_EQ((s32)o.valueA4, 0, "kind mismatch valueA4 unchanged");
        o.kind = 0x22;
        gSetType34StreamState(&o, 1234);
        FCK_EQ((s32)o.valueA4, 1234, "stream state value stored");
        FCK_EQ((s32)D_8009F9B0_[0], 6, "preempt flag bumped");
        FCK_EQ((s32)o.flags, (s32)0xF0F8, "flags bit 3 set");
        FCK_EQ((s32)o.stateA0, 0x41, "stateA0 bit 0 set");
    }

    /* ================================================== func_8004228C */
    /* @leaf 8004228C source portrait screen step reading gFrameSwitch (D_8009FAC8) and... */
    LK_FUNC("func_8004228C");
    {
        struct { u8 pad1[0x24]; u32 flags; u8 pad2[0x78];
                 u8 unkA0; u8 pad3[3]; s16 unkA4; } o;
        lk_fill(&o, 0, sizeof(o));
        o.flags = 0xFF; o.unkA0 = 0x40;
        D_8009FAC8_[0] = 1;               /* active frame switch */
        D_8009F9B0_[0] = 5;
        gStepPortraitScreen(&o);
        FCK_EQ((s32)o.unkA0, 0x40, "portrait bit0 clear no change");
        FCK_EQ((s32)o.flags, 0xFF, "portrait flags unchanged");
        /* teardown: bit0|bit1 set, frame switch inactive */
        lk_fill(&o, 0, sizeof(o));
        o.flags = 0x08; o.unkA0 = 0x03; o.unkA4 = 7;
        D_8009FAC8_[0] = 0;
        D_8009F9B4_[0] = 7;
        D_8009F9B0_[0] = 5;
        gStepPortraitScreen(&o);
        FCK_EQ((s32)o.unkA4, -10, "portrait teardown stream id reset");
        FCK_EQ((s32)o.unkA0, 0, "portrait teardown bits cleared");
        FCK_EQ((s32)o.flags, 0, "portrait teardown flag bit cleared");
        FCK_EQ((s32)D_8009F9B0_[0], 4, "portrait preempt flag decremented");
        /* activation: bit1 clear, frame switch active -> only bit1 set */
        lk_fill(&o, 0, sizeof(o));
        o.flags = 0x08; o.unkA0 = 0x01; o.unkA4 = 7;
        D_8009FAC8_[0] = 1;
        D_8009F9B4_[0] = 7;
        D_8009F9B0_[0] = 5;
        gStepPortraitScreen(&o);
        FCK_EQ((s32)o.unkA0, 0x03, "portrait activation bit1 set");
        FCK_EQ((s32)o.unkA4, 7, "portrait activation id kept");
        FCK_EQ((s32)D_8009F9B0_[0], 5, "portrait activation preempt unchanged");
        lk_fill(&o, 0, sizeof(o));
        o.flags = 0x08; o.unkA0 = 0x03; o.unkA4 = 9;
        D_8009FAC8_[0] = 1;        /* frame switch active BUT id mismatch */
        D_8009F9B4_[0] = 7;
        D_8009F9B0_[0] = 5;
        gStepPortraitScreen(&o);
        FCK_EQ((s32)o.unkA4, -10, "portrait active mismatch tears down");
        FCK_EQ((s32)o.unkA0, 0, "portrait active mismatch bits cleared");
        FCK_EQ((s32)D_8009F9B0_[0], 4, "portrait active mismatch preempt decrement");
        /* active stream but id mismatch with bit1 clear: activation branch
           runs first (active -> bit1 set, no teardown this frame) */
        lk_fill(&o, 0, sizeof(o));
        o.flags = 0x08; o.unkA0 = 0x01; o.unkA4 = 9;
        D_8009FAC8_[0] = 1;
        D_8009F9B4_[0] = 3;        /* mismatch */
        D_8009F9B0_[0] = 5;
        gStepPortraitScreen(&o);
        FCK_EQ((s32)o.unkA0, 0x03, "portrait mismatch still activates");
        FCK_EQ((s32)D_8009F9B0_[0], 5, "portrait no teardown when active");
    }

    /* ================================================== func_80042004 */
    /* @leaf 80042004 source POLY_FT4 quad packet emission */
    LK_FUNC("func_80042004");
    {
        u8 memory[0x100];
        u8 points[0x40];
        u8 flags[8];
        u32 *mem32 = (u32 *)memory;
        s32 x;
        lk_fill(memory, 0xEE, sizeof(memory));
        lk_fill(points, 0, sizeof(points));
        *(s16 *)(points + 4)  = 100;
        *(s16 *)(points + 12) = 200;
        *(s16 *)(points + 36) = 300;
        *(s16 *)(points + 28) = 400;
        *(u32 *)(points + 0)  = 0x00110022;
        *(u32 *)(points + 8)  = 0x00330044;
        *(u32 *)(points + 24) = 0x00550066;
        *(u32 *)(points + 32) = 0x00770088;
        flags[0] = 0xC0; flags[1] = 0x40; flags[3] = 0x40; flags[4] = 0x40;
        D_8009F9F0_[0] = (u32)memory;
        D_8009FA28_[0] = (u32)(memory + sizeof(memory));
        x = 5;
        gEmitTexturedQuadPacket(x, points, flags);
        FCK_EQ((s32)D_8009F9F0_[0], (s32)(u32)(memory + 0x34), "pool advanced by 0x34");
        FCK_EQ((s32)mem32[1], (s32)0x2C808080, "packet code");
        /* tag: (100+200+300+400)>>2 = 250, minus 0x180 = -134 -> 0xFFFF7A
           under the 24-bit mask; the canary top byte 0xEE is kept by
           (tag & 0xff000000), then the unit's byte-3 store (little-endian
           top byte) overwrites it with 9: 0x09FFFF7A */
        FCK_EQ((s32)mem32[0], (s32)0x09FFFF7A, "packet tag from average");
        FCK_EQ((s32)memory[3], 9, "packet top tag byte");
        FCK_EQ((s32)*(u16 *)(memory + 22), 0x11F, "packet v1");
        FCK_EQ((s32)*(u16 *)(memory + 14), 0, "packet v0");
        FCK_EQ((s32)*(u16 *)(memory + 12), x, "packet u0");
        FCK_EQ((s32)*(u16 *)(memory + 20), x + 0x1F, "packet u1");
        FCK_EQ((s32)*(u16 *)(memory + 28), x + 0xB00, "packet u2");
        FCK_EQ((s32)*(u16 *)(memory + 36), x + 0xB1F, "packet u3");
        FCK_EQ((s32)mem32[2], (s32)0x00110022, "packet xy0 copied");
        FCK_EQ((s32)mem32[4], (s32)0x00330044, "packet xy1 copied");
        FCK_EQ((s32)mem32[6], (s32)0x00550066, "packet xy2 copied");
        FCK_EQ((s32)mem32[8], (s32)0x00770088, "packet xy3 copied");
        /* non-matching flags (bit 0 set): no write, no advance */
        lk_fill(memory, 0xEE, sizeof(memory));
        flags[0] = 0x45;
        D_8009F9F0_[0] = (u32)memory;
        gEmitTexturedQuadPacket(x, points, flags);
        FCK_EQ((s32)D_8009F9F0_[0], (s32)(u32)memory, "pool unchanged on flag mismatch");
        FCK_EQ((s32)mem32[1], (s32)0xEEEEEEEE, "no packet on flag mismatch");
        /* pool exhausted: writes at the head but does not advance */
        lk_fill(memory, 0xEE, sizeof(memory));
        flags[0] = 0xC0;
        D_8009F9F0_[0] = (u32)memory;
        D_8009FA28_[0] = (u32)memory;
        gEmitTexturedQuadPacket(x, points, flags);
        FCK_EQ((s32)D_8009F9F0_[0], (s32)(u32)memory, "pool not advanced when exhausted");
        FCK_EQ((s32)mem32[1], (s32)0x2C808080, "packet still written when exhausted");
    }

    /* ================================================== func_80049F4C */
    /* @leaf 80049F4C source world->screen projection */
    LK_FUNC("func_80049F4C");
    {
        s16 p[6];
        int r;
        p[0] = 100; p[1] = -50; p[2] = 200; p[4] = 160; p[5] = -160;
        D_8009FA0C_[0] = 2;                 /* wide mode */
        r = gProjectWorldPointAndExtent(p);
        FCK_EQ(r, 0, "projection return code");
        FCK_EQ((s32)p[0], 671, "projection screen x wide");
        FCK_EQ((s32)p[1], 495, "projection screen y");
        FCK_EQ((s32)p[4], 819, "projection extent8");
        FCK_EQ((s32)p[5], -820, "projection extent10");
        p[0] = 100; p[1] = -50; p[2] = 200; p[4] = 0; p[5] = 0;
        D_8009FA0C_[0] = 1;                 /* single player */
        r = gProjectWorldPointAndExtent(p);
        FCK_EQ((s32)p[0], 831, "projection screen x narrow");
        p[0] = 100; p[1] = -50; p[2] = 64; p[4] = 0; p[5] = 0;
        D_8009FA0C_[0] = 2;
        r = gProjectWorldPointAndExtent(p);
        FCK_EQ(r, 1, "projection behind-camera rejected");
        p[0] = 500; p[1] = 250; p[2] = 330; p[4] = -320; p[5] = 320;
        D_8009FA0C_[0] = 2;
        r = gProjectWorldPointAndExtent(p);
        FCK_EQ((s32)p[0], 1711, "projection screen x second case");
        FCK_EQ((s32)p[1], -536, "projection screen y second case");
        FCK_EQ((s32)p[4], -993, "projection extent8 second case");
        FCK_EQ((s32)p[5], 992, "projection extent10 second case");
        p[0] = 10; p[1] = -20; p[2] = 65; p[4] = 5; p[5] = 5;
        D_8009FA0C_[0] = 2;
        r = gProjectWorldPointAndExtent(p);
        FCK_EQ((s32)p[0], 317, "projection screen x z65");
        FCK_EQ((s32)p[1], 555, "projection screen y z65");
    }

    /* ================================================== func_800488A4 */
    /* @leaf 800488A4 source colour ramp row build */
    LK_FUNC("func_800488A4");
    {
        u16 *rows = (u16 *)D_800A1E10_;
        D_8009F360_[0] = 4;                  /* gRampFillCounter */
        lk_fill(rows, 0x5A, 0x200 * 4);
        gBuildColorRampRow(2, 0x80, 0x40, 0x20);
        FCK_EQ((s32)rows[2 * 16 + 0], 0, "ramp row zero");
        FCK_EQ((s32)rows[2 * 16 + 1], (s32)0xC104, "ramp first entry");
        FCK_EQ((s32)rows[2 * 16 + 2], (s32)0xE186, "ramp second entry");
        FCK_EQ((s32)rows[2 * 16 + 6], (s32)0xE38E, "ramp sixth entry");
        FCK_EQ((s32)rows[2 * 16 + 7], (s32)0x0410, "ramp seventh entry");
        FCK_EQ((s32)rows[2 * 16 + 15], (s32)0x0820, "ramp last entry");
        /* reject: index above the fill counter leaves the row untouched */
        lk_fill(rows, 0x5A, 0x200 * 4);
        gBuildColorRampRow(5, 1, 2, 3);
        FCK_EQ((s32)rows[5 * 16 + 0], (s32)0x5A5A, "ramp reject index too high");
        /* reject: negative index */
        gBuildColorRampRow(-1, 1, 2, 3);
        FCK_EQ((s32)rows[0], (s32)0x5A5A, "ramp reject negative index");
        /* boundary: index == fill counter still builds */
        lk_fill(rows, 0x5A, 0x200 * 4);
        gBuildColorRampRow(4, 1, 2, 3);
        FCK_EQ((s32)rows[4 * 16 + 0], 0, "ramp boundary row zero");
        /* boundary: index == 0 is valid too */
        lk_fill(rows, 0x5A, 0x200 * 4);
        gBuildColorRampRow(0, 0x40, 0x20, 0x10);
        FCK_EQ((s32)rows[0], 0, "ramp row-zero entry zero");
        FCK_EQ((s32)rows[0 * 16 + 1], (s32)0xA082, "ramp row-zero first entry");
        FCK_EQ((s32)rows[0 * 16 + 15], (s32)0x0410, "ramp row-zero last entry");
    }

    /* ================================================== func_80045884 */
    /* @leaf 80045884 source position/angle entry writer */
    LK_FUNC("func_80045884");
    {
        s32 pos[4] = { 111, -222, 333, 777 };
        s32 *table = (s32 *)D_800A1920_;
        lk_fill(table, 0, 0x20 * 4);
        gSetPositionAngleEntry(pos, 0xF00F, 0x1FF, 2);
        FCK_EQ(table[2 * 4 + 0], 111, "posangle pos0");
        FCK_EQ(table[2 * 4 + 1], -222, "posangle pos1");
        FCK_EQ(table[2 * 4 + 2], 333, "posangle pos2");
        FCK_EQ(table[2 * 4 + 3], 0x1FF, "posangle angle wrapped");
        FCK_EQ(table[3 * 4 + 0], 0, "posangle neighbour untouched");
        gSetPositionAngleEntry(pos, 0, 0x2FFF, 2);
        FCK_EQ(table[2 * 4 + 3], 0xFFF, "posangle wrap second");
        gSetPositionAngleEntry(pos, 0, -3, 2);
        FCK_EQ(table[2 * 4 + 3], 0xFFD, "posangle negative angle");
    }

    /* ================================================== func_8004D664 */
    /* @leaf 8004D664 source entity id search */
    LK_FUNC("func_8004D664");
    {
        struct { u32 next; u32 update; s32 id; } e1, e2, e3;
        e1.next = (u32)&e2; e1.update = 0; e1.id = 10;
        e2.next = (u32)&e3; e2.update = 0; e2.id = 20;
        e3.next = 0;             e3.update = 0; e3.id = 30;
        D_8009FA40_[0] = (u32)&e1;
        FCK_EQ((s32)(u32)gFindUnitById(10), (s32)(u32)&e1, "unit find first");
        FCK_EQ((s32)(u32)gFindUnitById(20), (s32)(u32)&e2, "unit find middle");
        FCK_EQ((s32)(u32)gFindUnitById(30), (s32)(u32)&e3, "unit find last");
        FCK_EQ((s32)(u32)gFindUnitById(99), 0, "unit find miss walks to null");
        D_8009FA40_[0] = 0;
        FCK_EQ((s32)(u32)gFindUnitById(10), 0, "unit find empty list");
    }

    /* ================================================== func_80056994 */
    /* @leaf 80056994 source demo camera table step */
    LK_FUNC("func_80056994");
    {
        u8 *rows = (u8 *)D_800A2290_;
        u8 st[0x314];
        u8 *sp = st;
        lk_fill(st, 0, sizeof(st));
        rows[1 * 8 + 1] = 0x64;
        rows[1 * 8 + 2] = 0x5A;
        *(s8 *)(sp + 0xC1) = 1;             /* side */
        *(s16 *)(sp + 0x2FE) = 4000;        /* timer */
        *(u8 *)(sp + 0x313) = 1;            /* step */
        FCK_EQ((s32)gStepDemoCameraTable(sp, 500), 0x64, "camera row byte");
        FCK_EQ((s32)*(s16 *)(sp + 0x2FE), 3500, "camera timer decremented");
        FCK_EQ((s32)*(u8 *)(sp + 0x313), 1, "camera step unchanged");
        /* exact-zero timer: not negative, no reset, no step bump */
        lk_fill(st, 0, sizeof(st));
        *(s8 *)(sp + 0xC1) = 1;
        *(s16 *)(sp + 0x2FE) = 100;
        *(u8 *)(sp + 0x313) = 1;
        FCK_EQ((s32)gStepDemoCameraTable(sp, 100), 0x64, "camera exact zero timer");
        FCK_EQ((s32)*(s16 *)(sp + 0x2FE), 0, "camera timer zero no reset");
        FCK_EQ((s32)*(u8 *)(sp + 0x313), 1, "camera zero no step bump");
        /* one unit below zero triggers the reset */
        FCK_EQ((s32)gStepDemoCameraTable(sp, 1), 0x5A, "camera one-past-zero wrap");
    }

    /* ================================================== func_800977C4 */
    /* @leaf 800977C4 source SPU register write through the base pointer at D_8009EEA4 */
    LK_FUNC("func_800977C4");
    {
        lk_fill(spu_regs, 0, sizeof(spu_regs));
        gSpuWriteRegister(3, 0x1234, 0);
        FCK_EQ((s32)spu_regs[3], 0x1234, "spu write raw");
        D_8009EECC_[0] = 5;
        gSpuWriteRegister(4, 0xABCDEF, 1);
        FCK_EQ((s32)spu_regs[4], 0x5E6F, "spu write shifted");
        FCK_EQ((s32)spu_regs[5], 0, "spu neighbour untouched");
    }

    /* ================================================== func_800978AC */
    /* @leaf 800978AC source SPU register read through D_8009EEA4 */
    LK_FUNC("func_800978AC");
    {
        lk_fill(spu_regs, 0, sizeof(spu_regs));
        spu_regs[7] = 0x1234;
        D_8009EECC_[0] = 5;
        FCK_EQ(gReadSpuRegister(7, -1), 0x1234, "spu read raw");
        FCK_EQ(gReadSpuRegister(7, 0), 0x24680, "spu read shifted");
        spu_regs[8] = 0xFFFF;
        FCK_EQ(gReadSpuRegister(8, 1), 0x1FFFE0, "spu read full-scale shift");
    }

    /* ================================================== func_80091F74 */
    /* @leaf 80091F74 source returns gGpuDrawOffset (D_8009DD90) unchanged; a negative... */
    LK_FUNC("func_80091F74");
    {
        D_8009DD90_[0] = 0x40;
        FCK_EQ(gGetGpuDrawOffset(), 0x40, "gpu offset first");
        D_8009DD90_[0] = (u32)-7;
        FCK_EQ(gGetGpuDrawOffset(), -7, "gpu offset second");
    }

    /* ================================================== func_8008DA7C */
    /* @leaf 8008DA7C source persistent-IRQ check through the state pointer at D_8009C808 */
    LK_FUNC("func_8008DA7C");
    {
        intr_state[0] = 1; intr_state[1] = 1;
        FCK_EQ(gCheckIrqBit0Persistent(), 1, "irq both bits set");
        intr_state[0] = 0; intr_state[1] = 1;
        FCK_EQ(gCheckIrqBit0Persistent(), 0, "irq previous clear");
        intr_state[0] = 1; intr_state[1] = 0;
        FCK_EQ(gCheckIrqBit0Persistent(), 0, "irq current clear");
        intr_state[0] = 0x10; intr_state[1] = 0x10;
        FCK_EQ(gCheckIrqBit0Persistent(), 0, "irq other bits ignored");
    }

    return lk_done("leaf_follow_b");
}
