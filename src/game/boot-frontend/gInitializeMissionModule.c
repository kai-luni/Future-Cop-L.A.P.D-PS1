// Categories: Game Engine > Init | Gameplay > Mission Setup
#include "game/types.h"

struct Obj {
    char pad0[0x0C];
    s32 unk0C;
    char pad1[0x18];
    s32 unk28;
};

struct Name {
    char b[10];
};

extern s32 gGlobalTick __asm__("D_8009F284");
extern s32 gRequestListHead __asm__("D_8009F668");
extern s32 gRequestListTail __asm__("D_8009F66C");
extern u8 gWasSuspended __asm__("D_8009F680");
extern s32 gMissionId __asm__("D_8009F684");
extern s32 gMissionParam __asm__("D_8009F688");
extern s32 gScratchBlockPtr __asm__("D_8009F68C");
extern s32 gGameSuspended __asm__("D_8009FA24");
extern s32 gOpenPanelCount __asm__("D_8009FA6C");
extern s32 gScreenResetFlag __asm__("D_8009FA70");
extern s32 gDemoPlayback __asm__("D_8009FA84");
extern struct Obj *gGameSession __asm__("D_8009FA94");
extern s32 gPanelSoundIds __asm__("D_8009FAC0");

extern s32 gSlotVoiceHandle[] __asm__("D_8009F678");
extern s32 gSlotTrackId[] __asm__("D_8009FA78");
extern struct Name gMissionPanelSoundTable __asm__("D_80010DC0");
extern void *gFocusPositions __asm__("D_8009F9A0");
extern u8 gModuleNameBuffer[] __asm__("D_800A0B30");
extern u8 gRandomTable[] __asm__("D_800A8540");
extern s32 gPlayerEntity2[] __asm__("D_8009FA5C");

extern void func_80017C68(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);
extern void gAllocDecodeBuffers(void) __asm__("func_800860CC");
extern void gBiosStrcpy(void *a0, s32 a1) __asm__("func_8008CCB0");
extern void func_80019D60(void);
extern void gClearDescriptorRegistry(void) __asm__("func_8006A880");
extern void func_80019A64(void);
extern void gSeedPrng(s32 seed) __asm__("func_800197A8");
extern void gInitDemoCameraStates(void) __asm__("func_80056924");
extern void gInitReciprocalTable(void) __asm__("func_80019828");
extern void gResetPlayerInvulnFlags(void) __asm__("func_8005A044");
extern void gInitDefinitionEntryCount(void) __asm__("func_800609F4");
extern void gInitScriptVmState(void) __asm__("func_80069B58");
extern void func_8001774C(void);
extern void func_8001B2C8(s32 a0, s32 a1);
extern void gInitMarkerColorsAndPools(void) __asm__("func_8004CAE4");
extern void gClearInitStateWords(void) __asm__("func_8002A598");
extern void gInitScriptTimers(void) __asm__("func_80049400");
extern void gAllocSpawnRegistry(void) __asm__("func_80060E28");
extern void gResetPlacementSlotCount(void) __asm__("func_8005130C");
extern void func_80013A14(void);
extern s32 gInitMapTables(void) __asm__("func_80063428");
extern void func_80012E1C(s32 arg0);
extern void gInitCellEntityWorkBuffers(void) __asm__("func_800617D8");
extern void gResetHudTextState(void) __asm__("func_8004783C");
extern void gLoadDemoRecording(s32 a0, s32 a1, s32 a2) __asm__("func_80059BFC");
extern void gInitModuleStream(void *a0) __asm__("func_80029F50");
extern void gInitMissionBootStub(void) __asm__("func_8005A738");
extern void gInitMapLighting(void) __asm__("func_80049690");
extern void gInitTextureDescriptorTable(void) __asm__("func_8005A2B0");
extern void gInitSceneNodeAndVoicePools(void) __asm__("func_8008347C");
extern void gInitMapZoneList(void) __asm__("func_80031984");
extern void gInitDebrisFragmentPool(void) __asm__("func_8004AB4C");
extern void gInitMissionSpawnState(s32 a0) __asm__("func_8003F79C");
extern void gClearBeamNodeList(void) __asm__("func_800734B0");

void gInitializeMissionModule(s32 arg0) __asm__("func_80024A24");

/**
 * @brief Resets mission state, selects a panel value and initializes the mission subsystems.
 *
 * @param arg0 Module name address, stored in the mission word and forwarded to startup helpers.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Clears two preview-handle and track slots and copies a ten-byte table to the stack. Scans its
 * five key/value pairs against session +0x28, initializes viewport and decode state, copies the
 * module name, clears 128 random-table bytes and both player pointers, then runs the fixed
 * subsystem chain. A nonzero demo flag supplies its absolute value as a mission parameter and
 * triggers a separate setup call.
 *
 * @par Globals
 * Updates gScratchBlockPtr, gWasSuspended, gSlotVoiceHandle, gSlotTrackId, gPanelSoundIds,
 * gFocusPositions, gMissionId, gGameSuspended, gOpenPanelCount, gModuleNameBuffer,
 * gRandomTable, gRequestListHead, gRequestListTail, gPlayerEntities, gMissionParam and
 * gScreenResetFlag. Reads gMissionPanelSoundTable, gGameSession, gDemoPlayback and gGlobalTick.
 *
 * @par Callers
 * Direct calls from gStartModuleByName (func_80024E3C).
 *
 * @par Role and path
 * The module startup path calls this before gameplay frame processing begins.
 *
 * @par Unknown
 * The meaning of each viewport constant and the flag passed to func_80059BFC is unresolved.
 */
void gInitializeMissionModule(s32 arg0)
{
    struct Name name;
    s32 i;
    s32 v;
    s32 seed;
    s32 target;

    gScratchBlockPtr = 0;
    gWasSuspended = 0;
    {
        register s32 *p __asm__("$4");
        register s32 *q __asm__("$5");

        i = 0;
        q = gSlotTrackId;
        p = gSlotVoiceHandle;
        do {
            *p = 0;
            *q = 0;
            q++;
            i++;
            p++;
        } while (i < 2);
    }

    name = gMissionPanelSoundTable;

    gPanelSoundIds = 0;
    target = gGameSession->unk28;
    for (i = 0; i < 10; i += 2) {
        if (name.b[i] == target) {
            gPanelSoundIds = name.b[i + 1];
            goto call_init;
        }
    }

call_init:
    gFocusPositions = 0;
    gMissionId = arg0;
    func_80017C68(0x168, 0x82, 0x118, 0x64, 0xFC, 0x145);
    gGameSuspended = 0;
    gOpenPanelCount = 0;
    gAllocDecodeBuffers();
    gBiosStrcpy(gModuleNameBuffer, arg0);

    {
        register s32 n __asm__("$3");
        register u32 bp __asm__("$2");

        n = 0x7F;
        bp = (u32)gRandomTable;
        bp += 0x7F;
        do {
            *(u8 *)bp = 0;
            n--;
            bp -= 1;
        } while (n >= 0);
    }

    gRequestListHead = 0;
    gRequestListTail = 0;

    {
        s32 n = 1;
        s32 *bp = gPlayerEntity2;

        do {
            *bp = 0;
            bp--;
            n--;
        } while (n >= 0);
    }

    func_80019D60();
    gClearDescriptorRegistry();
    func_80019A64();

    seed = gGlobalTick;
    v = gDemoPlayback;
    if (v != 0) {
        if (v > 0) {
            gSeedPrng(seed);
            gInitDemoCameraStates();
            v = gDemoPlayback;
        }
        seed = ((v >> 31) + v) ^ (v >> 31);
        gMissionParam = seed;
    }

    gInitReciprocalTable();
    gSeedPrng(seed);
    gResetPlayerInvulnFlags();
    gInitDefinitionEntryCount();
    gInitScriptVmState();
    func_8001774C();
    func_8001B2C8(0x898, 0x400);
    gInitMarkerColorsAndPools();
    gClearInitStateWords();
    gInitScriptTimers();
    gAllocSpawnRegistry();
    gResetPlacementSlotCount();
    func_80013A14();
    func_80012E1C(gInitMapTables());
    gInitCellEntityWorkBuffers();
    gResetHudTextState();

    if (gDemoPlayback != 0) {
        s32 flag = (gGameSession->unk0C == 2) ? 2 : 1;

        gLoadDemoRecording(flag, gMissionParam, arg0);
    }

    gInitModuleStream(gModuleNameBuffer);
    gScreenResetFlag = 0;
    gInitMissionBootStub();
    gInitMapLighting();
    gInitTextureDescriptorTable();
    gInitSceneNodeAndVoicePools();
    gInitMapZoneList();
    gInitDebrisFragmentPool();
    gInitMissionSpawnState(arg0);
    gClearBeamNodeList();
}
