/* Host harness: Precinct Assault moving map objects (lifts / turret carriers).
 *
 * Runs the UNCHANGED retail units (compiled with the pinned gcc-2.7.2-cdk +
 * maspsx pipeline exactly like tests/host/spawner_path_test.sh):
 *
 *   func_80041B6C gHandleElevatorEntityMessages  lift entity, dispatch (type 10)
 *   func_80041488 gInitElevator                 lift message-2 init
 *   func_80041880 gStepElevator                  lift tick
 *   func_80041728 gRunEntityCommands             4-slot command interpreter
 *   func_80039504 gSetupMountedTurretCarrier    carrier setup (type 20 spawn)
 *   func_800395A8 gHandleMountedTurretCarrierMessages carrier dispatch
 *   func_80039298 gHandleMountedPartMsg          part-container dispatch
 *   func_80039418 gSpawnMountedPartChild         kind-0x15 child spawn request
 *
 * Memory model (like spawner_path_harness.c): qemu-mipsel cannot map guest
 * addresses >= 0x80000000, so the retail RAM window lives at
 * (retail address - 0x70000000) in a section relocated to 0x10010000.
 * Pointers STORED into that window are therefore host-form values; the
 * helper host_of()/retail_of_host() converts, and the capture helpers map
 * host pointers back to their retail form for assertions. Every fixture
 * field the tests rely on is written explicitly per case - nothing relies
 * on zero-initialised D_ slots.
 *
 * Stubs (NOT part of the parity claim): the GTE entry points func_80010750 /
 * func_80010234 / func_800188D0 are recording stubs - no cop2/GTE asm unit
 * is ever linked. Ground/table services func_80066F78 / func_8006314C /
 * func_80063194, sound func_800457B8 / func_80047284, the class-5 handler
 * func_800382B4 and the remaining callees func_8003FA34 / func_8003BA50 /
 * func_80038A00 / func_80038754 / func_800398EC / func_80017668 /
 * func_8004D444 / func_8002A5B4 / func_8002AD58 record what they received
 * and return configured values; unconfigured returns are 0 (fc_testkit
 * rule 3).
 *
 * Every expected value below derives independently from the C source of the
 * units plus hand-traced arithmetic, never from running a re-implementation.
 * See docs/precinct-assault-movers.md for the documented behaviour.
 */
#include "fc_testkit.h"

#define RETAIL_BASE 0x80010000u
#define RELOC 0x70000000u
#define RAM_SIZE 0x9C000u

/* Harness-owned fixture addresses inside the relocated retail window. */
#define ADDR_LIFT 0x800A0100u      /* type-10 lift entity under test */
#define ADDR_MODELREC 0x800A0300u  /* struct Model of the lift (frame ptr +0x34) */
#define ADDR_FRAMES 0x800A0400u    /* frame vertex data the GTE stub sees */
#define ADDR_RECORD 0x800A0480u    /* synthetic script record (struct Data) */
#define ADDR_WRAPPER 0x800A0500u   /* struct Arg wrapper (data ptr at +0x08) */
#define ADDR_GAME 0x800A0700u      /* struct Game41880 player record */
#define ADDR_PSTATE 0x800A0780u    /* struct UnkAC player state (unk40 +0x40) */
#define ADDR_CARRIER 0x800A1000u   /* type-20 carrier entity */
#define ADDR_OTHER 0x800A1100u     /* struct Unk39504Other spawn descriptor */
#define ADDR_SUB04 0x800A1200u     /* other->unk04 (+0x20 word) */
#define ADDR_SUB08 0x800A1300u     /* other->unk08 (+0x48/+0x4A/+0x4C) */
#define ADDR_CHILD 0x800A1400u     /* spawned carrier child */
#define ADDR_CONT 0x800A1600u      /* func_80039298 container (flags 0xE8, child 0xEC) */
#define ADDR_CCHILD 0x800A1700u    /* struct Child (type +0x16, count +0x73, objects +0x158) */

/* retail field offsets used by the tests (per unit view - see the units) */
/* func_80041880 struct Obj41880 */
#define O_UNK08 0x08u
#define O_UNK0C 0x0Cu
#define O_UNK21 0x21u
#define O_UNK22 0x22u
#define O_UNK24 0x24u
#define O_UNK28 0x28u
#define O_UNK2C 0x2Cu
#define O_UNK50 0x50u
#define O_UNK54 0x54u
#define O_UNK58 0x58u
#define O_UNK5C 0x5Cu
#define O_UNK70 0x70u
#define O_UNKA0 0xA0u
#define O_UNKAC 0xACu
#define O_UNKB8 0xB8u
#define O_UNKBC 0xBCu
#define O_UNKC0 0xC0u
#define O_UNKC4 0xC4u
#define O_UNKC6 0xC6u
#define O_UNKC8 0xC8u
#define O_UNKC9 0xC9u
#define O_UNKCA 0xCAu
#define O_UNKCB 0xCBu
#define O_UNKCC 0xCCu
#define O_UNKCD 0xCDu
#define CMD_BASE 0x0Cu            /* func_80041728 commands[4] at +0x0C */
/* func_80041488 struct Self / struct Data */
#define S_OLD 0x21u
#define S_MODEL 0x28u
#define S_BASIS 0x2Cu
#define S_POS 0x50u
#define S_INDEX 0x70u
#define S_VALUES 0xA0u
#define S_SPEEDS 0xACu
#define S_CURRENT 0xC0u
#define S_ZERO 0xC4u               /* s16: same block as the msg-11 hit counter */
#define S_FLAGS 0xC6u
#define S_KIND 0xC7u
#define S_COUNT 0xC8u
#define S_LAST 0xC9u
#define S_END 0xCAu
#define S_MODE 0xCBu
#define S_PREVIOUS 0xCCu
#define S_EXTRA 0xCDu
#define D_COUNT 0x24u
#define D_END 0x25u
#define D_POS 0x26u
#define D_SPEED 0x2Cu
#define D_OTHER 0x32u
#define D_ANGLE 0x36u
#define D_FLAGS 0x38u
#define D_KIND 0x39u
#define D_EXTRA 0x3Au
/* carrier / child fields */
#define C_UNK1C 0x1Cu
#define C_UNK20 0x20u
#define C_UNK24 0x24u
#define C_UNK158 0x158u
#define C_UNKA0 0xA0u
/* container / child struct of func_80039298 */
#define P_FLAGS 0xE8u
#define P_CHILD 0xECu
#define K_TYPE 0x16u
#define K_UNK08 0x08u
#define K_UNK1C 0x1Cu
#define K_COUNT 0x73u
#define K_OBJECTS 0x158u

/* ------------------------------------------------------------- memory model */

extern u8 gRetailRam[] __asm__("D_RAM");

static u8 *mem(u32 retail) { return gRetailRam + (retail - RETAIL_BASE); }
static u32 host_of(u32 retail) { return retail - RELOC; }
static u32 retail_of_host(u32 p) { return p + RELOC; }
static u32 rd32(u32 a) { u8 *p = mem(a); return p[0] | (u32)p[1] << 8 | (u32)p[2] << 16 | (u32)p[3] << 24; }
static s32 rds32(u32 a) { return (s32)rd32(a); }
static s32 rds16(u32 a) { u8 *p = mem(a); return (s16)(p[0] | p[1] << 8); }
static s32 rdu8(u32 a) { return *mem(a); }
static void wr8(u32 a, u32 v) { *mem(a) = (u8)v; }
static void wr16(u32 a, u32 v) { u8 *p = mem(a); p[0] = (u8)v; p[1] = (u8)(v >> 8); }
static void wr32(u32 a, u32 v) { u8 *p = mem(a); p[0] = (u8)v; p[1] = (u8)(v >> 8); p[2] = (u8)(v >> 16); p[3] = (u8)(v >> 24); }

/* Map a runtime pointer to its retail form for assertions. Host pointers
 * inside the relocated window convert back; others pass through. */
static u32 to_retail_ptr(void *p)
{
    u32 v = (u32)p;
    if (v >= RETAIL_BASE - RELOC && v < RETAIL_BASE - RELOC + RAM_SIZE) return v + RELOC;
    return v;
}

/* -------------------------------------------------------------- under test */

extern s32 gHandleElevatorEntityMessages(s32, void *, s32, s32) __asm__("func_80041B6C");
extern void gInitElevator(void *, void *, s32) __asm__("func_80041488");
extern s32 gStepElevator(void *, s32) __asm__("func_80041880");
extern void gRunEntityCommands(void *) __asm__("func_80041728");
extern void gSetupMountedTurretCarrier(s32, void *, void *, s32) __asm__("func_80039504");
extern s32 gHandleMountedTurretCarrierMessages(s32, void *, void *, s32) __asm__("func_800395A8");
extern s32 gHandleMountedPartMsg(s32, void *, void *, s32) __asm__("func_80039298");
extern void *gSpawnMountedPartChild(void *, s32, s16, s32, u8, u8, s32) __asm__("func_80039418");

/* The two-slot player table func_80041880 walks (retail D_8009FA58, zeroed
 * at boot; the harness defines the slot explicitly and sets it per case). */
struct UnkAC41880 { u8 pad[0x40]; s32 unk40; };
struct Game41880 { u8 pad00[0xAC]; struct UnkAC41880 *unkAC; u8 padB0[0x14]; void *unkC4; };
struct Game41880 *gPlayerEntities[2] __asm__("D_8009FA58");

/* ------------------------------------------------------- recording stubs */

/* Recorded-call labels: shared string constants so pointer-identity order
 * assertions work regardless of string-literal merging. */
static const char N_BASE[] = "func_8002AD58";
static const char N_REGGROUND[] = "func_8006314C";
static const char N_BUILDROT[] = "func_800188D0";
static const char N_GROUNDH[] = "func_80066F78";
static const char N_GTELOAD[] = "func_80010750";
static const char N_GTETRANS[] = "func_80010234";
static const char N_REMOVEKEY[] = "func_80063194";
static const char N_REMOVECMD[] = "func_8002A5B4";
static const char N_STOPCH[] = "func_800457B8";
static const char N_POSFX[] = "func_80047284";
static const char N_TYPE5[] = "func_800382B4";
static const char N_QUEUE[] = "func_8003FA34";
static const char N_QUART[] = "func_8003BA50";
static const char N_TURRET[] = "func_80038A00";
static const char N_ACTORINIT[] = "func_80038754";
static const char N_TYPE28[] = "func_800398EC";
static const char N_TRANSVTX[] = "func_80017668";
static const char N_SPAWNQ[] = "func_8004D444";

/* Configured return values / captured arguments. */
static s32 gBaseRet;           /* func_8002AD58 */
static s32 gGroundRet;         /* func_80066F78 */
static s32 gChildRet;          /* func_8004D444: retail pointer of the spawned child */
static u32 gCapBaseObj;        /* func_8002AD58 self (retail form) */
static u32 gCapRegGroundObj; static s32 gCapRegKind;
static s32 gCapBuildYaw; static u32 gCapBuildOut;
static u32 gCapGroundPos;
static u32 gCapGteIn, gCapGteOut; static s32 gCapGteCount;
static s32 gCapStopId, gCapStopCh;
static s32 gCapPosId, gCapPosX, gCapPosZ;
static u32 gCapType5Obj, gCapType5Data; static s32 gCapType5Msg, gCapType5A3;
static u32 gCapQueueChild;
static u32 gCapQuartChild; static s32 gCapQuartPrev;
static u32 gCapTurretChild; static u32 gCapTurretData;
static u32 gCapActorObj, gCapActorData; static s32 gCapActorA3;
static s32 gCapType28Msg; static u32 gCapType28Obj;
static u32 gCapTransCoord; static s32 gCapTransPart; static u32 gCapTransOut;
static u32 gCapSpawnReq; static s32 gCapSpawnHandlerIsContainer;
static s32 gCapSpawnKind, gCapSpawnTag, gCapSpawnFlags;
static s32 gCapSpawnArg1, gCapSpawnArg2, gCapSpawnArg3;
static s32 gCapSpawnArg4, gCapSpawnArg5, gCapSpawnArg6;
static s32 gCapSpawnPos0, gCapSpawnPos1, gCapSpawnPos2;
static s32 gCapRemoveCmdIdx; static s32 gRemoveCmdClears;
static u32 gCapOwner;         /* args.owner of the spawn request (retail form) */

/* func_8002AD58 gEntityBaseHandler - the shared base handler. */
s32 gEntityBaseHandler(s32 msg, void *obj, void *data, s32 a3) __asm__("func_8002AD58");
s32 gEntityBaseHandler(s32 msg, void *obj, void *data, s32 a3)
{ gCapBaseObj = to_retail_ptr(obj); fck_record(N_BASE, msg, (s32)data, a3); return gBaseRet; }

/* func_8006314C gRegisterGroundObject(self, kind) - lift kind registration. */
void gRegisterGroundObjectStub(void *self, u8 kind) __asm__("func_8006314C");
void gRegisterGroundObjectStub(void *self, u8 kind)
{ gCapRegGroundObj = to_retail_ptr(self); gCapRegKind = kind; fck_record(N_REGGROUND, kind, 0, 0); }

/* func_800188D0 gBuildRotationMatrix(yaw, pitch, roll, out). */
void gBuildRotationMatrixStub(s32 a0, s32 a1, s32 a2, void *out) __asm__("func_800188D0");
void gBuildRotationMatrixStub(s32 a0, s32 a1, s32 a2, void *out)
{ gCapBuildYaw = a0; gCapBuildOut = to_retail_ptr(out); fck_record(N_BUILDROT, a0, a1, a2); }

/* func_80066F78 gGetGroundHeight(pos, ...) - returns the configured height. */
s32 gGetGroundHeightStub(void *pos, s32 a1, s32 a2, s32 a3) __asm__("func_80066F78");
s32 gGetGroundHeightStub(void *pos, s32 a1, s32 a2, s32 a3)
{ gCapGroundPos = to_retail_ptr(pos); fck_record(N_GROUNDH, a1, a2, a3); return gGroundRet; }

/* func_80010750 gGteLoadMatrix(matrix). */
void gGteLoadMatrixStub(void *m) __asm__("func_80010750");
void gGteLoadMatrixStub(void *m)
{ fck_record(N_GTELOAD, (s32)to_retail_ptr(m), 0, 0); }

/* func_80010234 gGteTransformVerticesWide(in, out, count). */
void gGteTransformVerticesWideStub(void *in, void *out, s32 n) __asm__("func_80010234");
void gGteTransformVerticesWideStub(void *in, void *out, s32 n)
{ gCapGteIn = to_retail_ptr(in); gCapGteOut = to_retail_ptr(out); gCapGteCount = n;
  fck_record(N_GTETRANS, n, 0, 0); }

/* func_80063194 gRemoveGroundKeyEntry(obj). */
void gRemoveGroundKeyEntryStub(void *obj) __asm__("func_80063194");
void gRemoveGroundKeyEntryStub(void *obj)
{ fck_record(N_REMOVEKEY, (s32)to_retail_ptr(obj), 0, 0); }

/* func_8002A5B4 gRemoveEntityCommand(script, index) - removes a handled
 * command from the queue by shifting the remaining slots down (the stand-in
 * re-implements only what the retail removal guarantees; the shift keeps the
 * interpreter loop equivalent). */
void gRemoveEntityCommandStub(void *script, s32 index) __asm__("func_8002A5B4");
void gRemoveEntityCommandStub(void *script, s32 index)
{
    u32 base = to_retail_ptr(script);
    s32 slot;
    if (gRemoveCmdClears && index >= 0 && index < 4) {
        for (slot = index; slot < 3; slot++) {
            wr8(base + CMD_BASE + slot * 2, rdu8(base + CMD_BASE + (slot + 1) * 2));
            wr8(base + CMD_BASE + slot * 2 + 1, rdu8(base + CMD_BASE + (slot + 1) * 2 + 1));
        }
        wr8(base + CMD_BASE + 3 * 2, 0);
        wr8(base + CMD_BASE + 3 * 2 + 1, 0);
    }
    gCapRemoveCmdIdx = index;
    fck_record(N_REMOVECMD, index, 0, 0);
}

/* func_800457B8 gStopSoundChannelByIndex(id, channel). */
void gStopSoundChannelByIndexStub(s32 id, s32 ch) __asm__("func_800457B8");
void gStopSoundChannelByIndexStub(s32 id, s32 ch)
{ gCapStopId = id; gCapStopCh = ch; fck_record(N_STOPCH, id, ch, 0); }

/* func_80047284 gPlayPositionalSoundEffect(id, x, z, range). */
void gPlayPositionalSoundEffectStub(s32 id, s32 x, s32 z, s32 range) __asm__("func_80047284");
void gPlayPositionalSoundEffectStub(s32 id, s32 x, s32 z, s32 range)
{ gCapPosId = id; gCapPosX = x; gCapPosZ = z; fck_record(N_POSFX, id, x, z); (void)range; }

/* func_800382B4 gHandleType5Entity - class-5 base handler. */
void gHandleType5EntityStub(s32 msg, void *self, void *data, s32 a3) __asm__("func_800382B4");
void gHandleType5EntityStub(s32 msg, void *self, void *data, s32 a3)
{ gCapType5Obj = to_retail_ptr(self); gCapType5Data = to_retail_ptr(data);
  gCapType5Msg = msg; gCapType5A3 = a3;
  fck_record(N_TYPE5, msg, a3, 0); }

/* func_8003FA34 gQueueOrApplyMapOccupancy(child). */
void gQueueOrApplyMapOccupancyStub(void *child) __asm__("func_8003FA34");
void gQueueOrApplyMapOccupancyStub(void *child)
{ gCapQueueChild = to_retail_ptr(child); fck_record(N_QUEUE, 0, 0, 0); }

/* func_8003BA50 gFlagQuartileChange(child, previous). */
void gFlagQuartileChangeStub(void *child, s32 value) __asm__("func_8003BA50");
void gFlagQuartileChangeStub(void *child, s32 value)
{ gCapQuartChild = to_retail_ptr(child); gCapQuartPrev = value;
  fck_record(N_QUART, value, 0, 0); }

/* func_80038A00 gUpdateMountedTurret(child, payload). */
void gUpdateMountedTurretStub(void *child, void *payload) __asm__("func_80038A00");
void gUpdateMountedTurretStub(void *child, void *payload)
{ gCapTurretChild = to_retail_ptr(child); gCapTurretData = (u32)payload;
  fck_record(N_TURRET, (s32)payload, 0, 0); }

/* func_80038754 gInitActorFromSpawn(obj, data, a3). */
void gInitActorFromSpawnStub(void *obj, void *data, s32 a3) __asm__("func_80038754");
void gInitActorFromSpawnStub(void *obj, void *data, s32 a3)
{ gCapActorObj = to_retail_ptr(obj); gCapActorData = to_retail_ptr(data); gCapActorA3 = a3;
  fck_record(N_ACTORINIT, a3, 0, 0); }

/* func_800398EC gHandleType28Entity(msg, obj, data, a3). */
void gHandleType28EntityStub(s32 msg, void *obj, void *data, s32 a3) __asm__("func_800398EC");
void gHandleType28EntityStub(s32 msg, void *obj, void *data, s32 a3)
{ gCapType28Msg = msg; gCapType28Obj = to_retail_ptr(obj);
  fck_record(N_TYPE28, msg, a3, 0); (void)data; }

/* func_80017668 gTransformPartVertex(coord, part, out) - writes a fixed
 * position so the spawn-request chain is observable. */
void gTransformPartVertexStub(u8 *coord, s32 part, void *out) __asm__("func_80017668");
void gTransformPartVertexStub(u8 *coord, s32 part, void *out)
{
    s32 *dst = (s32 *)out;
    gCapTransCoord = to_retail_ptr(coord); gCapTransPart = part; gCapTransOut = (u32)out;
    dst[0] = 0x11110000; dst[1] = 0x22220000; dst[2] = 0x33330000;
    fck_record(N_TRANSVTX, part, 0, 0);
}

/* func_8004D444 gSpawnQueuedEntity(request) - records the request the REAL
 * func_80039418 assembled and returns the configured child. The retail
 * request/args layouts are mirrored from 80039418.c; reading them here pins
 * the spawn chain's argument flow without fabricating behaviour. */
struct Request39418 {
    void (*handler)();
    s32 unk04;
    void *args;
    s32 unk0C;
};
struct Args39418 {
    s32 tag;               /* +0x00 */
    u8 kind;               /* +0x04 */
    u8 pad05[3];
    s32 pos[3];            /* +0x08 position from gTransformPartVertex */
    u32 flags;             /* +0x14 */
    u16 unk18;             /* +0x18 */
    u16 unk1A;             /* +0x1A */
    u8 unk1C;              /* +0x1C */
    u8 unk1D;              /* +0x1D */
    u8 unk1E;              /* +0x1E */
    u8 unk1F;              /* +0x1F */
    u8 unk20;              /* +0x20 */
    u8 unk21;              /* +0x21 */
    u8 unk22;              /* +0x22 */
    u8 unk23;              /* +0x23 */
    s32 unk24;             /* +0x24 */
    s16 unk28;             /* +0x28 */
    u8 unk2A;              /* +0x2A */
    u8 unk2B;              /* +0x2B */
    s32 unk2C;             /* +0x2C */
    void *owner;           /* +0x30 */
};
/* Same retail label as gHandleMountedPartMsg, used only for the identity
 * check of the request handler (aliased declaration, never defined here). */
extern void gHandleMountedPartMsgAlias() __asm__("func_80039298");

void *gSpawnQueuedEntityStub(void *request) __asm__("func_8004D444");
void *gSpawnQueuedEntityStub(void *request)
{
    struct Request39418 *req = (struct Request39418 *)request;
    struct Args39418 *args = (struct Args39418 *)req->args;
    gCapSpawnReq = (u32)request;
    gCapSpawnHandlerIsContainer = (req->handler == (void (*)())gHandleMountedPartMsgAlias);
    gCapSpawnKind = args->kind;
    gCapSpawnTag = args->tag;
    gCapSpawnFlags = (s32)args->flags;
    gCapSpawnArg1 = args->unk24;
    gCapSpawnArg2 = args->unk28;
    gCapSpawnArg3 = args->unk2A;
    gCapSpawnArg4 = args->unk2B;
    gCapSpawnArg5 = args->unk20;
    gCapSpawnArg6 = args->unk2C;
    gCapSpawnPos0 = args->pos[0]; gCapSpawnPos1 = args->pos[1]; gCapSpawnPos2 = args->pos[2];
    gCapOwner = to_retail_ptr(args->owner);
    fck_record(N_SPAWNQ, 0, 0, 0);
    if (gChildRet == 0) return 0;
    return (void *)host_of((u32)gChildRet);
}

/* ------------------------------------------------------------------ reset */

static int g_cases;
static void fck_case(const char *name) { g_cases++; }

static void reset_all(void)
{
    int i;
    for (i = 0; i < (int)RAM_SIZE; i++) gRetailRam[i] = 0;
    fck_log_reset();
    gBaseRet = 0; gGroundRet = 0; gChildRet = 0;
    gCapBaseObj = 0; gCapRegGroundObj = 0; gCapRegKind = -1;
    gCapBuildYaw = -1; gCapBuildOut = 0; gCapGroundPos = 0;
    gCapGteIn = 0; gCapGteOut = 0; gCapGteCount = 0;
    gCapStopId = -1; gCapStopCh = -1; gCapPosId = -1; gCapPosX = 0; gCapPosZ = 0;
    gCapType5Obj = 0; gCapType5Data = 0; gCapType5Msg = -1; gCapType5A3 = 0;
    gCapQueueChild = 0;
    gCapQuartChild = 0; gCapQuartPrev = 0;
    gCapTurretChild = 0; gCapTurretData = 0;
    gCapActorObj = 0; gCapActorData = 0; gCapActorA3 = 0;
    gCapType28Msg = -1; gCapType28Obj = 0;
    gCapTransCoord = 0; gCapTransPart = -1; gCapTransOut = 0;
    gCapSpawnReq = 0; gCapSpawnHandlerIsContainer = 0; gCapSpawnKind = 0;
    gCapSpawnTag = 0; gCapSpawnFlags = 0;
    gCapSpawnArg1 = 0; gCapSpawnArg2 = 0; gCapSpawnArg3 = 0;
    gCapSpawnArg4 = 0; gCapSpawnArg5 = 0; gCapSpawnArg6 = 0;
    gCapSpawnPos0 = 0; gCapSpawnPos1 = 0; gCapSpawnPos2 = 0;
    gCapOwner = 0;
    gCapRemoveCmdIdx = -1; gRemoveCmdClears = 1;
    gPlayerEntities[0] = 0;
    gPlayerEntities[1] = 0;
    /* the lift's model records (used by the init/step GTE tail) */
    wr32(ADDR_LIFT + S_MODEL, host_of(ADDR_MODELREC));
    wr32(ADDR_MODELREC + 0x34, host_of(ADDR_FRAMES));
}

/* Builds the synthetic script record + wrapper the message-2 path receives.
 * Layout per struct Data in 80041488.c. */
static void build_record(s32 count, s32 end, const s16 *pos, const s16 *spd,
                         s32 other0, s32 other1, s32 angle, s32 flags, s32 kind, s32 extra)
{
    int i;
    u8 *rec = mem(ADDR_RECORD);
    for (i = 0; i < 0x18; i++) rec[D_COUNT + i] = 0;
    wr8(ADDR_RECORD + D_COUNT, (u32)count);
    wr8(ADDR_RECORD + D_END, (u32)end);
    for (i = 0; i < 3; i++) wr16(ADDR_RECORD + D_POS + i * 2, (u32)pos[i]);
    for (i = 0; i < 3; i++) wr16(ADDR_RECORD + D_SPEED + i * 2, (u32)spd[i]);
    wr16(ADDR_RECORD + D_OTHER + 0, (u32)other0);
    wr16(ADDR_RECORD + D_OTHER + 2, (u32)other1);
    wr16(ADDR_RECORD + D_ANGLE, (u32)angle);
    wr8(ADDR_RECORD + D_FLAGS, (u32)flags);
    wr8(ADDR_RECORD + D_KIND, (u32)kind);
    wr8(ADDR_RECORD + D_EXTRA, (u32)extra);
    wr32(ADDR_WRAPPER + 0x08, host_of(ADDR_RECORD));
}

/* --------------------------------------------- A. func_80041B6C dispatch */

/* The dispatch test uses the REAL init/step units (linked unchanged), so the
 * msg-2 and msg-6 cases also prove the argument forwarding end to end. */
static void test_dispatch(void)
{
    s32 r;
    static const s16 pos1[3] = { 50, 0, 0 };
    static const s16 spd1[3] = { 218, 0, 0 };
    struct FckCall c;

    fck_case("A1 msg 1 returns the block size 0xD0 and calls nothing");
    reset_all();
    r = gHandleElevatorEntityMessages(1, mem(ADDR_LIFT), 0x77, 0x88);
    FCK_EQ(r, 0xD0, "dispatch: msg 1 returns 0xD0");
    FCK_EQ(fck_log_count, 0, "dispatch: msg 1 calls nothing");

    fck_case("A2 msg 2 forwards (obj, a2, a3) to the real init");
    reset_all();
    gGroundRet = 0x100;
    build_record(1, 1, pos1, spd1, 0, 0, 0x400, 0x1, 0x42, 0x77);
    wr8(ADDR_LIFT + S_OLD, 0x5A);
    wr16(ADDR_LIFT + S_INDEX, 0);
    r = gHandleElevatorEntityMessages(2, mem(ADDR_LIFT), (void *)host_of(ADDR_WRAPPER), 0xABC);
    /* msg 2 hardcodes 0; the callee assertions prove the real init ran */
    c = fck_call_n(N_BASE, 0);
    FCK_EQ(r, 0, "dispatch: msg 2 returns 0");
    FCK_EQ(c.a0, 2, "dispatch: msg 2 runs the base handler inside the init");
    FCK_EQ(rdu8(ADDR_LIFT + S_COUNT), 1, "dispatch: msg 2 ran the real init (count copied)");
    FCK_EQ(rdu8(ADDR_LIFT + S_MODE), 2, "dispatch: msg 2 init derivation ran (mode 2)");
    FCK_EQ(rds32(ADDR_LIFT + S_VALUES + 0), (50 << 4) + 0x100, "dispatch: init stop height copied + ground");
    FCK_EQ(c.a2, 0xABC, "dispatch: msg 2 forwards a3 to init and its base handler");

    fck_case("A3 msg 5 removes the ground key first, then the base handler");
    reset_all();
    gBaseRet = 0x999;   /* must be ignored: msg 5 hardcodes 0 */
    r = gHandleElevatorEntityMessages(5, mem(ADDR_LIFT), 0x555, 0x666);
    FCK_EQ(r, 0, "dispatch: msg 5 returns 0");
    FCK_EQ(fck_log_count, 2, "dispatch: msg 5 calls exactly two callees");
    FCK_EQ(fck_log[0].name == N_REMOVEKEY, 1, "dispatch: msg 5 calls the key removal first");
    FCK_EQ(fck_log[1].name == N_BASE, 1, "dispatch: msg 5 then the base handler");
    c = fck_call_n(N_BASE, 0);
    FCK_EQ(c.a0, 5, "dispatch: msg 5 base call carries message 5");

    fck_case("A4 msg 6 forwards (obj, a2) to the real step");
    reset_all();
    wr8(ADDR_LIFT + O_UNKC6, 0);
    wr32(ADDR_LIFT + O_UNKC0, (u32)-1);   /* armed */
    wr8(ADDR_LIFT + O_UNKCB, 1);          /* direction up */
    wr8(ADDR_LIFT + O_UNKCA, 0);          /* index 0 */
    wr8(ADDR_LIFT + O_UNKC8, 3);          /* three stops */
    wr32(ADDR_LIFT + O_UNKB8, 100);       /* up speed */
    wr32(ADDR_LIFT + O_UNKA0 + 0, 5000);  /* thresholds[0] */
    wr16(ADDR_LIFT + O_UNK70, 1);         /* frame index -> GTE input offset */
    r = gHandleElevatorEntityMessages(6, mem(ADDR_LIFT), 2, 0);
    /* The real step always returns 0; its side effects prove dispatch, not r. */
    FCK_EQ(r, 0, "dispatch: msg 6 returns 0");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 200, "dispatch: msg 6 ran the real step (height += 2*100)");
    FCK_EQ(fck_calls_to(N_GTELOAD), 1, "dispatch: msg 6 step reached the frame-refresh tail");

    fck_case("A5 msg 11 increments the s16 hit counter at +0xC4: 19 -> 20");
    reset_all();
    wr16(ADDR_LIFT + 0xC4, 19);
    r = gHandleElevatorEntityMessages(11, mem(ADDR_LIFT), 0, 0);
    FCK_EQ(r, 0, "dispatch: msg 11 returns 0");
    FCK_EQ(rds16(ADDR_LIFT + 0xC4), 20, "dispatch: msg 11 increments 19 -> 20");

    fck_case("A6 msg 11 caps the hit counter at 20: 20 -> 20");
    reset_all();
    wr16(ADDR_LIFT + 0xC4, 20);
    gHandleElevatorEntityMessages(11, mem(ADDR_LIFT), 0, 0);
    FCK_EQ(rds16(ADDR_LIFT + 0xC4), 20, "dispatch: msg 11 caps the counter at 20");
    FCK_EQ(fck_calls_to(N_BASE), 1, "dispatch: msg 11 still runs the base handler");

    fck_case("A7 msg 11 low values increment normally: 5 -> 6");
    reset_all();
    wr16(ADDR_LIFT + 0xC4, 5);
    gHandleElevatorEntityMessages(11, mem(ADDR_LIFT), 0, 0);
    FCK_EQ(rds16(ADDR_LIFT + 0xC4), 6, "dispatch: msg 11 increments 5 -> 6");

    fck_case("A8 msg 11 signed counter below zero still satisfies < 20: -1 -> 0");
    reset_all();
    wr16(ADDR_LIFT + 0xC4, 0xFFFF);
    gHandleElevatorEntityMessages(11, mem(ADDR_LIFT), 0, 0);
    FCK_EQ(rds16(ADDR_LIFT + 0xC4), 0, "dispatch: msg 11 increments -1 -> 0");

    fck_case("A9 default message forwards to the base handler, returns its result");
    reset_all();
    gBaseRet = 0x7788;
    r = gHandleElevatorEntityMessages(3, mem(ADDR_LIFT), 0x1357, 0x2468);
    FCK_EQ(r, 0x7788, "dispatch: default msg returns the base result");
    c = fck_call_n(N_BASE, 0);
    FCK_EQ(c.a0, 3, "dispatch: default msg forwards the message id");
    FCK_EQ(fck_log_count, 1, "dispatch: default msg calls only the base handler");

    fck_case("A10 another default message (12) takes the same path");
    reset_all();
    gBaseRet = -1;
    r = gHandleElevatorEntityMessages(12, mem(ADDR_LIFT), 0, 0);
    FCK_EQ(r, -1, "dispatch: msg 12 returns the base result (-1)");
}

/* ------------------------------------- B. func_80041488 init from the record */

static void test_init_elevator(void)
{
    static const s16 pos[3] = { 100, 200, 300 };
    static const s16 spd[3] = { 218, 436, 654 };

    fck_case("B1 init copies/rescales the record and derives mode/last/end/current");
    reset_all();
    gGroundRet = 0x1234;
    build_record(3, 1, pos, spd, 75, 300, 0x400, 0x1, 0x42, 0x77);
    wr8(ADDR_LIFT + S_OLD, 0x5A);
    wr16(ADDR_LIFT + S_INDEX, 3);
    gInitElevator(mem(ADDR_LIFT), (void *)host_of(ADDR_WRAPPER), 0x99);

    /* count/d->pos<<4 = {1600,3200,4800} + ground 0x1234 - hand computed */
    FCK_EQ(rdu8(ADDR_LIFT + S_COUNT), 3, "init: count copied from the record");
    FCK_EQ(rds32(ADDR_LIFT + S_VALUES + 0), (100 << 4) + 0x1234, "init: value[0] = pos<<4 + ground");
    FCK_EQ(rds32(ADDR_LIFT + S_VALUES + 4), (200 << 4) + 0x1234, "init: value[1] = pos<<4 + ground");
    FCK_EQ(rds32(ADDR_LIFT + S_VALUES + 8), (300 << 4) + 0x1234, "init: value[2] = pos<<4 + ground");
    /* speeds = (v<<8)/218, others = (v<<4)/300 - hand computed */
    FCK_EQ(rds32(ADDR_LIFT + S_SPEEDS + 0), 256, "init: speed[0] = (218<<8)/218 = 256");
    FCK_EQ(rds32(ADDR_LIFT + S_SPEEDS + 4), 512, "init: speed[1] = (436<<8)/218 = 512");
    FCK_EQ(rds32(ADDR_LIFT + S_SPEEDS + 8), 768, "init: speed[2] = (654<<8)/218 = 768");
    FCK_EQ(rds32(ADDR_LIFT + O_UNKB8 + 0), 4, "init: other[0] = (75<<4)/300 = 4");
    FCK_EQ(rds32(ADDR_LIFT + O_UNKB8 + 4), 16, "init: other[1] = (300<<4)/300 = 16");
    /* end = 1 < count = 3: last = 0, mode stays 1, end stays 1 */
    FCK_EQ(rdu8(ADDR_LIFT + S_LAST), 0, "init: last = end - 1 = 0");
    FCK_EQ(rdu8(ADDR_LIFT + S_END), 1, "init: end kept (end < count)");
    FCK_EQ(rdu8(ADDR_LIFT + S_MODE), 1, "init: mode = 1 (end < count)");
    /* volatile round trip: previous = old, old = 0 */
    FCK_EQ(rdu8(ADDR_LIFT + S_PREVIOUS), 0x5A, "init: previous = old entity byte");
    FCK_EQ(rdu8(ADDR_LIFT + S_OLD), 0, "init: old cleared to 0");
    /* flags & 5 != 0 -> current = 1; zero s16 cleared */
    FCK_EQ(rds32(ADDR_LIFT + S_CURRENT), 1, "init: flags & 5 -> current = 1");
    FCK_EQ(rds16(ADDR_LIFT + S_ZERO), 0, "init: zero field cleared");
    FCK_EQ(rdu8(ADDR_LIFT + S_FLAGS), 0x1, "init: flags copied from the record");
    FCK_EQ(rdu8(ADDR_LIFT + S_KIND), 0x42, "init: kind copied from the record");
    FCK_EQ(rdu8(ADDR_LIFT + S_EXTRA), 0x77, "init: extra copied from the record");
    /* ground snap added to all values, entity placed at values[last] */
    FCK_EQ(rds32(ADDR_LIFT + S_POS + 4), (100 << 4) + 0x1234, "init: entity y placed at values[last]");
    /* callee arguments */
    FCK_EQ(gCapRegKind, 0x42, "init: ground object registered with the record kind");
    FCK_EQ(gCapRegGroundObj, ADDR_LIFT, "init: ground object registered with the entity");
    FCK_EQ(gCapBuildYaw, 0x400, "init: basis built from the record angle");
    FCK_EQ(gCapBuildOut, ADDR_LIFT + S_BASIS, "init: basis written at entity +0x2C");
    FCK_EQ(gCapGroundPos, ADDR_LIFT + S_POS, "init: ground height queried at entity +0x50");
    /* frame vertex refresh: in = frames + (index<<4) + 8, out = +0x5C, count 1 */
    FCK_EQ(gCapGteIn, ADDR_FRAMES + (3 << 4) + 8, "init: frame vertex input = frames + (index<<4) + 8");
    FCK_EQ(gCapGteOut, ADDR_LIFT + O_UNK5C, "init: frame vertex output at +0x5C");
    FCK_EQ(gCapGteCount, 1, "init: exactly one frame vertex transformed");
    /* full order: base handler, kind registration, basis build, ground query,
     * GTE matrix load, GTE vertex transform */
    FCK_EQ(fck_log_count, 6, "init: exactly six callees touched");
    FCK_EQ(fck_log[0].name == N_BASE, 1, "init: base handler runs first");
    FCK_EQ(fck_log[1].name == N_REGGROUND, 1, "init: kind registration second");
    FCK_EQ(fck_log[2].name == N_BUILDROT, 1, "init: basis build third");
    FCK_EQ(fck_log[3].name == N_GROUNDH, 1, "init: ground query fourth");
    FCK_EQ(fck_log[4].name == N_GTELOAD, 1, "init: GTE matrix load fifth");
    FCK_EQ(fck_log[5].name == N_GTETRANS, 1, "init: GTE vertex transform last");

    fck_case("B2 end >= count: mode 2, end = last - 1, current = speeds[last]");
    reset_all();
    gGroundRet = 0;   /* ground snap adds nothing */
    {
        static const s16 pos2[3] = { 10, 11, 12 };
        static const s16 spd2[3] = { 218, 0, 436 };
        build_record(3, 3, pos2, spd2, 0, 0, 0, 0x2, 1, 0);
    }
    wr8(ADDR_LIFT + S_OLD, 0);
    wr16(ADDR_LIFT + S_INDEX, 0);
    gInitElevator(mem(ADDR_LIFT), (void *)host_of(ADDR_WRAPPER), 0);

    FCK_EQ(rdu8(ADDR_LIFT + S_LAST), 2, "init2: last = end - 1 = 2");
    FCK_EQ(rdu8(ADDR_LIFT + S_MODE), 2, "init2: end >= count -> mode 2");
    FCK_EQ(rdu8(ADDR_LIFT + S_END), 1, "init2: end becomes last - 1 = 1");
    /* flags 0x2 & 5 == 0 -> current = speeds[last] = (436<<8)/218 = 512 */
    FCK_EQ(rds32(ADDR_LIFT + S_CURRENT), 512, "init2: current = speeds[last] when (flags & 5) == 0");
    FCK_EQ(rds32(ADDR_LIFT + S_POS + 4), 12 << 4, "init2: entity y placed at values[last] (no ground)");
    FCK_EQ(gCapGteIn, ADDR_FRAMES + 8, "init2: frame vertex uses index 0 (frames + 8)");

    fck_case("B3 signed speed rescaling truncates toward zero");
    reset_all();
    {
        static const s16 fractional[3] = { 217, -217, -219 };
        build_record(3, 1, pos, fractional, 299, -299, 0, 1, 0, 0);
    }
    gInitElevator(mem(ADDR_LIFT), (void *)host_of(ADDR_WRAPPER), 0);

    /* Retail init divides the signed scaled inputs at +0xAC and +0xB8.
     * The numerators are +/-55552, -56064, and +/-4784 respectively. */
    FCK_EQ(rds32(ADDR_LIFT + S_SPEEDS + 0), 254, "init3: 55552 / 218 truncates to 254");
    FCK_EQ(rds32(ADDR_LIFT + S_SPEEDS + 4), -254, "init3: -55552 / 218 truncates to -254");
    FCK_EQ(rds32(ADDR_LIFT + S_SPEEDS + 8), -257, "init3: -56064 / 218 truncates to -257");
    FCK_EQ(rds32(ADDR_LIFT + O_UNKB8 + 0), 15, "init3: 4784 / 300 truncates to 15");
    FCK_EQ(rds32(ADDR_LIFT + O_UNKB8 + 4), -15, "init3: -4784 / 300 truncates to -15");
}

/* --------------------------------------------- C. func_80041880 step-tick */

/* The step test drives the REAL func_80041728 command interpreter through
 * the unk0C gate plus every motion/hold/dwell/trigger gate, all observable
 * through memory and the recording stubs. */
static void test_step_gates(void)
{
    s32 r;

    fck_case("C1 frozen gate (flags bit 2): whole body and tail skipped");
    reset_all();
    wr8(ADDR_LIFT + O_UNKC6, 0x2);
    wr32(ADDR_LIFT + O_UNKC0, 77);
    r = gStepElevator(mem(ADDR_LIFT), 5);
    FCK_EQ(r, 0, "step: always returns 0");
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), 77, "step: frozen gate leaves the timer untouched");
    FCK_EQ(fck_log_count, 0, "step: frozen gate skips everything including the tail");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 0, "step: frozen gate does not move");

    fck_case("C2 hold countdown (+0xC4 s16 > 0): decremented once, body skipped");
    reset_all();
    wr16(ADDR_LIFT + O_UNKC4, 3);
    wr32(ADDR_LIFT + O_UNKC0, 10);
    r = gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(r, 0, "step: countdown case returns 0");
    FCK_EQ(rds16(ADDR_LIFT + O_UNKC4), 2, "step: countdown decrements by one (not by dt)");
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), 10, "step: countdown case leaves the dwell timer");
    FCK_EQ(fck_log_count, 0, "step: countdown case skips the tail");

    fck_case("C3 armed motion, direction up, below threshold: height += dt*speed");
    reset_all();
    wr8(ADDR_LIFT + O_UNKC6, 0);
    wr32(ADDR_LIFT + O_UNKC0, (u32)-1);
    wr8(ADDR_LIFT + O_UNKCB, 1);
    wr8(ADDR_LIFT + O_UNKCA, 1);
    wr8(ADDR_LIFT + O_UNKC8, 3);
    wr32(ADDR_LIFT + O_UNKB8, 100);
    wr32(ADDR_LIFT + O_UNKA0 + 4, 10000);   /* thresholds[1] */
    wr32(ADDR_LIFT + O_UNK54, 9000);
    wr16(ADDR_LIFT + O_UNK70, 1);           /* frame index for the tail */
    r = gStepElevator(mem(ADDR_LIFT), 2);
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 9200, "step: height += dt * up speed");
    FCK_EQ(rdu8(ADDR_LIFT + O_UNKCA), 1, "step: index unchanged below threshold");
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), -1, "step: timer stays armed without a stop change");
    FCK_EQ(fck_calls_to(N_GTELOAD), 1, "step: tail reloads the matrix");
    FCK_EQ(gCapGteIn, ADDR_FRAMES + (1 << 4) + 8, "step: tail vertex input = frames + (unk70<<4) + 8");
    FCK_EQ(gCapGteOut, ADDR_LIFT + O_UNK5C, "step: tail vertex output at +0x5C");
    FCK_EQ(gCapGteCount, 1, "step: tail transforms exactly one vertex");

    fck_case("C4 stop change without events: index snap, dwell restart, previous saved");
    reset_all();
    wr8(ADDR_LIFT + O_UNKC6, 0);
    wr32(ADDR_LIFT + O_UNKC0, (u32)-1);
    wr8(ADDR_LIFT + O_UNKCB, 1);
    wr8(ADDR_LIFT + O_UNKCA, 1);
    wr8(ADDR_LIFT + O_UNKC8, 3);
    wr32(ADDR_LIFT + O_UNKB8, 40);
    wr32(ADDR_LIFT + O_UNKA0 + 4, 9500);    /* thresholds[1] */
    wr32(ADDR_LIFT + O_UNKAC + 4, 13);      /* dwell[1] */
    wr32(ADDR_LIFT + O_UNK54, 9490);
    wr8(ADDR_LIFT + O_UNKCC, 0);           /* no stop-sound pending */
    wr8(ADDR_LIFT + O_UNKCD, 0);           /* no positional sound id */
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rdu8(ADDR_LIFT + O_UNKCA), 2, "step: index advances 1 -> 2 on threshold cross");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 9500, "step: height snaps to thresholds[old]");
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), 13, "step: dwell timer restarts from dwell[old]");
    FCK_EQ(rdu8(ADDR_LIFT + O_UNKC9), 1, "step: previous index recorded at +0xC9");
    FCK_EQ(rdu8(ADDR_LIFT + O_UNK21), 0, "step: unk21 stays 0 when unkCC was 0");
    FCK_EQ(fck_calls_to(N_STOPCH), 0, "step: no stop-sound without flag bit 0x400000");
    FCK_EQ(fck_calls_to(N_POSFX), 0, "step: no positional sound with unkCD 0");

    fck_case("C5 stop change with pending stop-sound (unkCC + flags bit 0x400000)");
    reset_all();
    wr8(ADDR_LIFT + O_UNKC6, 0);
    wr32(ADDR_LIFT + O_UNKC0, (u32)-1);
    wr8(ADDR_LIFT + O_UNKCB, 1);
    wr8(ADDR_LIFT + O_UNKCA, 1);
    wr8(ADDR_LIFT + O_UNKC8, 3);
    wr32(ADDR_LIFT + O_UNKB8, 40);
    wr32(ADDR_LIFT + O_UNKA0 + 4, 9500);
    wr32(ADDR_LIFT + O_UNKAC + 4, 13);
    wr32(ADDR_LIFT + O_UNK54, 9490);
    wr8(ADDR_LIFT + O_UNKCC, 1);
    wr32(ADDR_LIFT + O_UNK24, 0x400000);
    wr32(ADDR_LIFT + O_UNK08, 0x321);      /* stop-sound id */
    wr8(ADDR_LIFT + O_UNK22, 7);           /* channel */
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rdu8(ADDR_LIFT + O_UNKCA), 2, "stepC5: index advanced");
    FCK_EQ(rdu8(ADDR_LIFT + O_UNK21), 0, "stepC5: unk21 cleared after the change");
    FCK_EQ(fck_calls_to(N_STOPCH), 1, "stepC5: channel stop fired once");
    FCK_EQ(gCapStopId, 0x321, "stepC5: stop-sound id is unk08");
    FCK_EQ(gCapStopCh, 7, "stepC5: stop-sound channel is unk22");
    FCK_EQ(rd32(ADDR_LIFT + O_UNK24) & 0x400000u, 0, "stepC5: flag bit 0x400000 cleared");

    fck_case("C6 stop change with positional sound id unkCD");
    reset_all();
    wr8(ADDR_LIFT + O_UNKC6, 0);
    wr32(ADDR_LIFT + O_UNKC0, (u32)-1);
    wr8(ADDR_LIFT + O_UNKCB, 1);
    wr8(ADDR_LIFT + O_UNKCA, 1);
    wr8(ADDR_LIFT + O_UNKC8, 3);
    wr32(ADDR_LIFT + O_UNKB8, 40);
    wr32(ADDR_LIFT + O_UNKA0 + 4, 9500);
    wr32(ADDR_LIFT + O_UNKAC + 4, 13);
    wr32(ADDR_LIFT + O_UNK54, 9490);
    wr8(ADDR_LIFT + O_UNKCC, 0);
    wr8(ADDR_LIFT + O_UNKCD, 0x66);
    wr32(ADDR_LIFT + O_UNK50, 1122);       /* x */
    wr32(ADDR_LIFT + O_UNK58, 3344);       /* z */
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(fck_calls_to(N_POSFX), 1, "stepC6: positional sound fired once");
    FCK_EQ(gCapPosId, 0x66, "stepC6: positional sound id is unkCD");
    FCK_EQ(gCapPosX, 1122, "stepC6: positional sound x is unk50");
    FCK_EQ(gCapPosZ, 3344, "stepC6: positional sound z is unk58");

    fck_case("C7 direction up reversal at the last stop: dir 2, index old-1");
    reset_all();
    wr8(ADDR_LIFT + O_UNKC6, 0);
    wr32(ADDR_LIFT + O_UNKC0, (u32)-1);
    wr8(ADDR_LIFT + O_UNKCB, 1);
    wr8(ADDR_LIFT + O_UNKCA, 2);
    wr8(ADDR_LIFT + O_UNKC8, 3);
    wr32(ADDR_LIFT + O_UNKB8, 10);
    wr32(ADDR_LIFT + O_UNKA0 + 8, 5000);   /* thresholds[2] */
    wr32(ADDR_LIFT + O_UNKAC + 8, 21);     /* dwell[2] */
    wr32(ADDR_LIFT + O_UNK54, 4995);
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rds16(ADDR_LIFT + O_UNKCB), 2, "step: direction reverses to 2 at the top");
    FCK_EQ(rdu8(ADDR_LIFT + O_UNKCA), 1, "step: index falls back to old - 1");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 5000, "step: height snaps to thresholds[old]");
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), 21, "step: dwell restarts from dwell[old]");

    fck_case("C8 direction down: height -= dt*speed, bottom reversal to dir 1");
    reset_all();
    wr8(ADDR_LIFT + O_UNKC6, 0);
    wr32(ADDR_LIFT + O_UNKC0, (u32)-1);
    wr8(ADDR_LIFT + O_UNKCB, 2);
    wr8(ADDR_LIFT + O_UNKCA, 0);
    wr8(ADDR_LIFT + O_UNKC8, 3);
    wr32(ADDR_LIFT + O_UNKBC, 30);
    wr32(ADDR_LIFT + O_UNKA0 + 0, 200);    /* thresholds[0] */
    wr32(ADDR_LIFT + O_UNKAC + 0, 31);     /* dwell[0] */
    wr32(ADDR_LIFT + O_UNK54, 205);
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rds16(ADDR_LIFT + O_UNKCB), 1, "step: direction reverses to 1 at the bottom");
    FCK_EQ(rdu8(ADDR_LIFT + O_UNKCA), 1, "step: index goes back to old + 1");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 200, "step: height snaps to thresholds[old]");

    fck_case("C9 dwell countdown: timer -= dt, still positive -> body skipped");
    reset_all();
    wr8(ADDR_LIFT + O_UNKC6, 0);
    wr32(ADDR_LIFT + O_UNKC0, 5);
    gStepElevator(mem(ADDR_LIFT), 2);
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), 3, "step: dwell timer decreases by dt");
    FCK_EQ(fck_log_count, 0, "step: dwell countdown skips the tail");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 0, "step: dwell countdown does not move");

    fck_case("C10 dwell expiry arms the timer and moves on the same tick");
    reset_all();
    wr8(ADDR_LIFT + O_UNKC6, 0);
    wr32(ADDR_LIFT + O_UNKC0, 2);
    wr8(ADDR_LIFT + O_UNKCB, 1);
    wr8(ADDR_LIFT + O_UNKCA, 0);
    wr8(ADDR_LIFT + O_UNKC8, 1);
    wr32(ADDR_LIFT + O_UNKB8, 10);
    wr32(ADDR_LIFT + O_UNKA0 + 0, 1000);
    wr32(ADDR_LIFT + O_UNK54, 0);
    gStepElevator(mem(ADDR_LIFT), 5);
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), -1, "step: dwell expiry sets the timer to -1");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 50, "step: motion starts on the arming tick");

    fck_case("C11 timer 0 with no player riding: unchanged, body skipped");
    reset_all();
    wr8(ADDR_LIFT + O_UNKC6, 0);
    wr32(ADDR_LIFT + O_UNKC0, 0);
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), 0, "step: timer 0 without a player stays 0");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 0, "step: timer 0 without a player does not move");
    FCK_EQ(fck_log_count, 0, "step: timer 0 without a player skips the tail");

    fck_case("C12 timer 0 with a player riding: armed, motion starts");
    reset_all();
    wr32(ADDR_PSTATE + 0x40, 0);                    /* player state ok */
    wr32(ADDR_GAME + 0xAC, host_of(ADDR_PSTATE));
    wr32(ADDR_GAME + 0xC4, host_of(ADDR_LIFT));     /* game references the lift */
    gPlayerEntities[0] = (struct Game41880 *)host_of(ADDR_GAME);
    wr8(ADDR_LIFT + O_UNKC6, 0);
    wr32(ADDR_LIFT + O_UNKC0, 0);
    wr8(ADDR_LIFT + O_UNKCB, 2);
    wr8(ADDR_LIFT + O_UNKCA, 0);
    wr8(ADDR_LIFT + O_UNKC8, 1);
    wr32(ADDR_LIFT + O_UNKBC, 25);
    wr32(ADDR_LIFT + O_UNKA0 + 0, 50);
    wr32(ADDR_LIFT + O_UNK54, 90);
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), -1, "step: player trigger arms the timer");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 65, "step: motion starts downward from the player trigger");

    fck_case("C13 timer 0 with a busy player (state word != 0): not armed");
    reset_all();
    wr32(ADDR_PSTATE + 0x40, 5);
    wr32(ADDR_GAME + 0xAC, host_of(ADDR_PSTATE));
    wr32(ADDR_GAME + 0xC4, host_of(ADDR_LIFT));
    gPlayerEntities[0] = (struct Game41880 *)host_of(ADDR_GAME);
    wr8(ADDR_LIFT + O_UNKC6, 0);
    wr32(ADDR_LIFT + O_UNKC0, 0);
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), 0, "step: busy player state does not arm the timer");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 0, "step: busy player state does not move");

    fck_case("C14 trigger mode (flags & 5), timer >= 0, player, bit 4 clear: armed");
    reset_all();
    wr32(ADDR_PSTATE + 0x40, 0);
    wr32(ADDR_GAME + 0xAC, host_of(ADDR_PSTATE));
    wr32(ADDR_GAME + 0xC4, host_of(ADDR_LIFT));
    gPlayerEntities[0] = (struct Game41880 *)host_of(ADDR_GAME);
    wr8(ADDR_LIFT + O_UNKC6, 0x1);       /* trigger mode, bit 4 clear */
    wr32(ADDR_LIFT + O_UNKC0, 7);
    wr8(ADDR_LIFT + O_UNKCB, 1);
    wr8(ADDR_LIFT + O_UNKCA, 0);
    wr8(ADDR_LIFT + O_UNKC8, 1);
    wr32(ADDR_LIFT + O_UNKB8, 10);
    wr32(ADDR_LIFT + O_UNKA0 + 0, 100);
    wr32(ADDR_LIFT + O_UNK54, 0);
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), -1, "step: trigger mode arms on the player");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 10, "step: trigger mode moves after arming");

    fck_case("C15 trigger mode with mask 4 set: stays held even with the player");
    reset_all();
    wr32(ADDR_PSTATE + 0x40, 0);
    wr32(ADDR_GAME + 0xAC, host_of(ADDR_PSTATE));
    wr32(ADDR_GAME + 0xC4, host_of(ADDR_LIFT));
    gPlayerEntities[0] = (struct Game41880 *)host_of(ADDR_GAME);
    wr8(ADDR_LIFT + O_UNKC6, 0x5);      /* trigger mode (bit0) + bit 2: the hold bit itself */
    wr32(ADDR_LIFT + O_UNKC0, 7);
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), 7, "step: mask 0x4 keeps the trigger mode held");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 0, "step: mask 0x4 suppresses the motion");

    fck_case("C16 trigger-mode stop change reloads the timer with the constant 1");
    reset_all();
    wr8(ADDR_LIFT + O_UNKC6, 0x1);       /* (flags & 5) != 0 */
    wr32(ADDR_LIFT + O_UNKC0, (u32)-1);  /* already armed: motion runs directly */
    wr8(ADDR_LIFT + O_UNKCB, 1);
    wr8(ADDR_LIFT + O_UNKCA, 1);
    wr8(ADDR_LIFT + O_UNKC8, 3);
    wr32(ADDR_LIFT + O_UNKB8, 40);
    wr32(ADDR_LIFT + O_UNKA0 + 4, 9500);
    wr32(ADDR_LIFT + O_UNKAC + 4, 1234);  /* per-stop dwell must be ignored */
    wr32(ADDR_LIFT + O_UNK54, 9490);
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rdu8(ADDR_LIFT + O_UNKCA), 2, "stepC16: index advanced");
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), 1, "stepC16: trigger-mode stop change sets timer 1, not dwell");

    fck_case("C17 command 0x53 arg 0 sets the frozen bit via the real interpreter");
    reset_all();
    wr8(ADDR_LIFT + O_UNK0C, 1);                 /* command pending -> interpreter runs */
    wr8(ADDR_LIFT + CMD_BASE + 0, 0x53);         /* commands[0] = {0x53, 0} */
    wr8(ADDR_LIFT + CMD_BASE + 1, 0);
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rdu8(ADDR_LIFT + O_UNKC6) & 2, 2, "step: opcode 0x53 arg 0 sets the frozen bit");
    FCK_EQ(gCapRemoveCmdIdx, 0, "step: handled command removed from slot 0");
    FCK_EQ(fck_calls_to(N_REMOVECMD), 1, "step: exactly one removal recorded");
    /* the command froze the entity, so nothing else runs */
    FCK_EQ(fck_log_count, 1, "step: frozen-after-command body skipped");

    fck_case("C18 command 0x53 arg 1 clears the frozen bit");
    reset_all();
    wr8(ADDR_LIFT + O_UNK0C, 1);
    wr8(ADDR_LIFT + O_UNKC6, 0x2);               /* start frozen */
    wr8(ADDR_LIFT + CMD_BASE + 0, 0x53);
    wr8(ADDR_LIFT + CMD_BASE + 1, 1);            /* arg != 0 -> clear bit 1 */
    /* after clearing, the body runs: give it a benign armed state */
    wr32(ADDR_LIFT + O_UNKC0, (u32)-1);
    wr8(ADDR_LIFT + O_UNKCB, 1);
    wr8(ADDR_LIFT + O_UNKCA, 0);
    wr8(ADDR_LIFT + O_UNKC8, 1);
    wr32(ADDR_LIFT + O_UNKB8, 10);
    wr32(ADDR_LIFT + O_UNKA0 + 0, 1000);
    wr32(ADDR_LIFT + O_UNK54, 0);
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rdu8(ADDR_LIFT + O_UNKC6) & 2, 0, "step: opcode 0x53 arg != 0 clears the frozen bit");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 10, "step: body ran after clearing (height += 1*10)");

    fck_case("C19 command 0x52 arg 0 arms the dwell timer to -1");
    reset_all();
    wr8(ADDR_LIFT + O_UNK0C, 1);
    wr8(ADDR_LIFT + CMD_BASE + 0, 0x52);
    wr8(ADDR_LIFT + CMD_BASE + 1, 0);
    wr32(ADDR_LIFT + O_UNKC0, 5);                /* timer must be overwritten */
    wr8(ADDR_LIFT + O_UNKCB, 1);
    wr8(ADDR_LIFT + O_UNKCA, 0);
    wr8(ADDR_LIFT + O_UNKC8, 1);
    wr32(ADDR_LIFT + O_UNKB8, 10);
    wr32(ADDR_LIFT + O_UNKA0 + 0, 1000);
    wr32(ADDR_LIFT + O_UNK54, 0);
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), -1, "step: opcode 0x52 arg 0 sets the timer to -1");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 10, "step: motion ran on the arming tick");

    fck_case("C20 command 0x52 arg 1..10 switches index and direction");
    reset_all();
    wr8(ADDR_LIFT + O_UNK0C, 1);
    wr8(ADDR_LIFT + CMD_BASE + 0, 0x52);
    wr8(ADDR_LIFT + CMD_BASE + 1, 3);            /* arg 3 -> target stop 3-1 = 2 */
    wr32(ADDR_LIFT + O_UNKC0, 5);               /* > 0: the switch proceeds */
    wr8(ADDR_LIFT + O_UNKC9, 0);                /* current stop 0 */
    /* keep the following motion inert: tiny dt, thresholds above the height */
    wr8(ADDR_LIFT + O_UNKCB, 1);
    wr32(ADDR_LIFT + O_UNKB8, 1);
    wr32(ADDR_LIFT + O_UNKA0 + 8, 1000);        /* thresholds[2] */
    gStepElevator(mem(ADDR_LIFT), 3);
    FCK_EQ(rdu8(ADDR_LIFT + O_UNKCA), 2, "step: opcode 0x52 small arg sets the index to arg-1");
    FCK_EQ(rds16(ADDR_LIFT + O_UNKCB), 1, "step: opcode 0x52 target above current -> direction 1");
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), -1, "step: opcode 0x52 switch arms the timer");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 3, "step: motion tick applied dt (height 0 + 3*1)");

    fck_case("C21 command 0x52 arg >= 11 takes the slot-array path (interpreter called directly)");
    reset_all();
    wr8(ADDR_LIFT + CMD_BASE + 0, 0x52);
    wr8(ADDR_LIFT + CMD_BASE + 1, 13);          /* arg 13 -> slot 13-11 = 2 */
    wr32(ADDR_LIFT + O_UNKA0 + 2 * 4, 4321);    /* slot 2 value */
    gRunEntityCommands(mem(ADDR_LIFT));
    FCK_EQ(rdu8(ADDR_LIFT + O_UNKCA), 2, "step: opcode 0x52 arg>=11 sets index arg-11");
    FCK_EQ(rds32(ADDR_LIFT + O_UNK54), 4321, "step: opcode 0x52 arg>=11 loads height from slot[arg-11]");
    FCK_EQ(rds32(ADDR_LIFT + O_UNKC0), -1, "step: opcode 0x52 arg>=11 arms the timer");
    FCK_EQ(fck_calls_to(N_GTELOAD), 1, "stepC21: interpreter reloaded the matrix");
    FCK_EQ(fck_calls_to(N_GTETRANS), 1, "stepC21: interpreter transformed the frame vertex");
    FCK_EQ(gCapGteIn, ADDR_FRAMES + 8, "stepC21: transform used index 0 (frames + 8)");
    FCK_EQ(gCapGteOut, ADDR_LIFT + O_UNK5C, "stepC21: transform output at +0x5C");
    FCK_EQ(gCapRemoveCmdIdx, 0, "stepC21: handled command removed from slot 0");
    FCK_EQ(rdu8(ADDR_LIFT + CMD_BASE + 0), 0, "stepC21: queue empty after the removal");

    fck_case("C22 unknown opcode stays queued and is skipped");
    reset_all();
    wr8(ADDR_LIFT + O_UNK0C, 1);
    wr8(ADDR_LIFT + CMD_BASE + 0, 0x99);         /* unknown */
    wr8(ADDR_LIFT + CMD_BASE + 1, 0);
    wr8(ADDR_LIFT + CMD_BASE + 2, 0);            /* terminator after the unknown */
    gStepElevator(mem(ADDR_LIFT), 1);
    FCK_EQ(fck_calls_to(N_REMOVECMD), 0, "step: unknown opcode is not removed");
    FCK_EQ(rdu8(ADDR_LIFT + CMD_BASE + 0), 0x99, "step: unknown opcode stays queued");
}

/* ---------------------------------- D. func_80039504 setup of the carrier */

static void test_setup_carrier(void)
{
    struct FckCall c;

    fck_case("D1 setup runs the base handler, spawns the child through the real "
             "func_80039418 chain and registers it");
    reset_all();
    gChildRet = ADDR_CHILD;
    /* fixture: unk48 = 3 -> arg1 = 0x300; unk4A = 4 -> arg2 = 4;
     * unk4C word = 2 -> flags = ((2 & 6) == 2) << 1 = 2; unk04->unk20 = 0x87654321 */
    wr16(ADDR_SUB08 + 0x48, 3);
    wr16(ADDR_SUB08 + 0x4A, 4);
    wr32(ADDR_SUB08 + 0x4C, 2);
    wr32(ADDR_SUB04 + 0x20, 0x87654321);
    wr32(ADDR_OTHER + 0x04, host_of(ADDR_SUB04));
    wr32(ADDR_OTHER + 0x08, host_of(ADDR_SUB08));
    /* carrier fields func_80039418 copies (object view, tag/params/flags/coord) */
    wr16(ADDR_CARRIER + 0x14, 0xBEEF);          /* tag */
    wr32(ADDR_CARRIER + 0x24, 0xFFFFFFFF);     /* flags: bit 0x2000 must be cleared */
    wr8(ADDR_CARRIER + 0x17, 0x17);
    wr8(ADDR_CARRIER + 0x18, 0x18);
    wr8(ADDR_CARRIER + 0x19, 0x7E);             /* unk1F = 0x7E & 0xF = 14 */
    wr16(ADDR_CARRIER + 0x1A, 0x1A1A);
    wr16(ADDR_CARRIER + 0x1C, 0x4242);
    wr8(ADDR_CARRIER + 0x92, 0x92);
    wr8(ADDR_CARRIER + 0x93, 0x93);

    gSetupMountedTurretCarrier(2, mem(ADDR_CARRIER), mem(ADDR_OTHER), 0x99);

    /* class-5 base ran first with the same args */
    FCK_EQ(fck_calls_to(N_TYPE5), 1, "setup: class-5 base handler ran");
    c = fck_call_n(N_TYPE5, 0);
    FCK_EQ(c.a0, 2, "setup: class-5 base got msg 2");
    FCK_EQ(gCapType5Obj, ADDR_CARRIER, "setup: class-5 base got the carrier");
    /* the child pointer is stored (host form in the window) and fields written */
    FCK_EQ(rd32(ADDR_CARRIER + C_UNK158), host_of(ADDR_CHILD), "setup: child pointer stored at +0x158");
    FCK_EQ(rd32(ADDR_CARRIER + C_UNKA0), 0, "setup: +0xA0 cleared");
    FCK_EQ(rd32(ADDR_CHILD + C_UNK24) & 0x10u, 0x10, "setup: child flag 0x10 ORed in");
    FCK_EQ(gCapQueueChild, ADDR_CHILD, "setup: child registered for map occupancy");
    /* order: type5 -> transform vertex -> spawn request -> occupancy queue */
    FCK_EQ(fck_log_count, 4, "setup: exactly four callees touched");
    FCK_EQ(fck_log[0].name == N_TYPE5, 1, "setup: order 0 = class-5 base");
    FCK_EQ(fck_log[1].name == N_TRANSVTX, 1, "setup: order 1 = part vertex transform");
    FCK_EQ(fck_log[2].name == N_SPAWNQ, 1, "setup: order 2 = spawn request");
    FCK_EQ(fck_log[3].name == N_QUEUE, 1, "setup: order 3 = occupancy registration");

    /* the real func_80039418 assembled the request from the computed args */
    FCK_EQ(gCapSpawnHandlerIsContainer, 1, "setup: request handler is func_80039298");
    FCK_EQ(gCapSpawnKind, 0x15, "setup: spawned child kind is 0x15");
    FCK_EQ(gCapSpawnTag, 0xBEEF, "setup: child tag copied from the carrier");
    FCK_EQ(gCapSpawnFlags, 0x7FFFDFFF, "setup: child flags mask bit 0x2000 cleared");
    FCK_EQ(gCapSpawnArg1, 3 << 8, "setup: arg1 = unk48 << 8");
    FCK_EQ(gCapSpawnArg2, 4, "setup: arg2 = unk4A forwarded");
    FCK_EQ(gCapSpawnArg3, 2, "setup: part index = computed flags ((word & 6) == 2) << 1");
    FCK_EQ(gCapSpawnArg4, 2, "setup: arg4 = unk4C byte");
    FCK_EQ(gCapSpawnArg5, 0, "setup: arg5 = unk4D byte");
    FCK_EQ(gCapSpawnArg6, (s32)0x87654321, "setup: arg6 = other->unk04->unk20");
    FCK_EQ(gCapOwner, ADDR_CARRIER, "setup: request owner is the carrier");
    FCK_EQ(gCapTransCoord, ADDR_CARRIER + 0x28, "setup: part vertex read from coord +0x28");
    FCK_EQ(gCapTransPart, 2, "setup: part vertex index = the computed flags slot");
    /* Check forwarding into the spawn request, not the transform math. */
    FCK_EQ(gCapSpawnPos0, 0x11110000, "setup: forwards transform output x to spawn");
    FCK_EQ(gCapSpawnPos1, 0x22220000, "setup: forwards transform output y to spawn");
    FCK_EQ(gCapSpawnPos2, 0x33330000, "setup: forwards transform output z to spawn");
}

/* E. func_800395A8 carrier message dispatch ------------------------------- */

static void test_carrier_dispatch(void)
{
    s32 r;
    struct FckCall c;

    fck_case("E1 msg 1 returns the block size 0x15C");
    reset_all();
    r = gHandleMountedTurretCarrierMessages(1, mem(ADDR_CARRIER), 0, 0);
    FCK_EQ(r, 0x15C, "carrier: msg 1 returns 0x15C");
    FCK_EQ(fck_log_count, 0, "carrier: msg 1 calls nothing");

    fck_case("E2 msg 2 runs the real setup unit");
    reset_all();
    gChildRet = ADDR_CHILD;
    wr16(ADDR_SUB08 + 0x48, 0);
    wr16(ADDR_SUB08 + 0x4A, 0);
    wr32(ADDR_SUB08 + 0x4C, 0);
    wr32(ADDR_SUB04 + 0x20, 0);
    wr32(ADDR_OTHER + 0x04, host_of(ADDR_SUB04));
    wr32(ADDR_OTHER + 0x08, host_of(ADDR_SUB08));
    r = gHandleMountedTurretCarrierMessages(2, mem(ADDR_CARRIER), mem(ADDR_OTHER), 0x99);
    FCK_EQ(r, 0, "carrier: msg 2 returns 0");
    FCK_EQ(fck_calls_to(N_TYPE5), 1, "carrier: msg 2 reached the setup (class-5 base ran)");
    FCK_EQ(rd32(ADDR_CARRIER + C_UNK158), host_of(ADDR_CHILD), "carrier: msg 2 spawned the child");

    fck_case("E3 msg 3 with flag 0x20: returns 0 without any call");
    reset_all();
    wr32(ADDR_CARRIER + C_UNK24, 0x20);
    r = gHandleMountedTurretCarrierMessages(3, mem(ADDR_CARRIER), 0, 0);
    FCK_EQ(r, 0, "carrier: msg 3 with flag 0x20 returns 0");
    FCK_EQ(fck_log_count, 0, "carrier: msg 3 with flag 0x20 calls nothing");

    fck_case("E4 msg 3 without the flag: base handler on self and child");
    reset_all();
    wr32(ADDR_CARRIER + C_UNK24, 0);
    wr32(ADDR_CARRIER + C_UNK158, host_of(ADDR_CHILD));
    gBaseRet = 0x1234;   /* must be ignored: msg 3 hardcodes 0 */
    r = gHandleMountedTurretCarrierMessages(3, mem(ADDR_CARRIER), (void *)0x55, 0x66);
    FCK_EQ(r, 0, "carrier: msg 3 returns 0");
    FCK_EQ(fck_calls_to(N_BASE), 2, "carrier: msg 3 draws self and child");
    FCK_EQ(gCapBaseObj, ADDR_CHILD, "carrier: msg 3 second base call is the child");

    fck_case("E5 msg 3 with a null child: only self");
    reset_all();
    wr32(ADDR_CARRIER + C_UNK24, 0);
    wr32(ADDR_CARRIER + C_UNK158, 0);
    gHandleMountedTurretCarrierMessages(3, mem(ADDR_CARRIER), 0, 0);
    FCK_EQ(fck_calls_to(N_BASE), 1, "carrier: msg 3 without a child draws only self");

    fck_case("E6 msg 5 sets the child health to -1 and falls through to class 5");
    reset_all();
    wr32(ADDR_CARRIER + C_UNK158, host_of(ADDR_CHILD));
    wr16(ADDR_CHILD + C_UNK1C, 5);
    gHandleMountedTurretCarrierMessages(5, mem(ADDR_CARRIER), (void *)0x77, 0x88);
    FCK_EQ(rds16(ADDR_CHILD + C_UNK1C), -1, "carrier: msg 5 clears the child health");
    FCK_EQ(fck_calls_to(N_TYPE5), 1, "carrier: msg 5 falls through to the class-5 handler");
    /* NOTE: the shared tail has no return statement in the retail source, so
     * the case 5..default return value is UNDEFINED retail behaviour and is
     * deliberately not asserted here. */

    fck_case("E7 msg 6: class 5 on self, health copied, child ticked with the payload");
    reset_all();
    wr32(ADDR_CARRIER + C_UNK158, host_of(ADDR_CHILD));
    wr16(ADDR_CARRIER + C_UNK1C, 42);
    wr16(ADDR_CHILD + C_UNK1C, 0);
    r = gHandleMountedTurretCarrierMessages(6, mem(ADDR_CARRIER), (void *)0x456, 0x78);
    FCK_EQ(r, 0, "carrier: msg 6 returns 0");
    FCK_EQ(rds16(ADDR_CHILD + C_UNK1C), 42, "carrier: msg 6 copies the health to the child");
    FCK_EQ(gCapTurretChild, ADDR_CHILD, "carrier: msg 6 ticks the child");
    FCK_EQ((s32)gCapTurretData, 0x456, "carrier: msg 6 forwards the payload untouched");

    fck_case("E8 msg 6 with a null child: no health copy, no tick");
    reset_all();
    wr32(ADDR_CARRIER + C_UNK158, 0);
    r = gHandleMountedTurretCarrierMessages(6, mem(ADDR_CARRIER), (void *)0x456, 0x78);
    FCK_EQ(r, 0, "carrier: msg 6 without a child returns 0");
    FCK_EQ(fck_calls_to(N_TURRET), 0, "carrier: msg 6 without a child does not tick");

    fck_case("E9 msg 11 with a live child: flag bit copied, health synced, quartile change");
    reset_all();
    wr32(ADDR_CARRIER + C_UNK158, host_of(ADDR_CHILD));
    wr32(ADDR_CARRIER + C_UNK24, 0x02000000);
    wr32(ADDR_CHILD + C_UNK24, 0);
    wr8(ADDR_CHILD + C_UNK20, 1);     /* child alive */
    wr16(ADDR_CARRIER + C_UNK1C, 9);
    wr16(ADDR_CHILD + C_UNK1C, 0);
    r = gHandleMountedTurretCarrierMessages(11, mem(ADDR_CARRIER), 0, 0);
    FCK_EQ(r, 0, "carrier: msg 11 returns 0");
    FCK_EQ(rd32(ADDR_CHILD + C_UNK24) & 0x02000000u, 0x02000000, "carrier: msg 11 propagates flag 0x02000000");
    FCK_EQ(rds16(ADDR_CHILD + C_UNK1C), 9, "carrier: msg 11 copies the health");
    FCK_EQ(fck_calls_to(N_QUART), 1, "carrier: live child reports the quartile change");
    FCK_EQ(gCapQuartChild, ADDR_CHILD, "carrier: quartile change reported for the child");
    FCK_EQ(gCapQuartPrev, 0, "carrier: quartile previous value is the old child health");

    fck_case("E10 msg 11 with the 0x40000 bit set on the child: plain health copy");
    reset_all();
    wr32(ADDR_CARRIER + C_UNK158, host_of(ADDR_CHILD));
    wr32(ADDR_CARRIER + C_UNK24, 0);
    wr32(ADDR_CHILD + C_UNK24, 0x40000);
    wr8(ADDR_CHILD + C_UNK20, 1);
    wr16(ADDR_CARRIER + C_UNK1C, 9);
    wr16(ADDR_CHILD + C_UNK1C, 0);
    gHandleMountedTurretCarrierMessages(11, mem(ADDR_CARRIER), 0, 0);
    FCK_EQ(rd32(ADDR_CHILD + C_UNK24) & 0x02000000u, 0, "carrier: unset flag bit stays unset on the child");
    FCK_EQ(fck_calls_to(N_QUART), 0, "carrier: 0x40000 suppresses the quartile call");
    FCK_EQ(rds16(ADDR_CHILD + C_UNK1C), 9, "carrier: health still copied without the quartile call");

    fck_case("E11 msg 11 with a dead child (unk20 = 0): plain health copy");
    reset_all();
    wr32(ADDR_CARRIER + C_UNK158, host_of(ADDR_CHILD));
    wr8(ADDR_CHILD + C_UNK20, 0);
    wr16(ADDR_CARRIER + C_UNK1C, 3);
    wr16(ADDR_CHILD + C_UNK1C, 0);
    gHandleMountedTurretCarrierMessages(11, mem(ADDR_CARRIER), 0, 0);
    FCK_EQ(fck_calls_to(N_QUART), 0, "carrier: dead child skips the quartile flag");
    FCK_EQ(rds16(ADDR_CHILD + C_UNK1C), 3, "carrier: dead child still gets the health copy");

    fck_case("E12 msg 11 with a clear flag bit: propagates the clear to the child");
    reset_all();
    wr32(ADDR_CARRIER + C_UNK158, host_of(ADDR_CHILD));
    wr32(ADDR_CARRIER + C_UNK24, 0);
    wr32(ADDR_CHILD + C_UNK24, 0x02000000);
    wr8(ADDR_CHILD + C_UNK20, 0);
    gHandleMountedTurretCarrierMessages(11, mem(ADDR_CARRIER), 0, 0);
    FCK_EQ(rd32(ADDR_CHILD + C_UNK24) & 0x02000000u, 0, "carrier: clear flag bit propagated to the child");

    fck_case("E13 default message 4: class-5 handler only");
    reset_all();
    gHandleMountedTurretCarrierMessages(4, mem(ADDR_CARRIER), (void *)0x33, 0x44);
    FCK_EQ(fck_calls_to(N_TYPE5), 1, "carrier: msg 4 runs the class-5 handler");
    c = fck_call_n(N_TYPE5, 0);
    FCK_EQ(c.a0, 4, "carrier: msg 4 forwarded the message id");
    FCK_EQ(c.a1, 0x44, "carrier: msg 4 forwarded arg3");
    FCK_EQ(fck_log_count, 1, "carrier: msg 4 calls nothing else");
}

/* --------------------------------- F. func_80039298 part-child dispatch */

static void test_part_dispatch(void)
{
    s32 r;

    fck_case("F1 msg 1 returns the block size 0xF0");
    reset_all();
    r = gHandleMountedPartMsg(1, mem(ADDR_CONT), 0, 0);
    FCK_EQ(r, 0xF0, "part: msg 1 returns 0xF0");

    fck_case("F2 msg 2 runs the constructor and falls through to 0");
    reset_all();
    r = gHandleMountedPartMsg(2, mem(ADDR_CONT), (void *)0x77, 0x88);
    FCK_EQ(r, 0, "part: msg 2 falls through to 0");
    FCK_EQ(fck_calls_to(N_ACTORINIT), 1, "part: msg 2 runs the constructor");
    FCK_EQ(gCapActorObj, ADDR_CONT, "part: constructor got the container");
    FCK_EQ((s32)gCapActorData, 0x77, "part: constructor got the data payload");
    FCK_EQ(gCapActorA3, 0x88, "part: constructor got arg3");

    fck_case("F3 msg 3 and 6 return 0 without any call");
    reset_all();
    r = gHandleMountedPartMsg(3, mem(ADDR_CONT), 0, 0);
    FCK_EQ(r, 0, "part: msg 3 returns 0");
    r = gHandleMountedPartMsg(6, mem(ADDR_CONT), 0, 0);
    FCK_EQ(r, 0, "part: msg 6 returns 0");
    FCK_EQ(fck_log_count, 0, "part: msgs 3/6 call nothing");

    fck_case("F4 msg 10 identity query: 0 for the current child, 1 otherwise");
    reset_all();
    wr32(ADDR_CONT + P_CHILD, host_of(ADDR_CCHILD));
    r = gHandleMountedPartMsg(10, mem(ADDR_CONT), (void *)host_of(ADDR_CCHILD), 0);
    FCK_EQ(r, 0, "part: msg 10 returns 0 for the current child");
    r = gHandleMountedPartMsg(10, mem(ADDR_CONT), (void *)host_of(ADDR_CHILD), 0);
    FCK_EQ(r, 1, "part: msg 10 returns 1 for another pointer");

    fck_case("F5 msg 9 without flag 1 returns 0; with flag 1 goes to the base handler");
    reset_all();
    r = gHandleMountedPartMsg(9, mem(ADDR_CONT), 0, 0);
    FCK_EQ(r, 0, "part: msg 9 returns 0 without the flag");
    FCK_EQ(fck_log_count, 0, "part: msg 9 without the flag calls nothing");
    reset_all();
    gBaseRet = 0x4321;
    wr16(ADDR_CONT + P_FLAGS, 1);
    r = gHandleMountedPartMsg(9, mem(ADDR_CONT), (void *)0x11, 0x22);
    FCK_EQ(r, 0x4321, "part: msg 9 with flag 1 returns the base result");

    fck_case("F6 msg 5, child type 0x1C, walk finds the container in slot 1");
    reset_all();
    gBaseRet = 0x99AB;
    wr32(ADDR_CONT + P_CHILD, host_of(ADDR_CCHILD));
    wr8(ADDR_CCHILD + K_TYPE, 0x1C);
    wr32(ADDR_CCHILD + K_UNK08, 1);            /* forces the walk */
    wr8(ADDR_CCHILD + K_COUNT, 2);
    wr32(ADDR_CCHILD + K_OBJECTS + 0, 0x11111111);   /* slot 0: not a match */
    wr32(ADDR_CCHILD + K_OBJECTS + 4, host_of(ADDR_CONT));  /* slot 1: match */
    wr16(ADDR_CCHILD + K_UNK1C, 7);
    r = gHandleMountedPartMsg(5, mem(ADDR_CONT), 0, 0);
    FCK_EQ(rd32(ADDR_CCHILD + K_OBJECTS + 0), 0x11111111, "part: walk leaves non-matching slots");
    FCK_EQ(rd32(ADDR_CCHILD + K_OBJECTS + 4), 0, "part: walk clears the matching slot");
    FCK_EQ(rds16(ADDR_CCHILD + K_UNK1C), -1, "part: msg 5 sets the child countdown to -1");
    FCK_EQ(fck_calls_to(N_BASE), 1, "part: msg 5 finishes in the base handler");
    FCK_EQ(r, 0x99AB, "part: msg 5 returns the base handler result");

    fck_case("F7 msg 5, child type 0x1C, no matching slot: nothing cleared");
    reset_all();
    wr32(ADDR_CONT + P_CHILD, host_of(ADDR_CCHILD));
    wr8(ADDR_CCHILD + K_TYPE, 0x1C);
    wr32(ADDR_CCHILD + K_UNK08, 1);
    wr8(ADDR_CCHILD + K_COUNT, 2);
    wr32(ADDR_CCHILD + K_OBJECTS + 0, 0x22222222);
    wr32(ADDR_CCHILD + K_OBJECTS + 4, 0x33333333);
    gHandleMountedPartMsg(5, mem(ADDR_CONT), 0, 0);
    FCK_EQ(rd32(ADDR_CCHILD + K_OBJECTS + 0), 0x22222222, "part: no match leaves slot 0");
    FCK_EQ(rd32(ADDR_CCHILD + K_OBJECTS + 4), 0x33333333, "part: no match leaves slot 1");
    FCK_EQ(rds16(ADDR_CCHILD + K_UNK1C), -1, "part: countdown still set to -1 on no match");

    fck_case("F8 msg 5, other child type: slot 0 cleared directly");
    reset_all();
    wr32(ADDR_CONT + P_CHILD, host_of(ADDR_CCHILD));
    wr8(ADDR_CCHILD + K_TYPE, 0x14);           /* not 0x1C -> direct clear */
    wr32(ADDR_CCHILD + K_OBJECTS + 0, host_of(ADDR_CONT));
    wr32(ADDR_CCHILD + K_OBJECTS + 4, 0xDEADBEEF);
    gHandleMountedPartMsg(5, mem(ADDR_CONT), 0, 0);
    FCK_EQ(rd32(ADDR_CCHILD + K_OBJECTS + 0), 0, "part: non-0x1C type clears slot 0");
    FCK_EQ(rd32(ADDR_CCHILD + K_OBJECTS + 4), 0xDEADBEEF, "part: non-0x1C type touches nothing else");

    fck_case("F9 msg 5 with flag 1: countdown kept, base handler still runs");
    reset_all();
    wr16(ADDR_CONT + P_FLAGS, 1);
    wr32(ADDR_CONT + P_CHILD, host_of(ADDR_CCHILD));
    wr8(ADDR_CCHILD + K_TYPE, 0x14);
    wr16(ADDR_CCHILD + K_UNK1C, 5);
    gHandleMountedPartMsg(5, mem(ADDR_CONT), 0, 0);
    FCK_EQ(rds16(ADDR_CCHILD + K_UNK1C), 5, "part: flag 1 keeps the child countdown");
    FCK_EQ(fck_calls_to(N_BASE), 1, "part: msg 5 with flag 1 still reaches the base handler");

    fck_case("F10 msg 11 with child type 0x14 forwards to the REAL carrier handler");
    reset_all();
    wr32(ADDR_CONT + P_CHILD, host_of(ADDR_CCHILD));
    wr8(ADDR_CCHILD + K_TYPE, 0x14);
    /* the carrier (child as self) has no grandchild: msg 11 runs class 5 only */
    wr32(ADDR_CCHILD + C_UNK158, 0);
    r = gHandleMountedPartMsg(11, mem(ADDR_CONT), (void *)0x25, 0x35);
    FCK_EQ(r, 0, "part: msg 11 returns 0 after the forward");
    FCK_EQ(fck_calls_to(N_TYPE5), 1, "part: forwarded carrier ran its class-5 handler");
    FCK_EQ(gCapType5Obj, ADDR_CCHILD, "part: forwarded carrier ran on the child");
    FCK_EQ((s32)gCapType5Data, 0x25, "part: forwarded carrier got the data payload");
    FCK_EQ(gCapType5Msg, 11, "part: forwarded carrier saw message 11");
    FCK_EQ(gCapType5A3, 0x35, "part: forwarded carrier saw arg3");

    fck_case("F11 msg 14 with child type 0x1C forwards to the type-28 handler");
    reset_all();
    wr32(ADDR_CONT + P_CHILD, host_of(ADDR_CCHILD));
    wr8(ADDR_CCHILD + K_TYPE, 0x1C);
    gHandleMountedPartMsg(14, mem(ADDR_CONT), (void *)0x77, 0x88);
    FCK_EQ(fck_calls_to(N_TYPE28), 1, "part: msg 14 forwards to the type-28 handler");
    FCK_EQ(gCapType28Msg, 14, "part: type-28 got message 14");
    FCK_EQ(gCapType28Obj, ADDR_CCHILD, "part: type-28 ran on the child");

    fck_case("F12 msg 11 with flag 1: skips the child handler, goes to base");
    reset_all();
    gBaseRet = 0xAABB;
    wr16(ADDR_CONT + P_FLAGS, 1);
    wr32(ADDR_CONT + P_CHILD, host_of(ADDR_CCHILD));
    wr8(ADDR_CCHILD + K_TYPE, 0x1C);
    r = gHandleMountedPartMsg(11, mem(ADDR_CONT), 0, 0);
    FCK_EQ(fck_calls_to(N_TYPE28), 0, "part: flag 1 skips the child handler");
    FCK_EQ(fck_calls_to(N_BASE), 1, "part: flag 1 goes to the base handler");
    FCK_EQ(r, 0xAABB, "part: flag 1 returns the base result");

    fck_case("F13 msg 11 with an unknown child type returns 0 untouched");
    reset_all();
    wr32(ADDR_CONT + P_CHILD, host_of(ADDR_CCHILD));
    wr8(ADDR_CCHILD + K_TYPE, 0x77);
    r = gHandleMountedPartMsg(11, mem(ADDR_CONT), 0, 0);
    FCK_EQ(r, 0, "part: unknown child type returns 0");
    FCK_EQ(fck_log_count, 0, "part: unknown child type calls nothing");

    fck_case("F14 default message goes to the base handler");
    reset_all();
    gBaseRet = 0x1234;
    r = gHandleMountedPartMsg(7, mem(ADDR_CONT), (void *)0x9, 0x8);
    FCK_EQ(r, 0x1234, "part: default message returns the base result");
    FCK_EQ(fck_calls_to(N_BASE), 1, "part: default message runs the base handler");
}

/* ------------------------------------------------------------------ main */

int main(void)
{
    test_dispatch();
    test_init_elevator();
    test_step_gates();
    test_setup_carrier();
    test_carrier_dispatch();
    test_part_dispatch();

    if (fck_report("movers"))
        return 1;
    fck_write("movers: ALL PASS (");
    fck_write_int(g_cases);
    fck_write(" cases)\n");
    return 0;
}
