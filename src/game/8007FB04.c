// Categories: Resources > Save Data | Front-End > Screen State Machine | Platform > Memory Card
#include "game/types.h"

extern u8 *gSaveProfileImage __asm__("D_8009F92C");
extern u8 *gFontBankTable __asm__("D_8009F468");
extern s32 gStillRunningState __asm__("D_8009F464");
extern u8 gUiHoldFlag __asm__("D_8009F8BD");
extern u8 gUiInteractionEnabled __asm__("D_8009F8BE");
extern u8 gPlayerMenuDepth __asm__("D_8009F8C0");
extern u8 gProfileDetailMenuDescriptor[] __asm__("D_800A36E8");
extern u8 gProfileNameEntryMenuDescriptor[] __asm__("D_800A36B8");
extern u8 gMenuActivationBlock[] __asm__("D_800A3718");
extern u8 gProfileImageName[] __asm__("D_800120A4");

extern s32 func_800207B4();
extern void gPopMenuPanel(void) __asm__("func_80078740");
extern s32 gGetMcPendingFlag(void) __asm__("func_8001F37C");
extern void func_80080B70(void);
extern void gQueueMemoryCardFormat(void) __asm__("func_8008204C");
extern void func_80081204(void);
extern s32 func_8007F750(void);
extern void func_80080010(void);
extern void func_80080AD4(void);
extern s32 func_80020C90(u8 *arg0);
extern s32 func_80020E68(s32 arg0);
extern void *gFillMemory(void *destination, int value, unsigned int count) __asm__("func_8001E4C0");
extern void func_80078650(u8 *arg0, s32 arg1);
extern void func_80082020(void);

/**
 * @brief One step of the profile save/load transfer state machine.
 *
 * @return 0 when nothing changed, non-zero after a state transition (the
 *         callers only test for zero).
 *
 * @par Confirmed flow
 * 1. func_800207B4(0) returns the memory-card result code; an unchanged code
 *    (cached at +0x208) exits immediately - a card result, not a stream status.
 * 2. When the active front-end screen is neither D_800A36E8 nor D_800A36B8 (the
 *    two save screens) the menu stack is popped one level with gPopMenuPanel().
 *    D_8009F468 holds that screen descriptor; the identifier gFontBankTable is a
 *    leftover of an earlier round.
 * 3. The previous state and sub-state are copied to +0x203/+0x204, the state
 *    becomes 2 and both UI flags are raised.
 * 4. The sub-state selects the step: 1 -> func_80081204 (load), 2 ->
 *    func_80080B70 (save), 3 -> func_8008204C (format). The retry budget at
 *    +0x922 is decremented first, and a spent budget sets +0x202 = 3.
 * 5. After a load, func_8007F750() validates the image and func_80080AD4()
 *    applies the options; while the boot wait is active (D_8009F464 == 2) the
 *    wait is released (= 3).
 * 6. Sub-state 0 probes the card: func_80020C90(BASLUS-00739FCOPLAPD) returns the
 *    slot of an existing save, otherwise func_80020E68(0x678) looks for 0x678
 *    contiguous free bytes; on success the 0x678-byte image at +0x20C is
 *    cleared. -1 (no save on the card) enters state 3 and pushes the save-slot
 *    menu D_800A3718 with the group-0 cursor on slot 1.
 *
 * @par Globals
 * D_8009F92C (profile context), D_8009F464 (boot wait state),
 * D_8009F8BD/D_8009F8BE (UI flags), D_8009F468 (active screen),
 * D_800120A4 (save file name).
 */
s32 func_8007FB04(void)
{
    register s32 state __asm__("$18");
    register u32 s0 __asm__("$16");
    register u32 s1 __asm__("$17");

    state = func_800207B4(0);
    if (state != *(s32 *)(gSaveProfileImage + 0x208)) {
        if (gFontBankTable != 0 &&
            gFontBankTable != gProfileDetailMenuDescriptor &&
            gFontBankTable != gProfileNameEntryMenuDescriptor) {
            gPopMenuPanel();
        }

        gSaveProfileImage[0x203] = gSaveProfileImage[0x200];
        gSaveProfileImage[0x204] = gSaveProfileImage[0x201];

        if (state >= 0) {
            gSaveProfileImage[0x200] = 2;
            gSaveProfileImage[0x201] = 0;
            gSaveProfileImage[0x202] = 0;
            s0 = 1;
            gUiHoldFlag = s0;
            gUiInteractionEnabled = s0;
            s1 = gSaveProfileImage[0x203];

            if (s1 == 2 && (u32)(gSaveProfileImage[0x204] - 1) < 3) {
                if (gGetMcPendingFlag() != 0) {
                    if (*(s8 *)(gSaveProfileImage + 0x922) != 0) {
                        gSaveProfileImage[0x922] = gSaveProfileImage[0x922] - 1;
                        if (gSaveProfileImage[0x204] == s1) {
                            func_80080B70();
                        } else if (gSaveProfileImage[0x204] == 3) {
                            gQueueMemoryCardFormat();
                        } else {
                            func_80081204();
                        }
                    } else {
                        gSaveProfileImage[0x202] = 3;
                    }
                } else if ((u32)(gSaveProfileImage[0x204] - 1) < 2) {
                    gSaveProfileImage[0x202] = s1;
                    gSaveProfileImage[0x205] = s0;
                    s0 = gSaveProfileImage[0x204];
                    if (s0 == 1) {
                        if (func_8007F750() != 0) {
                            func_80080010();
                            if (*(s32 *)(gSaveProfileImage + 0x40C) != s0) {
                                gSaveProfileImage[0x205] = 3;
                                *(s16 *)(gSaveProfileImage + 0x942) = 0x37D2;
                            }
                            if (gStillRunningState == s1) {
                                func_80080AD4();
                                gStillRunningState = 3;
                            }
                        } else {
                            gSaveProfileImage[0x205] = s1;
                            gUiHoldFlag = 0;
                        }
                    } else {
                        *(s16 *)(gSaveProfileImage + 0x942) = 0x37A0;
                    }
                }
            }

            if (gSaveProfileImage[0x202] == 0) {
                if (func_80020C90(gProfileImageName) != 0) {
                    func_80081204();
                } else if (func_80020E68(0x678) == 0) {
                    gUiHoldFlag = 0;
                    gSaveProfileImage[0x206] = 1;
                } else {
                    gFillMemory(gSaveProfileImage + 0x20C, 0, 0x678);
                    func_80080010();
                    if (gSaveProfileImage[0x921] == 0) {
                        gUiHoldFlag = 0;
                    }
                }
            }
        } else {
            gSaveProfileImage[0x205] = 0;
            gSaveProfileImage[0x206] = 0;

            switch (state) {
            default:
                gSaveProfileImage[0x200] = 0;
                gSaveProfileImage[0x201] = 0;
                gSaveProfileImage[0x207] = 0;
                gUiHoldFlag = 0;
                gUiInteractionEnabled = 1;
                *(s16 *)(gSaveProfileImage + 0x942) = 0;
                func_80082020();
                __asm__ volatile ("");
                break;
            case -1:
                gSaveProfileImage[0x200] = 3;
                gSaveProfileImage[0x202] = 0;
                gSaveProfileImage[0x201] = 0;
                gSaveProfileImage[0x207] = 0;
                func_80078650(gMenuActivationBlock, 1);
                gFontBankTable[0] = 1;
                gUiHoldFlag = 1;
                gUiInteractionEnabled = 1;
                *(s16 *)(gSaveProfileImage + 0x942) = 0;
                func_80082020();
                __asm__ volatile ("");
                break;
            case -2:
                gSaveProfileImage[0x200] = 1;
                gSaveProfileImage[0x202] = 0;
                gSaveProfileImage[0x201] = 0;
                gSaveProfileImage[0x207] = 0;
                gUiHoldFlag = 0;
                gUiInteractionEnabled = 0;
                *(s16 *)(gSaveProfileImage + 0x942) = 0;
                func_80082020();
                __asm__ volatile ("");
                break;
            case -4:
                gSaveProfileImage[0x200] = 2;
                gSaveProfileImage[0x201] = 1;
                gSaveProfileImage[0x207] = 0;
                gUiHoldFlag = 0;
                gUiInteractionEnabled = 0;
                *(s16 *)(gSaveProfileImage + 0x942) = 0;
                func_80082020();
                __asm__ volatile ("");
                break;
            case -5:
                gSaveProfileImage[0x200] = 2;
                if (gSaveProfileImage[0x207] != 0) {
                    gSaveProfileImage[0x201] = 3;
                } else {
                    gSaveProfileImage[0x201] = 2;
                }
                gSaveProfileImage[0x207] = 0;
                gUiHoldFlag = 0;
                gUiInteractionEnabled = 0;
                *(s16 *)(gSaveProfileImage + 0x942) = 0;
                break;
            case -6:
                gSaveProfileImage[0x200] = 3;
                gSaveProfileImage[0x202] = 0;
                gSaveProfileImage[0x201] = 3;
                gSaveProfileImage[0x207] = 1;
                gUiHoldFlag = 0;
                gUiInteractionEnabled = 0;
                func_80082020();
                *(s16 *)(gSaveProfileImage + 0x942) = 0;
                break;
            }
            gPlayerMenuDepth = 0;
        }
        *(s32 *)(gSaveProfileImage + 0x208) = state;
    }
    return state;
}
