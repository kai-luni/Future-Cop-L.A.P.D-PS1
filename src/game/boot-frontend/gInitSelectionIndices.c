// Categories: Game Engine > Configuration
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef struct { u8 *entries; int unused; u8 count, min, max; u8 pad[9]; } List;
typedef struct { u8 pad0[0x28]; u8 a; u8 pad29[3]; u8 b; u8 pad2D[3]; u8 c; u8 pad31[3]; u8 d; u8 pad35[3]; u8 e; u8 pad39[3]; u8 f; u8 pad3D[0x1F]; int mode; u8 pad60[0x2C]; int x,y,z; } State;
typedef struct { u8 pad[0x19]; u8 max0,max1,max2; u8 tail[8]; } Entry;
extern State *gGameSession __asm__("D_8009FA94");
extern List gLoadoutTable[3] __asm__("D_8009C0A8");
extern u8 gAltSpecialLoadoutEntries[] __asm__("D_8009C090");
extern u8 gDefaultSpecialWeaponRows[] __asm__("D_8009C070");
extern u8 gOnePlayerLoadoutMenuBackgroundIndex[] __asm__("D_800A362B");
extern s8 gLoadoutPrimaryWeaponIndices[2] __asm__("D_8009F924");
extern s8 gLoadoutHeavyWeaponIndices[2] __asm__("D_8009F926");
extern s8 gLoadoutSpecialWeaponIndices[2] __asm__("D_8009F928");
extern s8 gSelectedMissionIndex __asm__("D_8009F880");
extern s8 *gMissionListTablePtr __asm__("D_8009F884");
extern Entry gWideTextTable[] __asm__("D_8009B4E4");
extern u8 gLoadoutHeavyMinIndex[] __asm__("D_8009C0C5");
extern u8 gLoadoutSpecialMinIndex[] __asm__("D_8009C0D9");
extern int gPlayer1MenuReadyFlag __asm__("D_8009F8E0");
extern int gPlayer2MenuReadyFlag __asm__("D_8009F8E4");
typedef struct { u8 pad[0x16]; u16 a; u8 pad2[0x6E]; u16 b; } Tail;
extern Tail gTwoPlayerLoadoutMenuItems __asm__("D_8009B7D0");
extern u8 gFindListEntryByValue(List *, u8) __asm__("func_8007D39C");
#define CLAMP(x,lo,hi) ((x)<(lo)?(lo):(u8)((x)>(hi)?(hi):(x)))

/**
 *  @brief Initializes and clamps three pairs of selection indices from state.
 *
 *  Configures list entries and limits according to the current mode and selected
 *  table entry, then resets associated counters and two values to 100.
 *  @par Globals
 *  D_8009FA94 supplies mode, six values, and three minimum selectors.
 *  D_8009C0A8 holds lists; D_8009C090 and D_8009C070 supply entries;
 *  D_800A362B receives a mode-dependent value. D_8009F924, D_8009F926,
 *  and D_8009F928 receive indices. D_8009F880 and D_8009F884 select
 *  limits from D_8009B4E4; D_8009C0C5 and D_8009C0D9 receive minima.
 *  D_8009F8E0 and D_8009F8E4 are cleared; D_8009B7D0 receives two values.
 *  @par Control flow
 *  Branches on mode and each minimum selector, then clamps both indices per list.
 *  @par Calls
 *  func_8007D39C maps each of the six state values through its list.
 *  @par Side effects
 *  Modifies list configuration, selection indices, counters, and stored values.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gInitSelectionIndices(void) __asm__("func_8007D408");

void gInitSelectionIndices(void)
{
    if (gGameSession->mode == 1) {
        gLoadoutTable[2].entries = gAltSpecialLoadoutEntries;
        gLoadoutTable[2].count = 4;
        gOnePlayerLoadoutMenuBackgroundIndex[0] = 7;
        gLoadoutTable[1].count = 4;
    } else {
        gLoadoutTable[2].entries = gDefaultSpecialWeaponRows;
        gLoadoutTable[2].count = 5;
        gOnePlayerLoadoutMenuBackgroundIndex[0] = 2;
        gLoadoutTable[1].count = 5;
    }
    gLoadoutPrimaryWeaponIndices[0] = gFindListEntryByValue(&gLoadoutTable[0], gGameSession->a);
    gLoadoutPrimaryWeaponIndices[1] = gFindListEntryByValue(&gLoadoutTable[0], gGameSession->d);
    gLoadoutHeavyWeaponIndices[0] = gFindListEntryByValue(&gLoadoutTable[1], gGameSession->b);
    gLoadoutHeavyWeaponIndices[1] = gFindListEntryByValue(&gLoadoutTable[1], gGameSession->e);
    gLoadoutSpecialWeaponIndices[0] = gFindListEntryByValue(&gLoadoutTable[2], gGameSession->c);
    gLoadoutSpecialWeaponIndices[1] = gFindListEntryByValue(&gLoadoutTable[2], gGameSession->f);
    if (gGameSession->x) gLoadoutTable[0].min = 0;
    else gLoadoutTable[0].min = 1;
    gLoadoutTable[0].max = gWideTextTable[gMissionListTablePtr[gSelectedMissionIndex]].max0;
    gLoadoutPrimaryWeaponIndices[0] = CLAMP(gLoadoutPrimaryWeaponIndices[0], gLoadoutTable[0].min, gLoadoutTable[0].max);
    gLoadoutPrimaryWeaponIndices[1] = CLAMP(gLoadoutPrimaryWeaponIndices[1], gLoadoutTable[0].min, gLoadoutTable[0].max);
    if (gGameSession->y) gLoadoutHeavyMinIndex[0] = 0;
    else gLoadoutHeavyMinIndex[0] = 1;
    gLoadoutTable[1].max = gWideTextTable[gMissionListTablePtr[gSelectedMissionIndex]].max1;
    gLoadoutHeavyWeaponIndices[0] = CLAMP(gLoadoutHeavyWeaponIndices[0], gLoadoutTable[1].min, gLoadoutTable[1].max);
    gLoadoutHeavyWeaponIndices[1] = CLAMP(gLoadoutHeavyWeaponIndices[1], gLoadoutTable[1].min, gLoadoutTable[1].max);
    if (gGameSession->z) gLoadoutSpecialMinIndex[0] = 0;
    else gLoadoutSpecialMinIndex[0] = 1;
    gLoadoutTable[2].max = gWideTextTable[gMissionListTablePtr[gSelectedMissionIndex]].max2;
    gLoadoutSpecialWeaponIndices[0] = CLAMP(gLoadoutSpecialWeaponIndices[0], gLoadoutTable[2].min, gLoadoutTable[2].max);
    gLoadoutSpecialWeaponIndices[1] = CLAMP(gLoadoutSpecialWeaponIndices[1], gLoadoutTable[2].min, gLoadoutTable[2].max);
    gPlayer1MenuReadyFlag = 0;
    gPlayer2MenuReadyFlag = 0;
    gTwoPlayerLoadoutMenuItems.a = 100;
    gTwoPlayerLoadoutMenuItems.b = 100;
}
