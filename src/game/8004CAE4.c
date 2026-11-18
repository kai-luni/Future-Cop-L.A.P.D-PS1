// Categories: Graphics > Color Tables | Game Engine > State Initialization
#include "game/types.h"

struct Unk4CAE4State {
    u8 unk00[0x0C];
    s32 mode0C;
    u8 unk10[0x4C];
    s32 mode5C;
};

extern s32 gEntityIdCounter __asm__("D_8009F7A0");
extern s32 D_8009F7A4;
extern s32 gPendingMessageMask __asm__("D_8009F7A8");
extern u8 gMessageDoneFlag __asm__("D_8009F7AC");
extern s32 gLastSpawnSlot[] __asm__("D_8009F9F4");
extern s32 gUnitListHead __asm__("D_8009FA40");
extern s32 gListTailBranch __asm__("D_8009FA88");
extern struct Unk4CAE4State *gGameSession[] __asm__("D_8009FA94");
extern s32 gMarkerColorTable[] __asm__("D_800A6F60");
extern s32 gCommandListHead[] __asm__("D_800A8F90");

/**
 *  @brief Initializes a color table and resets associated state.
 *
 *  The routine installs 32 fixed color values and selects a limit from
 *  two mode fields in the first state object.
 *  @par Globals
 *  D_800A6F60 receives colors; D_800A8F90 receives zero and the selected limit.
 *  D_8009F7A0, D_8009F7A4, D_8009F7A8, D_8009FA40, and D_8009FA88 are cleared.
 *  D_8009F7AC is set, D_8009F9F4[0] is cleared, and D_8009FA94 supplies modes.
 *  @par Control flow
 *  Mode 5C equal to one selects 0xFE; otherwise mode 0C equal to two selects
 *  0x12C, and all other cases select 0xC8.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the listed table entries and global state values.
 *  @par Callers
 *  Confirmed: func_80024A24.
 */
void func_8004CAE4(void)
{
    register s32 colorF8F800 __asm__("$13");
    register s32 colorF800F8 __asm__("$14");
    register s32 colorF8F8F8 __asm__("$12");
    register s32 color787800 __asm__("$11");
    register s32 color780078 __asm__("$15");
    register s32 color787878 __asm__("$10");
    register s32 color8080F8 __asm__("$18");
    register s32 colorF88070 __asm__("$17");
    register s32 color80F880 __asm__("$16");
    register s32 *entityTable __asm__("$9");
    s32 limit;

    colorF8F800 = 0xF8F800;
    colorF800F8 = 0xF800F8;
    colorF8F8F8 = 0xF8F8F8;
    color787800 = 0x787800;
    color780078 = 0x780078;
    color787878 = 0x787878;
    color8080F8 = 0x8080F8;
    colorF88070 = 0xF88070;
    color80F880 = 0x80F880;
    entityTable = gCommandListHead;

    entityTable[0] = 0;
    gMarkerColorTable[0] = 0;
    D_8009F7A4 = 0;
    gUnitListHead = 0;
    gListTailBranch = 0;
    gEntityIdCounter = 0;

    gMarkerColorTable[1] = 0xF8;
    gMarkerColorTable[2] = 0xF80000;
    gMarkerColorTable[3] = 0xF800;
    gMarkerColorTable[4] = colorF8F800;
    gMarkerColorTable[5] = 0xF8F8;
    gMarkerColorTable[6] = colorF800F8;
    gMarkerColorTable[7] = colorF8F8F8;
    gMarkerColorTable[8] = 0x78F8;
    gMarkerColorTable[9] = 0x78;
    gMarkerColorTable[10] = 0x780000;
    gMarkerColorTable[11] = 0x7800;
    gMarkerColorTable[12] = color787800;
    gMarkerColorTable[13] = 0x7878;
    gMarkerColorTable[14] = color780078;
    gMarkerColorTable[15] = color787878;
    gMarkerColorTable[16] = 0x3878;
    gMarkerColorTable[17] = 0xF8;
    gMarkerColorTable[18] = 0xF80000;
    gMarkerColorTable[19] = 0xF800;
    gMarkerColorTable[20] = color8080F8;
    gMarkerColorTable[21] = colorF88070;
    gMarkerColorTable[22] = color80F880;
    gMarkerColorTable[23] = 0xF8;
    gMarkerColorTable[24] = 0xF80000;
    gMarkerColorTable[25] = 0xF800;
    gMarkerColorTable[26] = colorF8F800;
    gMarkerColorTable[27] = 0xF8F8;
    gMarkerColorTable[28] = colorF800F8;
    gMarkerColorTable[29] = colorF8F8F8;
    gMarkerColorTable[30] = 0x78F8;
    gMarkerColorTable[31] = color787878;
    gLastSpawnSlot[0] = 0;

    gMessageDoneFlag = 1;
    gPendingMessageMask = 0;
    if (gGameSession[0]->mode5C != 1) {
        goto check_secondary;
    }
    limit = 0xFE;
    goto store_limit;
check_secondary:
    if (gGameSession[0]->mode0C == 2) {
        limit = 0x12C;
    } else {
        limit = 0xC8;
    }
store_limit:
    entityTable[1] = limit;
}
