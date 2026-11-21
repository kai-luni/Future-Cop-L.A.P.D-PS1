// Categories: Gameplay > State Transition
#include "game/types.h"

extern s8 *gFontBankTable __asm__("D_8009F468");
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern volatile s32 D_8009F8E0;
extern volatile s32 D_8009F8E4;
extern s32 D_8009F8E0_arr[];
extern s32 *gGameSession __asm__("D_8009FA94");
extern u8 gFrontEndReturnMode __asm__("D_8009F45A");
extern s8 D_8009F924[2];
extern s8 D_8009F926[2];
extern s8 D_8009F928[2];
extern u8 gRowClearFlag __asm__("D_8009FA10");
extern s32 gScreenState __asm__("D_8009FA00");
extern u8 gRowRecordTable[] __asm__("D_8009C030");
extern u8 gSecondLoadoutRowRecords[] __asm__("D_8009C050");
extern u8 *gLoadoutTable[] __asm__("D_8009C0A8");
extern s16 D_8009B856[4];
extern s16 D_8009B7E6[4];

extern void gClampLoadoutSelections(s32 mode) __asm__("func_8007D2DC");

/**
 *  @brief Updates selection readiness or initiates a shared state transition.
 *
 *  Mode 3 marks readiness and, when permitted, copies two sets of table values
 *  into shared state; mode 0 updates readiness-related values for a selection.
 *  @param a0 Operation selector; only 0 and 3 are handled.
 *  @param a1 Unused.
 *  @param a2 Unused.
 *  @param a3 Unused.
 *  @param a4 Selection value compared in mode 0.
 *  @par Globals
 *  D_8009F468, D_8009F8B8, D_8009F8E0_arr, D_8009F8E0, and D_8009F8E4 control readiness; D_8009FA94 holds shared state.
 *  D_8009F924, D_8009F926, D_8009F928 index D_8009C030, D_8009C050, D_8009C0A8; D_8009F45A, D_8009FA10, D_8009FA00, D_8009B856, D_8009B7E6 are updated.
 *  @par Control flow
 *  Branches on a0 and the shared mode; mode 3 may wait for both readiness flags.
 *  @par Calls
 *  func_8007D2DC is called with 1 when the transition is permitted.
 *  @par Side effects
 *  Changes readiness flags, shared state, and selection-related values.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gFinalizeMissionLoadout(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) __asm__("func_8007E3EC");

void gFinalizeMissionLoadout(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4)
{
    u8 flag;
    s32 want;
    register s32 idx __asm__("$2");
    register s32 off __asm__("$3");
    want = a4;
    flag = 0;
    if (a0 != 0) {
        if (a0 != 3) {
            return;
        }
        if (gFontBankTable[5] == 2) {
            idx = gActivePlayerIndex;
            __asm__ volatile("" ::: "memory");
            idx[D_8009F8E0_arr] = 1;
            __asm__ volatile("" ::: "memory");
            if (D_8009F8E0 != 0 && D_8009F8E4 != 0) {
                flag = 1;
                if (gGameSession[23] == 1) {
                    gFrontEndReturnMode = 2;
                } else {
                    gFrontEndReturnMode = 1;
                }
            }
        } else {
            flag = 1;
            if (gGameSession[23] == 1) {
                gFrontEndReturnMode = 2;
            } else {
                gFrontEndReturnMode = 1;
            }
        }
        if (flag != 0) {
            gClampLoadoutSelections(1);
            gGameSession[10] = gRowRecordTable[D_8009F924[0] * 6];
            gGameSession[11] = gSecondLoadoutRowRecords[D_8009F926[0] * 6];
            gGameSession[12] = gLoadoutTable[10][D_8009F928[0] * 6];
            gGameSession[13] = gRowRecordTable[D_8009F924[1] * 6];
            gGameSession[14] = gSecondLoadoutRowRecords[D_8009F926[1] * 6];
            gGameSession[15] = gLoadoutTable[10][D_8009F928[1] * 6];
            gRowClearFlag = 0;
            gScreenState = 5;
        }
    } else {
        if (gFontBankTable[5] == 2) {
            s32 i = gActivePlayerIndex;
            if (gFontBankTable[i] != want) {
                off = i << 2;
                *(s32 *)((u8 *)D_8009F8E0_arr + off) = 0;
                if (i == 1) {
                    D_8009B856[0] = 100;
                } else {
                    D_8009B7E6[0] = 100;
                }
                i = gActivePlayerIndex;
            }
            if (D_8009F8E0_arr[i] != 0) {
                if (i == 1) {
                    D_8009B856[0] = 110;
                } else {
                    D_8009B7E6[0] = 110;
                }
            }
        }
    }
}
