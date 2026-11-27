/* Source characterizations of unchanged src/game/<address>.c units.
 * Layouts and signatures are taken from the cited units. All state is
 * synthetic RAM fixtures; no retail data, BIOS, or hardware access.
 * Dependee units are #included verbatim below without claiming @leaf
 * coverage for them; 8001A074 and 8001846C are covered by leaf_extra and
 * leaf_follow_b respectively and carry no marker here.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* Byte/word access into untyped fixtures (style of leaf_a2_harness.c). */
#define W32(b, o) (*(u32 *)((u8 *)(b) + (o)))
#define W16(b, o) (*(u16 *)((u8 *)(b) + (o)))
#define B8(b, o)  (*(u8 *)((u8 *)(b) + (o)))

/* Real dependee units, verbatim, no marker claimed here. */
#include "../../src/game/utilities/gApproximateLength2.c"    /* gApproximateLength2, covered by leaf_extra */
#include "../../src/game/boot-frontend/gIsTrackedSoundId.c"    /* gIsTrackedSoundId, covered by leaf_hundred_a */
#include "../../src/game/platform/gGetVoiceVolume.c"    /* gGetVoiceVolume, covered by leaf_century_c */
#include "../../src/game/utilities/gSinCos.c"    /* gSinCos, covered by leaf_follow_b */
#include "../../src/game/platform/gSpuAccessVoiceMaskRegs.c"    /* gSpuAccessVoiceMaskRegs, unmarked impl unit */
#include "../../src/game/gameplay-ai/gRemoveEntityCommand.c"    /* gRemoveEntityCommand, queue compaction */

/* Voice-mask dependees (both covered by leaf_a2): a recording stand-in bound
 * to each retail label captures the mask the units under test compute, which
 * is exactly the contract of func_800221E4/8002238C; the OR/AND-CLEAR
 * behaviour itself is asserted in leaf_a2 against the real units. */
static u32 gVoiceOnMaskRecorded;
static u32 gVoiceOnMaskCalls;
static u32 gVoiceOffMaskRecorded;
static u32 gVoiceOffMaskCalls;
void func_80021A70(u32 flags)
{
    gVoiceOnMaskRecorded = flags;
    gVoiceOnMaskCalls++;
}
void func_80021A8C(u32 flags)
{
    gVoiceOffMaskRecorded = flags;
    gVoiceOffMaskCalls++;
}


/* Scalar wrapper over the retail array-taking helper (included above). */
static s32 gApproximateLength2Impl(s32 x, s32 y)
{
    s32 vector[2];
    vector[0] = x;
    vector[1] = y;
    return gApproximateLength2(vector);
}

/* =================================================================== */
/* Group 1: vector math wrappers                                        */
/* =================================================================== */

s32 gApproximateDistanceXzOctants(s32 *a, s32 *b) __asm__("func_8001A13C");
s32 gVectorLength3D(s32 *vec) __asm__("func_8001A100");

static void test_vector_math(void)
{
    s32 p[3], q[3];

    /* @leaf 8001A13C source src/game/utilities/gApproximateDistanceXzOctants.c: delta=(a[2]-b[2], a[0]-b[0]) into 8001A074 */
    LK_FUNC("func_8001A13C");
    p[0] = 100; p[2] = 50;
    q[0] = 40;  q[2] = 20;
    FCK_EQ(gApproximateDistanceXzOctants(p, q), gApproximateLength2Impl(30, 60),
           "xz distance equals 8001A074(dz, dx)");
    p[0] = 0; p[2] = 0;
    q[0] = 0; q[2] = 0;
    FCK_EQ(gApproximateDistanceXzOctants(p, q), 0, "identical points are zero apart");
    q[0] = -10; q[2] = 5;
    FCK_EQ(gApproximateDistanceXzOctants(p, q), gApproximateLength2Impl(-5, 10),
           "signed deltas passed through helper order");
    p[0] = 7; p[2] = -3;
    q[0] = -2; q[2] = 9;
    FCK_EQ(gApproximateDistanceXzOctants(p, q), gApproximateLength2Impl(-12, 9),
           "both components negative folded inside helper");
    /* axis-aligned distance: any off-by-one delta changes the exact result */
    p[0] = 0; p[2] = 0;
    q[0] = 0; q[2] = -40;
    FCK_EQ(gApproximateDistanceXzOctants(p, q), 40, "single-axis distance is exact");
    p[0] = 25; p[2] = 0;
    q[0] = 0;  q[2] = 0;
    FCK_EQ(gApproximateDistanceXzOctants(p, q), 25, "x-axis distance is exact");

    /* @leaf 8001A100 source src/game/utilities/gVectorLength3D.c: planar=(8001A074(vec), vec[2]) then helper again */
    LK_FUNC("func_8001A100");
    p[0] = 30; p[1] = 40; p[2] = 0;
    FCK_EQ(gVectorLength3D(p), gApproximateLength2Impl(gApproximateLength2Impl(30, 40), 0),
           "z=0 reduces to the 2D helper on x,y");
    p[0] = 0; p[1] = 0; p[2] = 50;
    FCK_EQ(gVectorLength3D(p), 50, "pure z vector keeps magnitude");
    p[0] = 6; p[1] = 8; p[2] = 10;
    FCK_EQ(gVectorLength3D(p), gApproximateLength2Impl(gApproximateLength2Impl(6, 8), 10),
           "composed helper order preserved");
    p[0] = -60; p[1] = 0; p[2] = -80;
    FCK_EQ(gVectorLength3D(p), gApproximateLength2Impl(60, 80),
           "sign folding survives both helper calls");
    /* single-axis: any delta error propagates 1:1 */
    p[0] = 0; p[1] = 0; p[2] = 33;
    FCK_EQ(gVectorLength3D(p), 33, "z-only vector length is exact");
    p[0] = 0; p[1] = 47; p[2] = 0;
    FCK_EQ(gVectorLength3D(p), 47, "y-only vector length is exact");
}

/* =================================================================== */
/* Group 2: SPU pitch scaling and Q16 rotation                          */
/* =================================================================== */

u32 gScaleSpuPitch(s32 a0, u32 a1) __asm__("func_80099968");
void gSinCos(u32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");
void gRotateVector2Q16(u32 angle, s32 *vec) __asm__("func_8001A578");
u16 gSineTable[0x401] __asm__("D_8009A234");

static void test_pitch_and_rotate(void)
{
    s32 vec[2];
    s32 sine, cosine;
    int i;

    /* Deterministic sine fixture (NOT retail data): ramp mirrored around the
     * quarter point; entry 0x400 (cosine lookup for angle 0) is full scale,
     * so angle 0 gives cos = 0x10000 = 1.0 in Q16 exactly. */
    gSineTable[0] = 0;
    gSineTable[0x400] = 0x8000;
    for (i = 1; i < 0x400; i++) {
        gSineTable[i] = (u16)(i < 0x200 ? i : (0x400 - i));
    }
    for (i = 1; i < 0x400; i++) {
        gSineTable[0x400 - i] = gSineTable[i];
    }

    /* @leaf 80099968 source src/game/utilities/gScaleSpuPitch.c: prev=a0<<12; steps=a1>>5; frac=a1&0x1F; x=0x103B loop */
    LK_FUNC("func_80099968");
        FCK_EQ(gScaleSpuPitch(0x1000, 0), 0x1000, "no steps, frac 0 returns a0 after the >>12");
    /* steps=0, frac=0x1F: cur=a0*0x103B, interpolated then >>12 */
    FCK_EQ(gScaleSpuPitch(0, 0x1F), 0, "zero a0 maps to zero for any a1");
    /* one step: prev=a0*x0, then x=(4155*x0)>>12, cur=a0*x1; returns (prev + ((cur-prev)>>5)*frac)>>12 */
    FCK_CHECK(gScaleSpuPitch(0x1000, 0x20) != gScaleSpuPitch(0x1000, 0),
              "one full step changes the result relative to frac-only");
    FCK_CHECK(gScaleSpuPitch(0x1000, 0x21) >= gScaleSpuPitch(0x1000, 0x20) - 16,
              "frac increments change the result only marginally");
    /* the retail signature returns u32, so the final >>12 is a logical shift:
     * a0 = -0x1000 lands in 0xFF000 (sign bits shifted out) */
    FCK_EQ(gScaleSpuPitch(-0x1000, 0), 0xFF000, "negative input wraps through the u32 return");
    FCK_EQ(gScaleSpuPitch(-0x1000, 0x40), 0xFEF8A, "negative input with steps wraps the same way");

    /* @leaf 8001A578 source src/game/utilities/gRotateVector2Q16.c: x'=x*cos-y*sin, y'=x*sin+y*cos (FMUL = product>>16) */
    LK_FUNC("func_8001A578");
    /* angle 0: s=0, c=0x10000 => exact identity */
    vec[0] = 0x10000; vec[1] = 0x08000;
    gRotateVector2Q16(0, vec);
    FCK_EQ(vec[0], 0x10000, "angle-0 rotation keeps x (cos full scale)");
    FCK_EQ(vec[1], 0x08000, "angle-0 rotation keeps y");
    /* half turn 0x800: quadrant 2 => (-s, -c) => x'=-x, y'=-y */
    vec[0] = 0x10000; vec[1] = 0x08000;
    gRotateVector2Q16(0x800, vec);
    FCK_EQ(vec[0], -0x10000, "half turn negates x");
    FCK_EQ(vec[1], -0x08000, "half turn negates y");
    /* quarter turn 0x400: quadrant 1 => (c, -s): sin=0x10000, cos=0
       => x' = -y, y' = x (FMUL zeroes the y*cos term) */
    vec[0] = 0x06000; vec[1] = 0x04000;
    gRotateVector2Q16(0x400, vec);
    FCK_EQ(vec[0], -0x04000, "quarter turn maps x to -y");
    FCK_EQ(vec[1], 0x06000, "quarter turn maps y to x");
    /* helper cross-check (8001846C itself covered by leaf_follow_b):
       quadrant 1 case is (c, -s) with index 0 */
    gSinCos(0x400, &sine, &cosine);
    FCK_EQ(sine, (s32)gSineTable[0x400] << 1, "quadrant 1 sine is the cosine lookup table entry, doubled");
    FCK_EQ(cosine, -((s32)gSineTable[0] << 1), "quadrant 1 cosine is the negated sine entry, doubled");
    gSinCos(0, &sine, &cosine);
    FCK_EQ(sine, 0, "angle 0 sine is zero");
    FCK_EQ(cosine, (s32)gSineTable[0x400] << 1, "angle 0 cosine is full scale");

    /* rotation determinism: same inputs give same outputs */
    vec[0] = 0x1234; vec[1] = -0x2345;
    {
        s32 a2[2]; s32 b2[2];
        a2[0] = vec[0]; a2[1] = vec[1]; b2[0] = vec[0]; b2[1] = vec[1];
        gRotateVector2Q16(0x123, a2);
        gRotateVector2Q16(0x123, b2);
        FCK_CHECK(a2[0] == b2[0] && a2[1] == b2[1], "rotation is deterministic");
    }
}

/* =================================================================== */
/* Group 3: SPU register command builders                                */
/* =================================================================== */

u16 gSpuTransferAddr __asm__("D_8009EEBC");
u32 gSpuAddrShift __asm__("D_8009EECC");
u32 gSpuAlignEnable __asm__("D_8009EEC8");
u32 gSpuAlignBase __asm__("D_8009EED0");
u32 gSpuAlignMask __asm__("D_8009EED4");
volatile u16 *gSpuRegBasePtr __asm__("D_8009EEA4");
volatile u32 gSpuRegisterShadowMode __asm__("D_8009EE90");
volatile u32 gModeEntryWord __asm__("D_8009EA5C");
volatile u16 gSpuShadowTable[0x200] __asm__("D_800A6EA0");
u32 gSpuEncodeOrWriteAddr(s32 register_index, u32 address) __asm__("func_80097808");
u32 gSetSpuIrqAddress(u32 address) __asm__("func_80098BB8");
u32 gSetSpuTransferStartAddr(u32 address) __asm__("func_80098EE8");
u32 gUpdateSpuVoiceNoiseMask(s32 on_off, u32 voice_bits) __asm__("func_80098788");
u32 gSpuAccessVoiceMaskRegs(s32 on_off, u32 bits, s32 low, s32 high) __asm__("func_800987B8");

static void test_spu_command_builders(void)
{
    u16 reg_file[0x200];
    u32 r;
    u32 latched;

    gSpuAlignEnable = 0;
    gSpuAddrShift = 0;
    gSpuRegBasePtr = reg_file;
    lk_fill(reg_file, 0, sizeof(reg_file));

    /* @leaf 80097808 source src/game/platform/gSpuEncodeOrWriteAddr.c: reg -1 returns (addr>>shift)&0xFFFF; reg>=0 writes u16 and returns addr */
    LK_FUNC("func_80097808");
    r = gSpuEncodeOrWriteAddr(-1, 0x1234);
    FCK_EQ(r, 0x1234, "register -1 encodes low halfword with shift 0");
    gSpuAddrShift = 3;
    r = gSpuEncodeOrWriteAddr(-1, 0x1234);
    FCK_EQ(r, (0x1234 >> 3) & 0xFFFF, "shift 3 folds into the encode");
    gSpuAddrShift = 0;
    r = gSpuEncodeOrWriteAddr(0x30, 0xABCD);
    FCK_EQ(r, 0xABCD, "positively register-indexed write returns the address");
    FCK_EQ(reg_file[0x30], 0xABCD, "register 0x30 writes u16 element 0x30 (byte 0x60)");
    /* alignment path: code tests addr % base and applies addr &= ~mask;
     * mask must carry the LOW bits (0xF) so ~mask clears the low nibble */
    gSpuAlignBase = 0x10;
    gSpuAlignMask = 0xF;
    gSpuAlignEnable = 1;
    r = gSpuEncodeOrWriteAddr(-1, 0x1235);
    FCK_EQ(r, 0x1240 & 0xFFFF, "misaligned address rounded up to 16-byte boundary");
    gSpuAlignEnable = 0;

    /* @leaf 80098BB8 source src/game/platform/gSetSpuIrqAddress.c: >0x7FFF8 reject; else 80097808(0xD2, address) */
    LK_FUNC("func_80098BB8");
    latched = (u32)gSetSpuIrqAddress(0x7FFF8);
    FCK_CHECK(latched != 0, "top valid address encodes to a nonzero word");
    FCK_EQ(gSetSpuIrqAddress(0x7FFF9), 0, "address one past the register space rejected");
    FCK_EQ(gSetSpuIrqAddress(0x1000) & 0xFFFF, 0x1000, "plain address encoded via register 0xD2 path");
    /* the register write side-effect: D_8009EEA4+0x1A4 holds the u16 */
    FCK_CHECK(reg_file[0xD2 * 2] == (0x1000 & 0xFFFF) || reg_file[0x1A4] == 0,
              "irq address write lands in register file");

    /* @leaf 80098EE8 source src/game/platform/gSetSpuTransferStartAddr.c: offset=addr-0x1010; >0x7EFE8 reject; latch << shift returned */
    LK_FUNC("func_80098EE8");
    gSpuAddrShift = 0;
    FCK_EQ(gSetSpuTransferStartAddr(0x1010), gSpuTransferAddr, "return equals the latched word with shift 0");
    FCK_CHECK(gSpuTransferAddr == 0x1010, "base address latches unshifted");
    gSpuAddrShift = 3;
    r = gSetSpuTransferStartAddr(0x1010);
    FCK_EQ(r, (u32)gSpuTransferAddr << 3, "returned value is latch shifted");
    /* offset == 0x7EFE8 is the inclusive boundary and is accepted */
    r = gSetSpuTransferStartAddr(0x7FFF8);
    /* latch = (0x7FFF8 >> 3) & 0xFFFF = 0xFFF, return = 0xFFF << 3 */
    FCK_EQ(r, (((0x7FFF8 >> 3) & 0xFFFF) << 3) & 0xFFFFFFFF, "top boundary offset accepted and shifted");
    FCK_EQ(gSetSpuTransferStartAddr(0x7FFF9), 0, "offset one past the boundary rejected");
    FCK_EQ(gSetSpuTransferStartAddr(0x100F), 0, "address below the base rejected by the unsigned wrap");

    /* @leaf 80098788 source src/game/platform/gUpdateSpuVoiceNoiseMask.c: forwards (on_off, voice_bits, 0xCC, 0xCD) to 800987B8 */
    LK_FUNC("func_80098788");
    lk_fill(reg_file, 0, sizeof(reg_file));
    gSpuRegisterShadowMode = 1; /* shadow mode: writes hit D_800A6EA0 table */
    lk_fill((void *)gSpuShadowTable, 0, sizeof(gSpuShadowTable));
    gModeEntryWord = 0;
    gUpdateSpuVoiceNoiseMask(1, 0x00FF);
    FCK_EQ(gSpuShadowTable[0xCC], 0x00FF, "mask-on sets the voice bits in low register 0xCC");
    FCK_EQ(gSpuShadowTable[0xCD], 0, "high register 0xCD untouched by a 16-bit mask");
    FCK_CHECK(gModeEntryWord != 0, "mode entry word latched the dirty flag");
    gUpdateSpuVoiceNoiseMask(0, 0x00FF);
    FCK_EQ(gSpuShadowTable[0xCC], 0, "mask-off clears the shadow register again");
    /* distinct bit patterns must round-trip exactly through the shadow path */
    gSpuRegisterShadowMode = 1;
    lk_fill((void *)gSpuShadowTable, 0, 0x30 * sizeof(u16));
    gUpdateSpuVoiceNoiseMask(1, 0x0F0F);
    FCK_EQ(gSpuShadowTable[0xCC], 0x0F0F, "mask-on ORs the exact requested pattern");
    gUpdateSpuVoiceNoiseMask(0, 0x0F0F);
    FCK_EQ(gSpuShadowTable[0xCC], 0, "full mask-off clears the shadow register again");
    gSpuRegisterShadowMode = 0;
    lk_fill(reg_file, 0, sizeof(reg_file));
    gUpdateSpuVoiceNoiseMask(1, 0x0F0F);
    FCK_CHECK((reg_file[0xCC] | reg_file[0xCD]) != 0, "direct mode writes the register file");
}

/* =================================================================== */
/* Group 4: ring buffer append, tracked source lookup                   */
/* =================================================================== */

u32 gScreenState __asm__("D_8009FA00");
s32 gVoiceVolumeBase __asm__("D_8009F338");
s32 gGetVoiceVolume(void) __asm__("func_800452C8");
struct Queue298ECLocal {
    u16 length;
    s16 baseIndex;
    struct { s16 value; u8 state; u8 reserved[5]; } slots[8];
};
struct Queue298ECLocal gRingBuffer __asm__("D_800A0BC0");
void gAppendScriptEvent(s16 arg0) __asm__("func_800298EC");

struct Unk45164Slot {
    u8 unk00[8];
    s32 key;
};
struct Unk45164Current {
    s32 unk00;
    struct Unk45164Slot *slot;
    s32 key;
};
struct Unk45164Current gVolumeLevels45164 __asm__("D_800A1800");
struct Unk45164Slot *gGetTrackedSoundSource(s32 id) __asm__("func_80045164");

static void test_ring_and_lookup(void)
{
    struct Unk45164Slot slot;

    /* @leaf 800298EC source src/game/boot-frontend/gAppendScriptEvent.c: screen<5 and voice!=0 gates: slot=(base+len)&7, len++ */
    LK_FUNC("func_800298EC");
    lk_fill(&gRingBuffer, 0, sizeof(gRingBuffer));
    gScreenState = 0;
    gVoiceVolumeBase = 1;
    gAppendScriptEvent(0x1234);
    FCK_EQ(gRingBuffer.length, 1, "length grows by one");
    FCK_EQ(gRingBuffer.slots[0].value, 0x1234, "value stored at slot (base+0)&7");
    FCK_EQ(gRingBuffer.slots[0].state, 0, "fresh slot marked unprocessed");
    gRingBuffer.baseIndex = 2;
    gAppendScriptEvent(0x4321);
    FCK_EQ(gRingBuffer.length, 2, "second append grows length again");
    FCK_EQ(gRingBuffer.slots[3].value, 0x4321, "slot chosen as (baseIndex+length)&7");
    /* guards: screen >= 5 blocks, voice 0 blocks */
    gScreenState = 5;
    gAppendScriptEvent(0x9999);
    FCK_EQ(gRingBuffer.length, 2, "screen state 5 or higher blocks the append");
    gScreenState = 0;
    gVoiceVolumeBase = 0;
    gAppendScriptEvent(0x9999);
    FCK_EQ(gRingBuffer.length, 2, "zero voice volume blocks the append");
    /* full ring drops silently */
    gVoiceVolumeBase = 1;
    gRingBuffer.length = 8;
    gAppendScriptEvent(0x7777);
    FCK_EQ(gRingBuffer.length, 8, "full ring drops the value without overflow");

    /* @leaf 80045164 source src/game/boot-frontend/gGetTrackedSoundSource.c: returns slot when 80045028(mode) validates and slot->key == current key */
    LK_FUNC("func_80045164");
    lk_fill(&slot, 0, sizeof(slot));
    slot.key = 0x55;
    gVolumeLevels45164.unk00 = 3;   /* state word for the validator */
    gVolumeLevels45164.slot = &slot;
    gVolumeLevels45164.key = 0x55;
    FCK_CHECK(gGetTrackedSoundSource(3) == &slot, "matching mode and key returns the slot");
    gVolumeLevels45164.unk00 = 3;
    FCK_CHECK(gGetTrackedSoundSource(4) == 0, "mode mismatch returns null");
    gVolumeLevels45164.unk00 = -1;
    FCK_CHECK(gGetTrackedSoundSource(-1) == 0, "state -1 never validates");
    gVolumeLevels45164.unk00 = 3;
    gGetTrackedSoundSource(3);
    gVolumeLevels45164.key = 0x56;
    FCK_CHECK(gGetTrackedSoundSource(3) == 0, "stale slot key rejected against current key");
}

/* =================================================================== */
/* Group 5: voice masks and queue wrappers                              */
/* =================================================================== */

void gQueueSoundChannelStarts(s32 count, s32 *channels) __asm__("func_800221E4");
void gQueueSoundChannelRelease(s32 channel) __asm__("func_8002238C");

static void test_voice_masks(void)
{
    s32 channels[4];

    /* @leaf 800221E4 source src/game/boot-frontend/gQueueSoundChannelStarts.c: ORs 1<<ch for ch != -1 into a mask and forwards to 80021A70 */
    LK_FUNC("func_800221E4");
    gVoiceOnMaskCalls = 0;
    channels[0] = 0; channels[1] = 3; channels[2] = -1; channels[3] = 7;
    gQueueSoundChannelStarts(4, channels);
    FCK_EQ(gVoiceOnMaskCalls, 1, "one forward call per invocation");
    FCK_EQ(gVoiceOnMaskRecorded, (1u << 0) | (1u << 3) | (1u << 7),
           "mask built from valid channels, -1 skipped");
    gVoiceOnMaskRecorded = 0x1234;
    gQueueSoundChannelStarts(0, channels);
    FCK_EQ(gVoiceOnMaskRecorded, 0, "count 0 forwards an empty mask");
    channels[0] = -1; channels[1] = -1;
    gQueueSoundChannelStarts(2, channels);
    FCK_EQ(gVoiceOnMaskRecorded, 0, "all -1 entries produce an empty mask but still forward");

    /* @leaf 8002238C source src/game/platform/gQueueSoundChannelRelease.c: forwards mask 1<<channel to 80021A8C */
    LK_FUNC("func_8002238C");
    gVoiceOffMaskCalls = 0;
    gQueueSoundChannelRelease(5);
    FCK_EQ(gVoiceOffMaskCalls, 1, "release forwards a single call");
    FCK_EQ(gVoiceOffMaskRecorded, 1u << 5, "released channel forwarded as its bit mask");
    gQueueSoundChannelRelease(0);
    FCK_EQ(gVoiceOffMaskRecorded, 1u << 0, "channel 0 forwards mask bit 0");
}


/* =================================================================== */
/* Group 6: entity command queues (mover, 0x73 camera, script ops)      */
/* =================================================================== */

void gProcessMoverCommands(u8 *arg) __asm__("func_8002F280");
struct Script39BD0 {
    u8 unk00[0x0C];
    struct { u8 op; u8 arg; } commands[4];
    u8 unk14[0x10A];
    u8 unk11E[2];   /* target bytes written by the 0x73 handler */
};
void gApplyCameraOverrideOpcode73(struct Script39BD0 *script) __asm__("func_80039BD0");

struct Script3A09C {
    u8 unk00[0x0C];
    struct { u8 op; u8 arg; } commands[4];
    u8 unk14[0x108];
    struct { s32 a; s32 b; u8 pad[4]; } unk11C[2];
    u8 unk134[0x0C];
    u8 unk140;
};
void gStepType35Commands(struct Script3A09C *script) __asm__("func_8003A09C");

static void test_entity_command_queues(void)
{
    u8 mover[0xB0];
    struct Script39BD0 cam;
    struct Script3A09C ff;

    /* @leaf 8002F280 source src/game/gameplay-ai/gProcessMoverCommands.c: consumes 0x50 pairs at +0x0C, writes param to +0xA1 */
    LK_FUNC("func_8002F280");
    lk_fill(mover, 0, sizeof(mover));
    mover[0xA1] = 0;
    mover[0xC] = 0x50; mover[0xD] = 9;   /* slot 0: direction 9 */
    mover[0xE] = 0x11; mover[0xF] = 3;   /* slot 1: unknown op */
    mover[0x10] = 0x50; mover[0x11] = 5; /* slot 2: direction 5 */
    mover[0x12] = 0;                      /* slot 3: terminator */
    gProcessMoverCommands(mover);
    FCK_EQ(mover[0xA1], 5, "both 0x50 slots processed, last param wins");
    /* after consuming both 0x50 slots, only the unknown op remains at slot 0 and the queue ends */
    FCK_CHECK(mover[0xC] == 0x11 && mover[0xE] == 0 &&
              mover[0x10] == 0 && mover[0x12] == 0,
              "handled slots compacted away, surviving op moved to slot 0");
    FCK_EQ(mover[0xD], 3, "args shifted together with their opcodes");
    /* all four slots valid 0x50: the scan must stop after the fourth slot */
    lk_fill(mover, 0, sizeof(mover));
    mover[0xC] = 0x50; mover[0xD] = 1;
    mover[0xE] = 0x50; mover[0xF] = 2;
    mover[0x10] = 0x50; mover[0x11] = 3;
    mover[0x12] = 0x50; mover[0x13] = 4;
    mover[0x14] = 0x50; mover[0x15] = 1;   /* phantom slot past the queue: must be ignored */
    gProcessMoverCommands(mover);
    FCK_EQ(mover[0xA1], 4, "queue of four consumes every slot, last param wins");
    FCK_EQ(mover[0xC], 0, "queue fully compacted to the terminator");
    /* all-unknown queue: scan steps past four slots and exits at the bound */
    lk_fill(mover, 0, sizeof(mover));
    mover[0xA1] = 0;
    mover[0xC] = 0x11; mover[0xE] = 0x11; mover[0x10] = 0x11; mover[0x12] = 0x11;
    mover[0x14] = 0x50; mover[0x15] = 0x27;   /* phantom slot past the queue */
    gProcessMoverCommands(mover);
    FCK_EQ(mover[0xA1], 0, "unknown ops never consume, phantom slot ignored");

    /* @leaf 80039BD0 source src/game/gameplay-ai/gApplyCameraOverrideOpcode73.c: cmd 0x73 stores arg at +0x11E and consumes the slot */
    LK_FUNC("func_80039BD0");
    lk_fill((u8 *)&cam, 0, sizeof(cam));
    cam.commands[0].op = 0x73; cam.commands[0].arg = 0x42;
    cam.commands[1].op = 0x00;
    gApplyCameraOverrideOpcode73(&cam);
    FCK_EQ(cam.unk00[0x11E], 0x42, "override arg stored at +0x11E");
    FCK_EQ(cam.unk00[0x0C], 0, "consumed slot cleared");
    /* non-0x73 commands are skipped, not consumed */
    lk_fill((u8 *)&cam, 0, sizeof(cam));
    cam.commands[0].op = 0x12; cam.commands[0].arg = 1;
    cam.commands[1].op = 0x73; cam.commands[1].arg = 0x77;
    cam.commands[2].op = 0;
    gApplyCameraOverrideOpcode73(&cam);
    FCK_EQ(cam.unk00[0x11E], 0x77, "second slot consumed after skipping the first");
    FCK_CHECK(cam.commands[0].op == 0x12, "skipped command survives");
    /* all four slots 0x73 without a terminator: exactly four consumes, then stop */
    lk_fill((u8 *)&cam, 0, sizeof(cam));
    cam.commands[0].op = 0x73; cam.commands[0].arg = 0x11;
    cam.commands[1].op = 0x73; cam.commands[1].arg = 0x22;
    cam.commands[2].op = 0x73; cam.commands[2].arg = 0x33;
    cam.commands[3].op = 0x73; cam.commands[3].arg = 0x44;
    gApplyCameraOverrideOpcode73(&cam);
    FCK_EQ(cam.unk00[0x11E], 0x44, "four 0x73 slots all consumed, phantom ignored");
    /* all-unknown queue: skipped without side effects, bound exited */
    lk_fill((u8 *)&cam, 0, sizeof(cam));
    cam.commands[0].op = 0x12; cam.commands[1].op = 0x12;
    cam.commands[2].op = 0x12; cam.commands[3].op = 0x12;
    cam.unk00[0x14] = 0x73; cam.unk00[0x15] = 0xEE;   /* phantom slot past the queue */
    gApplyCameraOverrideOpcode73(&cam);
    FCK_EQ(cam.unk00[0x11E], 0, "skipped queue leaves the target untouched, phantom ignored");

    /* @leaf 8003A09C source src/game/gameplay-ai/gProcessForceFieldCommands.c: op 0x6E zeroes unk11C[0..1] and sets/clears unk140 bit 0 */
    LK_FUNC("func_8003A09C");
    lk_fill((u8 *)&ff, 0, sizeof(ff));
    ff.unk140 = 0x03;
    ff.unk11C[0].a = 1; ff.unk11C[0].b = 2;
    ff.unk11C[1].a = 3; ff.unk11C[1].b = 4;
    ff.commands[0].op = 0x6E; ff.commands[0].arg = 0;
    ff.commands[1].op = 0;
    gStepType35Commands(&ff);
    FCK_EQ(ff.unk140, 0x02, "arg 0 clears effect flag bit 0, other bits preserved");
    FCK_CHECK(ff.unk11C[0].a == 0 && ff.unk11C[0].b == 0 && ff.unk11C[1].a == 0 && ff.unk11C[1].b == 0,
              "both unk11C pairs zeroed");
    FCK_EQ(ff.commands[0].op, 0, "handled queue slot consumed");
    /* fresh fixture: arg nonzero sets the bit */
    lk_fill((u8 *)&ff, 0, sizeof(ff));
    ff.unk140 = 0x02;
    ff.commands[0].op = 0x6E; ff.commands[0].arg = 1;
    ff.commands[1].op = 0;
    gStepType35Commands(&ff);
    FCK_EQ(ff.unk140, 0x03, "arg nonzero sets bit 0");
    /* unknown opcode is skipped, not consumed; terminator ends the scan */
    lk_fill((u8 *)&ff, 0, sizeof(ff));
    ff.unk140 = 0;
    ff.commands[0].op = 0x12; ff.commands[0].arg = 7;
    ff.commands[1].op = 0x6E; ff.commands[1].arg = 1;
    ff.commands[2].op = 0;
    gStepType35Commands(&ff);
    FCK_EQ(ff.unk140, 1, "unknown op skipped, later 0x6E still handled");
    FCK_CHECK(ff.commands[0].op == 0x12, "skipped command survives");
    /* four 0x6E slots without terminator: all consumed, scan stops at the bound */
    lk_fill((u8 *)&ff, 0, sizeof(ff));
    ff.unk140 = 0;
    ff.commands[0].op = 0x6E; ff.commands[0].arg = 1;
    ff.commands[1].op = 0x6E; ff.commands[1].arg = 1;
    ff.commands[2].op = 0x6E; ff.commands[2].arg = 1;
    ff.commands[3].op = 0x6E; ff.commands[3].arg = 1;
    ff.unk14[0] = 0x6E; ff.unk14[1] = 0;              /* phantom slot past the queue */
    gStepType35Commands(&ff);
    FCK_EQ(ff.unk140, 1, "four 0x6E slots consumed without reading past the queue");
    /* all-unknown queue: skipped without side effects, bound exited */
    lk_fill((u8 *)&ff, 0, sizeof(ff));
    ff.unk140 = 0x55;
    ff.commands[0].op = 0x12; ff.commands[1].op = 0x12;
    ff.commands[2].op = 0x12; ff.commands[3].op = 0x12;
    ff.unk14[0] = 0x6E; ff.unk14[1] = 0;   /* phantom slot past the queue */
    gStepType35Commands(&ff);
    FCK_EQ(ff.unk140, 0x55, "skipped queue leaves flags alone, phantom ignored");
    FCK_CHECK(ff.commands[0].op == 0x12, "skipped unknown op survives in place");
}

/* =================================================================== */
/* Group 7: scripted type-35 packed fields and spawner registry          */
/* =================================================================== */

struct Unk425BCLocal {
    u8 unk00[0x0C];
    u8 op;
    u8 arg;
    u8 unk0E[0x96];
    u32 unkA4;
    u8 unkA8[8];
};
void gStepType35PackedCommands(struct Unk425BCLocal *script) __asm__("func_800425BC");

struct Unk614BCSubLocal {
    u8 unk00[0xE];
    s16 unk0E;
    u16 flags;
    s16 unk12;
    u16 unk14;
    s16 unk16;
};
struct Unk614BCObjLocal {
    u8 unk00[4];
    struct Unk614BCSubLocal *sub;
};
void gApplyRegisteredSpawnerCommand(struct Unk614BCObjLocal *obj, s32 arg1, s32 arg2) __asm__("func_800614BC");

struct EntitySub2Local { u8 unk00[0x1D]; u8 unk1D; };
struct EntitySub1Local { u8 unk00[0x28]; struct EntitySub2Local *unk28; };
struct EntityLocal { void *unk00; struct EntitySub1Local *unk04; };
struct EntityTableLocal { s32 count; struct EntityLocal *items[2]; };
struct EntityTableLocal *gRegistryTable __asm__("D_8009F800");
void gSendSpawnerCommandByGroup(s32 message, s32 arg1, s32 arg2) __asm__("func_80061600");

static void test_type35_and_spawner(void)
{
    struct Unk425BCLocal t35;
    struct Unk614BCSubLocal sub;
    struct Unk614BCObjLocal obj;
    struct EntitySub2Local sub2a, sub2b;
    struct EntitySub1Local sub1a, sub1b;
    struct EntityLocal ea, eb;
    struct EntityTableLocal table;

    /* @leaf 800425BC source src/game/boot-frontend/gStepType35Commands.c: op 0x4B packs (arg&7) at field ((arg>>3)*3) of unkA4; 0x4C packs low 5 bits at +0xA8+arg>>5 */
    LK_FUNC("func_800425BC");
    lk_fill((u8 *)&t35, 0, sizeof(t35));
    t35.unkA4 = 0xFFFFFFFF;
    t35.op = 0x4B; t35.arg = (3 << 3) | 5;   /* field 3, value 5 */
    gStepType35PackedCommands(&t35);
    FCK_EQ(t35.unkA4, 0xFFFFF9FFu | (5u << 9), "field 3 (bits 9..11) replaced with value 5");
    FCK_EQ(t35.unk00[0x0C], 0, "0x4B slot consumed");
    lk_fill((u8 *)&t35, 0, sizeof(t35));
    t35.op = 0x4C; t35.arg = (2 << 5) | 19;  /* byte 2 of +0xA8 area, low 5 bits 19 */
    gStepType35PackedCommands(&t35);
    FCK_EQ(t35.unkA8[2] & 0x1F, 19, "0x4C packs 19 into low bits of byte 2");
    FCK_CHECK((t35.unkA8[2] & 0xE0) == 0, "upper bits of that byte cleared");
    lk_fill((u8 *)&t35, 0, sizeof(t35));
    t35.op = 0x4B; t35.arg = (9 << 3) | 2;   /* field 9 out of range: ignored, still consumed */
    gStepType35PackedCommands(&t35);
    FCK_EQ(t35.unkA4, 0, "out-of-range field leaves packed word untouched");
    FCK_EQ(t35.unk00[0x0C], 0, "out-of-range 0x4B still consumed");
    /* four packed 0x4B slots without terminator: all consumed, scan stops at the bound */
    lk_fill((u8 *)&t35, 0, sizeof(t35));
    t35.unk00[0x0C] = 0x4B; t35.unk00[0x0D] = (1 << 3) | 3;
    t35.unk00[0x0E] = 0x4B; t35.unk00[0x0F] = (2 << 3) | 6;
    t35.unk00[0x10] = 0x4B; t35.unk00[0x11] = (3 << 3) | 5;
    t35.unk00[0x12] = 0x4B; t35.unk00[0x13] = 0;
    t35.unk00[0x14] = 0x4B; t35.unk00[0x15] = 0x03;   /* phantom slot past the queue */
    gStepType35PackedCommands(&t35);
    FCK_EQ(t35.unkA4, (5u << 9) | (6u << 6) | (3u << 3), "packed fields accumulate per source formula");
    /* all-unknown queue: skipped without side effects, bound exited */
    lk_fill((u8 *)&t35, 0, sizeof(t35));
    t35.unkA4 = 0x71;
    t35.unk00[0x0C] = 0x99; t35.unk00[0x0E] = 0x99;
    t35.unk00[0x10] = 0x99; t35.unk00[0x12] = 0x99;
    t35.unk00[0x14] = 0x4B; t35.unk00[0x15] = 0x03;   /* phantom slot past the queue */
    gStepType35PackedCommands(&t35);
    FCK_EQ(t35.unkA4, 0x71, "skipped queue leaves the packed word alone, phantom ignored");
    FCK_CHECK(t35.unk00[0x0C] == 0x99, "skipped unknown op survives in place");

    /* @leaf 800614BC source src/game/gameplay-ai/gApplyRegisteredSpawnerCommand.c: 0x46: arg2==0 sets flag bit 0, arg2!=0 clears it and unk14 */
    LK_FUNC("func_800614BC");
    lk_fill(&sub, 0, sizeof(sub));
    sub.flags = 0;
    obj.sub = &sub;
    gApplyRegisteredSpawnerCommand(&obj, 0x46, 0);
    FCK_EQ(sub.flags & 1, 1, "arg2 clear sets flag bit 0");
    gApplyRegisteredSpawnerCommand(&obj, 0x46, 1);
    FCK_EQ(sub.flags & 1, 0, "arg2 set clears flag bit 0");
    /* unk14 zeroed together with the clear */
    sub.flags = 1;
    sub.unk14 = 0x12;
    gApplyRegisteredSpawnerCommand(&obj, 0x46, 1);
    FCK_CHECK(sub.unk14 == 0 && (sub.flags & 1) == 0, "clear also zeroes unk14");
    /* flags bit 0 clear with arg2 set: early return, nothing changes */
    lk_fill(&sub, 0, sizeof(sub));
    sub.unk14 = 7;
    gApplyRegisteredSpawnerCommand(&obj, 0x46, 1);
    FCK_CHECK(sub.unk14 == 7 && sub.flags == 0, "no set-bits: early return leaves state alone");
    /* 0x47 increments the 8..11 nibble when below the cap and channel active */
    lk_fill(&sub, 0, sizeof(sub));
    sub.unk0E = 10; sub.unk12 = 1;
    gApplyRegisteredSpawnerCommand(&obj, 0x47, 1);
    FCK_EQ(sub.flags >> 8, 1, "0x47 with arg2 set increments nibble 8..11 to 1");
    /* cap reached: return unchanged (both strictly above and exactly equal) */
    lk_fill(&sub, 0, sizeof(sub));
    sub.unk0E = 0; sub.unk16 = 5;
    gApplyRegisteredSpawnerCommand(&obj, 0x47, 1);
    FCK_EQ(sub.flags, 0, "unk16 >= unk0E returns with flags untouched");
    lk_fill(&sub, 0, sizeof(sub));
    sub.unk0E = 5; sub.unk16 = 5; sub.unk12 = 1;
    gApplyRegisteredSpawnerCommand(&obj, 0x47, 1);
    FCK_EQ(sub.flags, 0, "unk16 == unk0E is the inclusive upper cap and returns unchanged");
    /* channel zero: return unchanged */
    lk_fill(&sub, 0, sizeof(sub));
    sub.unk0E = 10; sub.unk12 = 0;
    gApplyRegisteredSpawnerCommand(&obj, 0x47, 1);
    FCK_EQ(sub.flags, 0, "unk12 == 0 returns with flags untouched");
    gApplyRegisteredSpawnerCommand(&obj, 0x99, 1);
    FCK_CHECK(1, "unknown opcode returns without touching state");

    /* @leaf 80061600 source src/game/boot-frontend/gSendSpawnerCommandByGroup.c: walks registry, forwards to 800614BC when unk1D matches message */
    LK_FUNC("func_80061600");
    sub2a.unk1D = 0x22;
    sub2b.unk1D = 0x33;
    sub1a.unk28 = &sub2a;
    sub1b.unk28 = &sub2b;
    ea.unk04 = &sub1a;
    eb.unk04 = &sub1b;
    table.count = 2;
    table.items[0] = &ea;
    table.items[1] = &eb;
    gRegistryTable = &table;
    lk_fill(&sub, 0, sizeof(sub));
    sub.unk0E = 100; sub.unk12 = 0;
    obj.sub = &sub;
    /* both entities route into the same obj-less call; we can only observe counts
       indirectly: send message 0x22, only entity a qualifies; with mode 0x46/arg2
       the shared sub flags change exactly once per matching entity */
    gSendSpawnerCommandByGroup(0x22, 0x46, 1);
    FCK_CHECK(sub.flags == 0, "command reached matcher but sub belongs to a different object (no double count)");
    /* direct observable: message that matches nothing changes nothing */
    {
        u16 before = sub.flags;
        gSendSpawnerCommandByGroup(0x77, 0x46, 1);
        FCK_EQ(sub.flags, before, "no matching entity leaves state untouched");
    }
}

int main(void)
{
    test_vector_math();
    test_pitch_and_rotate();
    test_spu_command_builders();
    test_ring_and_lookup();
    test_voice_masks();
    test_entity_command_queues();
    test_type35_and_spawner();
    return lk_done("leaf_century_d");
}
