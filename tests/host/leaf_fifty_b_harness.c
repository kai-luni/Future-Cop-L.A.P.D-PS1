/* Source characterizations of 25 unchanged src/game/<address>.c units.
 * Layouts and signatures are verified from the cited units. All state is
 * synthetic RAM fixtures; no retail data, BIOS, or hardware access.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* Real dependencies already covered by other suites. Preprocessor guards
 * isolate C89 struct/typedef/extern names so unchanged src/game units compile
 * cleanly in one translation unit without claiming coverage for them.
 */
#include "../../src/game/8001846C.c"
#include "../../src/game/80019720.c"
#include "../../src/game/8001E318.c"
#include "../../src/game/8001E4C0.c"
#include "../../src/game/8001E9AC.c"

#define PadActuator PadActuator_8001EA5C
#define gPadActuators gPadActuators_8001EA5C
#define gFrameTickCounter gFrameTickCounter_8001EA5C
#include "../../src/game/8001EA5C.c"
#undef PadActuator
#undef gPadActuators
#undef gFrameTickCounter

#define gPadActuators gPadActuators_8001EB2C
#include "../../src/game/8001EB2C.c"
#undef gPadActuators

#include "../../src/game/8001F180.c"
#include "../../src/game/80020610.c"

#define State State_800223B0
#define gSoundSystem gSoundSystem_800223B0
#include "../../src/game/800223B0.c"
#undef State
#undef gSoundSystem

#include "../../src/game/8003B65C.c"
#include "../../src/game/80045504.c"
#include "../../src/game/8004582C.c"

#define SoundBank SoundBank_800472E8
#define gSoundEntryTable gSoundEntryTable_800472E8
#include "../../src/game/800472E8.c"
#undef SoundBank
#undef gSoundEntryTable

#include "../../src/game/8005FF70.c"

#define u16 u16_80073BA8
#include "../../src/game/80073BA8.c"
#undef u16

#include "../../src/game/800787D8.c"

#define u8 u8_8008731C
#define u32 u32_8008731C
#include "../../src/game/8008731C.c"
#undef u8
#undef u32

#include "../../src/game/8008E194.c"
#include "../../src/game/80091F74.c"

/* ---- 1..4: Pad rumble wrappers (8001E96C, 8001EBA8, 8001EB6C, 8001EC04) ---- */
struct PadActuator gPadActuators_s[3] __asm__("D_800A6FF8");
s32 gFrameTickCounter_s __asm__("D_8009FA2C");
void gSetPortSmallRumble(s32 port, s32 enable) __asm__("func_8001E96C");
void gStopPadRumble(s32 port) __asm__("func_8001EBA8");
void gStopAllPadRumble(void) __asm__("func_8001EB6C");
void gSetPadRumbleEnabled(s32 port, s32 enable) __asm__("func_8001EC04");

static void reset_actuators(void)
{
    int i;
    lk_fill(gPadActuators_s, 0, sizeof(gPadActuators_s));
    for (i = 0; i < 3; i++) {
        gPadActuators_s[i].port = i;
        gPadActuators_s[i].smallIndex = 0;
        gPadActuators_s[i].largeIndex = 1;
        gPadActuators_s[i].enabled = 1;
    }
}

static void test_pad_rumble(void)
{
    /* @leaf 8001E96C source src/game/8001E96C.c: clamp port to [0,1] and forward to gSetSmallMotor */
    LK_FUNC("func_8001E96C");
    reset_actuators();
    gFrameTickCounter_s = 100;
    gSetPortSmallRumble(-5, 1);
    FCK_CHECK(gPadActuators_s[0].motor[0] == 1 && gPadActuators_s[0].smallTimer == 400 &&
              gPadActuators_s[1].motor[0] == 0, "negative port clamps to 0");
    gSetPortSmallRumble(2, 1);
    FCK_CHECK(gPadActuators_s[1].motor[0] == 1 && gPadActuators_s[1].smallTimer == 400 &&
              gPadActuators_s[2].motor[0] == 0, "port==2 clamps to 1 without touching guard slot 2");
    gSetPortSmallRumble(1, 0);
    FCK_CHECK(gPadActuators_s[1].motor[0] == 0 && gPadActuators_s[1].smallTimer == 0 &&
              gPadActuators_s[0].motor[0] == 1, "port 1 cleared without touching port 0");

    /* @leaf 8001EBA8 source src/game/8001EBA8.c: clamp port to [0,1] and zero both small and large motors */
    LK_FUNC("func_8001EBA8");
    reset_actuators();
    gPadActuators_s[0].motor[0] = 1; gPadActuators_s[0].motor[1] = 200;
    gPadActuators_s[0].smallTimer = 50; gPadActuators_s[0].largeTimer = 60;
    gPadActuators_s[1].motor[0] = 1; gPadActuators_s[1].motor[1] = 180;
    gPadActuators_s[1].smallTimer = 70; gPadActuators_s[1].largeTimer = 80;
    gPadActuators_s[2].motor[0] = 1; gPadActuators_s[2].motor[1] = 150;
    gStopPadRumble(-1);
    FCK_CHECK(gPadActuators_s[0].motor[0] == 0 && gPadActuators_s[0].motor[1] == 0 &&
              gPadActuators_s[0].smallTimer == 0 && gPadActuators_s[0].largeTimer == 0,
              "negative port stops both motors on port 0");
    FCK_CHECK(gPadActuators_s[1].motor[0] == 1 && gPadActuators_s[1].motor[1] == 180, "port 1 untouched");
    gStopPadRumble(2);
    FCK_CHECK(gPadActuators_s[1].motor[0] == 0 && gPadActuators_s[1].motor[1] == 0 &&
              gPadActuators_s[1].smallTimer == 0 && gPadActuators_s[1].largeTimer == 0 &&
              gPadActuators_s[2].motor[0] == 1,
              "port==2 stops both motors on port 1 without touching guard slot 2");

    /* @leaf 8001EB6C source src/game/8001EB6C.c: stop rumble on both ports 0 and 1 */
    LK_FUNC("func_8001EB6C");
    reset_actuators();
    gPadActuators_s[0].motor[0] = 1; gPadActuators_s[0].motor[1] = 99; gPadActuators_s[0].smallTimer = 10;
    gPadActuators_s[1].motor[0] = 1; gPadActuators_s[1].motor[1] = 88; gPadActuators_s[1].largeTimer = 20;
    gStopAllPadRumble();
    FCK_CHECK(gPadActuators_s[0].motor[0] == 0 && gPadActuators_s[0].motor[1] == 0 &&
              gPadActuators_s[0].smallTimer == 0, "port 0 stopped");
    FCK_CHECK(gPadActuators_s[1].motor[0] == 0 && gPadActuators_s[1].motor[1] == 0 &&
              gPadActuators_s[1].largeTimer == 0, "port 1 stopped");

    /* @leaf 8001EC04 source src/game/8001EC04.c: clamp port and enable or stop-then-disable rumble */
    LK_FUNC("func_8001EC04");
    reset_actuators();
    gPadActuators_s[0].enabled = 0;
    gPadActuators_s[1].motor[0] = 1; gPadActuators_s[1].motor[1] = 77;
    gSetPadRumbleEnabled(-2, 1);
    FCK_EQ(gPadActuators_s[0].enabled, 1, "negative port enables port 0");
    gSetPadRumbleEnabled(2, 0);
    FCK_CHECK(gPadActuators_s[1].enabled == 0 && gPadActuators_s[1].motor[0] == 0 &&
              gPadActuators_s[1].motor[1] == 0 && gPadActuators_s[2].enabled == 1,
              "port==2 stops motors then disables port 1 without touching guard slot 2");
    FCK_EQ(gPadActuators_s[0].enabled, 1, "port 0 remains enabled");
}

/* ---- 5..8: Trig-based helpers (8006062C, 8005A740, 80060668, 8006C4C4) ---- */
u16 gSineTable_s[0x401] __asm__("D_8009A234");
u8 *gPacketCursor_s __asm__("D_8009FA90");
u8 *gPacketLimit_s __asm__("D_8009F9C0");
s32 gDefinitionEntryCount_s __asm__("D_8009F7F8");
struct EffKeyRow {
    u8 key, pad01;
    u16 f02, f04, f06, f08, f0A;
    u8 pad0C[0x24];
};
struct EffKeyRow gEffectKeyTable_s[4] __asm__("D_800A2300");

s32 gCosineClamped(u32 id) __asm__("func_8006062C");
void gEmitAngledLine(s32 origin, s32 id) __asm__("func_8005A740");
struct LineF2Pkt { u32 tag; u32 code; s16 x0, y0, x1, y1; };
struct AttackIn { s16 f00; u16 f02; u8 f04, f05; u16 f06, f08, f0A, f0C; s16 f0E; };
struct AttackOut {
    struct EffKeyRow *f00; u16 f04; u8 f06, f07; u16 f08, f0A;
    s32 f0C, f10; s16 f14, f16; s32 f18, f1C; u16 f20; u8 f22, f23; u16 f24, f26;
};
void gInitAttackSchedulerSlot(struct AttackIn *in, struct AttackOut *out) __asm__("func_80060668");
void gGenDomePatchVertices(s16 *dst, s16 step, s16 count) __asm__("func_8006C4C4");

static void init_synth_sine(void)
{
    lk_fill(gSineTable_s, 0, sizeof(gSineTable_s));
    gSineTable_s[0] = 0;
    gSineTable_s[0x100] = 0x30FB; /* ~sin(22.5 deg) * 0x8000 */
    gSineTable_s[0x200] = 0x5A82; /* ~sin(45 deg) * 0x8000 */
    gSineTable_s[0x300] = 0x7642; /* ~sin(67.5 deg) * 0x8000 */
    gSineTable_s[0x400] = 0x8000; /* 1.0 * 0x8000 -> <<1 == 0x10000 */
}

static void test_trig_units(void)
{
    u8 line_buf[0x38];
    struct LineF2Pkt *ln;
    struct AttackIn ain;
    struct AttackOut aout;
    s16 verts[32];

    init_synth_sine();

    /* @leaf 8006062C source src/game/8006062C.c: cosine via 8001846C below 0x800, else -0x10000 */
    LK_FUNC("func_8006062C");
    FCK_EQ(gCosineClamped(0), 0x10000, "cos(0) = +0x10000");
    FCK_EQ(gCosineClamped(0x200), 0x5A82 << 1, "cos(45 deg) from table");
    FCK_EQ(gCosineClamped(0x400), 0, "cos(90 deg) = 0");
    FCK_EQ(gCosineClamped(0x600), -(0x5A82 << 1), "cos(135 deg) negative");
    FCK_EQ(gCosineClamped(0x7FF), -(gSineTable_s[0x3FF] << 1), "0x7FF still uses table");
    gSineTable_s[0x400] = 0x7000; /* distinguish clamp from table lookup at 0x800 */
    FCK_EQ(gCosineClamped(0x800), -0x10000, "0x800 boundary clamps instead of reading table");
    gSineTable_s[0x400] = 0x8000;
    FCK_EQ(gCosineClamped(0xC00), -0x10000, "above 0x800 clamps to -0x10000");

    /* @leaf 8005A740 source src/game/8005A740.c: emit LineF2 at radius 24..30 and advance cursor below limit */
    LK_FUNC("func_8005A740");
    lk_fill(line_buf, 0xA5, sizeof(line_buf));
    gPacketCursor_s = line_buf; gPacketLimit_s = line_buf + 0x1C;
    gEmitAngledLine(0x640000, 0); /* sin=0, cos=0x10000 */
    ln = (struct LineF2Pkt *)line_buf;
    FCK_CHECK(gPacketCursor_s == line_buf + 0x1C, "cursor advances by 0x1C");
    FCK_CHECK(ln->tag == 0x03000000U && ln->code == 0x40808080U, "LineF2 header");
    FCK_CHECK(ln->x0 == 124 && ln->y0 == 44 && ln->x1 == 130 && ln->y1 == 44, "angle 0 endpoints");
    gEmitAngledLine(0x640000, 0x400); /* sin=0x10000, cos=0, cursor at limit */
    ln = (struct LineF2Pkt *)(line_buf + 0x1C);
    FCK_CHECK(gPacketCursor_s == line_buf + 0x1C, "cursor stops at limit");
    FCK_CHECK(ln->x0 == 100 && ln->y0 == 68 && ln->x1 == 100 && ln->y1 == 74, "angle 90 endpoints");

    /* @leaf 80060668 source src/game/80060668.c: populate attack slot from effect table or null on f00<0 */
    LK_FUNC("func_80060668");
    lk_fill(gEffectKeyTable_s, 0, sizeof(gEffectKeyTable_s));
    gDefinitionEntryCount_s = 3;
    gEffectKeyTable_s[0].key = 0;
    gEffectKeyTable_s[1].key = 25;
    gEffectKeyTable_s[1].f02 = 0x1111; gEffectKeyTable_s[1].f06 = 0x2222; gEffectKeyTable_s[1].f0A = 0x3333;
    gEffectKeyTable_s[2].key = 40;
    lk_fill(&aout, 0xA5, sizeof(aout));
    ain.f00 = -1;
    gInitAttackSchedulerSlot(&ain, &aout);
    FCK_CHECK(aout.f00 == 0 && lk_all(&aout, 4, sizeof(aout), 0xA5), "negative key clears only f00");
    ain.f00 = 0; ain.f02 = 1; ain.f04 = 2; ain.f05 = 3; ain.f06 = 4; ain.f08 = 0; ain.f0A = 0; ain.f0C = 5; ain.f0E = 0;
    gInitAttackSchedulerSlot(&ain, &aout);
    FCK_CHECK(aout.f00 == &gEffectKeyTable_s[0], "key 0 is valid non-negative lookup");
    ain.f00 = 25; ain.f02 = 0x1234; ain.f04 = 0x44; ain.f05 = 0x55;
    ain.f06 = 0x6789; ain.f08 = 0x0400; ain.f0A = 0x1000; ain.f0C = 0xABCD; ain.f0E = 109;
    gInitAttackSchedulerSlot(&ain, &aout);
    FCK_CHECK(aout.f00 == &gEffectKeyTable_s[1], "effect table row resolved");
    FCK_CHECK(aout.f04 == 0x1234 && aout.f06 == 0x55 && aout.f07 == 0x44 && aout.f08 == 0x6789 && aout.f0A == 0xABCD,
              "input fields mapped");
    FCK_CHECK(aout.f0C == (0x5A82 << 1) && aout.f10 == -0x10000, "half-angle clamped cosines");
    FCK_EQ(aout.f14, 2048, "f0E scaled by 4096/218");
    FCK_CHECK(aout.f16 == 0 && aout.f18 == 0 && aout.f1C == 0 && aout.f22 == 0 && aout.f23 == 0,
              "runtime fields zeroed");
    FCK_CHECK(aout.f20 == 0x1111 && aout.f24 == 0x3333 && aout.f26 == 0x2222, "effect row fields copied");

    /* @leaf 8006C4C4 source src/game/8006C4C4.c: generate dome patch vertices for count==0 and count!=0 */
    LK_FUNC("func_8006C4C4");
    lk_fill(verts, 0xA5, sizeof(verts));
    gGenDomePatchVertices(verts, 0x200, 0);
    FCK_CHECK(verts[0] == 0 && verts[1] == 0x1000 && verts[2] == 0, "count==0 apex at y=0x1000");
    /* row=0x200 (s=c=0xB50), col=0 (s=0,c=0x1000) -> n=1 */
    FCK_CHECK(verts[4] == 0 && verts[5] == 0x0B50 && verts[6] == 0x0B50, "count==0 row=0x200 col=0");
    /* row=0x400 (s=0x1000,c=0), col=0x200 (s=c=0xB50) -> n=4 */
    FCK_CHECK(verts[16] == 0x0B50 && verts[17] == 0 && verts[18] == 0x0B50, "count==0 row=0x400 col=0x200");
    FCK_CHECK(verts[20] == (s16)0xA5A5, "elements past 5 vertices untouched");

    lk_fill(verts, 0xA5, sizeof(verts));
    gGenDomePatchVertices(verts, 0x200, 1);
    FCK_CHECK(verts[0] == 0 && verts[1] == 0x0800 && verts[2] == 0, "count!=0 apex starts at y=0x0800");
    /* total=0x800, inc=0x400: row=0x200 uses total=0x800 -> y=(0x800*0xB50)>>12 = 0x5A8 */
    FCK_CHECK(verts[4] == 0 && verts[5] == 0x05A8 && verts[6] == 0x0B50, "count!=0 row=0x200 scaled y");
    FCK_CHECK(verts[16] == 0x0B50 && verts[17] == 0 && verts[18] == 0x0B50, "count!=0 row=0x400 col=0x200");
    lk_fill(verts, 0xA5, sizeof(verts));
    gGenDomePatchVertices(verts, 0x401, 0);
    FCK_CHECK(verts[1] == 0x1000 && verts[4] == (s16)0xA5A5, "step==0x401 emits only apex (row < 0x401)");
}

/* ---- 9, 10 & 11: Hub menu, quartile flag & ribbon retire (8007B3F4, 8003BA50, 800734BC) ---- */
struct HubSession { u8 pad00[0xC]; s32 unk0C; u8 pad10[0x4C]; s32 unk5C; };
struct HubFontTbl {
    u8 selected[2];
    s8 count;
    u8 pad03[0x21];
    void *unk24;
    Entry *entries;
};
struct HubFontTbl *gFontBankTable_s __asm__("D_8009F468");
u8 gRowClearFlag_s __asm__("D_8009FA10");
struct HubSession *gGameSession_s __asm__("D_8009FA94");
u8 gMenuDescriptor_s[16] __asm__("D_800A3808");
void gEnterGameHubMenu(void) __asm__("func_8007B3F4");

struct QuartObj { u8 unk00[0x1C]; s16 unk1C; s16 unk1E; u8 tableIndex; u8 unk21[3]; u32 flags; };
u8 *gSlotSpawnDescs_s[4] __asm__("D_800A0C70");
void gFlagQuartileChange(struct QuartObj *object, s32 value) __asm__("func_8003BA50");

struct RibbonNode { struct RibbonNode *next; u8 pad04[0xA]; u16 flags; };
struct RibbonNode *gBeamNodeList_s __asm__("D_8009F38C");
void gRetireAllRibbons(void) __asm__("func_800734BC");

static void test_misc_game_units(void)
{
    struct HubFontTbl ft;
    struct HubSession sess;
    Entry entries[5];
    struct QuartObj qobj;
    u8 desc_dummy;
    struct RibbonNode rn[3];

    /* @leaf 8007B3F4 source src/game/8007B3F4.c: install menu descriptor and conditionally select enabled item 3 */
    LK_FUNC("func_8007B3F4");
    lk_fill(&ft, 0, sizeof(ft));
    lk_fill(&sess, 0, sizeof(sess));
    lk_fill(entries, 0, sizeof(entries));
    ft.count = 5; ft.entries = entries;
    entries[3].unk0E = 0; entries[3].unk0F = 0;
    entries[4].unk0E = 1; entries[4].unk0F = 0;
    gFontBankTable_s = &ft; gGameSession_s = &sess;
    ft.selected[0] = 99; gRowClearFlag_s = 0; sess.unk5C = 0; sess.unk0C = 0;
    gEnterGameHubMenu();
    FCK_CHECK(ft.unk24 == gMenuDescriptor_s && ft.selected[0] == 99, "rowClear==0 installs descriptor only");
    gRowClearFlag_s = 1; sess.unk5C = 1; sess.unk0C = 0;
    gEnterGameHubMenu();
    FCK_EQ(ft.selected[0], 99, "unk5C!=0 && unk0C!=1 skips selection");
    sess.unk5C = 0;
    gEnterGameHubMenu();
    FCK_EQ(ft.selected[0], 4, "unk5C==0 selects next enabled item 4");
    ft.selected[0] = 99; sess.unk5C = 5; sess.unk0C = 1;
    gEnterGameHubMenu();
    FCK_EQ(ft.selected[0], 4, "unk0C==1 selects next enabled item 4");

    /* @leaf 8003BA50 source src/game/8003BA50.c: set 0x4000 flag on quartile transition with non-null desc */
    LK_FUNC("func_8003BA50");
    lk_fill(&qobj, 0, sizeof(qobj));
    gSlotSpawnDescs_s[0] = 0; gSlotSpawnDescs_s[1] = &desc_dummy;
    qobj.unk1C = -1; qobj.unk1E = 100; qobj.tableIndex = 1; qobj.flags = 0;
    gFlagQuartileChange(&qobj, 90);
    FCK_EQ(qobj.flags, 0U, "negative unk1C never flags");
    qobj.unk1C = 10; qobj.tableIndex = 0;
    gFlagQuartileChange(&qobj, 90);
    FCK_EQ(qobj.flags, 0U, "null descriptor skips flag");
    qobj.tableIndex = 1;
    gFlagQuartileChange(&qobj, 20); /* both quarter 0 */
    FCK_EQ(qobj.flags, 0U, "same quarter skips flag");
    gFlagQuartileChange(&qobj, 60); /* quarter 0 vs 2 */
    FCK_EQ(qobj.flags, 0x4000U, "quarter change sets 0x4000");
    qobj.unk1C = 0; qobj.flags = 0;
    gFlagQuartileChange(&qobj, 60); /* boundary unk1C==0 is non-negative */
    FCK_EQ(qobj.flags, 0x4000U, "unk1C==0 boundary still flags quartile change");

    /* @leaf 800734BC source src/game/800734BC.c: set 0x1000 flag on all linked nodes and clear head */
    LK_FUNC("func_800734BC");
    lk_fill(rn, 0, sizeof(rn));
    rn[0].next = &rn[1]; rn[0].flags = 0x0001;
    rn[1].next = &rn[2]; rn[1].flags = 0x0002;
    rn[2].next = 0;      rn[2].flags = 0x0004;
    gBeamNodeList_s = &rn[0];
    gRetireAllRibbons();
    FCK_CHECK(gBeamNodeList_s == 0, "head cleared");
    FCK_CHECK(rn[0].flags == 0x1001 && rn[1].flags == 0x1002 && rn[2].flags == 0x1004, "all 3 nodes flagged");
    gRetireAllRibbons();
    FCK_CHECK(gBeamNodeList_s == 0, "empty list is safe no-op");
}

/* ---- 12 & 13: PRNG seeding & controller preset snapshot (800197A8, 8007C088) ---- */
s32 gRandSeed_s __asm__("D_8009F214");
s32 gPrimaryPrngCursor_s __asm__("D_8009F5A8");
s32 gFreeRangeCursor_s __asm__("D_8009F5AC");
s32 gStateTable_s[21] __asm__("D_800A0480");
s32 gStateEntryTable_s[21] __asm__("D_800A04D0");
void gSeedPrng(s32 seed) __asm__("func_800197A8");

u8 gPresetIndexPerPort_s __asm__("D_8009FA98");
u8 gPort2ControllerPresetIndex_s __asm__("D_8009FA99");
u8 gRumbleEnabled_s __asm__("D_8009F8D8");
u8 gPort2RumbleEnabled_s __asm__("D_8009F8D9");
u8 gStagedControllerPresets_s __asm__("D_8009F900");
u8 gStagedPort2ControllerPreset_s __asm__("D_8009F901");
u8 gControllerDiagramViews_s __asm__("D_8009F902");
u8 D_8009F903_s __asm__("D_8009F903");
s8 gSnapshotResult_s __asm__("D_8009F904");
u8 gPortCheckTable_s[4] __asm__("D_800A374D");
void gSnapshotControllerPresets(void) __asm__("func_8007C088");

static void test_prng_and_snapshot(void)
{
    /* @leaf 800197A8 source src/game/800197A8.c: seed LCG, set cursors to 20, fill two 20-word tables */
    LK_FUNC("func_800197A8");
    lk_fill(gStateTable_s, 0xA5, sizeof(gStateTable_s));
    lk_fill(gStateEntryTable_s, 0xA5, sizeof(gStateEntryTable_s));
    gSeedPrng(1);
    FCK_CHECK(gPrimaryPrngCursor_s == 0x14 && gFreeRangeCursor_s == 0x14, "both cursors set to 0x14");
    FCK_EQ(gStateTable_s[0], 16807, "first Park-Miller step");
    FCK_EQ(gStateTable_s[1], 282475249, "second Park-Miller step");
    FCK_CHECK(gStateTable_s[19] != (s32)0xA5A5A5A5 && gStateEntryTable_s[0] != (s32)0xA5A5A5A5 &&
              gStateEntryTable_s[19] == gRandSeed_s, "both 20-word tables populated in sequence");
    FCK_CHECK(gStateTable_s[20] == (s32)0xA5A5A5A5 && gStateEntryTable_s[20] == (s32)0xA5A5A5A5,
              "21st guard words untouched");

    /* @leaf 8007C088 source src/game/8007C088.c: stage presets/rumble, clear diagram views, set port check 1 or 3 */
    LK_FUNC("func_8007C088");
    reset_actuators();
    gPadActuators_s[0].enabled = 1; gPadActuators_s[1].enabled = 0;
    gPresetIndexPerPort_s = 2; gPort2ControllerPresetIndex_s = 3;
    gControllerDiagramViews_s = 7; D_8009F903_s = 8;
    gSnapshotResult_s = 1; gPortCheckTable_s[0] = 0; gPortCheckTable_s[1] = 0x5A;
    gSnapshotControllerPresets();
    FCK_CHECK(gStagedControllerPresets_s == 2 && gStagedPort2ControllerPreset_s == 3, "presets staged");
    FCK_CHECK(gRumbleEnabled_s == 1 && gPort2RumbleEnabled_s == 0, "rumble flags queried via 8001EB2C");
    FCK_CHECK(gControllerDiagramViews_s == 0 && D_8009F903_s == 0, "diagram views cleared");
    FCK_CHECK(gPortCheckTable_s[0] == 3 && gPortCheckTable_s[1] == 0x5A, "result==1 sets check=3");
    gSnapshotResult_s = 0;
    gSnapshotControllerPresets();
    FCK_EQ(gPortCheckTable_s[0], 1, "result!=1 sets check=1");
}

/* ---- 14..17: Memory card manager (8001F24C, 80020618, 800207CC, 80020DF8) ---- */
struct Packed6 { u8 bytes[6]; };
struct Packed9 { u8 bytes[9]; };
struct CardDirFrame { u8 bytes[0x80]; };
struct McState {
    s32 unk00;
    s32 state[16];
    u8 *primary[16];
    u8 *secondary[16];
    u8 unk0C4[0x830];
    s32 field_8F4;
    s32 unk8F8;
    u8 unk8FC[0x10];
    s32 unk90C;
    s32 unk910;
    u8 unk914[0x80];
    s32 unk994;
    s32 unk998;
    s32 unk99C;
    u8 unk9A0[4];
    s32 unk9A4;
    u8 unk9A8[0x88];
    struct CardDirFrame directory[15];
    u8 tail[0x1BB8 - 0xA30 - 15 * 0x80];
};
static struct McState mc_storage;
struct McState *gOperationState_s __asm__("D_8009AB08");
struct Packed9 gCardSlotZeroLabel_s __asm__("D_80010AE8") = { { 'S','L','O','T','0','L','A','B',0 } };
struct Packed9 gCardFileSecondaryLabel_s __asm__("D_80010AF4") = { { 'S','E','C','O','N','D','A','R',0 } };
struct Packed9 gCardOtherSlotLabel_s __asm__("D_80010B00") = { { 'O','T','H','E','R','S','L','T',0 } };
struct Packed6 gCardEmptySlotLabel_s __asm__("D_8009F260") = { { 'E','M','P','T','Y',0 } };
void gQueueMcOpMode3(u32 a0, u32 a1, u32 a2, u32 a3) __asm__("func_8001F24C");
void *gInitMcState(s32 arg0, void *arg1) __asm__("func_80020618");
void gParseCardDirectory(void) __asm__("func_800207CC");
s32 gReadCardSlotState(u32 slot) __asm__("func_80020DF8");

static void set_dir_checksum(struct CardDirFrame *f)
{
    u8 x = 0;
    int i;
    for (i = 0; i < 0x7F; i++) x ^= f->bytes[i];
    f->bytes[0x7F] = x;
}

static void test_memory_card(void)
{
    void *ret;
    int i;

    /* @leaf 80020618 source src/game/80020618.c: validate null global and port<2, zero 0x1BB8, init 16+16 ptrs */
    LK_FUNC("func_80020618");
    gOperationState_s = 0;
    FCK_CHECK(gInitMcState(2, &mc_storage) == 0 && gOperationState_s == 0, "arg0==2 rejected");
    FCK_CHECK(gInitMcState(-1, &mc_storage) == 0 && gOperationState_s == 0, "negative arg0 rejected");
    lk_fill(&mc_storage, 0xA5, sizeof(mc_storage));
    ret = gInitMcState(1, &mc_storage);
    FCK_CHECK(ret == &mc_storage && gOperationState_s == &mc_storage, "valid port 1 initializes global");
    FCK_EQ(mc_storage.unk00, 0x10, "port 1 shifted <<4 at +0");
    FCK_CHECK(mc_storage.primary[0] == (u8 *)&mc_storage + 0xC4 &&
              mc_storage.secondary[0] == (u8 *)&mc_storage + 0x104 &&
              mc_storage.primary[15] == (u8 *)&mc_storage + 0xC4 + 15 * 0x80 &&
              mc_storage.secondary[15] == (u8 *)&mc_storage + 0x104 + 15 * 0x80,
              "16 primary and secondary pointers initialized");
    FCK_CHECK(gInitMcState(0, &mc_storage) == 0, "already initialized rejects re-init");

    /* @leaf 8001F24C source src/game/8001F24C.c: queue mode-3 op when idle, else set busy flag unk9A4=1 */
    LK_FUNC("func_8001F24C");
    mc_storage.field_8F4 = 0; mc_storage.unk8F8 = 0;
    mc_storage.unk994 = 9; mc_storage.unk9A4 = 9;
    gQueueMcOpMode3(0x11, 0x22, 0x33, 0x44);
    FCK_CHECK(mc_storage.unk998 == 0x11 && mc_storage.unk99C == 0x22 &&
              mc_storage.unk910 == 0x33 && mc_storage.unk90C == 0x44 &&
              mc_storage.unk994 == 0 && mc_storage.unk9A4 == 0 && mc_storage.unk8F8 == 3,
              "idle manager queues mode 3 op");
    gQueueMcOpMode3(0x55, 0x66, 0x77, 0x88);
    FCK_CHECK(mc_storage.unk9A4 == 1 && mc_storage.unk998 == 0x11, "busy manager sets unk9A4=1 without overwriting");

    /* @leaf 800207CC source src/game/800207CC.c: parse 15 directory frames with XOR checksum and label copy */
    LK_FUNC("func_800207CC");
    lk_fill(mc_storage.directory, 0, sizeof(mc_storage.directory));
    /* slot 1 (dir[0]): 0xA0 empty with valid checksum */
    mc_storage.directory[0].bytes[0] = 0xA0;
    set_dir_checksum(&mc_storage.directory[0]);
    /* slot 2 (dir[1]): 0x51 file with valid checksum and 20-byte filename */
    mc_storage.directory[1].bytes[0] = 0x51;
    for (i = 0; i < 20; i++) mc_storage.directory[1].bytes[10 + i] = (u8)('A' + i);
    set_dir_checksum(&mc_storage.directory[1]);
    /* slot 3 (dir[2]): 0xA0 but corrupted checksum -> forced to 0x53 -> other */
    mc_storage.directory[2].bytes[0] = 0xA0;
    set_dir_checksum(&mc_storage.directory[2]);
    mc_storage.directory[2].bytes[0x7F] ^= 1;
    /* remaining slots: 0xA3 empty */
    for (i = 3; i < 15; i++) {
        mc_storage.directory[i].bytes[0] = 0xA3;
        set_dir_checksum(&mc_storage.directory[i]);
    }
    gParseCardDirectory();
    FCK_CHECK(mc_storage.state[0] == 1 && mc_storage.primary[0][0] == 'S' && mc_storage.secondary[0][0] == 'S',
              "slot 0 labeled");
    FCK_CHECK(mc_storage.state[1] == 2 && mc_storage.primary[1][0] == 'E' && mc_storage.secondary[1][0] == 'E',
              "valid 0xA0 slot marked empty");
    FCK_CHECK(mc_storage.state[2] == 1 && mc_storage.primary[2][0] == 'A' && mc_storage.primary[2][19] == 'T' &&
              mc_storage.secondary[2][0] == 'S', "valid 0x51 slot copies 20-byte name + secondary label");
    FCK_CHECK(mc_storage.state[3] == 1 && mc_storage.directory[2].bytes[0] == 0x53 &&
              mc_storage.primary[3][0] == 'O', "bad checksum forces 0x53 and other label");
    FCK_EQ(mc_storage.state[15], 2, "last slot 15 parsed");

    /* @leaf 80020DF8 source src/game/80020DF8.c: bounds check slot<16, lazy parse when 0, return state */
    LK_FUNC("func_80020DF8");
    FCK_EQ(gReadCardSlotState(16), -1, "slot 16 out of range returns -1");
    FCK_EQ(gReadCardSlotState(0xFFFFFFFFU), -1, "unsigned -1 out of range returns -1");
    mc_storage.state[1] = 7;
    FCK_EQ(gReadCardSlotState(1), 7, "nonzero state returned without re-parse");
    mc_storage.state[1] = 0;
    FCK_EQ(gReadCardSlotState(1), 2, "zero state triggers gParseCardDirectory and returns parsed 2");
}

/* ---- 18: CD seek done callback (8001C614) ---- */
struct CdBootState { volatile s32 mode; u8 pad[24]; s32 value; };
struct CdBootState gExecutableImageBase_s __asm__("D_80010000");
s32 gCdSyncCallback_s __asm__("D_8009C8A8");
void gCdSeekDoneCallback(u8 arg) __asm__("func_8001C614");

static void test_cd_seek_cb(void)
{
    /* @leaf 8001C614 source src/game/8001C614.c: arg==2 && mode==1 advances mode to 2 and installs sync cb */
    LK_FUNC("func_8001C614");
    gExecutableImageBase_s.mode = 1; gExecutableImageBase_s.value = 0x12345678;
    gCdSyncCallback_s = 0x55;
    gCdSeekDoneCallback(1);
    FCK_CHECK(gExecutableImageBase_s.mode == 1 && gCdSyncCallback_s == 0x55, "arg!=2 does nothing");
    gExecutableImageBase_s.mode = 0;
    gCdSeekDoneCallback(2);
    FCK_CHECK(gExecutableImageBase_s.mode == 0 && gCdSyncCallback_s == 0x55, "arg==2 with mode!=1 does nothing");
    gExecutableImageBase_s.mode = 1;
    gCdSeekDoneCallback(2);
    FCK_CHECK(gExecutableImageBase_s.mode == 2 && gCdSyncCallback_s == 0x12345678,
              "arg==2 with mode==1 sets mode=2 and updates sync callback");
}

/* ---- 19, 20, 23, 24 & 25: Sound channel, bank tables, register, sample append, voice alloc ---- */
struct SoundChannel gVoiceTable_s[30] __asm__("D_800A1160");
s32 gChannelCount_s __asm__("D_8009F720");
struct SoundChannel *gChannelListHead_s __asm__("D_8009F724");
struct SoundChannel *gSoundChannelQueueHead_s __asm__("D_8009F728");
struct SoundChannel *gSoundChannelQueueTail_s __asm__("D_8009F72C");
void gReleaseSoundChannel(struct SoundChannel *channel) __asm__("func_80045548");

u8 gSoundBankTableLow_s[0x48] __asm__("D_800A1830");
u8 gSoundIdTable_s[0x48] __asm__("D_800A1880");
u8 gHighSoundIdRegistrationBase_s[0x88] __asm__("D_800A188A");
u8 gSoundBankTableHigh_s[0x40] __asm__("D_800A18D0");
u8 gRegisteredSoundBankCount_s __asm__("D_8009F75C");
struct Unk7938 gSoundEntryTable_s[0x70] __asm__("D_800A7938");
void gResetSoundBankTables(void) __asm__("func_80044EF8");

struct SoundObjArg { u16 id; u8 pad[2]; u8 unk04, unk05, unk06, unk07; u8 unk08[4]; u8 guard_id; };
s32 gRegisterSoundBankByKey(struct SoundObjArg *self, s32 key) __asm__("func_80047344");
s32 gAppendSoundProgramSample(s32 key, s32 a, s32 c, s32 b, void *self) __asm__("func_800241D0");

struct SndVoice22 {
    s32 active; u8 unk04, unk05, unk06; u8 pad07[7]; u16 unk0E, unk10; u8 pad12[2]; s32 unk14, owner;
};
struct SndChan22 { void *unk00; u8 unk04[8]; s32 unk0C, unk10, unk14, unk18; };
struct SndSys22 {
    s32 bankCount; void **banks; s32 channelCount; struct SndChan22 *channels;
    s32 active; u32 mask; void *unk18; struct SndVoice22 voices[3]; s32 scale;
};
struct VoiceArgs22 { s32 unk00, unk04, unk08; };
struct SndSys22 gSoundSystem_s __asm__("D_800A07A8");
s32 gAllocateSoundChannelVoice(s32 unused, s32 index, struct VoiceArgs22 *args, s32 arg3) __asm__("func_800220C4");

static void test_sound_units(void)
{
    struct SoundChannel ch[3];
    struct SoundObjArg sobj;
    struct SndChan22 chans[2];
    struct VoiceArgs22 vargs;

    /* @leaf 80045548 source src/game/80045548.c: unlink channel from queue, prepend to free list, dec or reinit */
    LK_FUNC("func_80045548");
    lk_fill(ch, 0, sizeof(ch));
    ch[0].next = &ch[1]; ch[1].prev = &ch[0]; ch[1].next = &ch[2]; ch[2].prev = &ch[1];
    gSoundChannelQueueHead_s = &ch[0]; gSoundChannelQueueTail_s = &ch[2];
    gChannelListHead_s = 0; gChannelCount_s = 3;
    gReleaseSoundChannel(&ch[1]);
    FCK_CHECK(ch[0].next == &ch[2] && ch[2].prev == &ch[0] && gChannelListHead_s == &ch[1] &&
              ch[1].next == 0 && gChannelCount_s == 2, "middle channel unlinked and freed");
    gReleaseSoundChannel(&ch[0]);
    FCK_CHECK(gSoundChannelQueueHead_s == &ch[2] && ch[2].prev == 0 && gChannelListHead_s == &ch[0] &&
              ch[0].next == &ch[1] && gChannelCount_s == 1, "head channel unlinked");
    gChannelCount_s = 0;
    gReleaseSoundChannel(&ch[2]);
    FCK_CHECK(gSoundChannelQueueHead_s == 0 && gSoundChannelQueueTail_s == 0 &&
              gChannelListHead_s == gVoiceTable_s && gVoiceTable_s[0].id == -1,
              "count==0 triggers gInitSoundChannelList");

    /* @leaf 80044EF8 source src/game/80044EF8.c: fill low/id(0x46) & high(0x3C) tables with 0xFF and reset 0x6E entries */
    LK_FUNC("func_80044EF8");
    lk_fill(gSoundBankTableLow_s, 0x11, sizeof(gSoundBankTableLow_s));
    lk_fill(gSoundIdTable_s, 0x22, sizeof(gSoundIdTable_s));
    lk_fill(gSoundBankTableHigh_s, 0x33, sizeof(gSoundBankTableHigh_s));
    lk_fill(gSoundEntryTable_s, 0x44, sizeof(gSoundEntryTable_s));
    gRegisteredSoundBankCount_s = 9;
    gResetSoundBankTables();
    FCK_EQ(gRegisteredSoundBankCount_s, 0, "bank count zeroed");
    FCK_CHECK(lk_all(gSoundBankTableLow_s, 0, 0x46, 0xFF) && gSoundBankTableLow_s[0x46] == 0x11,
              "low table filled exactly 0x46 bytes");
    FCK_CHECK(lk_all(gSoundIdTable_s, 0, 0x46, 0xFF) && gSoundIdTable_s[0x46] == 0x22,
              "id table filled exactly 0x46 bytes");
    FCK_CHECK(lk_all(gSoundBankTableHigh_s, 0, 0x3C, 0xFF) && gSoundBankTableHigh_s[0x3C] == 0x33,
              "high table filled exactly 0x3C bytes");
    FCK_CHECK(gSoundEntryTable_s[0].unk13 == 1 && gSoundEntryTable_s[0].unk16 == 0xFF &&
              gSoundEntryTable_s[0x6D].unk13 == 1 && gSoundEntryTable_s[0x6D].unk16 == 0xFF &&
              gSoundEntryTable_s[0x6E].unk13 == 0x44, "entries 0..0x6D reset, 0x6E untouched");

    /* @leaf 80047344 source src/game/80047344.c: register sound bank, clamp fields, route IDs to low/id/high tables */
    LK_FUNC("func_80047344");
    gResetSoundBankTables();
    lk_fill(gHighSoundIdRegistrationBase_s, 0xEE, sizeof(gHighSoundIdRegistrationBase_s));
    lk_fill(&sobj, 0, sizeof(sobj));
    sobj.unk04 = 0; sobj.unk05 = 9; sobj.unk06 = 0x77; sobj.unk07 = 1;
    sobj.unk08[0] = 5; sobj.unk08[1] = 0x50; sobj.unk08[2] = 0; sobj.unk08[3] = 0x82; sobj.guard_id = 6;
    FCK_EQ(gRegisterSoundBankByKey(&sobj, 0x2A), 0, "first registration returns slot 0");
    FCK_CHECK(sobj.id == 0 && sobj.unk04 == 1 && sobj.unk05 == 4, "unk04 clamped up to 1, unk05 clamped to 4");
    FCK_CHECK(gSoundEntryTable_s[0].unk13 == 1 && gSoundEntryTable_s[0].unk14 == 4 &&
              gSoundEntryTable_s[0].unk15 == 0x77 && gSoundEntryTable_s[0].unk16 == 0x2A, "entry fields stored");
    FCK_CHECK(gSoundBankTableLow_s[5] == 0 && gSoundBankTableLow_s[6] == 0xFF &&
              gHighSoundIdRegistrationBase_s[0x50] == 0 && gHighSoundIdRegistrationBase_s[0x82] == 0xEE,
              "exactly 4 IDs routed with bounds");
    /* duplicate key returns existing slot 0 */
    FCK_EQ(gRegisterSoundBankByKey(&sobj, 0x12A), 0, "duplicate low-byte key returns existing slot");
    /* second registration with unk04>10 and unk07!=1 routes to gSoundIdTable */
    sobj.unk04 = 20; sobj.unk05 = 2; sobj.unk07 = 0; sobj.unk08[0] = 7; sobj.unk08[1] = 0;
    FCK_EQ(gRegisterSoundBankByKey(&sobj, 0x3B), 1, "second registration returns slot 1");
    FCK_CHECK(sobj.unk04 == 10 && gSoundIdTable_s[7] == 1, "unk04 clamped to 10 and routed to id table");
    /* full table rejects */
    gRegisteredSoundBankCount_s = 0x6E;
    FCK_EQ(gRegisterSoundBankByKey(&sobj, 0x4C), -1, "full table returns -1");

    /* @leaf 800241D0 source src/game/800241D0.c: register bank and append up to 4 samples when enabled */
    LK_FUNC("func_800241D0");
    gResetSoundBankTables();
    lk_fill(&sobj, 0, sizeof(sobj));
    sobj.unk04 = 2; sobj.unk05 = 1;
    FCK_EQ(gAppendSoundProgramSample(0x51, 10, 30, 20, &sobj), 1, "first sample appended");
    FCK_CHECK(gSoundEntryTable_s[0].unk00[0] == 10 && gSoundEntryTable_s[0].unk00[1] == 20 &&
              gSoundEntryTable_s[0].unk00[2] == 30 && gSoundEntryTable_s[0].active == 1,
              "sample bytes a/b/c and count updated");
    gSoundEntryTable_s[0].active = 4;
    FCK_EQ(gAppendSoundProgramSample(0x51, 1, 2, 3, &sobj), 0, "full 4-sample bank returns 0");
    sobj.unk05 = 0;
    FCK_EQ(gAppendSoundProgramSample(0x52, 1, 2, 3, &sobj), 0, "disabled bank returns 0");

    /* @leaf 800220C4 source src/game/800220C4.c: allocate first free voice of 3 and compute stereo levels */
    LK_FUNC("func_800220C4");
    lk_fill(&gSoundSystem_s, 0, sizeof(gSoundSystem_s));
    lk_fill(chans, 0, sizeof(chans));
    gSoundSystem_s.channels = chans;
    gSoundSystem_s.scale = 2;
    gSoundSystem_s.voices[0].active = 1; /* voice 0 busy, voice 1 free */
    vargs.unk00 = 0x1234; vargs.unk04 = 100; vargs.unk08 = 0x40; /* center pan */
    FCK_EQ(gAllocateSoundChannelVoice(0, 1, &vargs, 0x99), 1, "allocates voice 1 and returns channel index 1");
    FCK_CHECK(gSoundSystem_s.voices[1].active == 1 && gSoundSystem_s.voices[1].owner == 1 &&
              gSoundSystem_s.voices[1].unk05 == 0x7F && gSoundSystem_s.voices[1].unk06 == 0x7F &&
              gSoundSystem_s.voices[1].unk0E == 0x80FF && gSoundSystem_s.voices[1].unk10 == 0x5FCE &&
              gSoundSystem_s.voices[1].unk14 == 0x99, "voice 1 initialized");
    FCK_CHECK(chans[1].unk00 == (u8 *)gSoundSystem_s.voices + 4 + 0x1C &&
              chans[1].unk10 == 0x1234 && chans[1].unk14 == 200 && chans[1].unk18 == 200,
              "channel linked to voice 1 with stereo levels");
    gSoundSystem_s.voices[2].active = 1; gSoundSystem_s.scale = 0;
    FCK_EQ(gAllocateSoundChannelVoice(0, 0, &vargs, 0x55), -1, "all 3 voices busy returns -1 without reading past voices[2]");
    FCK_CHECK(chans[0].unk00 == 0, "failed alloc clears channel ptr");
}

/* ---- 21 & 22: SetDefDrawEnv & Movie audio ring split (80092140, 80087430) ---- */
struct DrawEnvObj {
    s16 x, y, z, size;
    s16 x2, y2;
    s16 a, b, c, d;
    s16 value;
    u8 flag, small, pad, e, f, g;
};
s32 gGpuDrawOffset_s __asm__("D_8009DD90");
struct DrawEnvObj *gSetDefDrawEnv(struct DrawEnvObj *obj, int x, int y, int z, int size) __asm__("func_80092140");

struct MovieRing {
    u32 unk0, unk4;
    u8 pad08[8];
    u8 buf0[0x3C00];
    u8 buf1[0x3C00];
};
static struct MovieRing movie_ring_storage;
volatile struct MovieRing *gMovieAudioState_s __asm__("D_8009F4D8");
void gMovieAudioChunkPushSplit(u8 *arg0) __asm__("func_80087430");

static void test_drawenv_and_movie_ring(void)
{
    struct DrawEnvObj env;
    u8 chunk[0x10 + 60]; /* 16-byte header + 30 bytes left + 30 bytes right */
    int i;

    /* @leaf 80092140 source src/game/80092140.c: initialize DRAWENV defaults and mode-dependent small threshold */
    LK_FUNC("func_80092140");
    lk_fill(&env, 0xA5, sizeof(env));
    gGpuDrawOffset_s = 0;
    FCK_CHECK(gSetDefDrawEnv(&env, 10, 20, 320, 0x100) == &env, "returns obj pointer");
    FCK_CHECK(env.x == 10 && env.y == 20 && env.z == 320 && env.size == 0x100 &&
              env.x2 == 10 && env.y2 == 20 && env.value == 10 && env.flag == 1 && env.small == 1 &&
              env.a == 0 && env.b == 0 && env.c == 0 && env.d == 0 &&
              env.e == 0 && env.f == 0 && env.g == 0 && env.pad == 0, "mode 0 size=0x100 sets small=1");
    gSetDefDrawEnv(&env, 0, 0, 320, 0x101);
    FCK_EQ(env.small, 0, "mode 0 size=0x101 sets small=0");
    gGpuDrawOffset_s = 1;
    gSetDefDrawEnv(&env, 0, 0, 320, 0x120);
    FCK_EQ(env.small, 1, "mode 1 size=0x120 sets small=1");
    gSetDefDrawEnv(&env, 0, 0, 320, 0x121);
    FCK_EQ(env.small, 0, "mode 1 size=0x121 sets small=0");

    /* @leaf 80087430 source src/game/80087430.c: big-endian chunk split into padded 16-byte stereo ring with wrap */
    LK_FUNC("func_80087430");
    gMovieAudioState_s = &movie_ring_storage;
    lk_fill(&movie_ring_storage, 0xAA, sizeof(movie_ring_storage));
    lk_fill(chunk, 0, sizeof(chunk));
    /* total size = 0x10 + 60 = 76 = 0x0000004C big-endian -> s0=30, s5=32 */
    chunk[4] = 0; chunk[5] = 0; chunk[6] = 0; chunk[7] = 0x4C;
    for (i = 0; i < 30; i++) {
        chunk[0x10 + i] = (u8)(0x10 + i);
        chunk[0x10 + 30 + i] = (u8)(0x80 + i);
    }
    /* Case 1: exact free-space boundary (free == s5 == 32) at offset 0 */
    movie_ring_storage.unk0 = 32; movie_ring_storage.unk4 = 0x3C00;
    gMovieAudioChunkPushSplit(chunk);
    FCK_EQ(movie_ring_storage.unk4, 0x3C20U, "exact free==32 boundary proceeds and advances by 32");
    FCK_CHECK(movie_ring_storage.buf0[0] == 0x10 && movie_ring_storage.buf0[1] == 0 &&
              movie_ring_storage.buf0[2] == 0x11 && movie_ring_storage.buf0[16] == 0x1F &&
              movie_ring_storage.buf0[17] == 0 && movie_ring_storage.buf0[31] == 0x2D,
              "left channel expanded 15->16 contiguously");
    FCK_CHECK(movie_ring_storage.buf1[0] == 0x80 && movie_ring_storage.buf1[1] == 0 &&
              movie_ring_storage.buf1[16] == 0x8F && movie_ring_storage.buf1[31] == 0x9D,
              "right channel expanded 15->16 contiguously");
    /* Case 2: wrap-around at offset 0x3BF0 (16 bytes left before wrap, 16 bytes after) */
    movie_ring_storage.unk0 = 0x3BF0; movie_ring_storage.unk4 = 0x3BF0;
    gMovieAudioChunkPushSplit(chunk);
    FCK_EQ(movie_ring_storage.unk4, 0x3C10U, "wrapped cursor advances by 32");
    FCK_CHECK(movie_ring_storage.buf0[0x3BF0] == 0x10 && movie_ring_storage.buf0[0x3BF1] == 0 &&
              movie_ring_storage.buf0[0] == 0x1F && movie_ring_storage.buf0[1] == 0 &&
              movie_ring_storage.buf0[15] == 0x2D, "left channel wraps cleanly across 0x3C00");
    FCK_CHECK(movie_ring_storage.buf1[0x3BF0] == 0x80 && movie_ring_storage.buf1[0] == 0x8F &&
              movie_ring_storage.buf1[15] == 0x9D, "right channel wraps cleanly across 0x3C00");
}

int main(void)
{
    test_pad_rumble();
    test_trig_units();
    test_misc_game_units();
    test_prng_and_snapshot();
    test_memory_card();
    test_cd_seek_cb();
    test_sound_units();
    test_drawenv_and_movie_ring();
    return lk_done("leaf_fifty_b");
}
